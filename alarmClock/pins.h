#ifndef PINS
#define PINS

short speakerV = 10;
short speakerG = 9;
short analogInput = A3;
short sendPin = A4;
short receivePin = A5;
short led = 11;

short cathodeSegments[6] = {0, //SEG 1
	1, //SEG 2
	2, //SEG 3
	3, //SEG 4
	4, //SEG 5
	5}; //SEG 6

short anodeSegments[8] = {6, //SEG A
	7, //SEG B
	8, //SEG C
	12, //SEG D
	13, //SEG E
	A0, //SEG F
	A1, //SEG G
	A2}; //SEG H

#endif
