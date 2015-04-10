#ifndef PINS
#define PINS

short speaker = A4;
short analogInput = A3;
short button = 10;
short led = 9;

short cathodeSegments[6] = {1, //SEG 1
	2, //SEG 2
	3, //SEG 3
	4, //SEG 4
	5, //SEG 5
	6}; //SEG 6

short anodeSegments[8] = {7, //SEG A
	8, //SEG B
	11, //SEG C
	12, //SEG D
	13, //SEG E
	A0, //SEG F
	A1, //SEG G
	A2}; //SEG H

#endif
