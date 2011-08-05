
#import "Preferences.h"


@implementation PrefReceiver

@synthesize udn;
@synthesize room;
@synthesize group;
@synthesize name;
@synthesize available;


- (id) initWithDict:(NSDictionary*)aDict
{
    [super init];

    udn = [[aDict objectForKey:@"Udn"] retain];
    room = [[aDict objectForKey:@"Room"] retain];
    group = [[aDict objectForKey:@"Group"] retain];
    name = [[aDict objectForKey:@"Name"] retain];
    available = [[aDict objectForKey:@"Available"] boolValue];
    
    return self;
}


- (NSDictionary*) convertToDict
{
    return [NSDictionary dictionaryWithObjectsAndKeys:
            udn, @"Udn",
            room, @"Room",
            group, @"Group",
            name, @"Name",
            [NSNumber numberWithBool:available], @"Available", nil];
}

@end



@implementation Preferences


- (id) initWithBundle:(NSBundle*)aBundle
{
    self = [super init];
    
    appId = (CFStringRef)[NSLocalizedStringFromTableInBundle(@"PreferencesAppId", @"NonLocalizable", aBundle, @"") retain];
    
    return self;
}


- (bool) enabled
{
    CFPropertyListRef pref = CFPreferencesCopyAppValue(CFSTR("Enabled"), appId);
    
    if (pref)
    {
        if (CFGetTypeID(pref) == CFBooleanGetTypeID())
        {
            CFBooleanRef enabled = (CFBooleanRef)pref;
            return (enabled == kCFBooleanTrue);
        }
        
        CFRelease(pref);
    }
    
    return false;
}


- (void) setEnabled:(bool)aEnabled
{
    if (aEnabled)
    {
        CFPreferencesSetAppValue(CFSTR("Enabled"), kCFBooleanTrue, appId);
    }
    else
    {
        CFPreferencesSetAppValue(CFSTR("Enabled"), kCFBooleanFalse, appId);
    }    
    CFPreferencesAppSynchronize(appId);
}


- (NSArray*) receiverList
{
    // create a temporary mutable array for building the list
    NSMutableArray* receiverList = [NSMutableArray arrayWithCapacity:0];
    
    CFPropertyListRef pref = CFPreferencesCopyAppValue(CFSTR("ReceiverList"), appId);

    if (pref)
    {
        if (CFGetTypeID(pref) == CFArrayGetTypeID())
        {
            CFArrayRef list = (CFArrayRef)pref;
        
            for (CFIndex i=0 ; i<CFArrayGetCount(list) ; i++)
            {
                const void* item = CFArrayGetValueAtIndex(list, i);
            
                if (item && CFGetTypeID(item) == CFDictionaryGetTypeID())
                {
                    PrefReceiver* receiver = [[PrefReceiver alloc] initWithDict:(NSDictionary*)item];
                    [receiverList addObject:receiver];
                    [receiver release];
                }
            }
        }

        CFRelease(pref);
    }

    // return a new immutable array of receivers
    return [NSArray arrayWithArray:receiverList];
}


- (void) setReceiverList:(NSArray*)aReceiverList
{
    NSMutableArray* list = [NSMutableArray arrayWithCapacity:0];

    for (uint i=0 ; i<[aReceiverList count] ; i++)
    {
        [list addObject:[[aReceiverList objectAtIndex:i] convertToDict]];
    }

    CFPreferencesSetAppValue(CFSTR("ReceiverList"), list, appId);
    CFPreferencesAppSynchronize(appId);
}


- (void) synchronize
{
    CFPreferencesAppSynchronize(appId);
}


@end

