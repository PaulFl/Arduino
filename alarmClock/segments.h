#ifndef SEGMENTS
#define SEGMENTS

#define ASEG 0
#define BSEG 1
#define CSEG 2
#define DSEG 3
#define ESEG 4
#define FSEG 5
#define GSEG 6
#define HSEG 7

#define ACHAR 10
#define CCHAR 11
#define ECHAR 12
#define FCHAR 13
#define HCHAR 14
#define ICHAR 15
#define JCHAR 16
#define LCHAR 17
#define OCHAR 0
#define PCHAR 18
#define SCHAR 5
#define UCHAR 19

boolean chars[20][7] = {{1, 1, 1, 1, 1, 1, 0}, // 0
	{0, 1, 1, 0, 0, 0, 0}, //1
	{1, 1, 0, 1, 1, 0, 1}, //2
	{1, 1, 1, 1, 0, 0, 1}, //3
	{0, 1, 1, 0, 0, 1, 1}, //4
	{1, 0, 1, 1, 0, 1, 1}, //5
	{1, 0, 1, 1, 1, 1, 1}, //6
	{1, 1, 1, 0, 0, 0, 0}, //7
	{1, 1, 1, 1, 1, 1, 1}, //8
	{1, 1, 1, 1, 0, 1, 1}, //9
	{1, 1, 1, 0, 1, 1, 1}, //A
	{1, 0, 0, 1, 1, 1, 0}, //C
	{1, 0, 0, 1, 1, 1, 1}, //E
	{1, 0, 0, 0, 1, 1, 1}, //F
	{0, 1, 1, 0, 1, 1, 0}, //H
	{0, 0, 0, 0, 1, 1, 0}, //I
	{0, 1, 1, 1, 1, 0, 0}, //J
	{0, 0, 0, 1, 1, 1, 0}, //L
	{1, 1, 0, 0, 1, 1, 1}, //P
	{0, 1, 1, 1, 1, 1, 0}}; //U

#endif	
