uint8_t decode(uint32_t values); // forward declaration

volatile boolean flag = false;
volatile boolean isRepeated = false;
volatile uint32_t ir_code = 0;
uint8_t  count = 0;
uint8_t  t_WH = 0, t_WL = 0; // width+ and width-
uint32_t t_prev = 0;
uint32_t ir_code_prev = 0;

typedef enum {S0, S1, S2} state_t;
static state_t state = S0;

#define BUF_SIZE  (80)
static uint8_t widths[BUF_SIZE];
static uint8_t buf_index = 0;

#define NUM_KEYS  (17)
const char *KEY_NAMES[] = {
  "UP", "DOWN", "LEFT", "RIGHT", "OK",
  "0", "1", "2", "3",
  "4", "5", "6", "7",
  "8", "9", "*", "#", "INVALID"
};

char sbuf[30]; // used for sprintf()

uint8_t ir_pin = 3; // D2

void IRAM_ATTR eint_isr() {
  uint32_t t  = micros();
  uint8_t  ir_pin_status  = digitalRead(ir_pin);
  uint32_t dt = (t - t_prev);
  uint16_t width = ((dt > 12500) ? 12500 : dt) / 50; // in 50 usec step
  t_prev = t;
  if (ir_pin_status) { t_WL = width; } else { t_WH = width;}
  
  if (buf_index < BUF_SIZE) {
    widths[buf_index++] = width; // width in 50usec step
  }
  
  if (ir_pin_status) {
    Serial.print("state: ");
    Serial.println(state);
    switch (state) {
      case S0:
        isRepeated = false;
        Serial.println(t_WH);
        Serial.println(t_WL);
        // width+ > 12000 usec and width- > 8500 usec
//        if (t_WH > 240 && t_WL > 170) {
          state = S1;
//        }
        break;
      case S1:
        // width+ > 4000 usec and width- > 450 usec
        if (t_WH > 80 && t_WL > 9) { // start code
          state = S2;
          count = 0;
          ir_code = 0;
        } else if (t_WH > 40 && t_WL > 9) { // repeat code
          state = S0;
          flag  = true;
          isRepeated = true;
          ir_code = ir_code_prev;
          buf_index = 0;
        } else {
          state = S0;
        }
        break;
      case S2:
        // width+ < 2000 usec and width- > 450 usec
        if (t_WH < 40 && t_WL > 9) {
          ir_code = (ir_code << 1);
          if (t_WH > 28) { // width+ > 1400 usec
            ir_code |= 1;
          }
          count++;
          
          if (count == 32) {
            ir_code_prev = ir_code;
            flag  = true;
            count = 0;
            state = S0;
            buf_index = 0;
          }
        } else if (t_WH > 240 && t_WL > 170) {
          state = S1;
        }
        break;
      default:
        break;
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(ir_pin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ir_pin), eint_isr, CHANGE);  // Pin D2 = EINT0
  observeError();
}

void loop() {
  if (flag) {
    flag = false; // clear flag
    uint8_t key = decode(ir_code);
    if (isRepeated) {
      Serial.println("repeated!");
    } else {
      sprintf(sbuf, "0x%08lx %s", ir_code, KEY_NAMES[key]);
      Serial.println(sbuf);
    }
  }
}

uint8_t decode(uint32_t value) {
  uint8_t ret_val;
  switch (value) {
    case 0xFF629D: ret_val = 0; break;
    case 0xFFA857: ret_val = 1; break;
    case 0xFF22DD: ret_val = 2; break;
    case 0xFFC23D: ret_val = 3; break;
    case 0xFF02FD: ret_val = 4; break;
    case 0xFF4AB5: ret_val = 5; break;
    case 0xFF6897: ret_val = 6; break;
    case 0xFF9867: ret_val = 7; break;
    case 0xFFB04F: ret_val = 8; break;
    case 0xFF30CF: ret_val = 9; break;
    case 0xFF18E7: ret_val = 10; break;
    case 0xFF7A85: ret_val = 11; break;
    case 0xFF10EF: ret_val = 12; break;
    case 0xFF38C7: ret_val = 13; break;
    case 0xFF5AA5: ret_val = 14; break;
    case 0xFF42BD: ret_val = 15; break;
    case 0xFF52AD: ret_val = 16; break;
    default: ret_val = NUM_KEYS; // invalid
  } //end-switch
  return ret_val;
}

void observeError() {
  if (digitalRead(ir_pin) != HIGH) {
    Serial.println("Error: IR input should be HIGH!");
  }
}

