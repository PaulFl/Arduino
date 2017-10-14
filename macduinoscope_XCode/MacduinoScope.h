//
//  MacduinoScope.h
//  MacduinoScope
//
//  Created by Gabe Ghearing on 4/26/09.
//  Copyright 2009 Gabe Ghearing. All rights reserved.
//
//  Description:
//    This class will read values from an MacduinoScope into a buffer. The buffer
//    constantly overwrites and provides for at least 2 seconds of data. The
//    values are the 10 bit values from the ADC and the digital pin 12. The
//    buffer is a 16bit signed array with the following schema:
//        bits 0-9 = ADC value
//        bit 10 = Digital pin 12
//    The samplerate and current writing index of the sample array are available.
//
//   This class is not thread-safe!
//
//    -----------------------------------------------------------------------
//
//   A 16Mhz MacduinoScope/AVR is best used at a baud rate it can really hit...
//     Most baud rates (9600, 38400, 56K... etc) are faked by using a
//     very close baud-rate and correcting frames to bring things into sync.
//     By default, the MacduinoScope samples every incoming bit/symbol 16 times and
//     from that can tell when it is drifting out of sync. The extra sampling
//     only helps with recieveing data, there is no difference for sending data.
//   
//   Go to http://www.wormfood.net/avrbaudcalc.php to find what baud rates work
//     with which AVR clock settings.
//
//  There are two maximum settings for a 16Mhz MacduinoScope:
//     - 1000000 baud @ 16 samples
//     - 2000000 baud @ 8 samples
//  Both of these have 0% error (9600 baud has 0.2% error).
//
//
//  To use 2000000 baud with the MacduinoScope-14/15 IDE use something like:
//
//		Serial.begin(1000000); // init to 1Mbaud @ 16 samples
//		UCSR0A |= 2; // double that to 2Mbaud @ 8 samples
//
//
//	22.6.1 Analog Input Circuitry 
//

#import <Cocoa/Cocoa.h>

// not certain that all of these are needed...
#import <CoreFoundation/CoreFoundation.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/serial/IOSerialKeys.h>
#include <IOKit/IOBSD.h>
#include <IOKit/serial/ioss.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <paths.h>
#include <termios.h>
#include <sysexits.h>
#include <sys/param.h>
#include <sys/select.h>
#include <sys/time.h>
#include <time.h>
#include <AvailabilityMacros.h>

// size of buffer for data as it comes in
#define MAX_SERIAL_BUFFER 8192

// size of buffer for sampled values (needs to hold several seconds)
#define MAX_SAMPLE_BUFFER 4194304

// the number of samples to use to calculate the sample rate, taken ~1/10 sec
#define RATE_SAMPLES 30

@interface MacduinoScope : NSObject {
	float _sample_buffer[MAX_SAMPLE_BUFFER];
	int current_sample_index; // the index that is currently being written
	int trigger_sample_index; // the index where the last trigger happened
	int trigger_type; // 0 = None, 1 = Auto, 2 = Rising, 3 = Falling, 4 = Ext Rising, 5 = Ext Falling
	int trigger_threshold; // the threshold of the trigger ( % )
	bool trigger_triggered; // whether the selected trigger is working
	float sampleFreq; // the calculated frquency... this sucks so it's not displayed to the user
	int period_time_length; // the length of the period being looked at in microseconds
	int period_sample_length; // the number of samples in the period
	float hwSampleRate; // the bytes per microsecond sent from the MacduinoScope
	float hwSync; // the error rate sent from the MacduinoScope in % of bad/total
	bool holdPressed; // whether the hold button is engaged
@private
	uint8_t byte_buffer[MAX_SERIAL_BUFFER]; // raw serial data buffer
	struct termios gOriginalTTYAttrs; // Hold the original termios attributes so we can reset them on quit
	int serialFileDescriptor; // file handle to the serial port
	bool dataThreadRunning; // know whether we are reading with a thread or still synchronously
	int rateArray[4][RATE_SAMPLES]; // bytes, miliseonds, badbytes, goodbytes, to calculate the hardware sampling and error rate
	uint8 ADCprescaler, ADCchannel;
	int edge; // -1 for falling, 0 for N/A, 1 to rising
	int samplesPerEdgeCount; // samples for calculating frequency of triggers
	float floatingAverageSample; // a rough average sample
	bool innerTrigger;
}

@property (readonly) float hwSampleRate; // the hardware sampling rate
@property (readonly) float hwSync; // percent of data that isn't garbled
@property (readonly) float *sample_buffer; // the values of the samples clamped to 0.0-1.0
@property (readonly) int current_sample_index; // index into the samples that is currently being written
@property (readwrite) int trigger_type; // 0 = None, 1 = Auto, 2 = Rising, 3 = Falling, 4 = Ext Rising, 5 = Ext Falling
@property (readwrite) int period_time_length; // the length of the period being looked at in microseconds
@property (readonly) int period_sample_length; // the number of samples in the period
@property (readwrite) bool holdPressed;
@property (readwrite) int trigger_threshold;
@property (readonly) bool trigger_triggered;
@property (readonly) float sampleFreq;


// grab all the available serial ports that the MacduinoScope may be on
- (NSArray *) getPorts;

// open the given port (8bit data, no parity, and 1 stop bit) at the given baud rate.
//
//   1. DTR is used to reset the MacduinoScope
//   2. waits for the MacduinoScope to say "Hello World"
//   3. sends the ADC prescaler setting and channel
//   4. continually grabs data from the ADC into the buffer until the port is closed
//
//   return an error message, or nil on success
- (NSString*) open: (NSString*)bsdPath analogPrescaler: (int) prescaler channel: (int) channel;

// returns YES if open and NO if closed
- (bool) isOpen;

// close the serial port
- (void) close;

@end





