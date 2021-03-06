/*

  Firmware for Linobit


  Power managing based on:
  http://www.gammon.com.au/power



  Coil 0      A0
  Coil 1      A1
  Coil 2      A2
  Coil 3      A3
  Coil 4      INT1 / D3
  Latch RST   D10 / PB2

  Decoder EN  D8 / PB0
  Decoder 0 D7 / PD7
  Decoder 1 D6 / PD6
  Decoder 2 D5 / PD5
*/



#include <Arduino.h>
#include <avr/sleep.h>
#include <avr/wdt.h>


#include <Wire.h>

#include <Adafruit_SSD1306.h>
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);


bool readCoilsPress = false;


uint8_t sleepLength = 2;

//###############################
//  Char Array
//###############################
//uint8_t charBeingRead = 9;
uint8_t chars[] = {0, 0, 0, 0, 0, 0, 0, 0};

char intToChar[] = {
  '_',
  'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
  'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
  'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
  'Y', 'Z'
};



//###############################
//  Coil 5 Interrupt
//###############################
bool coil5 = false;
void coil5interrupt() {
  coil5 = true;
}



//###############################
//  Wake Func
//###############################
void wake ()
{
  readCoilsPress = true;
  sleep_disable(); // cancel sleep as a precaution
  detachInterrupt (0); // precautionary while we do other stuff
}



//###############################
//  Sleep Func
//###############################
void goToSleep(uint8_t _mode) {

  if (_mode == 1) {

    // clear various "reset" flags
    MCUSR = 0;
    // allow changes, disable reset
    WDTCSR = bit (WDCE) | bit (WDE);
    // set interrupt mode and an interval
    WDTCSR = bit (WDIE) | bit (WDP1) | bit (WDP2) | bit (WDP3);// | bit (WDP1) | bit (WDP2);

    wdt_reset();  // pat the dog

    MCUCR = bit (BODS) | bit (BODSE);
    MCUCR = bit (BODS);

  }


  set_sleep_mode (SLEEP_MODE_PWR_DOWN);
  sleep_enable();

  // Do not interrupt before we go to sleep, or the
  // ISR will detach interrupts and we won't wake.
  noInterrupts ();
  pinMode(2, INPUT_PULLUP);

  // will be called when pin D2 goes low
  attachInterrupt (0, wake, FALLING);
  EIFR = bit (INTF0);  // clear flag for interrupt 0


  // We are guaranteed that the sleep_cpu call will be done
  // as the processor executes the next instruction after
  // interrupts are turned on.
  interrupts ();  // one cycle
  sleep_cpu ();   // one cycle
}





//###############################
//  Set decoder outputs
//###############################
void setDecoder(uint8_t _decode) {
  if (_decode == 0b100) _decode = 001;
  else if (_decode == 0b001) _decode = 100;
  else if (_decode == 0b110) _decode = 0b011;
  else if (_decode == 0b011) _decode = 0b110;


  PORTD = (_decode << 5);
  PORTB &= ~_BV(PB0); // LOW - enable decoder
  PORTB |= _BV(PB0); // HIGH - disable decoder
}


//###############################
//  Read the coils and return
//###############################
uint8_t readCoils() {

  int _charSum = 0;

  _charSum = 0;

  //chars[_charBeingRead] = PINC & 0b00001111;

  _charSum += digitalRead(A0) * 16;
  _charSum += digitalRead(A1) * 8;
  _charSum += digitalRead(A2) * 4;
  _charSum += digitalRead(A3) * 2;


  if (coil5) {
    _charSum += 1;
    coil5 = false;
  }

  //reset Latch
  PORTB |= _BV(PB2); // digitalWrite(10, HIGH);
  delay(1);
  PORTB &= ~_BV(PB2); //digitalWrite(10, LOW);

  return _charSum;
}




//###############################
//  Setup IO
//###############################
void setupIO() {

  pinMode(2, INPUT_PULLUP); // button interrupt pull-up

  pinMode(8, OUTPUT);
  digitalWrite(8, HIGH); // disable 3-8 decoder

  pinMode(A0, INPUT); // coil inputs
  pinMode(A1, INPUT); // coil inputs
  pinMode(A2, INPUT); // coil inputs
  pinMode(A3, INPUT); // coil inputs

  pinMode(10, OUTPUT);
  digitalWrite(10, LOW); // disable RS Latch reset

  pinMode(5, OUTPUT); // decoder outputs
  pinMode(6, OUTPUT); // decoder outputs
  pinMode(7, OUTPUT); // decoder outputs

  attachInterrupt (1, coil5interrupt, RISING);

}

//###############################
//  Watchdog Interrupt
//###############################
ISR (WDT_vect)
{
  wdt_disable();  // disable watchdog
}  // end of WDT_vect



//###############################
//  Runs on boot
//###############################
void setup ()
{
  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(16, 24);
  display.setTextSize(2);
  display.println(" LINOBIT");
  display.display();



  // disable ADC
  ADCSRA = 0;

  setupIO();


  delay(200);
  readCoilsPress = true;
  //goToSleep(0);
}




//###############################
//  Runs everytime it wakes
//###############################
bool showDisplay = true;


void loop ()
{
  // this is used if only one char is to be read per wake up
  //  charBeingRead++;
  //  if (charBeingRead >= 8) {
  //    charBeingRead = 0;
  //    display.clearDisplay();
  //    display.setCursor(16, 16);
  //  }

  if (readCoilsPress) {
    for (uint8_t charBeingRead = 0; charBeingRead < 8; charBeingRead++) {
      setDecoder(charBeingRead); // drive the decoder
      chars[charBeingRead] = readCoils();

    }
  }


  display.clearDisplay();
  display.setCursor(16, 16);

  if (showDisplay) {
    showDisplay = false;
    if (readCoilsPress) {
      for (uint8_t charBeingRead = 0; charBeingRead < 8; charBeingRead++) {
        display.print(intToChar[chars[charBeingRead]]);
        display.display();
      }
      readCoilsPress = false;
    }
    else {
      for (uint8_t charBeingRead = 0; charBeingRead < 8; charBeingRead++) {
        display.print(intToChar[chars[charBeingRead]]);
      }
      display.display();
    }
    sleepLength = 2;
  }
  else {
    showDisplay = true;
    sleepLength = 2;
    display.display();
  }

  goToSleep(1); // go back to sleep
}

