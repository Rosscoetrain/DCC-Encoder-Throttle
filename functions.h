/*
 * functions
 */


/*
 * update the screen
 */

void doMainLCD() {

String temp = "";


#ifdef I2CLCD
  lcd.setCursor(0, 0);      // start of 1st line
  lcd.print("S=");
  lcd.print(LocoSpeed[ActiveAddress], DEC);
  lcd.print(" ");
  lcd.setCursor(5 , 0);
  if (LocoDirection[ActiveAddress] == 1 ) {
    lcd.print(" > ");
  }
  else {
    lcd.print(" < ");
  }
  lcd.setCursor(8, 0);
  lcd.print("L=");
  if (LocoAddress[ActiveAddress] <= 9) {
    lcd.print("0");  // add leading zero for single digit addresses
  }
  lcd.print(LocoAddress[ActiveAddress] , DEC);
  lcd.print("   ");
  lcd.setCursor(14, 0);
  lcd.print("#");
  lcd.setCursor (15, 0);       // go to end of 1st line
  lcd.print(ActiveAddress + 1);

#if defined LCD1602 || defined LCD2002
  lcd.setCursor(0, 1);    // start of 2nd line

  if (debug == 2) {
    Serial.println();
    Serial.println(LocoDefs[ActiveAddress]);
  }
  lcd.print(LocoDefs[ActiveAddress]);
  delay(500);

  lcd.setCursor(0, 1);    // start of 2nd line
  lcd.print("                ");
#endif

  lcd.setCursor(5, 1); // start of 2nd line + 5
  temp = "0000" + String(LocoFN0to4[ActiveAddress], BIN);  // pad with leading zeros
  int tlen = temp.length() - 5;
  lcd.print(temp.substring(tlen));

  lcd.setCursor(0, 1);    // start of 2nd line
  temp = "000" + String(LocoFN5to8[ActiveAddress], BIN);
  tlen = temp.length() - 4;
  lcd.print(temp.substring(tlen));
#endif



#ifdef SERLCD
  lcd.write(254);      // start of 1st line
  lcd.write(1);
  lcd.write("S=");
  sprintf(tempstring,"%2d", LocoSpeed[ActiveAddress]);
  lcd.write(tempstring);
  lcd.write(" ");
  lcd.write(254);
  lcd.write(line1 + 5);
  if (LocoDirection[ActiveAddress] == 1 ) {
    lcd.print(" > ");
  }
  else {
    lcd.print(" < ");
  }
  lcd.write(254);
  lcd.write(line1 + 8);
  lcd.print("L=");

  sprintf(tempstring,"%02d",LocoAddress[ActiveAddress]);
  lcd.write(tempstring);
  lcd.write("    ");
  lcd.write(254);
  lcd.write(line1 + 13);
  lcd.write("#");
  lcd.write(254);
  lcd.write(line1 + 14);
  sprintf(tempstring,"%2d",ActiveAddress + 1);
  lcd.print(tempstring);

#if defined LCD1602 || defined LCD2002
  lcd.write(254);
  lcd.write(line2);

  if (debug == 2) {
    Serial.println();
    Serial.println(LocoDefs[ActiveAddress]);
  }
  lcd.print(LocoDefs[ActiveAddress]);
  delay(500);

  lcd.write(254);
  lcd.write(line2);
  lcd.print("                ");
#endif

#if defined LCD1604 || defined LCD2004
  Serial.println("4 lines");
  lcd.write(254);
  lcd.write(line4);

  if (debug == 2) {
    Serial.println();
    Serial.println(LocoDefs[ActiveAddress]);
  }
  lcd.print(LocoDefs[ActiveAddress]);
#endif

  lcd.write(254);
  lcd.write(line2 + 5);   // 5th character on second line
  temp = "0000" + String(LocoFN0to4[ActiveAddress], BIN);  // pad with leading zeros
  int tlen = temp.length() - 5;
  lcd.print(temp.substring(tlen));

  lcd.write(254);         // start of 2nd line
  lcd.write(line2);

  temp = "000" + String(LocoFN5to8[ActiveAddress], BIN);
  tlen = temp.length() - 4;
  lcd.print(temp.substring(tlen));
#endif

}


/*
 * get loco addresses from eeprom
 */

void getAddresses() {
  int xxx = 0;
  for (int xyz = 0; xyz <= maxLocos - 1; xyz++) {
    LocoAddress[xyz] = EEPROM.read(eepromAddressLocoAddress + xyz * 2) * 256;
    LocoAddress[xyz] = LocoAddress[xyz] + EEPROM.read(eepromAddressLocoAddress + xyz * 2 + 1);
    if (LocoAddress[xyz] >= 10000) LocoAddress[xyz] = 3;
    if (debug == 1) Serial.println(" ");
    if (debug == 1) Serial.print("loco = ");
    if (debug == 1) Serial.print(LocoAddress[xyz]);
    if (debug == 1) Serial.print("  address# = ");
    if (debug == 1) Serial.print(xyz + 1);
  }
  if (debug == 1) Serial.println(" ");
  maxLocos = EEPROM.read(eepromAddressLocoMaxLocos);
  if (debug == 1) Serial.print("EEPROM maxLocos = ");
  if (debug == 1) Serial.println(maxLocos);
  if (maxLocos >= MAXLOCOS) maxLocos = MAXLOCOS;
}

/*
 * save loco addresses to eeprom
 */

void saveAddresses() {
  int xxx = 0;
  for (int xyz = 0; xyz <= maxLocos - 1; xyz++) {
    xxx = LocoAddress[xyz] / 256;
    if (debug == 1) Serial.println("saveAddresses");
    if (debug == 1) Serial.println(" ");
    if (debug == 1) Serial.print("loco = ");
    if (debug == 1) Serial.print(LocoAddress[xyz]);
    if (debug == 1) Serial.print("  address# = ");
    if (debug == 1) Serial.print(xyz);
    if (debug == 1) Serial.print(" msb ");
    if (debug == 1) Serial.print(xxx);
    if (debug == 1) Serial.print(" writing to ");
    if (debug == 1) Serial.print(xyz * 2);
    if (debug == 1) Serial.print(" and ");
    if (debug == 1) Serial.print(xyz * 2 + 1);
    EEPROM.write(eepromAddressLocoAddress + xyz * 2, xxx);
    xxx = LocoAddress[xyz] - (xxx * 256);
    if (debug == 1) Serial.print(" lsb ");
    if (debug == 1) Serial.print(xxx);
    EEPROM.write(eepromAddressLocoAddress + xyz * 2 + 1, xxx);
  }
  EEPROM.write(eepromAddressLocoMaxLocos, maxLocos);
}


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
//  Serial.print("<1>");
  Serial.print("<t1 ");
  Serial.print(LocoAddress[ActiveAddress] );//locoID);
  Serial.print(" ");
  Serial.print(LocoSpeed[ActiveAddress] );
  Serial.print(" ");
  Serial.print(LocoDirection[ActiveAddress] );
  Serial.println(">");
}

/*
 * modified to use <F cab function on/off> 15 Oct 2022
 * <f ...> is no longer recommended to be used
 */

void doDCCfunction04(byte key) {
  Serial.print("<F ");
  Serial.print(LocoAddress[ActiveAddress] );
  Serial.print(" ");
  Serial.print(key);
  Serial.print(" ");
  int fx = bitRead(LocoFN0to4[ActiveAddress], key);
  Serial.print(fx);
  Serial.print(">");
}

void doDCCfunction58(byte key) {
  Serial.print("<F ");
  Serial.print(LocoAddress[ActiveAddress] );
  Serial.print(" ");
  Serial.print(key);
  Serial.print(" ");
  int fx = bitRead(LocoFN5to8[ActiveAddress], key);
  Serial.print(fx);
  Serial.print(">");
/*
  Serial.write("<f ");
  Serial.print(LocoAddress[ActiveAddress] );
  Serial.print(" ");
  int fx = LocoFN5to8[ActiveAddress] + 176;
  Serial.print(fx);
  Serial.print(" >");
*/
}

/*
 * end of mods 15 Oct 2022
 */

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
    doDCCfunction04(key);
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
    doDCCfunction58(key);
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
    doDCCfunction04(key);
    doDCCfunction58(key);
    delay(500);
    key = 0;
  }
  key = 0;
  // delay(500);
  doMainLCD();
}//END DO FUNCTION BUTTONS




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



/*
 * These are added functions from the original by d. bodnar
 * 
 */


/*
 * save the loco descriptions to EEPROM
 */

void saveDescriptions() {
//  int addressStart = 100;
  Serial.println("Save Descriptions");
  for (int xyz = 0; xyz <= maxLocos - 1; xyz++) {
    EEPROM.put (eepromAddressLocoDescriptions + 20 * xyz, LocoDefs[xyz]);
    if (debug == 2) {
      Serial.print(xyz + 1);
      Serial.println();
      Serial.print(LocoDefs[xyz]);
      Serial.println();
    }
  }
}

/*
 * get loco descriptions from EEPROM
 */
void getDescriptions() {
//  int addressStart = 100;
  if (debug == 1) {
    Serial.println();
    Serial.println("Get descriptions");
  }
  for (int xyz = 0; xyz <= maxLocos - 1; xyz++) {
    EEPROM.get (eepromAddressLocoDescriptions + 20 * xyz, LocoDefs[xyz]);
    if (debug == 2) {
      Serial.println();
      Serial.print(LocoDefs[xyz]);
      Serial.println();
    }
  }
}



/*
 * set backlight brightness
 * 
 * minimum 128
 * maximum 157
 * 
 */

void setBacklight(byte step=0) {

/* 

  if ( (backLightBrightness + step > 157) || (backLightBrightness + step < 128) ) {
   step = 0;
  }
  backLightBrightness += step;
*/
  backLightBrightness += step;

  if (backLightBrightness > 157) {
    backLightBrightness = 128;
  }

  if (debug == 1) {
    Serial.print("backLightBrightness = ");
    Serial.println(backLightBrightness);
  }
  lcd.write(124);
  lcd.write(backLightBrightness);
  delay(1000);

}


/*
 * Get loco roster from DCC-EX Command Station
 * 
 * Need to send <JR> to get all ids returns <jR id1 id2 id3 ...>
 * Then send <JR id> to get the details for each loco returns <jR id "description" "function1/function2/function3/...">
 */

 void getLocoRoster() {

  int id[20];
  String str = "";
  int StringCount = -1;

#ifdef I2CLCD
  lcd.clear();
#endif
#ifdef SERLCD
  lcd.write(254);
  lcd.write(1);
#endif

  lcd.print("Waiting for roster");

  Serial.setTimeout(1000);
  Serial.print("<JR>");


  String tempstr = Serial.readString();       //read until timeout
  tempstr.trim();

#ifdef I2CLCD
  lcd.clear();
#endif
#ifdef SERLCD
  lcd.write(254);
  lcd.write(1);
#endif


  lcd.print(tempstr);

  delay(1000);

    // Split the string into substrings
  while (tempstr.length() > 0)
  {
    int index = tempstr.indexOf(' ');
    if (index == -1) // No space found
    {
//      strs[StringCount++] = tempstr;
      break;
    }
    else
    {
      str = tempstr.substring(0, index);
      if (str.indexOf('<jr') == -1) {
        id[StringCount++] = str.toInt();
      }
//      strs[StringCount++] = str.substring(0, index);
      tempstr = tempstr.substring(index+1);
    }
  }

#ifdef I2CLCD
  lcd.clear();
#endif
#ifdef SERLCD
  lcd.write(254);
  lcd.write(1);
#endif

   for (i = 0; i < StringCount; i++) {
    lcd.print(id[i]);
    lcd.print(" - ");
   }

  delay(5000);


 }
