#ifndef FUNCTIONS
#define FUNCTIONS

#define TIME 0
#define DATE 1
#define ALARM 2

void display(byte text[6]);
void setSeparators(boolean separators[6]);
void flip();
void setTimeUser();
void setDateUser();
void displayDate();
void displayTime(boolean displaySec);
int readIR();
int readButton();

#endif
