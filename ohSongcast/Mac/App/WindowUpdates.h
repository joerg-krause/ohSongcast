
#import <Cocoa/Cocoa.h>
#import "AutoUpdate.h"


@interface UpdateCheck : NSObject
{
    bool isManual;
    AutoUpdateInfo* updateInfo;
}

@property (assign) bool isManual;
@property (retain) AutoUpdateInfo* updateInfo;

@end


@interface WindowUpdates : NSWindowController
{
    IBOutlet NSProgressIndicator* progressChecking;
    IBOutlet NSProgressIndicator* progressDownloading;
    IBOutlet NSTextField* textAvailable;
    IBOutlet NSView* viewChecking;
    IBOutlet NSView* viewAvailable;
    IBOutlet NSView* viewUnavailable;
    IBOutlet NSView* viewDownloading;
    AutoUpdate* iAutoUpdate;
    UpdateCheck* iUpdateCheck;
}

@property (assign) NSProgressIndicator* progressChecking;
@property (assign) NSProgressIndicator* progressDownloading;
@property (assign) NSTextField* textAvailable;
@property (assign) NSView* viewChecking;
@property (assign) NSView* viewAvailable;
@property (assign) NSView* viewUnavailable;
@property (assign) NSView* viewDownloading;

- (IBAction) buttonDetailsClicked:(id)aSender;
- (IBAction) buttonInstallClicked:(id)aSender;
- (IBAction) buttonNotNowClicked:(id)aSender;
- (void) setAutoUpdate:(AutoUpdate*)aAutoUpdate;
- (void) startAutomaticCheck;
- (void) startManualCheck;

@end
