#include <string.h>

#define PPM_CHANS 5

#define IBUS_MAXCHANNELS 14

static uint16_t rcFailsafe[IBUS_MAXCHANNELS] = {  1500, 1500, 950, 1500, 2000, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500 };
#define FAILSAFELIMIT 1020




#define IBUS_BUFFSIZE 32    // Max iBus packet size (2 byte header, 14 channels x 2 bytes, 2 byte checksum)
                                                                                                                                     
static uint16_t rcValue[IBUS_MAXCHANNELS];
static uint16_t rcValueSafe[IBUS_MAXCHANNELS]; // read by interrupt handler. Data copied here in cli/sei block
static boolean rxFrameDone;
static boolean failsafe = 0;
uint16_t dummy;

// Prototypes
void setupRx();
void readRx();

void setup() {

  setupRx();
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);  // Checksum error - turn on error LED

}

void loop() {
  readRx();  
  digitalWrite(13, rcValue[4]>1500);
}

static uint8_t ibusIndex = 0;
static uint8_t ibus[IBUS_BUFFSIZE] = {0};

void setupRx()
{
  uint8_t i;
  for (i = 0; i < PPM_CHANS; i++) { rcValue[i] = 1127; }
  Serial.begin(115200);
}

void readRx()
{
  uint8_t i;
  uint16_t chksum, rxsum;

  rxFrameDone = false;

  uint8_t avail = Serial.available();
  
  if (avail)
  {
    digitalWrite(4, LOW);
    uint8_t val = Serial.read();
    // Look for 0x2040 as start of packet
    if (ibusIndex == 0 && val != 0x20) {
      return;
    }
    if (ibusIndex == 1 && val != 0x40) {
      ibusIndex = 0;
      return;
    }
 
    if (ibusIndex < IBUS_BUFFSIZE) ibus[ibusIndex] = val;
    ibusIndex++;

    if (ibusIndex == IBUS_BUFFSIZE)
    {
      ibusIndex = 0;
      chksum = 0xFFFF;
      for (i = 0; i < 30; i++)
        chksum -= ibus[i];

      rxsum = ibus[30] + (ibus[31] << 8);
      if (chksum == rxsum)
      {
        //Unrolled loop  for 10 channels - no need to copy more than needed.
        // MODIFY IF MORE CHANNELS NEEDED
        rcValue[0] = (ibus[ 3] << 8) + ibus[ 2];
        rcValue[1] = (ibus[ 5] << 8) + ibus[ 4];
        rcValue[2] = (ibus[ 7] << 8) + ibus[ 6];
        rcValue[3] = (ibus[ 9] << 8) + ibus[ 8];
        rcValue[4] = (ibus[11] << 8) + ibus[10];
        rcValue[5] = (ibus[13] << 8) + ibus[12];
        rcValue[6] = (ibus[15] << 8) + ibus[14];
        rcValue[7] = (ibus[17] << 8) + ibus[16];
        rcValue[8] = (ibus[19] << 8) + ibus[18];
        rcValue[9] = (ibus[21] << 8) + ibus[20];
        rxFrameDone = true;
        if (rcValue[0] < FAILSAFELIMIT && rcValue[1] < FAILSAFELIMIT &&
            rcValue[2] < FAILSAFELIMIT && rcValue[3] < FAILSAFELIMIT &&
            rcValue[4] < FAILSAFELIMIT && rcValue[5] < FAILSAFELIMIT ) 
        {
          failsafe = 1;
          cli(); // disable interrupts
          memcpy(rcValueSafe, rcFailsafe, IBUS_MAXCHANNELS * sizeof(uint16_t));
          sei();
          digitalWrite(13, HIGH);  //  Error - turn on error LED
        }
        else
        {
          // Now we need to disable interrupts to copy 16-bit values atomicly
          // Only copy needed signals (10 channels default)
          // MODIFY IF MORE CHANNELS NEEDED
          cli(); // disable interrupts.
          rcValueSafe[0] = rcValue[0];
          rcValueSafe[1] = rcValue[1];
          rcValueSafe[2] = rcValue[2];
          rcValueSafe[3] = rcValue[3];
          rcValueSafe[4] = rcValue[4];
          rcValueSafe[5] = rcValue[5];
          rcValueSafe[6] = rcValue[6];
          rcValueSafe[7] = rcValue[7];
          rcValueSafe[8] = rcValue[8];
          rcValueSafe[9] = rcValue[9];         
          sei();
        }
      } 
      return;
    }
  }
}
