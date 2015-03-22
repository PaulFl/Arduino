#ifndef FUNCTIONS
#define FUNCTIONS

#define TIME
#define DATE
#define ALARM

void display(byte text[6]);
void setSeparators(boolean separators[6]);
void flip();
void setTimeUser();
void setDateUser();
void displayDate();
void displayTime();
int readCapSensor();

#endif
