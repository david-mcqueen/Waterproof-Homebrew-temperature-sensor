/*
  LiquidCrystal Library - Hello World
 
 Demonstrates the use a 16x2 LCD display.  The LiquidCrystal
 library works with all LCD displays that are compatible with the 
 Hitachi HD44780 driver. There are many of them out there, and you
 can usually tell them by the 16-pin interface.
 
 This sketch prints "Hello World!" to the LCD
 and shows the time.
 
  The circuit:
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)
 
 Library originally added 18 Apr 2008
 by David A. Mellis
 library modified 5 Jul 2009
 by Limor Fried (http://www.ladyada.net)
 example added 9 Jul 2009
 by Tom Igoe
 modified 22 Nov 2010
 by Tom Igoe
 
 This example code is in the public domain.

 http://www.arduino.cc/en/Tutorial/LiquidCrystal
 */

// include the library code:
#include <LiquidCrystal.h>
#include "Wire.h"
#define DS1307_ADDRESS 0x68

#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 7 /*-(Connect to Pin 7 )-*/

OneWire ourWire(ONE_WIRE_BUS);

/* Tell Dallas Temperature Library to use oneWire Library */
DallasTemperature sensors(&ourWire);

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
int temperaturePin = 0;
int weekDay = 0;
int monthDay = 0;
int month = 0;
int year = 0;
int hour =0;
int minute = 0;
int second = 0;

int oldMinute = 0;
int updateCount = -1;
int averageTemp = 0;
int averageTempHalfHour = 0;
int computedTemp = 0;
double tempC;
double tempF;

int relayPin = 13;


void setup() {
  // set up the LCD's number of columns and rows: 
  Wire.begin();
  lcd.begin(16, 4);
  
  pinMode(relayPin, OUTPUT);
  relayOn(false);
  
  Serial.begin(9600);
  Serial.println("YourDuino.com: Electronic Brick Test Program");
  Serial.println("Temperature Sensor DS18B20");
  delay(1000);
  sensors.begin();
}

void loop() {
  getDate();
  
  Serial.println();
  Serial.print("Requesting temperature...");
  sensors.requestTemperatures(); // Send the command to get temperatures
  Serial.println("DONE");
  
  tempC = sensors.getTempCByIndex(0);
  tempF = sensors.getTempFByIndex(0);

  Serial.print("Device 1 (index 0) = ");
  Serial.print(tempC);
  Serial.println(" Degrees C");
  Serial.print("Device 1 (index 0) = ");
  Serial.print(tempF);
  Serial.println(" Degrees F");
  
   // set the cursor to column 0, line 0
   // (note: line 1 is the first row, since counting begins with 0):
   //Print the date on the top row
  lcd.setCursor(0,0);
  lcd.print("Date: ");
  lcd.print(monthDay);
  lcd.print("/");
  lcd.print(month);
  lcd.print("/");
  lcd.print(year);
  
  //Print the time on the second row
  lcd.setCursor(0,1);
  lcd.print("Time: ");
  
  if(hour < 10){
    if(hour == 0){
      lcd.print("00");
    }else{
      lcd.print("0" + hour);
    }
  }else{
    lcd.print(hour);
  }
  
  lcd.print(":");
  
  if (minute < 10){ //Append a 0 on the GUI
    if(minute == 0){
      lcd.print("00");
    }else{
      lcd.print("0" + minute);
    }
  }else{
    lcd.print(minute);  
  }
  
  //Print the temp on the 3rd line
  lcd.setCursor(0, 2);
  lcd.print("Temp (c): ");
  lcd.print(tempC);
  
//If temp is less than 20 turn it on. When it reaches 26 turn it off
  if(tempC <= 20){
    relayOn(true);
  }else if(tempC >= 26){
    relayOn(false);
  }
    
  //get the temp from the small (old style) thermometer
  float temperature = getVoltage(temperaturePin); 
  averageTemp += (temperature - .5) * 100;
  delay(1000);
}

void relayOn(boolean toggle){
  lcd.setCursor(0, 3);
  lcd.print("Relay On: ");
  if (toggle){
    lcd.print("N-O");
    digitalWrite(relayPin, LOW); //NO has power
  }else{
    lcd.print("N-C");
    digitalWrite(relayPin, HIGH); //NC has power
  }
}

//Used for the Analogue Thermometer
float getVoltage(int pin){
  return (analogRead(pin) * .004882814);
}


//Used for the RTC
byte bcdToDec(byte val)  {
// Convert binary coded decimal to normal decimal numbers
  return ( (val/16*10) + (val%16) );
}

void getDate(){

  // Reset the register pointer
  Wire.beginTransmission(DS1307_ADDRESS);

  byte zero = 0x00;
  Wire.write(zero);
  Wire.endTransmission();

  Wire.requestFrom(DS1307_ADDRESS, 7);

  second = bcdToDec(Wire.read());
  minute = bcdToDec(Wire.read());
  hour = bcdToDec(Wire.read() & 0b111111); //24 hour time
  weekDay = bcdToDec(Wire.read()); //0-6 -> sunday - Saturday
  monthDay = bcdToDec(Wire.read());
  month = bcdToDec(Wire.read());
  year = bcdToDec(Wire.read());

  //print the date EG   3/1/11 23:59:59
  Serial.print(monthDay);
  Serial.print("/");
  Serial.print(month);
  Serial.print("/");
  Serial.print(year);
  Serial.print(" ");
  Serial.print(hour);
  Serial.print(":");
  Serial.print(minute);
  Serial.print(":");
  Serial.println(second);

}
