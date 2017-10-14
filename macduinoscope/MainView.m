//
//  MainView.m
//  MacduinoScope
//
//  Created by Gabe Ghearing on 4/11/09.
//  Copyright 2009 Gabe Ghearing. All rights reserved.
//

#import "MainView.h"

#import <CoreFoundation/CoreFoundation.h>
#import <IOKit/IOKitLib.h>
#import <IOKit/serial/IOSerialKeys.h>
#import <IOKit/IOBSD.h>
#import <IOKit/serial/ioss.h>

#import <stdio.h>
#import <string.h>
#import <unistd.h>
#import <fcntl.h>
#import <sys/ioctl.h>
#import <errno.h>
#import <paths.h>
#import <termios.h>
#import <sysexits.h>
#import <sys/param.h>
#import <sys/select.h>
#import <sys/time.h>
#import <time.h>
#import <AvailabilityMacros.h>


@implementation MainView

- (void)awakeFromNib {
	NSDictionary *defaultsDictionary = [[NSUserDefaultsController sharedUserDefaultsController] values];
	NSString *lastSerialPort = [defaultsDictionary valueForKey:@"SelectedSerialPort"];
	NSEnumerator * portEnumerator;
	
	//NSLog(@"%@",[defaultsDictionary valueForKey:@"SelectedGraphColor"]);
	serialFileDescriptor = -1;
	
	// create a serialData object to interface with the serial port
	serialPort = [[MacduinoScope alloc] init];
	
	// create the regular list of serial ports
	[self updateSerialList:@"Select a serial port"];
	
	// if the last selected serial port still exists, then select it
	portEnumerator = [[serialPort getPorts] objectEnumerator];
	for(NSString *port;port = [portEnumerator nextObject];) {
		if([port isEqualToString:lastSerialPort]) {
			[serialPullDown selectItemWithTitle:lastSerialPort];
			[self serialListSelector:serialPullDown];
			break;
		}
	}
	
	// create a timer to update the graph ~30 times per second
	graphTimer = [NSTimer scheduledTimerWithTimeInterval:(1.0/30.0) target:self selector:@selector(graphUpdate) userInfo:nil repeats:YES];
	
	// register the graph's timer for event tracking (otherwise it wouldn't update when the slider was being dragged)
	[[NSRunLoop currentRunLoop] addTimer:graphTimer forMode:NSEventTrackingRunLoopMode];
	
	// so that the formatter takes effect on the number...
	[periodSliderText setTitleWithMnemonic:[NSString stringWithFormat:@"%d", [periodSlider intValue]]];
	
	theGraph.dataBuffer = serialPort.sample_buffer;
	theGraph.dataLength = 0;
	
	// makes sure everything is synced
	[self periodChange:periodSlider];
	[self thresholdChange:thresholdSlider];
	[self triggerTypeChange:triggerTypePullDown];
}

// update the graph
- (void) graphUpdate {
	if(![holdBtn state]) {
		if(serialPort.hwSampleRate>0) {
			[hardwareText setTitleWithMnemonic:
				[NSString stringWithFormat:
					@"Sample: %.3f kS\nSync: %.3f %%",
						serialPort.hwSampleRate*1000,
						serialPort.hwSync]];
		} else {
			[hardwareText setTitleWithMnemonic:[NSString stringWithFormat:@"Sample: ----- kS\nSync: ----- %%"]];
		}
		theGraph.dataLength = serialPort.period_sample_length;
		theGraph.samplePeriod = serialPort.period_time_length;
	}
	serialPort.holdPressed = [holdBtn state];
	[triggerActiveText setBackgroundColor:[NSColor colorWithDeviceRed:!serialPort.trigger_triggered green:1.0 blue:!serialPort.trigger_triggered alpha:1.0]];
	if([theGraph hasSelection]) {
		[selectionText setTitleWithMnemonic:[NSString stringWithFormat:@"Min: %.2f%%\nMax: %.2f%%\nRMS: %.2f%%\nTime: %.2fμs\nFreq: %.2fhz",
						     theGraph.selectedMin*100,
						     theGraph.selectedMax*100,
						     theGraph.selectedRMS*100,
						     theGraph.selectedTime,
						     1000000/theGraph.selectedTime
		 ]];
	} else {
		[selectionText setTitleWithMnemonic:[NSString stringWithFormat:@"Min:\nMax:\nRMS:\nTime:\nFreq:"]];
	}
	[theGraph setNeedsDisplay:YES];
}

// handle the serial pull-down selections
- (IBAction) serialListSelector: (id) theList {
	NSString *itemTitle = [[serialPullDown selectedItem] title];
	NSString *topMessage = @"Select a serial port";
	
	// check if there is a device selected
	if([[itemTitle substringToIndex:5] compare:@"/dev/"]) {
		// if it wasn't a device, then just update the list
		[self updateSerialList:topMessage];
	} else {
		// pop the hold button up
		[holdBtn setIntValue:0];
		
		// setup serial communication on the new port if it is a device
		NSString *errorMessage = 
			[serialPort open:itemTitle analogPrescaler:7-[hwRatePullDown indexOfSelectedItem] channel:[channelPullDown indexOfSelectedItem]];
		if(errorMessage!=nil) {
			[self updateSerialList: errorMessage];
		}
	}
	if(![serialPort isOpen]) {
		// go to the top if the port isn't open
		[serialPullDown selectItemAtIndex:0];
	}
}

// update the serial pull-down list with the current serial-ports
- (void) updateSerialList: (NSString *)message {
	NSEnumerator *portEnumerator;
	
	// close the serial port
	[serialPort close];
	
	// remove all the serial port items currently in the menu
	[serialPullDown removeAllItems];
	
	// default message
	[serialPullDown addItemWithTitle:message];
	//[[serialPullDown itemAtIndex:0] setEnabled:NO]; // this looks ugly
	
	// add all the serial port paths
	portEnumerator = [[serialPort getPorts] objectEnumerator];
	for(NSString *port;port = [portEnumerator nextObject];) {
		[serialPullDown addItemWithTitle:port];
	}
	
	// last option is always to update menu
	[serialPullDown addItemWithTitle:@"Refresh"];
}

// update the period when the GUI changes
- (IBAction) periodChange: (id) val {
	int intVal = [val intValue];
	[periodSlider setIntValue:intVal];
	[periodSliderText setIntValue:intVal];
	serialPort.period_time_length = intVal;
}

- (IBAction) periodIncrement: (id) val {
	int intVal = [periodSlider intValue] + ([val intValue]-500000);
	[periodSlider setIntValue:intVal];
	[self periodChange:periodSlider];
	[val setIntValue:500000];
}

// update the trigger threshold when the GUI changes
- (IBAction) thresholdChange: (id) val {
	int intVal = [val intValue];
	[thresholdSlider setFloatValue:intVal];
	[thresholdSliderText setFloatValue:intVal];
	serialPort.trigger_threshold = intVal;
}

// update the trigger type when the GUI changes
- (IBAction) triggerTypeChange: (id) val {
	serialPort.trigger_type = [val indexOfSelectedItem];
}

@end
