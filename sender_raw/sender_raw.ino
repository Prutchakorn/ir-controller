const byte BUTTON_PIN = 2; // input push button
const byte IR_LED_PIN = 5; // control pin for IR LED

void setup() {
  pinMode( IR_LED_PIN, OUTPUT );
  pinMode( BUTTON_PIN, INPUT );
  digitalWrite( BUTTON_PIN, HIGH ); // enable pull-up
  Serial.begin( 115200 );
  while (!Serial) ;
  Serial.println("Ready...");
}

const byte CODE_POWER_ON[] = {
  1,1,0,0,0,1,0,0,1,1,0,1,0,0,1,1,0,1,1,0,0,1,0,0,1,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,1,1,0,0,0,0,0,0,
  1,0,1,0,0,0,0,0,1,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,1,0,
  255
};

const byte CODE_POWER_OFF[] = {
  1,1,0,0,0,1,0,0,1,1,0,1,0,0,1,1,0,1,1,0,0,1,0,0,1,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,1,0,0,0,0,0,0,
  1,0,1,0,0,0,0,0,1,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,
  255
};

const byte CODE_25_DEG[] = {
  1,1,0,0,0,1,0,0,1,1,0,1,0,0,1,1,0,1,1,0,0,1,0,0,1,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,1,1,0,0,0,0,0,0,
  0,1,1,0,0,0,0,0,1,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,1,0,0,1,0,
  255
};

const byte CODE_26_DEG[] = {
  1,1,0,0,0,1,0,0,1,1,0,1,0,0,1,1,0,1,1,0,0,1,0,0,1,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,1,1,0,0,0,0,0,0,
  1,0,1,0,0,0,0,0,1,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,1,0,
  255
};

const byte CODE_27_DEG[] = {
  1,1,0,0,0,1,0,0,1,1,0,1,0,0,1,1,0,1,1,0,0,1,0,0,1,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,1,1,0,0,0,0,0,0,
  0,0,1,0,0,0,0,0,1,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,1,0,0,1,0,
  255
};

const byte CODE_28_DEG[] = {
  1,1,0,0,0,1,0,0,1,1,0,1,0,0,1,1,0,1,1,0,0,1,0,0,1,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,1,1,0,0,0,0,0,0,
  1,1,0,0,0,0,0,0,1,1,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,1,1,0,
  255
};

void burst( int cycles ) {
  cli(); // turn off global interrupt
  for( uint32_t i=0; i < cycles; i++ ) {
     digitalWrite( IR_LED_PIN, HIGH );
     delayMicroseconds( 7 ); 
     digitalWrite( IR_LED_PIN, LOW );
     delayMicroseconds( 8 ); // may adjust this value to change the frequency
  }
  sei(); // turn on global interrupt
}

const char *CODE_NAMES[6] = {
  "Power On", "25 deg.", "26 deg.", "27 deg.", "28 deg.", "Power Off"
};

const byte *CODE_LIST[6] = {
  CODE_POWER_ON, CODE_25_DEG, CODE_26_DEG, CODE_27_DEG, CODE_28_DEG, CODE_POWER_OFF
};

void loop() {
  static byte code_index = 0;
  if ( digitalRead( BUTTON_PIN ) == LOW ) { // button pressed
    uint16_t i = 0;
    byte b;
    const byte *code = CODE_LIST[ code_index ];
    Serial.println( CODE_NAMES[ code_index] );
    code_index = (code_index+1) % 6;

    // send start bit
    burst( 143 ); // LOW: approx. 3800 usec 
    delayMicroseconds( 1460 ); // HIGH: approx 1460 usec

    burst( 20 ); // LOW: approx. 540 usec
    while (true) {
      b = code[ i ]; // get next bit from the code sequence
      if (b == 255) break; // end of bit sequence
      delayMicroseconds( (b==1) ? 1150 : 560 ); // HIGH
      burst( 20 ); // LOW: approx. 540 usec 
      i++;
    }
    while ( digitalRead(BUTTON_PIN) != HIGH ) ; // wait for button release
  }
  delay(10);
}
