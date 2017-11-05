/* 
 *  Based in the Serial 7-Segment Display Example Code from SparkFun Electronics by: Jim Lindblom
 *  Code: Oscar Gonzalez - BricoGeek.com
*/
#include <Wire.h> // Include the Arduino SPI library

#define DISPLAY_ADDRESS 0x71
#define BUTTON_PIN      10
#define BUZZER_PIN      9

char tempString[10];  // Will be used with sprintf to create strings
int minutes = 25;
int seconds = 0;

bool pomodoroStarted = false;

unsigned int startTime=0;

void updateTimer()
{

  if (pomodoroStarted == false) { return; }
  
  seconds--;
  if (seconds < 0) { 
    minutes--;
    seconds = 59;
  }
}

void setup()
{
  Wire.begin();  // Initialize hardware I2C pins
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);  
  
  // Clear the display, and then turn on all segments and decimals
  clearDisplayI2C();  // Clears display, resets cursor

  // Custom function to send four bytes via I2C
  //  The I2C.write function only allows sending of a single
  //  byte at a time.
  s7sSendStringI2C("8888");
  //setDecimalsI2C(0b111111);  // Turn on all decimals, colon, apos

  // Flash brightness values at the beginning
  /*
  setBrightnessI2C(0);  // Lowest brightness
  delay(1500);
  setBrightnessI2C(255);  // High brightness
  delay(1500);
  */
  
  delay(1000);
  clearDisplayI2C();  // Clears display, resets cursor
  startTime = millis();
}

void loop()
{

  if (digitalRead(BUTTON_PIN) == LOW)
  {
    delay(50);
    if (pomodoroStarted == false) { pomodoroStarted = true; tone(BUZZER_PIN,1500, 50); }
    else {pomodoroStarted = false;  }
  }

  if ( (millis()-startTime) > 1000)
  {
    startTime = millis();
    updateTimer();

    // Magical sprintf creates a string for us to send to the s7s.
    //  The %4d option creates a 4-digit integer.
    sprintf(tempString, "%02d%02d", minutes, seconds);
  
    // This will output the tempString to the S7S
    s7sSendStringI2C(tempString);
    
  }
  
}

// This custom function works somewhat like a serial.print.
//  You can send it an array of chars (string) and it'll print
//  the first 4 characters in the array.
void s7sSendStringI2C(String toSend)
{
  Wire.beginTransmission(DISPLAY_ADDRESS);
  for (int i=0; i<4; i++)
  {
    Wire.write(toSend[i]);
  }
  Wire.endTransmission();
}

// Send the clear display command (0x76)
//  This will clear the display and reset the cursor
void clearDisplayI2C()
{
  Wire.beginTransmission(DISPLAY_ADDRESS);
  Wire.write(0x76);  // Clear display command
  Wire.endTransmission();
}

// Set the displays brightness. Should receive byte with the value
//  to set the brightness to
//  dimmest------------->brightest
//     0--------127--------255
void setBrightnessI2C(byte value)
{
  Wire.beginTransmission(DISPLAY_ADDRESS);
  Wire.write(0x7A);  // Set brightness command byte
  Wire.write(value);  // brightness data byte
  Wire.endTransmission();
}

// Turn on any, none, or all of the decimals.
//  The six lowest bits in the decimals parameter sets a decimal 
//  (or colon, or apostrophe) on or off. A 1 indicates on, 0 off.
//  [MSB] (X)(X)(Apos)(Colon)(Digit 4)(Digit 3)(Digit2)(Digit1)
void setDecimalsI2C(byte decimals)
{
  Wire.beginTransmission(DISPLAY_ADDRESS);
  Wire.write(0x77);
  Wire.write(decimals);
  Wire.endTransmission();
}

