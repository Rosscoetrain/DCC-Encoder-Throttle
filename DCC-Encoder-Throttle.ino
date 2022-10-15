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

#ifdef FIRSTRUN
  saveAddresses();
  saveDescriptions();
#endif

  pinMode(ledPin, OUTPUT);
  pinMode(pinA, INPUT_PULLUP); // set pinA as an input, pulled HIGH to the logic voltage (5V or 3.3V for most cases)
  pinMode(pinB, INPUT_PULLUP); // set pinB as an input, pulled HIGH to the logic voltage (5V or 3.3V for most cases)
  pinMode(buttonPin, INPUT);
  attachInterrupt(0, PinA, RISING); // set an interrupt on PinA, looking for a rising edge signal and executing the "PinA" Interrupt Service Routine (below)
  attachInterrupt(1, PinB, RISING); // set an interrupt on PinB, looking for a rising edge signal and executing the "PinB" Interrupt Service Routine (below)
  pinMode(LED, OUTPUT);


  getAddresses();      // read eeprom
  getDescriptions();   // read descriptions from eeprom

#ifdef I2CLCD
  lcd.begin(Wire); //Set up the LCD for I2C communication

  lcd.setBacklight(255, 255, 255);           //Set backlight to bright white
  lcd.setContrast(5);                        //Set contrast. Lower to 0 for higher contrast.

  lcd.clear();                               //Clear the display - this moves the cursor to home position as well

  lcd.setCursor(0, 0);
  lcd.print("Hold button to");
  lcd.setCursor(0, 1);
  lcd.print("vary MaxLocos ");
  lcd.print(maxLocos);

#endif

#ifdef SERLCD
  lcd.begin(9600);                           // initialize at 9600 baud
  setBacklight();
  lcd.write(254);                            //Clear the display - this moves the cursor to home position as well
  lcd.write(1);

  lcd.print("Hold button to");
  lcd.write(254);
  lcd.write(line2);

  lcd.print("vary MaxLocos ");
  lcd.print(maxLocos);
#endif

  delay(1000);
  buttonState = digitalRead(buttonPin);
  // Serial.println(buttonState);


  if (buttonState == LOW) {
    getNumberOfLocos();

//    getLocoRoster();

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
//  Serial.print("<0>");// power off to DCC++ unit
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
  lcd.print("new # (1-6) ");
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
