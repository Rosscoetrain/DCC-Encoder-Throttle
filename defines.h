/*
 * compiler defines
 *
 */

/*
 * uncomment the define for the type of lcd you are using
 * 
 * SERLCD = serial lcd eg https://www.sparkfun.com/products/9395
 * I2CLCD = i2c lcd eg https://www.sparkfun.com/products/16398
 *
 * LCD1602 = 16 x 2 character lcd eg https://www.sparkfun.com/products/16396
 * LCD1604 = 16 x 4 character lcd eg 
 * LCD2002 = 20 x 2 character lcd eg https://www.digikey.com.au/en/products/detail/orient-display/AMC2002CR-B-Y6WFDY-I2C/12089321
 * LCD2004 = 20 x 4 character lcd eg https://www.sparkfun.com/products/9568
 *
 * KEY3X4 = 3 x 4 keypad eg  
 * KEY4x4 = 4 x 4 keypad eg 
 * KEY1x4 = 1 x 4 keypad eg
 *
 */

/*
 * this limits the maximum number of locomotives
 * 
 * original had a maximum of 4 locos
 * 
 * EEPROM space allows upto 20 locos
 * 
 */

#define MAXLOCOS 4


//#define I2CLCD
#define SERLCD

#define LCD1602
//#define LCD1604
//#define LCD2002
//#define LCD2004

#define KEY1x4
//#define KEY3x4
//#define KEY4x4

#if not defined LCD1602 && not defined LCD1604 && not defined LCD2002 && not defined LCD2004
#define LCD1602
#endif
