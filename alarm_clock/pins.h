#ifndef PINS
#define PINS

short speaker = A7;
short analogInput = A6;
short button = 10;
short led = A5;
short iRReceiver = 5;
short alarmSwitch = A4;

short cathodeSegments[6] = {12, //SEG 1
	11, //SEG 2
	9, //SEG 3
	8, //SEG 4
	7, //SEG 5
	6}; //SEG 6

short anodeSegments[8] = {13, //SEG A
	A0, //SEG B
	2, //SEG C
	A2, //SEG D
	3, //SEG E
	A1, //SEG F
	A3, //SEG G
	4}; //SEG H

#endif 
