#define BIT1_HIGH_MIN  1100
#define BIT1_HIGH_MAX  1250
#define BIT0_HIGH_MIN  440
#define BIT0_HIGH_MAX  650
#define BIT_LOW_MIN    500
#define BIT_LOW_MAX    650

const byte EINT0_PIN = 3; // External interrupt 0 pin

volatile int flag = 0;
volatile uint32_t tL = 0, tH = 0;

// global variables
int start = 0, current = 0;
uint32_t tF = 0;

#define BUF_SIZE 640
uint16_t buf[ BUF_SIZE + 1 ];

void setup() {
  Serial.begin( 115200 );
  attachInterrupt(0, eint0_isr, CHANGE);  // use EINT0
  while (!Serial) ;
  if (digitalRead(EINT0_PIN ) != HIGH) {
    Serial.println("Error: IR input should be HIGH!");
  }
  tF = tL = tH = micros();
  flag = 0;
  current = start = 0;
}

void loop() {
  boolean dump = (micros() - tF) > 500000;
  if ( dump && current < 8 ) {  // too short sequence
    current = 0; // clear buffer
  }
  else if ( dump || (current == BUF_SIZE) ) {
    for (int i = 0; i < current; i++) {
      Serial.print( buf[i] );
      if (buf[i] >= 2000) Serial.print( '*' );
      Serial.print( (i + 1 == current) ? '\n' : ',' );
    }
    Serial.println( current );
    showBitSequence( buf, current );
    current = 0;
  }

  if ( flag ) {
    uint16_t dT;
    if (flag == 1) // HIGH
      dT = (uint16_t)(tL - tH);
    else // LOW
      dT = (uint16_t)(tH - tL);
    dT = 5 * (dT / 5); // 5usec resolution

    if (start) { // start capture
      buf[current++] = dT;
    }
    else if ( !start && (dT < 5000) && (flag == 2) ) { // found start bit
      start = 1;
    }
    tF = micros();
    flag = 0; // clear flag
  }
}

void showBitSequence( uint16_t time_values[], int len ) {
  uint16_t t;
  char ch;
  for (int i = 0; i < len; i += 2) {
    t = time_values[i];
    if ( t >= BIT1_HIGH_MIN && t < BIT1_HIGH_MAX )
      ch = '1'; // bit 1
    else if ( t >= BIT0_HIGH_MIN && t < BIT0_HIGH_MAX )
      ch = '0'; // bit 0
    else
      ch = '?'; // unknown bit
    if ( i + 1 < len )
      t = time_values[i + 1];
    else
      t = 0;
    if ( t >= BIT_LOW_MIN && t < BIT_LOW_MAX )
      Serial.print( ch );
    else
      Serial.print( '-' );
    Serial.print( ',' );
  }
  Serial.println( "" );
}

void eint0_isr() { // ISR for Ext. Interrupt 0
  uint32_t t = micros();
  if (digitalRead( EINT0_PIN )) { // HIGH
    tH = t;
    flag = 2;
  } else { // LOW
    tL = t;
    flag = 1;
  }
}
