/*
 * variable declarations
 */

const int ledPin =  13;      // the number of the LED pin
int buttonPin = 4; // button on rotary
static int pinA = 2; // Rotary Encoder
static int pinB = 3; // Rotary Encoder
volatile byte aFlag = 0;
volatile byte bFlag = 0;
volatile byte encoderPos = 0;
volatile byte oldEncPos = 0;
volatile byte reading = 0;
int old_pos = encoderPos;
int dir = 0; // direction
int buttonState = 0;
int encoderChange = 0; // flag to show encoder changed
byte Key;

char key ;
int LED = 13; // LED to blink when DCC packets are sent in loop
byte Fx = 0;
// Array set for 4 Loco2 - change the 7 numbers in the next 7 declarations
int maxLocos = 4;// number of loco addresses
//int LocoAddress[4] = {1830, 3, 999, 4444};
int LocoAddress[4] = {3, 5, 13, 14};
int LocoDirection[4] = {1, 1, 1, 1};
int LocoSpeed[4] = {0, 0, 0, 0};
byte LocoFN0to4[4];
byte LocoFN5to8[4];
byte Locofn9to12[4];// 9-12 not yet implemented
int xxxxx = 0;
int pot1Pin = A3;
int potValue = 0;
int NewPotValue = 0;
int potValueOld = 0;
int counter = 0;
int Trigger1 = 3;
int Trigger2 = 4;
//int TrigVal1 = 0;
//int TrigVal2 = 0;
int old_speed = 0;
int ZeroSpeedFlag = 0;
int ActiveAddress = 0; // make address1 active

char tempstring[10];

int z = 0;
int powerTemp = 0;
int i = 0;
char VersionNum[] = "3.0a"; ///////////////////////// //////////////////////VERSION HERE///////

/*
 * keypad variables
 */
#ifdef KEY3x4
const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
char keys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};
byte rowPins[ROWS] = {5, 6, 7, 8 };   //{8,7,6,5 };   //connect to the row pinouts of the keypad
byte colPins[COLS] = {9, 10, 11};     // {11,10,9};   //connect to the column pinouts of the keypad
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
#endif

/*
 *  4 x 1 keypad is wired as
 *  1 = common
 *  2 = switch 2
 *  3 = switch 1
 *  4 = switch 4
 *  5 = switch 3
 *  
 *  
 */

#ifdef KEY1x4
const byte ROWS = 4; //four rows
const byte COLS = 1; //one column
char keys[ROWS][COLS] = {
  {'#'},
  {'1'},
  {'2'},
  {'3'}
};
byte rowPins[ROWS] = {5, 6, 7, 8 };   //{8,7,6,5 };  //connect to the row pinouts of the keypad
byte colPins[COLS] = {10};             // {9};       //connect to the column pinout of the keypad
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
#endif


//byte ledPin = 13;
boolean blink = false;
boolean ledPin_state;
unsigned long previousMillis = 0;        // will store last time LED was updated
unsigned long currentMillis = millis();
const long interval = 300;

int debug = 0; // set to 1 to show debug info on serial port - may cause issues with DCC++ depending on what is sent

#ifdef SERLCD
// Attach the serial enabled LCD's RX line to digital pin 12
SoftwareSerial lcd(13, 12);          // Arduino SS_RX = pin 13 (unused), Arduino SS_TX = pin 12 
#endif

#ifdef I2CLCD
SerLCD lcd; // Initialize the library with default I2C address 0x72
#endif
