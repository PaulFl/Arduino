//
//  MacduinoScope.m
//  MacduinoScope
//
//  Created by Gabe Ghearing on 4/26/09.
//  Copyright 2009 Gabe Ghearing. All rights reserved.
//

#import "MacduinoScope.h"


@interface MacduinoScope(MacduinoScopePrivate)
// thread to get data
- (void)getDataThread:(NSThread *)parentThread;

// jiggle the DTR to reset an MacduinoScope
- (void) restartMacduinoScope;

// add a sample to the buffer, also handle internal triggers
- (void) addSample: (float) value;

// handles external triggers
- (void) externalTrigger: (int) triggerBit;

// called when the current trigger signal is hit
- (void) triggerSignal;
@end

@implementation MacduinoScope
@synthesize hwSampleRate;
@synthesize hwSync;
@synthesize current_sample_index;
@synthesize trigger_type;
@synthesize period_time_length;
@synthesize period_sample_length;
@synthesize holdPressed;
@synthesize trigger_threshold;
@synthesize trigger_triggered;
@synthesize sampleFreq;
@dynamic sample_buffer;
- (float *)sample_buffer { return _sample_buffer; }

- (id)init {
	self = [super init];
	if (self) {
		serialFileDescriptor = -1;
		dataThreadRunning = FALSE;
		period_sample_length = 0;
	}
	return self;
}

// open the given port (8bit data, no parity, and 1 stop bit) at the given baud rate
// return a error message on error, or nil on success
- (NSString*) open: (NSString*)bsdPath analogPrescaler: (int) prescaler channel: (int) channel {
	struct termios options;
	speed_t baudRate = 2000000; // 2Mbaud
	unsigned long mics = 3; // receive latency ( 3 microseconds )
	
	// close the handle if it's open
	[self close];
	
	// save the prescaler and channel setting
	ADCprescaler = prescaler & 7;
	ADCchannel = channel & 7;
	
	// make absolutely sure the data grabbing thread is dead before continuing
	while(dataThreadRunning) [self close];
	
	// zero out the buffer
	for(int i=0; i<MAX_SERIAL_BUFFER; i++) {
		byte_buffer[i]=0;
	}
	
	// some message will be put here on an error
	NSMutableString *errorMessage = nil;
	
	// O_NONBLOCK causes the port to open without any delay (we'll block with another call)
	serialFileDescriptor = open([bsdPath cStringUsingEncoding:NSUTF8StringEncoding], O_RDWR | O_NOCTTY | O_NONBLOCK);
	if (serialFileDescriptor == -1) {
		errorMessage = [NSString stringWithFormat:@"Error opening serial port %s - %s(%d).\n", [bsdPath cStringUsingEncoding:NSUTF8StringEncoding], strerror(errno), errno];
	} else if (ioctl(serialFileDescriptor, TIOCEXCL) == -1) { // TIOCEXCL causes blocking of non-root processes on this serial-port
		errorMessage = [NSString stringWithFormat:@"Error setting TIOCEXCL on %s - %s(%d).\n", [bsdPath cStringUsingEncoding:NSUTF8StringEncoding], strerror(errno), errno];
	} else if (fcntl(serialFileDescriptor, F_SETFL, 0) == -1) { // clear the O_NONBLOCK flag; all calls from here on out are blocking for non-root processes
		errorMessage = [NSString stringWithFormat:@"Error clearing O_NONBLOCK %s - %s(%d).\n", [bsdPath cStringUsingEncoding:NSUTF8StringEncoding], strerror(errno), errno];
	} else if (tcgetattr(serialFileDescriptor, &gOriginalTTYAttrs) == -1) { // Get the current options and save them so we can restore the default settings later.
		errorMessage = [NSString stringWithFormat:@"Error getting tty attributes %s - %s(%d).\n", [bsdPath cStringUsingEncoding:NSUTF8StringEncoding], strerror(errno), errno];
	} else {
		// Set the serial bit settings with old termios; baud rate will be set with ioctl()/IOSSIOSPEED
		options = gOriginalTTYAttrs;
		options.c_cflag = CS8 | CSTOPB; // 8bit,no parity,2 stopbit
		if (tcsetattr(serialFileDescriptor, TCSANOW, &options) == -1) {
			errorMessage = [NSString stringWithFormat:@"Error setting tty attributes %s - %s(%d).\n", [bsdPath cStringUsingEncoding:NSUTF8StringEncoding], strerror(errno), errno];
		} else if (ioctl(serialFileDescriptor, IOSSIOSPEED, &baudRate) == -1) { // Set baud rate
			errorMessage = [NSString stringWithFormat:@"Error calling ioctl(..., IOSSIOSPEED, ...) %s - %s(%d).\n", [bsdPath cStringUsingEncoding:NSUTF8StringEncoding], strerror(errno), errno];
		} else if (ioctl(serialFileDescriptor, IOSSDATALAT, &mics) == -1) { // Set the receive latency (a.k.a. don't wait to buffer data)
			errorMessage = [NSString stringWithFormat:@"Error setting read latency %s - %s(%d).\n", [bsdPath cStringUsingEncoding:NSUTF8StringEncoding], strerror(errno), errno];
		}
	}
	
	// make sure the port is closed if a problem happens
	if (serialFileDescriptor != -1) {
		if(errorMessage != nil) {
			[self close];
		} else {
			// otherwise, start the reading thread
			dataThreadRunning = TRUE;
			// block until all data is written and then flush the buffers (to make sure the current thread is done before the new thread takes control.)
			tcdrain(serialFileDescriptor);
			tcflush(serialFileDescriptor, TCIOFLUSH);
			
			// start a new thread to grab data into the buffer
			[NSThread detachNewThreadSelector:@selector(getDataThread:) toTarget:self withObject:[NSThread currentThread]];
			//[self performSelectorInBackground:@selector(getDataThread:) withObject:[NSThread currentThread]]; // new 10.5 way...
		}
	}
	
	return errorMessage;
}

// returns YES if open and NO if closed
- (bool) isOpen {
	if(serialFileDescriptor==-1) {
		return NO;
	}
	return YES;
}

// close the serial port
- (void) close {
	// make sure the handle wasn't already closed
	if(serialFileDescriptor!=-1) {
		// Block until all output has been sent from the device.
		tcdrain(serialFileDescriptor);
		tcflush(serialFileDescriptor, TCIOFLUSH);
		
		// Reset the port back to how it was before we started
		if (tcsetattr(serialFileDescriptor, TCSANOW, &gOriginalTTYAttrs) == -1) printf("Error resetting tty attributes - %s(%d).\n", strerror(errno), errno);
		
		close(serialFileDescriptor);
		serialFileDescriptor = -1;
		
		// closing/opening the same port REALLY fast will fail spectacularly... better sleep for a sec 
		sleep(1);
		
	}
}

// add the bsd path of the available serial ports to a NSPopUpButton
- (NSArray *) getPorts {
	NSMutableArray *thePorts = [NSMutableArray array];
	io_object_t serialPort;
	io_iterator_t serialPortIterator;
	
	// ask for all the serial ports
	IOServiceGetMatchingServices(kIOMasterPortDefault, IOServiceMatching(kIOSerialBSDServiceValue), &serialPortIterator);
	
	// loop through all the serial ports and add them to the array
	while (serialPort = IOIteratorNext(serialPortIterator)) {
		[thePorts addObject:(NSString*)IORegistryEntryCreateCFProperty(serialPort, CFSTR(kIOCalloutDeviceKey),  kCFAllocatorDefault, 0)];
		//NSLog((NSString*)IORegistryEntryCreateCFProperty(serialPort, CFSTR(kIOCalloutDeviceKey),  kCFAllocatorDefault, 0));
		IOObjectRelease(serialPort);
	}
	
	IOObjectRelease(serialPortIterator);
	return thePorts;
}

// set and clear DTR to reset an MacduinoScope
- (void) restartMacduinoScope {
	struct timespec interval = {0,100000000}, remainder;
	if(serialFileDescriptor!=-1) {
		nanosleep(&interval, &remainder); // wait 0.1 seconds
		ioctl(serialFileDescriptor, TIOCSDTR);
		nanosleep(&interval, &remainder); // wait 0.1 seconds
		ioctl(serialFileDescriptor, TIOCCDTR);
	}
}

// thread to get data from the serial port, closing the file handle will kill this thread
- (void) getDataThread:(NSThread *)parentThread {
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// !!!!!!!MAKE SURE THIS IS THREAD SAFE!!!!!!!
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	
	// create a pool so we can use regular Cocoa stuff
	NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
	
	ssize_t numBytes=1;
	float intervalSamples=0; // Number of samples read
	static struct timeval time1 = {0,0}, time2 = {0,0}, time3 = {0,0};
	static struct timezone tzp_blah;
	int interval_index = 0;
	int goodbytes=0, badbytes=0;
	bool throwOutReading = TRUE;
	int throwOutCount = 0;
	bool rateLocked = FALSE;
	int 
	
	current_sample_index = 0;
	innerTrigger = TRUE;
	floatingAverageSample = 0.5;
	trigger_triggered = FALSE;
	
	// zero out the interval
	for(int i=0;i<4;i++) {
		for(int j=0;j<RATE_SAMPLES;j++) {
			rateArray[i][j] = 0;
		}
	}
	
	// reset the MacduinoScope
	[self restartMacduinoScope];
	
	// wait for the device to say "Hello World"
	for(int i=0;(i!=11)&&(numBytes>0);) {
		const char message[] = "Hello World";
		if((serialFileDescriptor!=-1)&&(numBytes>0)) {
			numBytes = read(serialFileDescriptor, byte_buffer, 1);
			if(numBytes==1) {
				if(byte_buffer[0]==message[i]) {
					i++;
				} else {
					if(byte_buffer[0]==message[0]) {
						i=1;
					} else {
						i=0;
					}
				}
			}
		} else {
			return;
		}
	}
	
	// write the prescaler setting (1 byte)
	uint8_t tempByte = (ADCprescaler<<3)|ADCchannel;
	write(serialFileDescriptor, &tempByte, 1);
	
	// get an approximate time offset to subtract from, the first measurement will still be way off.
	gettimeofday(&time2, &tzp_blah);
	
	// start taking in data
	while(numBytes>0) {
		
		// reset the sample frequency if the period has passed
		if(current_sample_index>period_sample_length+(hwSampleRate * 10000)) {
			trigger_triggered = FALSE;
			sampleFreq = 0;
		}
		if((ADCprescaler==0)||(ADCprescaler==1)) {
			// 3.5bit format
			numBytes = read(serialFileDescriptor, byte_buffer, MAX_SERIAL_BUFFER);
			
			// copy the data into the sample buffer
			for(int i=0;i<numBytes;i++) {
				[self addSample: (byte_buffer[i]&0xF)/15.0f];
				[self addSample: (((byte_buffer[i]>>3)&0xE)|(byte_buffer[i]&1))/15.0f];
				[self externalTrigger:(byte_buffer[i])>>7];
				goodbytes++;
				intervalSamples+=2;
				//printf("%d, %d, %d\n",byte_buffer[i]&0xF, ((byte_buffer[i]>>3)&0xE)|(byte_buffer[i]&1), (byte_buffer[i]&0x80)>>7);
			}
		} else if(ADCprescaler==2) {
			// 7bit format
			numBytes = read(serialFileDescriptor, byte_buffer, MAX_SERIAL_BUFFER);
			
			// copy the data into the sample buffer
			for(int i=0;i<numBytes;i++) {
				[self addSample: ((byte_buffer[i])&0x7F)/127.0f];
				[self externalTrigger:(byte_buffer[i])>>7];
				goodbytes++;
				intervalSamples++;
			}
		} else {
			// the 10bit/12bit format (rather complex)
			static int leftover_index = 0;
			static int16_t lastGoodSample = 0;
			int tempi = 0;
			
			// do a read() on the port to get some data
			numBytes = read(serialFileDescriptor, byte_buffer + leftover_index, MAX_SERIAL_BUFFER - leftover_index);
			
			// copy the data into the sample buffer
			for(int b0,b2,b1, val1,val2,trig1;tempi<numBytes+leftover_index-2;tempi+=3) {
				b0 = byte_buffer[tempi];
				b1 = byte_buffer[tempi+1];
				b2 = byte_buffer[tempi+2];
				
				//make sure the sync hasn't been messed up by a dropped byte
				if(((b0&0x80)==0)||((b1&0x80)==0)||((b2&0x80)!=0)) {
					badbytes++;
					tempi-=2; // 3 will be added on at the top of the for-loop so we only skip a single byte
					
					// salvage any information possible from this stray byte
					if((b0&0x80)!=0) {
						// if this is a high-order sample byte, then we might as well use it.
						//   the 7 high-order bits are combined with the last 3 low-order bits.
						[self addSample:(((b0 & 0x7F) << 3) | (lastGoodSample & 7))/1023.0f];
					} else {
						// if this is a low-order/trigger byte, then salvage the trigger bit
						//   the low order bits are thrown away
						trig1 = (b0&0x8)>>3;
						[self externalTrigger:trig1];
					}
				} else {
					goodbytes+=3;
					trig1 = (b2&0x8)>>3;
					val1 = ((b2&0x7) ^ ((b0&0x7F)<<3));
					val2 = (((b2&0x70)>>4) ^ ((b1&0x7F)<<3));
					
					[self addSample: val1/1023.0f];
					[self addSample: val2/1023.0f];
					[self externalTrigger:trig1];
					
					intervalSamples += 2;
					lastGoodSample = val2;
					
					//if(val1!=0) printf("%d, ", val1); else printf(" , ");
					//if(val2!=0) printf("%d, ", val2); else printf(" , ");
				}
			}
			
			// copy any extra data back to the beginning of the buffer
			if(numBytes>tempi) {
				leftover_index = numBytes-tempi;
				for(int i=0;i<leftover_index;i++) {
					byte_buffer[i]=byte_buffer[i+tempi];
				}
			} else {
				leftover_index = 0;
			}
		}
		
		// recalculate the samples in the period
		if(!holdPressed) period_sample_length = hwSampleRate * period_time_length;
		
		// every ~1/10 second reacalculate the data-rate and error-rate
		gettimeofday(&time1, &tzp_blah);
		timersub(&time1, &time2, &time3);
		if((time3.tv_sec > 0)||(time3.tv_usec >= 100000)) {
			time2 = time1;
			// record the number of ms and bytes in an interval
			rateArray[0][interval_index] = intervalSamples;
			rateArray[1][interval_index] = time3.tv_usec + (time3.tv_sec*1000000); // get the miliseconds
			rateArray[2][interval_index] = badbytes;
			rateArray[3][interval_index] = goodbytes;
			
			// reset the number of bytes on this interval
			intervalSamples = 0;
			badbytes = 0;
			goodbytes = 0;
			
			// iterate through the interval index
			interval_index++;
			if(interval_index == RATE_SAMPLES) {
				interval_index = 0;
				rateLocked = TRUE;
			}
			
			// calculate the datarate (bytes/sec)
			int total_bytes = 0, total_msec = 0, total_bad = 0, total_good = 0;
			for(int i=0;i<RATE_SAMPLES;i++) {
				total_bytes+=rateArray[0][i];
				total_msec+=rateArray[1][i];
				total_bad+=rateArray[2][i];
				total_good+=rateArray[3][i];
			}
			
			if(rateLocked){
				// once we have the sample buffer full don't let the rate jump by more than 0.5% every resample
				hwSampleRate = ((hwSampleRate*199)+((float)total_bytes)/((float)total_msec))/200;
			} else {
				hwSampleRate = ((float)total_bytes)/((float)total_msec);
			}
			hwSync = (1.0-(((float)total_bad)/((float)(total_good+total_bad))))*100;
			
			//ignore the first few measurements, this helps get a stable datarate much faster
			if(throwOutReading) {
				interval_index=0;
				
				throwOutCount++;
				if(throwOutCount==5) throwOutReading = FALSE;
			}
		}
	}
	
	// give back the pool
	[pool release];
	
	// zero out the interval
	hwSampleRate = 0;
	
	period_sample_length = 0;
	
	dataThreadRunning = FALSE;
	[NSThread exit];
}

// add a sample to the buffer, handle internal triggers, detecting rises and falls, and calculate frequency
// trigger_type; // 0 = None, 1 = Auto, 2 = Rising, 3 = Falling, 4 = Ext Rising, 5 = Ext Falling
- (void) addSample: (float) value {
	static int lastRead = 0, thisRead; // -1 for low, 0 for N/A, 1 to high
	static int lastReadCount; // the number of times the last read value occured
	bool edgeChanged = FALSE;
	float innerThreshold;
	
	samplesPerEdgeCount++;
	
	// add the sample
	_sample_buffer[current_sample_index] = value;
	
	// incrementment the buffer index if we aren't at the end
	if(current_sample_index<MAX_SAMPLE_BUFFER-2) current_sample_index++;
	
	// recalculate the average sample
	if(period_sample_length>0) floatingAverageSample = ((floatingAverageSample*(period_sample_length-1))+value)/period_sample_length;
	
	// no current trigger if we are past the end of the display
	if(current_sample_index>period_sample_length) {
		trigger_triggered = FALSE;
	}
	
	if(trigger_type==1) {
		innerThreshold = floatingAverageSample;
	} else {
		innerThreshold = trigger_threshold/100.2f;
	}
	
	// count the consecutive high or low bits
	if(value<=innerThreshold) {
		thisRead = -1; // the value is low
	} else {
		thisRead = 1; // the value is high
	}
	if(lastRead==thisRead) {
		lastReadCount++;
	} else {
		lastReadCount=1;
	}
	lastRead = thisRead;
	
	// none-triggered
	if(trigger_type==0) {
		[self triggerSignal];
	}
	
	// when there are enough lows or highs in a row then we know it's a true edge
	if(lastReadCount > (hwSampleRate * 10)) { // 10 microsecs of samples
		if(edge!=lastRead) {
			edgeChanged = TRUE;
			edge = lastRead;
			
			// handle internal triggers
			// trigger_type; // 0 = None, 1 = Auto, 2 = Rising, 3 = Falling, 4 = Ext Rising, 5 = Ext Falling
			
			// rising edge
			if(((trigger_type==1)||(trigger_type==2))&&(edge==1)) {
				[self triggerSignal];
			}
			
			// falling edge
			if((trigger_type==3)&&(edge==-1)) {
				[self triggerSignal];
			}
		}
	}
}

// handle external triggers
// trigger_type; // 0 = None, 1 = Auto, 2 = Rising, 3 = Falling, 4 = Ext Rising, 5 = Ext Falling
- (void) externalTrigger: (int) triggerBit {
	// rising edge
	if((trigger_type==4)&&(triggerBit!=0)) {
		[self triggerSignal];
	}
	
	// falling edge
	if((trigger_type==5)&&(triggerBit==0)) {
		[self triggerSignal];
	}
}


// calculate the frequency of the triggers
//   this function is called anytime the current trigger happens
- (void) triggerSignal {
#define NUM_EDGES_TO_SAMPLE 50
	static int samplesPerEdge[NUM_EDGES_TO_SAMPLE]; // edges to sample from.
	static int samplesPerEdgeIndex=0;
	
	// if the screen is full, then use this trigger to reset the scan
	if((current_sample_index>=period_sample_length)&&(!holdPressed)) {
		trigger_triggered = TRUE;
		current_sample_index = 0;
	}
	
	// add the current sample count to the array
	if(samplesPerEdgeCount > (hwSampleRate * (period_sample_length/100.0f))) { // no more than 100 cycles in the view
		samplesPerEdge[samplesPerEdgeIndex] = samplesPerEdgeCount;
		samplesPerEdgeCount = 0;
		samplesPerEdgeIndex++;
		if(samplesPerEdgeIndex==NUM_EDGES_TO_SAMPLE) samplesPerEdgeIndex=0;
		
		// calculate the frequency of the edges
		int total_samples = samplesPerEdge[0];
		int samples_used = 1;
		
		// loop through and get a rasonable value
		for(int i=0;i<NUM_EDGES_TO_SAMPLE;i++) {
			if(abs(samplesPerEdge[i] - samplesPerEdge[i-1])<(total_samples*.20)) {
				// if the next sample isn't crazy-different from what we were already seeing, use the sample
				total_samples+=samplesPerEdge[i];
				samples_used++;
			} else {
				// if what we were seeing is completely different from both current readings then don't update
				total_samples = samplesPerEdge[i];
				samples_used = 1;
			}
		}
		
		// if at least 85% of the edges were used, then update the frequency
		if(samples_used>NUM_EDGES_TO_SAMPLE*.85) {
			sampleFreq = (hwSampleRate*1000000)/(((float)total_samples)/samples_used);
		}
	}
	if(trigger_type==0) {
		sampleFreq = 0;
	}
	
	samplesPerEdgeCount = 0;
}

@end



