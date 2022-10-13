/*
  based on  d. bodnar  revised 6-16-2016

  rosscoe revised 10-10-2022
*/
#include "defines.h"

#include "Arduino.h"

#include<EEPROM.h>

#ifdef I2CLCD
#include <Wire.h>
#include <SerLCD.h>
#endif

#ifdef SERLCD
#include <SoftwareSerial.h>
#endif

#include <Keypad.h>

#include "variables.h"
#include "functions.h"

void setup() {

/*
 * uncomment these for a new throttle to initialize eeprom
 * then comment them out and upload to the throttle again
 * 
 */

  Serial.begin (115200);

//  saveAddresses();
//  saveDescriptions();

  pinMode(ledPin, OUTPUT);
  pinMode(pinA, INPUT_PULLUP); // set pinA as an input, pulled HIGH to the logic voltage (5V or 3.3V for most cases)
  pinMode(pinB, INPUT_PULLUP); // set pinB as an input, pulled HIGH to the logic voltage (5V or 3.3V for most cases)
  pinMode(buttonPin, INPUT);
  attachInterrupt(0, PinA, RISING); // set an interrupt on PinA, looking for a rising edge signal and executing the "PinA" Interrupt Service Routine (below)
  attachInterrupt(1, PinB, RISING); // set an interrupt on PinB, looking for a rising edge signal and executing the "PinB" Interrupt Service Routine (below)
  //  pinMode(Trigger1, INPUT);
  //  pinMode(Trigger2, INPUT);
  //  digitalWrite(Trigger1, HIGH);// turn on pullup resistors
  //  digitalWrite(Trigger2, HIGH);// turn on pullup resistors
  // randomSeed(analogRead(0));
  pinMode(LED, OUTPUT);

#ifdef I2CLCD
/*
  lcd.begin (16, 2); //  LCD is 16 characters x 2 lines
  lcd.setBacklightPin(BACKLIGHT_PIN, POSITIVE);
  lcd.setBacklight(HIGH);  // Switch on the backlight
  lcd.home (); // go home
*/
#endif


  getAddresses();      // read eeprom
  getDescriptions();   // read descriptions from eeprom

#ifdef SERLCD
  lcd.begin(9600);     // initialize with software serial
//  setBacklight();
  lcd.write(254);                            //Clear the display - this moves the cursor to home position as well
  lcd.write(1);

  lcd.print("Hold button to");
  lcd.write(254);
  lcd.write(line2);

  lcd.print("vary MaxLocos ");
  lcd.print(maxLocos);
#endif

#ifdef I2CLCD  
/*
  lcd.setCursor(0, 0);
  lcd.print("Hold button to");
  lcd.setCursor(0, 1);
  lcd.print("vary MaxLocos ");
  lcd.print(maxLocos);
*/
#endif

  delay(1000);
  buttonState = digitalRead(buttonPin);
  // Serial.println(buttonState);


  if (buttonState == LOW) {
    getNumberOfLocos();
    //    do {  // routine to stay here till button released & not toggle direction
    //      buttonState = digitalRead(buttonPin);
    //    }      while (buttonState == LOW);
  }


#ifdef I2CLCD
  lcd.clear();
  lcd.print("DCC++ Throttle");
  lcd.setCursor(0, 1);
  lcd.print("6-11-16 v");
  for (int i = 0; i < 4; i++) {
    lcd.print(VersionNum[i]);
  }
#endif

#ifdef SERLCD
// clear lcd
  lcd.write(254);
  lcd.write(1);
  lcd.print("DCC++ Throttle");
  lcd.write(254);
  lcd.write(line2);
  lcd.print("10-10-22 v");
  lcd.print(VersionNum);
#endif

  Serial.print("10-10-2022  version ");
  for (int i = 0; i < 4; i++) {
    Serial.print(VersionNum[i]);
  }
  if (debug == 1) Serial.println("");
  Serial.print("<0>");// power off to DCC++ unit
  delay(1500);
  pinMode(ledPin, OUTPUT);              // Sets the digital pin as output.
  digitalWrite(ledPin, HIGH);           // Turn the LED on.
  ledPin_state = digitalRead(ledPin);   // Store initial LED state. HIGH when LED is on.
  keypad.addEventListener(keypadEvent); // Add an event listener for this keypad

#ifdef I2CLCD
  lcd.clear();
#endif

#ifdef SERLCD
// clear lcd
  lcd.write(254);
  lcd.write(1);
#endif
  
  doMainLCD();
}  // END SETUP

void getNumberOfLocos() {
  //maxLocos = 4;// number of loco addresses

#ifdef I2CLCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Loco # now = ");
  lcd.print(maxLocos);
  lcd.setCursor(0, 1);
  lcd.print("new # (1-4) ");
#endif

#ifdef SERLCD
  lcd.write(254);
  lcd.write(1);
  lcd.write("Loco # now = ");
  sprintf(tempstring,"%2d",maxLocos);

  lcd.write(tempstring);
  lcd.write(254);
  lcd.write(line2);
  lcd.write("new # (1-4) ");
#endif

  do {
    key = keypad.getKey();
    if (debug == 1) Serial.print("key = ");
    if (debug == 1) Serial.println(key);
    key = key - 48 - 1; // convert from ascii value
    //Serial.println(key);
    if (key == 1 || key == 2 || key == 3 || key == 4) { // 1-4
      maxLocos = key + 1;
      if (debug == 1)    Serial.print("new maxLocos = ");
      if (debug == 1)  Serial.println(maxLocos);

      lcd.print(maxLocos);

      delay(1400);
      saveAddresses();
      saveDescriptions();
      break;
    }
  } while (key != 1 || key != 2 || key != 3 || key != 4);
}


void loop() {
  /*  // thie routine refreshes DCC commands ever "interval" milliseconds
    currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
    // save the last time
    previousMillis = currentMillis;
    doDCC();
    }
  */
  key = keypad.getKey();
  if (key == 42) { // *
    all2ZeroSpeeed();
    getLocoAddress();
    key = 0;
  }
  //NEW ROUTINE USING ROTARY ENCODER FOR SPEED AND BUTTON ON IT FOR DIRECTION
  getEncoder();
  if (encoderChange == 1) {
    encoderChange = 0;
    LocoSpeed[ActiveAddress] = encoderPos;
    doDCC();
    doMainLCD();
  }
  buttonState = digitalRead(buttonPin);
  // Serial.println(buttonState);
  if (buttonState == LOW) {
    delay(50);
    buttonState = digitalRead(buttonPin); // check a 2nd time to be sure
    if (buttonState == LOW) {// check a 2nd time to be sure
      LocoDirection[ActiveAddress] = !LocoDirection[ActiveAddress];
      if (LocoDirection[ActiveAddress] == 0) {
        digitalWrite(ledPin, LOW);
      }
      else digitalWrite(ledPin, HIGH);
      doDCC();
      doMainLCD();
      do {  // routine to stay here till button released & not toggle direction
        buttonState = digitalRead(buttonPin);
      }      while (buttonState == LOW);
    }
  }
  if (key == 35) { // #
    ActiveAddress++;
    if (ActiveAddress >= maxLocos) ActiveAddress = 0;
    doMainLCD();
    delay(200);
    key = 0;
    encoderPos = LocoSpeed[ActiveAddress];
    doDCC();
  }
  if (key != 42 && key != 35 && key >= 40) {
    doFunction();
  }
}  //END LOOP

void getEncoder() {
  if (oldEncPos != encoderPos) {
    encoderPos = constrain(encoderPos, 0, 126);
    oldEncPos = encoderPos;
    encoderChange = 1; // flag to show a change took place
  }
}//END GET ENCODER ROUTINE

//START DO FUNCTION BUTTONS
int doFunction() {
  key = key - 48 - 1; // convert from ascii value

#ifdef I2CLCD
  lcd.setCursor (14, 1);       // go to end of 2nd line
#endif

#ifdef SERLCD
  lcd.write(254);
  lcd.write(line2 + 14);
#endif

  ///  lcd.print("FN code ");
  lcd.print(key, DEC + 1);

  if (debug == 1) Serial.print("got a keypad button ");
  if (debug == 1) Serial.println(key, DEC);
  if (key <= 4) {
    if (bitRead(LocoFN0to4[ActiveAddress], key) == 0 ) {
      bitWrite(LocoFN0to4[ActiveAddress], key, 1);
    }
    else {
      if (bitRead(LocoFN0to4[ActiveAddress], key) == 1 ) {
        bitWrite(LocoFN0to4[ActiveAddress], key, 0);
      }
    }
    doDCCfunction04();
    Serial.print(LocoFN0to4[ActiveAddress], BIN);
    if (debug == 1) Serial.println(" LocoFN0to4[ActiveAddress] d ");
    if (debug == 1) Serial.print(LocoFN0to4[ActiveAddress], DEC);
    if (debug == 1) Serial.println(" LocoFN0to4[ActiveAddress]");
  }
  if (key >= 5 && key <= 8) {
    key = key - 5;
    if (bitRead(LocoFN5to8[ActiveAddress], key) == 0 ) {
      bitWrite(LocoFN5to8[ActiveAddress], key, 1);
    }
    else {
      if (bitRead(LocoFN5to8[ActiveAddress], key) == 1 ) {
        bitWrite(LocoFN5to8[ActiveAddress], key, 0);
      }
    }
    doDCCfunction58();
    if (debug == 1) Serial.print(LocoFN5to8[ActiveAddress], BIN);
    if (debug == 1) Serial.println(" LocoFN5to8[ActiveAddress] d ");
    if (debug == 1) Serial.print(LocoFN5to8[ActiveAddress], DEC);
    if (debug == 1) Serial.println(" LocoFN5to8[ActiveAddress]");
  }
  if (key == -1)
  {
#ifdef I2CLCD
    lcd.setCursor (14, 1);       // go to end of 2nd line
#endif

#ifdef SERLCD
  lcd.write(254);
  lcd.write(line2 + 14);
#endif

    ///    lcd.print("FN code ");
    lcd.print(key, DEC);

    key = 0;
    LocoFN0to4[ActiveAddress] = B00000000; //clear variables for which functions are set
    LocoFN5to8[ActiveAddress] = B00000000;
    doDCCfunction04();
    doDCCfunction58();
    delay(500);
    key = 0;
  }
  key = 0;
  // delay(500);
  doMainLCD();
}//END DO FUNCTION BUTTONS

void showFirstLine() {
  // break;
  if (debug == 1) Serial.println(" ");

#ifdef I2CLCD
  lcd.setCursor(0, 0);
  lcd.print("                ");// clear
  lcd.setCursor(0, 0);
#endif

#ifdef SERLCD
  lcd.write(254);
  lcd.write(line1);
  lcd.print("                ");// clear
  lcd.write(254);
  lcd.write(line1);
#endif

  lcd.print("L");
  lcd.print(ActiveAddress + 1);
  lcd.print(" = ");
  lcd.print(LocoAddress[ActiveAddress]);


  for (int zzz = 0; zzz <= 3; zzz++) {
    if (debug == 1) Serial.print("add # ");
    if (debug == 1) Serial.print(zzz + 1);
    if (debug == 1) Serial.print(" ");
    if (debug == 1) Serial.println(LocoAddress[zzz]);
  }
}

void getLocoAddress() {
  int saveAddress =   LocoAddress[ActiveAddress];
  Serial.print("<0>");// power off to DCC++ unit
  int total = 0;
  counter = 0;

#ifdef I2CLCD
  lcd.clear();
  lcd.setCursor(0, 0);
#endif

#ifdef SERLCD
  lcd.write(254);
  lcd.write(1);
#endif

  lcd.print("Set Dcc Addr # ");
  lcd.print(ActiveAddress + 1);

  if (debug == 1) Serial.println(" @ top");
  do {
    //  TrigVal2 = digitalRead(Trigger2);   // read the input pin
    //  if (TrigVal2 == 0) break;
    key = keypad.getKey();
    if (key == '#' | key == '*') { //abort when either is hit
      //LocoAddress[ActiveAddress] = saveAddress;
      total = saveAddress;
      break;// exit routine if # button pressed - ABORT new address
    }
    if (key) {
      counter++;
      int number =  key - 48;
      total = total * 10 + number;
      if (debug == 1) Serial.print("TOTAL = ");
      if (debug == 1) Serial.println(total);

#ifdef I2CLCD
      lcd.setCursor(0, 1);
#endif

#ifdef SERLCD
      lcd.write(254);
      lcd.write(line1);
#endif

      if (total == 0) {   // print multiple zeros for leading zero number
        for (int tempx = 1; tempx <= counter; tempx++) {
          lcd.print("0");
        }
      }
      else {
        lcd.print(total);
      }
      if (debug == 1) Serial.print("Counter = ");
      if (debug == 1) Serial.print(counter);
      if (debug == 1) Serial.print("  key = ");
      if (debug == 1) Serial.print(key);
      if (debug == 1) Serial.print("   val = ");
      if (debug == 1) Serial.println(number);
    }
  }
  while (counter <= 3); //  collect exactly 4 digits
  LocoAddress[ActiveAddress] = total;
  saveAddresses();
#ifdef I2CLCD
  lcd.clear();
#endif
#ifdef SERLCD
  lcd.write(254);
  lcd.write(1);
#endif

  doMainLCD();
}

// Taking care of some special events.
void keypadEvent(KeypadEvent key) {
  switch (keypad.getState()) {
    case PRESSED:
      if (key == '#') {
        digitalWrite(ledPin, !digitalRead(ledPin));
        ledPin_state = digitalRead(ledPin);        // Remember LED state, lit or unlit.
      }
      break;
    case RELEASED:
      if (key == '*') {
        digitalWrite(ledPin, ledPin_state);   // Restore LED state from before it started blinking.
        blink = false;
      }
      break;
    case HOLD:
      if (key == '*') {
        blink = true;    // Blink the LED when holding the * key.
      }
      break;
  }
}

void doDCC() {
  if (debug == 1) Serial.println(LocoDirection[ActiveAddress] );
  Serial.print("<1>");
  Serial.print("<t1 ");
  Serial.print(LocoAddress[ActiveAddress] );//locoID);
  Serial.print(" ");
  Serial.print(LocoSpeed[ActiveAddress] );
  Serial.print(" ");
  Serial.print(LocoDirection[ActiveAddress] );
  Serial.println(">");
}

void doDCCfunction04() {
  Serial.write("<f ");
  Serial.print(LocoAddress[ActiveAddress] );
  Serial.print(" ");
  int fx = LocoFN0to4[ActiveAddress] + 128;
  Serial.print(fx);
  Serial.print(" >");
}

void doDCCfunction58() {
  Serial.write("<f ");
  Serial.print(LocoAddress[ActiveAddress] );
  Serial.print(" ");
  int fx = LocoFN5to8[ActiveAddress] + 176;
  Serial.print(fx);
  Serial.print(" >");
}

void all2ZeroSpeeed() {  // set flag to 1 to stop, set to 0 to restore
  for (int tempx = 0; tempx <= maxLocos; tempx++) {
    Serial.print("<t1 ");
    Serial.print(LocoAddress[tempx] );//locoID);
    Serial.print(" ");
    if (ZeroSpeedFlag == 1) {
      Serial.print(0);//LocoSpeed[0] );
    }
    else Serial.print(LocoSpeed[0] );
    Serial.print(" ");
    Serial.print(LocoDirection[1] );
    Serial.write(">");
  }
}





void PinA() {
  cli(); //stop interrupts happening before we read pin values
  reading = PIND & 0xC; // read all eight pin values then strip away all but pinA and pinB's values
  if (reading == B00001100 && aFlag) { //check that we have both pins at detent (HIGH) and that we are expecting detent on this pin's rising edge
    if (encoderPos != 0)encoderPos --; //decrement the encoder's position count
    bFlag = 0; //reset flags for the next turn
    aFlag = 0; //reset flags for the next turn
  }
  else if (reading == B00000100) bFlag = 1; //signal that we're expecting pinB to signal the transition to detent from free rotation
  sei(); //restart interrupts
}

void PinB() {
  cli(); //stop interrupts happening before we read pin values
  reading = PIND & 0xC; //read all eight pin values then strip away all but pinA and pinB's values
  if (reading == B00001100 && bFlag) { //check that we have both pins at detent (HIGH) and that we are expecting detent on this pin's rising edge
    encoderPos ++; //increment the encoder's position count
    bFlag = 0; //reset flags for the next turn
    aFlag = 0; //reset flags for the next turn
  }
  else if (reading == B00001000) aFlag = 1; //signal that we're expecting pinA to signal the transition to detent from free rotation
  sei(); //restart interrupts
}
