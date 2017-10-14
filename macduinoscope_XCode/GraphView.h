//
//  GraphView.h
//  MacduinoScope
//
//  Created by Gabe Ghearing on 4/22/09.
//  Copyright 2009 Gabe Ghearing. All rights reserved.
//

#import <Cocoa/Cocoa.h>


// the maximum number of points in the graph buffer
#define MAX_GRAPH_BUFFER_SIZE 2000

@interface GraphView : NSView {
@public
	IBOutlet NSPopUpButton *divisionPullDown, *sampleTypePullDown;
	IBOutlet NSColorWell *graphColorWell;
	float *dataBuffer; // the data to chart
	int dataLength; // the amount of data to chart
	float samplePeriod; // the length of time of the sample
	
	// the calculated stuff
	float selectedMin;
	float selectedMax;
	float selectedRMS;
	float selectedTime;
	bool hasSelection;
@private
	float mouseSelection[2]; // the mouse selection
}

@property (readwrite) float *dataBuffer;
@property (readwrite) int dataLength;
@property (readwrite) float samplePeriod;

@property (readonly) float selectedMin;
@property (readonly) float selectedMax;
@property (readonly) float selectedRMS;
@property (readonly) float selectedTime;
@property (readonly) bool hasSelection;

@end
