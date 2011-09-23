
#import "Model.h"
#import "Receiver.h"
#include "../../Songcaster.h"


// Declaration for receiver callback - defined in ReceiverList.mm
extern void ReceiverListCallback(void* aPtr, ECallbackType aType, THandle aReceiver);

// Forward declarations of callback functions defined below
void ModelSubnetCallback(void* aPtr, ECallbackType aType, THandle aSubnet);
void ModelConfigurationChangedCallback(void* aPtr, THandle aSongcaster);



// Implementation of the model class
@implementation Model


- (id) init
{
    [super init];
    
    iSongcaster = nil;
    iPreferences = nil;
    iEnabled = false;
    iReceiverList = nil;
    iSelectedUdnList = nil;
    iObserver = nil;

    return self;
}


- (void) start
{
    // create the preferences object
    iPreferences = [[Preferences alloc] initWithBundle:[NSBundle mainBundle]];
    [iPreferences synchronize];

    // build the initial list of receivers from the preferences
    NSMutableArray* list = [NSMutableArray arrayWithCapacity:0];
    for (PrefReceiver* pref in [iPreferences receiverList])
    {
        [list addObject:[[[Receiver alloc] initWithPref:pref] autorelease]];
    }
    
    // create the receiver list
    iReceiverList = [[ReceiverList alloc] initWithReceivers:list];    
    
    // get other preference data
    iEnabled = [iPreferences enabled];
    iSelectedUdnList = [[iPreferences selectedUdnList] retain];

    // setup some event handlers
    [iReceiverList addObserver:self];
    [iPreferences addObserverEnabled:self selector:@selector(preferenceEnabledChanged:)];
    [iPreferences addObserverIconVisible:self selector:@selector(preferenceIconVisibleChanged:)];
    [iPreferences addObserverSelectedUdnList:self selector:@selector(preferenceSelectedUdnListChanged:)];
    [iPreferences addObserverRefreshReceiverList:self selector:@selector(preferenceRefreshReceiverList:)];
    [iPreferences addObserverReconnectReceivers:self selector:@selector(preferenceReconnectReceivers:)];
    
    // create the songcaster object
    uint32_t subnet = 0;
    uint32_t channel = 0;
    uint32_t ttl = 4;
    uint32_t multicast = 0;
    uint32_t preset = 0;
    iSongcaster = SongcasterCreate("av.openhome.org", subnet, channel, ttl, multicast, iEnabled ? 1 : 0, preset, ReceiverListCallback, iReceiverList, ModelSubnetCallback, self, ModelConfigurationChangedCallback, self, "OpenHome", "http://www.openhome.org", "http://www.openhome.org");
}


- (void) playReceiver:(Receiver*)aReceiver andReconnect:(bool)aReconnect
{
    switch ([aReceiver status])
    {
        case eReceiverStateOffline:
        case eReceiverStateBuffering:
        case eReceiverStatePlaying:
            // These states imply playing is not possible or not necessary
            break;

        case eReceiverStateDisconnected:
            // if reconnect flag is not set, do not play this receiver, otherwise fall
            // through to play it
            if (!aReconnect)
                break;

        case eReceiverStateStopped:
            // The receiver is stopped, so play it
            [aReceiver play];
            break;
    }
}


- (void) stopReceiver:(Receiver*)aReceiver
{
    switch ([aReceiver status])
    {
        case eReceiverStateOffline:
        case eReceiverStateDisconnected:
            // These states imply that there has been some sort of external interaction
            // with the receiver, such as changing source using Kinsky, so do not
            // tamper with them
            break;

        case eReceiverStateStopped:
        case eReceiverStateBuffering:
        case eReceiverStatePlaying:
            // These states imply the receiver is still connected to this songcast
            // sender for this songcaster, so stop them and put them into standby
            [aReceiver stop];
            [aReceiver standby];
            break;
    }
}


- (void) stopReceivers
{
    // Only stop receivers that are in the selected list
    for (Receiver* receiver in [iReceiverList receivers])
    {
        if ([iSelectedUdnList containsObject:[receiver udn]])
        {
            [self stopReceiver:receiver];
        }
    }
}


- (void) stop
{
    // app shutting down - stop receivers before destroying songcaster
    [self stopReceivers];

    // shutdown the songcaster
    SongcasterDestroy(iSongcaster);
    iSongcaster = NULL;

    [iSelectedUdnList release];
    [iReceiverList release];
    [iPreferences release];
}


- (void) setObserver:(id<IModelObserver>)aObserver
{
    iObserver = aObserver;
}


- (bool) iconVisible
{
    return [iPreferences iconVisible];
}


- (bool) enabled
{
    return iEnabled;
}


- (void) setEnabled:(bool)aValue
{
    // just set the preference - eventing by the preference change will
    // then cause the state of the songcaster to be updated
    [iPreferences setEnabled:aValue];
}


- (void) reconnectReceivers
{
    // if the songcaster is enabled, force all selected receivers to reconnect
    if (iEnabled)
    {
        for (Receiver* receiver in [iReceiverList receivers])
        {
            if ([iSelectedUdnList containsObject:[receiver udn]])
            {
                [self playReceiver:receiver andReconnect:true];
            }
        }
    }
}


- (void) preferenceEnabledChanged:(NSNotification*)aNotification
{
    // refresh cached preferences and update the local copy
    [iPreferences synchronize];
    iEnabled = [iPreferences enabled];

    // stop receivers before disabling songcaster
    if (!iEnabled)
    {
        [self stopReceivers];
    }

    // enable/disable the songcaster
    SongcasterSetEnabled(iSongcaster, iEnabled ? 1 : 0);

    // start receivers after songcaster is enabled
    if (iEnabled)
    {
        // On switching on the songcaster, only explicitly play receivers that are in the
        // stopped state - receivers that are disconnected are left alone
        for (Receiver* receiver in [iReceiverList receivers])
        {
            if ([iSelectedUdnList containsObject:[receiver udn]])
            {
                [self playReceiver:receiver andReconnect:false];
            }
        }
    }
    
    // notify UI
    [iObserver enabledChanged];    
}


- (void) preferenceIconVisibleChanged:(NSNotification*)aNotification
{
    // refresh cached preferences
    [iPreferences synchronize];
    
    // notify UI
    [iObserver iconVisibleChanged];
}


- (void) preferenceSelectedUdnListChanged:(NSNotification*)aNotification
{
    // refresh cached preferences
    [iPreferences synchronize];

    // get the new list of selected receivers in order to determine changes in the list
    NSArray* newSelectedUdnList = [[iPreferences selectedUdnList] retain];

    // if the songcaster is enabled, need to play/stop receivers that have been
    // selected/deselected
    if (iEnabled)
    {
        // build the list of receivers that have just been deselected
        NSMutableArray* deselected = [NSMutableArray arrayWithCapacity:0];
        for (NSString* udn in iSelectedUdnList)
        {
            if (![newSelectedUdnList containsObject:udn])
            {
                [deselected addObject:udn];
            }
        }
    
        // build the list of receivers that have just been selected
        NSMutableArray* selected = [NSMutableArray arrayWithCapacity:0];
        for (NSString* udn in newSelectedUdnList)
        {
            if (![iSelectedUdnList containsObject:udn])
            {
                [selected addObject:udn];
            }
        }
        
        // now play and stop the relevant receivers
        for (Receiver* receiver in [iReceiverList receivers])
        {
            if ([deselected containsObject:[receiver udn]])
            {
                [self stopReceiver:receiver];
            }
            else if ([selected containsObject:[receiver udn]])
            {
                [self playReceiver:receiver andReconnect:true];
            }
        }
    }
    
    // now discard the old list
    [iSelectedUdnList release];
    iSelectedUdnList = newSelectedUdnList;
}


- (void) updatePreferenceReceiverList
{
    // build a new list of receivers to store in the preferences
    NSMutableArray* list = [NSMutableArray arrayWithCapacity:0];
    
    for (Receiver* receiver in [iReceiverList receivers])
    {
        [list addObject:[receiver convertToPref]];
    }
    
    // set - this sends notification of the change
    [iPreferences setReceiverList:list];
}


- (void) preferenceRefreshReceiverList:(NSNotification*)aNotification
{
    // remove undiscovered, unselected receivers
    [iReceiverList removeUnavailableUnselected:iSelectedUdnList];
    
    // update the preferences
    [self updatePreferenceReceiverList];
    
    // now signal the songcaster lower level to refresh
    SongcasterRefreshReceivers(iSongcaster);
}


- (void) preferenceReconnectReceivers:(NSNotification*)aNotification
{
    [self reconnectReceivers];
}


- (void) receiverAdded:(Receiver *)aReceiver
{
    [self updatePreferenceReceiverList];

    // the receiver has just appeared on the network - start playing if required i.e.
    //  - songcaster is switched on
    //  - receiver is selected
    //  - receiver is connected and not playing i.e. stopped
    if (iEnabled && [iSelectedUdnList containsObject:[aReceiver udn]])
    {
        [self playReceiver:aReceiver andReconnect:false];
    }
}


- (void) receiverRemoved:(Receiver *)aReceiver
{
    [self updatePreferenceReceiverList];
}


- (void) receiverChanged:(Receiver *)aReceiver
{
    [self updatePreferenceReceiverList];
}


- (void) configurationChanged
{
    bool enabled = (SongcasterEnabled(iSongcaster) != 0);

    if (enabled != [self enabled])
    {
        [self setEnabled:enabled];
    }
}


@end



// Callbacks from the ohSongcaster code
void ModelSubnetCallback(void* aPtr, ECallbackType aType, THandle aSubnet)
{
}

void ModelConfigurationChangedCallback(void* aPtr, THandle aSongcaster)
{
    Model* model = (Model*)aPtr;
    [model configurationChanged];
}




