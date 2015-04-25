#ifndef FUNCTIONS
#define FUNCTIONS

boolean ajouterEtape();
void playSequence(boolean finished);
int checkEtape(char etape);
void lectureBoutons();
void animationReponseFausse(int lastPressed);
void ecranFin(boolean perdu);
void flip();
void delayFlip(unsigned int delay);
void display(byte text[2]);

#endif
