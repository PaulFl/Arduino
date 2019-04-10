//
//  segments_display.h
//  
//
//  Created by Paul Fleury on 10/04/2019.
//

#ifndef segments_display_h
#define segments_display_h

#define ACHAR 10
#define BCHAR 23
#define CCHAR 11
#define DCHAR 22
#define ECHAR 12
#define FCHAR 13
#define GCHAR 24
#define HCHAR 14
#define ICHAR 15
#define JCHAR 16
#define KCHAR 25
#define LCHAR 17
#define MCHAR 26
#define NCHAR 18
#define OCHAR 0
#define PCHAR 19
#define QCHAR 27
#define RCHAR 28
#define SCHAR 5
#define TCHAR 29
#define UCHAR 20
#define VCHAR 30
#define WCHAR 31
#define XCHAR 32
#define YCHAR 33
#define ZCHAR 34

#define OFFSEG 21

bool chars[35][7] = {{1, 1, 1, 1, 1, 1, 0}, // 0
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
    {0, 1, 1, 0, 1, 1, 1}, //H
    {0, 0, 0, 0, 1, 1, 0}, //I
    {0, 1, 1, 1, 1, 0, 0}, //J
    {0, 0, 0, 1, 1, 1, 0}, //L
    {0, 0, 1, 0, 1, 0, 1}, //n
    {1, 1, 0, 0, 1, 1, 1}, //P
    {0, 1, 1, 1, 1, 1, 0}, //U
    {0, 0, 0, 0, 0, 0, 0},//Off segment
    {0, 1, 1, 1, 1, 0, 1}, //d
    {0, 0, 1, 1, 1, 1, 1},//b
    {1, 0, 1, 1, 1, 1, 0},//G
    {1, 0, 1, 0, 1, 1, 1},//K
    {1, 1, 0, 1, 0, 1, 0},//M
    {1, 1, 1, 0, 0, 1, 1},//q
    {0, 0, 0, 0, 1, 0, 1},//r
    {0, 0, 0, 1, 1, 1, 1},//t
    {0, 1, 0, 1, 0, 1, 0},//V
    {0, 1, 1, 1, 1, 1, 1}, //W
    {1, 0, 0, 1, 0, 0, 1}, //X
    {0, 1, 1, 1, 0, 1, 1}, //y
    {1, 1, 0, 1, 1, 0, 1}
};

int alphabet[26] = {ACHAR, BCHAR, CCHAR, DCHAR, ECHAR, FCHAR, GCHAR, HCHAR, ICHAR, JCHAR, KCHAR, LCHAR, MCHAR, NCHAR, OCHAR, PCHAR, QCHAR, RCHAR, SCHAR, TCHAR, UCHAR, VCHAR, WCHAR, XCHAR, YCHAR, ZCHAR};

#endif /* segments_display_h */
