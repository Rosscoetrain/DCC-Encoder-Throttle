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
  Serial.println();
  Serial.println("Get descriptions");
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
 */

void setBacklight(byte step=0) {

 lcd.write(124);
 lcd.write(backLightBrightness + step);

}
