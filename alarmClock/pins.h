#ifndef PINS
#define PINS

short speaker = A0;
short potentio = A1;
short thermistor = A2;
short sendPin = A3;
short receivePin = A4;

short cathodeSegments[6] = {1, //SEG 1
	2, //SEG 2
	3, //SEG 3
	4, //SEG 4
	5, //SEG 5
	6}; //SEG 6

short anodeSegments[8] = {7, //SEG A
	9, //SEG B
	10, //SEG C
	11, //SEG D
	12, //SEG E
	13, //SEG F
	12, //SEG G
	13}; //SEG H

#endif
