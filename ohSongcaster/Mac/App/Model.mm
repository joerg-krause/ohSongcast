
#import "Model.h"
#import "Receiver.h"
#include "../../Songcaster.h"


// Implementation of the model class
@implementation Model


- (id) init
{
    [super init];
    
    iObserver = nil;
    iModelSongcaster = nil;

    // create the preferences object
    iPreferences = [[Preferences alloc] initWithBundle:[NSBundle mainBundle]];
    [iPreferences synchronize];

    // setup some event handlers
    [iPreferences addObserverEnabled:self selector:@selector(preferenceEnabledChanged:)];
    [iPreferences addObserverIconVisible:self selector:@selector(preferenceIconVisibleChanged:)];
    [iPreferences addObserverSelectedUdnList:self selector:@selector(preferenceSelectedUdnListChanged:)];
    [iPreferences addObserverRefreshReceiverList:self selector:@selector(preferenceRefreshReceiverList:)];
    [iPreferences addObserverReconnectReceivers:self selector:@selector(preferenceReconnectReceivers:)];

    return self;
}


- (void) start
{
    if (iModelSongcaster)
        return;

    // make sure preferences are synchronised so the songcaster is correctly initialised
    [iPreferences synchronize];

    // the songcaster is always started disabled - ensure that the preferences reflect this
    [iPreferences setEnabled:false];

    // create the songcaster model
    iModelSongcaster = [[ModelSongcaster alloc] initWithReceivers:[iPreferences receiverList] andSelectedUdns:[iPreferences selectedUdnList]];
    [iModelSongcaster setReceiversChangedObserver:self selector:@selector(receiversChanged)];
    [iModelSongcaster setConfigurationChangedObserver:self selector:@selector(configurationChanged)];
}


- (void) stop
{
    if (!iModelSongcaster)
        return;

    // disable the songcaster before destroying the songcaster - make sure the preferences reflect this and
    // the songcaster is disabled synchronously - if [self setEnabled:false] was called, the songcaster
    // would get disabled asynchronously, by which time it would have been destroyed and, therefore, the
    // receivers will not be put into standby
    [iModelSongcaster setEnabled:false];
    [iPreferences setEnabled:false];

    // dispose of the songcaster model before releasing - this will shutdown the
    // songcaster
    [iModelSongcaster dispose];

    // shutdown the songcaster
    [iModelSongcaster release];
    iModelSongcaster = 0;
}


- (void) setObserver:(id<IModelObserver>)aObserver
{
    iObserver = aObserver;

    // send events to update the observer
    [iObserver enabledChanged];    
    [iObserver iconVisibleChanged];
}


- (bool) iconVisible
{
    return [iPreferences iconVisible];
}


- (bool) enabled
{
    return [iPreferences enabled];
}


- (void) setEnabled:(bool)aValue
{
    // just set the preference - eventing by the preference change will
    // then cause the state of the songcaster to be updated
    [iPreferences setEnabled:aValue];
}


- (bool) hasRunWizard
{
    return [iPreferences hasRunWizard];
}


- (void) reconnectReceivers
{
    if (iModelSongcaster) {
        [iModelSongcaster playReceivers];
    }
}


- (void) preferenceEnabledChanged:(NSNotification*)aNotification
{
    // refresh cached preferences
    [iPreferences synchronize];

    // enable/disable the songcaster
    if (iModelSongcaster) {
        [iModelSongcaster setEnabled:[iPreferences enabled]];
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

    // set the selected list in the songcaster
    if (iModelSongcaster) {
        [iModelSongcaster setSelectedUdns:[iPreferences selectedUdnList]];
    }
}


- (void) preferenceRefreshReceiverList:(NSNotification*)aNotification
{
    if (iModelSongcaster) {
        [iModelSongcaster refreshReceivers];
    }
}


- (void) preferenceReconnectReceivers:(NSNotification*)aNotification
{
    [self reconnectReceivers];
}


- (void) receiversChanged
{
    if (!iModelSongcaster)
        return;

    // build a new list of receivers to store in the preferences
    NSMutableArray* list = [NSMutableArray arrayWithCapacity:0];
    
    for (Receiver* receiver in [iModelSongcaster receivers])
    {
        [list addObject:[receiver convertToPref]];
    }
    
    // set - this sends notification of the change
    [iPreferences setReceiverList:list];
}


- (void) configurationChanged
{
    if (!iModelSongcaster)
        return;

    [self setEnabled:[iModelSongcaster enabled]];
}


@end


