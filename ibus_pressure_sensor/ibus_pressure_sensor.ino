#include <Wire.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

#define BME280_ADDRESS 0x76
unsigned long int hum_raw, temp_raw, pres_raw;
signed long int t_fine;

uint16_t dig_T1;
int16_t dig_T2;
int16_t dig_T3;
uint16_t dig_P1;
int16_t dig_P2;
int16_t dig_P3;
int16_t dig_P4;
int16_t dig_P5;
int16_t dig_P6;
int16_t dig_P7;
int16_t dig_P8;
int16_t dig_P9;
int8_t  dig_H1;
int16_t dig_H2;
int8_t  dig_H3;
int16_t dig_H4;
int16_t dig_H5;
int8_t  dig_H6;


#define ALTITUDE_SHIFT  0
#define CLIMB_SHIFT 0
#define N_IBUS_CHANNELS 6

volatile uint8_t sens_addr[N_IBUS_CHANNELS]; // sensor addresses

// Sensor IDs taken from qba667 FlySky FS-i6 firmware mod. See the
// ibustelemetry.h in his source code.
#define IBUS_SENS_INTV    0x00 // Internal Voltage
#define IBUS_SENS_TEMP    0x01 // Temperature
#define IBUS_SENS_RPM   0x02 // RPM
#define IBUS_SENS_EXTV    0x03 // External Voltage
#define IBUS_SENS_CLIMB   0x09 // Clibm rate m/s *100
#define IBUS_SENS_PRES    0x41 // Pressure
#define IBUS_SENS_GPS_ALT 0x82 // 4 bytes signed GPS alt m*100
#define IBUS_SENS_ALT   0x83 // 4 bytes signed Alt m*100
#define IBUS_SENS_ALT_MAX 0x84 // 4 bytes signed Alt m*100

// Which sensors we have
const uint8_t sens_type[N_IBUS_CHANNELS] = {
  IBUS_SENS_TEMP,
  IBUS_SENS_ALT,        // absolute height, 4 bytes signed m*100
  IBUS_SENS_GPS_ALT,    // relative height, 4 bytes signed m*100
  IBUS_SENS_ALT_MAX,    // max relative height, 4 bytes signed m*100
  IBUS_SENS_CLIMB,      // climb rate m/s * 100, 2 bytes
  //IBUS_SENS_EXTV,
  IBUS_SENS_PRES,
};

volatile uint32_t sens_val[N_IBUS_CHANNELS];

static void handle_rx_packet(void);

int32_t alt, base_alt, alt_measured;
int32_t max_alt = 0, climb = 0, prev_alt = 0, climb_sum = 0;
uint8_t base_alt_measurements = 20, climb_measurements = 0;


void setup() {
  uint8_t osrs_t = 1;             //Temperature oversampling x 1
  uint8_t osrs_p = 1;             //Pressure oversampling x 1
  uint8_t osrs_h = 1;             //Humidity oversampling x 1
  uint8_t mode = 3;               //Normal mode
  uint8_t t_sb = 5;               //Tstandby 1000ms
  uint8_t filter = 0;             //Filter off
  uint8_t spi3w_en = 0;           //3-wire SPI Disable

  uint8_t ctrl_meas_reg = (osrs_t << 5) | (osrs_p << 2) | mode;
  uint8_t config_reg    = (t_sb << 5) | (filter << 2) | spi3w_en;
  uint8_t ctrl_hum_reg  = osrs_h;

  Wire.begin();

  writeReg(0xF2, ctrl_hum_reg);
  writeReg(0xF4, ctrl_meas_reg);
  writeReg(0xF5, config_reg);
  readTrim();                    //

  adc_init();
  led_init();

  base_alt = alt >> 0;

  serial_init();
  ibus_init();

  sei();

}

void loop() {
  double temp_act = 0.0, press_act = 0.0, hum_act = 0.0;
  signed long int temp_cal;
  unsigned long int press_cal, hum_cal;

  readData();

  temp_cal = calibration_T(temp_raw);
  press_cal = calibration_P(pres_raw);
  hum_cal = calibration_H(hum_raw);
  temp_act = (double)temp_cal / 100.0;
  press_act = (double)press_cal / 100.0;
  hum_act = (double)hum_cal / 1024.0;

  double  altitude_calc = (1 - pow(press_act / (double)1013.25, 0.1903)) / 0.0000225577;

  int32_t tmp;
  uint8_t sens = 0;

  led1_on();

  sens_val[sens++] = temp_act * (double)10.0 + (double)400;

  alt_measured = altitude_calc * (double)100.0;


  alt -= alt >> ALTITUDE_SHIFT;
  alt += alt_measured;

  tmp = alt >> ALTITUDE_SHIFT;
  // absolute altitude
  sens_val[sens++] = tmp;

  if (base_alt_measurements) {
    base_alt_measurements--;
    max_alt = 0;
    base_alt = tmp;
    tmp = 0;
    prev_alt = 0;
  } else {
    // convert to relative
    tmp -= base_alt;

    // maximum relative
    if (tmp > max_alt) {
      max_alt = tmp;
    }
  }
  // relative altitude
  sens_val[sens++] = tmp;

  // maximum altitude
  sens_val[sens++] = max_alt;

  climb_sum += alt_measured;

  if (++climb_measurements >= (1 << CLIMB_SHIFT)) {
    climb_measurements = 0;
    climb_sum >>= CLIMB_SHIFT;
    climb = (climb_sum - prev_alt) * 16 / (1 << CLIMB_SHIFT);
    prev_alt = climb_sum;
    climb_sum = 0;
  }

  // climb rate (RoC)
  sens_val[sens++] = climb;

  // ext_voltage
  //  tmp = read_adc_sync();
  //  tmp = adc_to_10mv(tmp);
  //  if (tmp < 100) { // when unconnected, don't send the noise
  //    tmp = 0;
  //  }
  //  sens_val[sens++] = tmp;

  sens_val[sens++] = press_act * (double)10.0;


  led1_off();
  _delay_ms(60);


}

void readTrim() {
  uint8_t data[32], i = 0;                   // Fix 2014/04/06
  Wire.beginTransmission(BME280_ADDRESS);
  Wire.write(0x88);
  Wire.endTransmission();
  Wire.requestFrom(BME280_ADDRESS, 24);      // Fix 2014/04/06
  while (Wire.available()) {
    data[i] = Wire.read();
    i++;
  }

  Wire.beginTransmission(BME280_ADDRESS);    // Add 2014/04/06
  Wire.write(0xA1);                          // Add 2014/04/06
  Wire.endTransmission();                    // Add 2014/04/06
  Wire.requestFrom(BME280_ADDRESS, 1);       // Add 2014/04/06
  data[i] = Wire.read();                     // Add 2014/04/06
  i++;                                       // Add 2014/04/06

  Wire.beginTransmission(BME280_ADDRESS);
  Wire.write(0xE1);
  Wire.endTransmission();
  Wire.requestFrom(BME280_ADDRESS, 7);       // Fix 2014/04/06
  while (Wire.available()) {
    data[i] = Wire.read();
    i++;
  }
  dig_T1 = (data[1] << 8) | data[0];
  dig_T2 = (data[3] << 8) | data[2];
  dig_T3 = (data[5] << 8) | data[4];
  dig_P1 = (data[7] << 8) | data[6];
  dig_P2 = (data[9] << 8) | data[8];
  dig_P3 = (data[11] << 8) | data[10];
  dig_P4 = (data[13] << 8) | data[12];
  dig_P5 = (data[15] << 8) | data[14];
  dig_P6 = (data[17] << 8) | data[16];
  dig_P7 = (data[19] << 8) | data[18];
  dig_P8 = (data[21] << 8) | data[20];
  dig_P9 = (data[23] << 8) | data[22];
  dig_H1 = data[24];
  dig_H2 = (data[26] << 8) | data[25];
  dig_H3 = data[27];
  dig_H4 = (data[28] << 4) | (0x0F & data[29]);
  dig_H5 = (data[30] << 4) | ((data[29] >> 4) & 0x0F); // Fix 2014/04/06
  dig_H6 = data[31];                                   // Fix 2014/04/06
}
void writeReg(uint8_t reg_address, uint8_t data) {
  Wire.beginTransmission(BME280_ADDRESS);
  Wire.write(reg_address);
  Wire.write(data);
  Wire.endTransmission();
}


void readData() {
  int i = 0;
  uint32_t data[8];
  Wire.beginTransmission(BME280_ADDRESS);
  Wire.write(0xF7);
  Wire.endTransmission();
  Wire.requestFrom(BME280_ADDRESS, 8);
  while (Wire.available()) {
    data[i] = Wire.read();
    i++;
  }
  pres_raw = (data[0] << 12) | (data[1] << 4) | (data[2] >> 4);
  temp_raw = (data[3] << 12) | (data[4] << 4) | (data[5] >> 4);
  hum_raw  = (data[6] << 8) | data[7];
}


signed long int calibration_T(signed long int adc_T) {

  signed long int var1, var2, T;
  var1 = ((((adc_T >> 3) - ((signed long int)dig_T1 << 1))) * ((signed long int)dig_T2)) >> 11;
  var2 = (((((adc_T >> 4) - ((signed long int)dig_T1)) * ((adc_T >> 4) - ((signed long int)dig_T1))) >> 12) * ((signed long int)dig_T3)) >> 14;

  t_fine = var1 + var2;
  T = (t_fine * 5 + 128) >> 8;
  return T;
}

unsigned long int calibration_P(signed long int adc_P) {
  signed long int var1, var2;
  unsigned long int P;
  var1 = (((signed long int)t_fine) >> 1) - (signed long int)64000;
  var2 = (((var1 >> 2) * (var1 >> 2)) >> 11) * ((signed long int)dig_P6);
  var2 = var2 + ((var1 * ((signed long int)dig_P5)) << 1);
  var2 = (var2 >> 2) + (((signed long int)dig_P4) << 16);
  var1 = (((dig_P3 * (((var1 >> 2) * (var1 >> 2)) >> 13)) >> 3) + ((((signed long int)dig_P2) * var1) >> 1)) >> 18;
  var1 = ((((32768 + var1)) * ((signed long int)dig_P1)) >> 15);
  if (var1 == 0)
  {
    return 0;
  }
  P = (((unsigned long int)(((signed long int)1048576) - adc_P) - (var2 >> 12))) * 3125;
  if (P < 0x80000000)
  {
    P = (P << 1) / ((unsigned long int) var1);
  }
  else
  {
    P = (P / (unsigned long int)var1) * 2;
  }
  var1 = (((signed long int)dig_P9) * ((signed long int)(((P >> 3) * (P >> 3)) >> 13))) >> 12;
  var2 = (((signed long int)(P >> 2)) * ((signed long int)dig_P8)) >> 13;
  P = (unsigned long int)((signed long int)P + ((var1 + var2 + dig_P7) >> 4));
  return P;
}

unsigned long int calibration_H(signed long int adc_H) {
  signed long int v_x1;

  v_x1 = (t_fine - ((signed long int)76800));
  v_x1 = (((((adc_H << 14) - (((signed long int)dig_H4) << 20) - (((signed long int)dig_H5) * v_x1)) +
            ((signed long int)16384)) >> 15) * (((((((v_x1 * ((signed long int)dig_H6)) >> 10) *
                (((v_x1 * ((signed long int)dig_H3)) >> 11) + ((signed long int) 32768))) >> 10) + (( signed long int)2097152)) *
                ((signed long int) dig_H2) + 8192) >> 14));
  v_x1 = (v_x1 - (((((v_x1 >> 15) * (v_x1 >> 15)) >> 7) * ((signed long int)dig_H1)) >> 4));
  v_x1 = (v_x1 < 0 ? 0 : v_x1);
  v_x1 = (v_x1 > 419430400 ? 419430400 : v_x1);
  return (unsigned long int)(v_x1 >> 12);
}

/* ------------ on-board LEDs ------------- */
static void led_init(void) {
  DDRB |= _BV(PB0); // Rx LED
  DDRD |= _BV(PD5); // Tx LED
}

static void led1_on(void) {
  PORTB &= ~_BV(PB0);
}

static void led1_off(void) {
  PORTB |= _BV(PB0);
}

static void led2_on(void) {
  PORTD &= ~_BV(PD5);
}

static void led2_off(void) {
  PORTD |= _BV(PD5);
}

/* ----------------- USART ----------------- */

// I-Bus uses 115200n8
#define UART_BAUD       115200
#define UBRR_VAL        ((F_CPU + 8UL * UART_BAUD) / (16UL*UART_BAUD) - 1)

#define BUFLEN 8
static volatile uint8_t buffer[BUFLEN];
static volatile uint8_t buf_offset;

static void serial_init(void) {
  UBRR0 = UBRR_VAL;

  UCSR0A = 0;
  // UCSR0B = _BV(RXCIE1)|_BV(RXEN1)|_BV(TXEN1);
  UCSR0B = _BV(RXEN0) | _BV(RXCIE0) | _BV(UDRIE0);
  UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);
}

static void recv_restart(void) {
  led2_off();

  buf_offset = 0;
  UCSR0B &= ~_BV(TXEN0);
  UCSR0B |= _BV(RXEN0) | _BV(RXCIE0);
}

static void tx_start(void) {
  buf_offset = 0;

  UCSR0B &= ~_BV(RXEN0);
  UCSR0B |= _BV(TXEN0) | _BV(UDRIE0);
}

// USART receive interrupt
ISR(USART_RX_vect) {
  uint8_t val = UDR0;

  // a shorthand - for now, we accept 4-byte packets only
  if (buf_offset == 0 && val != 4)
    return;

  buffer[buf_offset++] = val;

  if (buf_offset == buffer[0]) {
    handle_rx_packet();
    buf_offset = 0;
  }
}

// Next Tx byte wanted
ISR(USART_UDRE_vect) {
  if (buf_offset < buffer[0])
    UDR0 = buffer[buf_offset++];

  if (buf_offset >= buffer[0]) { // finished
    UCSR0B &= ~_BV(UDRIE0);
    UCSR0B |= _BV(TXCIE0);
  }
}

// Tx finished
ISR(USART_TX_vect) {
  UCSR0B &= ~_BV(TXCIE0);
  recv_restart();
}

/* ---- A/D converter for battery voltage ---- */
static void adc_init(void) {
  DIDR0 |= _BV(ADC4D); // disable digital input on ADC4
  ADCSRA = _BV(ADEN)   // enable ADC
           | _BV(ADPS0) | _BV(ADPS1) | _BV(ADPS2); // CLK/128 = 125 kHz
  ADMUX = _BV(REFS1) | _BV(REFS0) // internal 2.56V reference
          | _BV(MUX2); // ADC4 on pin PF4
}

// TODO: use the ADC interrupt instead
static uint16_t read_adc_sync(void) {
  uint16_t retval;

  ADCSRA |= _BV(ADSC); // start the conversion

  // wait for the result
  while ((ADCSRA & _BV(ADIF)) == 0)
    ;

  retval = ADCW;
  ADCSRA |= _BV(ADIF); // clear the interrupt flag

  return retval;
}

#define EXT_V_DIVIDER  ((390+15)/15) // 390k and 15k resistors
#define adc_to_10mv(x) ((x) * EXT_V_DIVIDER * 0.25)

/* ----------------- iBus ------------------ */
static void ibus_init(void) {
  uint8_t i;

  for (i = 0 ; i < N_IBUS_CHANNELS; i++)
    sens_addr[i] = 0;

  recv_restart();
}

static void send_buffer(void) {
  uint8_t i;
  uint16_t csum = 0xFFFF;

  led2_on(); // off after the frame is sent

  // compute the I-Bus checksum
  for (i = 0; i < buffer[0] - 2; i++)
    csum -= buffer[i];

  buffer[i++] = csum & 0xFF;
  buffer[i++] = csum >> 8;

  tx_start();
}

static void handle_rx_packet(void) {
  uint16_t csum = 0xFFFF;
  uint8_t i, cmd, dev;

  for (i = 0; i < buf_offset - 2; i++)
    csum -= (uint16_t)buffer[i];

  if ((buffer[buf_offset - 2] != (csum & 0xFF))
      || (buffer[buf_offset - 1] != (csum >> 8))) { // invalid csum
    buf_offset = 0; // start over
    return;
  }

  cmd = buffer[1] & 0xF0;
  dev = buffer[1] & 0x0F;

  switch (cmd) {
    case 0x80: // discovery/assign address
      for (i = 0; i < N_IBUS_CHANNELS; i++) {
        if (sens_addr[i] == 0 || sens_addr[i] == dev) {
          sens_addr[i] = dev;
          send_buffer();
          return;
        }
      }
      break;
    case 0x90: // telemetry type request
      for (i = 0; i < N_IBUS_CHANNELS; i++) {
        if (sens_addr[i] == dev) {
          buffer[0] = 0x06; // len
          buffer[2] = sens_type[i];
          // Sensor IDs >= 0x80 are four-byte
          buffer[3] = sens_type[i] < 0x80 ? 0x02 : 0x04;
          send_buffer();
          return;
        }
      }
      break;
    case 0xA0: // get measurement request
      for (i = 0; i < N_IBUS_CHANNELS; i++) {
        if (sens_addr[i] == dev) {
          buffer[2] = sens_val[i] & 0xFF;
          buffer[3] = (sens_val[i] >> 8) & 0xFF;
          // two-byte or four-byte sensor?
          if (sens_type[i] < 0x80) {
            buffer[0] = 0x06;
          } else {
            buffer[0] = 0x08;
            buffer[4] = (sens_val[i] >> 16) & 0xFF;
            buffer[5] = (sens_val[i] >> 24) & 0xFF;
          }
          send_buffer();
          return;
        }
      }
      break;
  }
}
