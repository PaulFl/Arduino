//
//  MainView.h
//  MacduinoScope
//
//  Created by Gabe Ghearing on 4/11/09.
//  Copyright 2009 Gabe Ghearing. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "GraphView.h"
#import "MacduinoScope.h"


@interface MainView : NSView {
@public
	IBOutlet NSPopUpButton *serialPullDown, *hwRatePullDown, *channelPullDown, *triggerTypePullDown;
	IBOutlet NSSlider *periodSlider, *thresholdSlider;
	IBOutlet NSTextField *periodSliderText, *thresholdSliderText, *hardwareText, *selectionText;
	IBOutlet GraphView *theGraph;
	IBOutlet NSButton *holdBtn;
	IBOutlet NSTextField *triggerActiveText;
	int serialFileDescriptor;
	NSTimer *graphTimer;
	MacduinoScope *serialPort;
}

- (IBAction) serialListSelector: (id) theList;
- (IBAction) periodChange: (id) val;
- (IBAction) periodIncrement: (id) val;
- (IBAction) thresholdChange: (id) val;
- (IBAction) triggerTypeChange: (id) val;
- (void) updateSerialList: (NSString *)message;
- (void) graphUpdate;
@end
