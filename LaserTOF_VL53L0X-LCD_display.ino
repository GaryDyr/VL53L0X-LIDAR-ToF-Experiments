 /* 

Measure distance with VL53L0X using display LCD1602 with attached I2C module
Distance is displayed in mm (melimeter) or cm (centimeter).

Part of this was from code from Robojax.com
Pin connection

VL53L0X Pin  Arduino Pin
VCC         5V
GND         GND
SDA         A4 or SDA pin 27
SCL         A5 or SCL pin 28
GPIO1       not connected
XSHUT       D12 (digital 12 or pin 12)

LCD1602 Pin

*/

//I2C library
#include <Wire.h>
// Several libraries exist, the pololu library seemed most appropriate.
//See https://github.com/pololu/vl53l0x-arduino for API fcns.
#include <VL53L0X.h>

//create VL53L0X object
VL53L0X sensor; 

//Does not use std library LiquidCrystal
//Look at LiquidCrystal arduino page to to get idea of most functions. 
#include <LiquidCrystal_I2C.h>
// Set the LCD address to 0x27 for a 16 chars and 2 line display
//0x27 is typical for LCD i2C
LiquidCrystal_I2C lcd(0x27, 16, 2);

const int type = 1;// 1=mm , 2= cm
String unit;// variable for unit, mm or cm
int distance;
int timing_budget;
int cnt;
boolean setOk;

// Uncomment this line to use long range mode. This
// increases the sensitivity of the sensor and extends its
// potential range, but increases the likelihood of getting
// an inaccurate reading because of reflections from objects
// other than the intended target. It works best in dark
// conditions.

//#define LONG_RANGE


//This code and the setup code for LONG_RANGE or HIGH_ACCURACY ARE FROM
//https://github.com/pololu/vl53l0x-arduino/blob/master/examples/Single/Single.ino
// Uncomment ONE of these two lines to get
// - higher speed at the cost of lower accuracy OR
// - higher accuracy at the cost of lower speed

//#define HIGH_SPEED
//#define HIGH_ACCURACY

void setup()
{
  Serial.begin(9600);
  Wire.begin();
  sensor.init();

  //if (!sensor.init())
  //{
  //  Serial.println("Failed to detect and initialize sensor!");
  //  while (1) {}
  //}

  sensor.setTimeout(500);

//#if defined LONG_RANGE above
  // lower the return signal rate limit (default is 0.25 MCPS)
  //sensor.setSignalRateLimit(0.1);
  // increase laser pulse periods (defaults are 14 and 10 PCLKs)
 // sensor.setVcselPulsePeriod(VL53L0X::VcselPeriodPreRange, 18);
 // sensor.setVcselPulsePeriod(VL53L0X::VcselPeriodFinalRange, 14);
//#endif

//#if defined HIGH_SPEED
  // reduce timing budget to 20 ms (default is about 33 ms)
  //note values are in nanoseconds
  //sensor.setMeasurementTimingBudget(20000);
//#elif defined HIGH_ACCURACY
  // increase timing budget to 200 ms
  //sensor.setMeasurementTimingBudget(200000);
//#endif
//}
  
  //Check sensor time  budget )accuracy and distance0
  timing_budget = sensor.getMeasurementTimingBudget();
  Serial.print("Timing budget is: ");
  Serial.print(timing_budget);
  Serial.println(" us");
  // BE CAREFUL; THESE CHANGES MAY ONLY BE APPLICABBLE TO WHEN USING readRangeSingleMillimeters()
  //if (timinG_budget < 66000) {
  //   setOk = sensor.setMeasurementTimingBudget(99000);
  //   }
  // if setOk != 1){
  //   serial.println("TimingBudget not set right");
  //   }
  //Set a timeout period in milliseconds after which read operations will abort if the sensor 
  //is not ready. A value of 0 disables the timeout.
  sensor.setTimeout(500); 

  // Start continuous back-to-back mode (take readings as
  // fast as possible).  To use continuous timed mode
  // instead, provide a desired inter-measurement period in
  // ms (e.g. sensor.startContinuous(100)).
  sensor.startContinuous();
  //next would be used with a btn or some trigger to take single reading.
  //the 1602 code in loop() would need to be transferred here.
  //readRangeSingleMillimeters()
  
  // initialize the LCD, 
  lcd.begin(16,2);
  // Turn on the blacklight and print a message.
  lcd.backlight(); //turn off with noBlacklight()
  lcd.clear();
  lcd.print("Distance:");
  Serial.print("Distance: ");
  cnt = 0;
  //take 1000 measurements
 for (cnt = 0; cnt <= 1000; cnt++){
   distance = sensor.readRangeContinuousMillimeters(); //must start with starContinuou(), which is already triggered above.
   //int distance = sensor.startContinuous(100); //take data every 100 ms
   //start of loop code for LCD with I2C
   //lcd.clear(); //will clear display and set cursor to upper left (0,0)
   //for 2m need 4 characters. but could be 2-4 characters.
   //lcd.setCursor (0,1); // go to start of 2nd line
   //lcd.print("Dist.: ");
   lcd.setCursor (0,1); // go to data start of 2nd line position 0 of lcd display
   lcd.print("               "); //clear the data area,chr 0-15       
   if(type ==2){
     float distanceCM = (float) (distance/10.0);
     unit ="cm";
     lcd.print(distanceCM);
     Serial.print(distanceCM);
     Serial.print(unit);      
    }
  else{
   //deal with out of range
   // found from early experiments that if no reading the VL53L0X
   // puts out a number 8190. This is way beyond the range of the sensor
   // so is easy to deal with
    if (distance > 8100){
      lcd.setCursor(0,1); // 1st chr; 2nd row
      lcd.print("Out of Range");
      Serial.print("Out of Range");
      }
      else { 
        unit ="mm";
        lcd.setCursor(0,1); //first chr; 2nd row
        lcd.print(distance);
        lcd.print(unit);
        Serial.print(distance);
        //print unit next to number; not used, because 
        //data was to be transferred from Serial Monitor to Excel
        //Serial.print(unit);
      } 
  }  

  if (sensor.timeoutOccurred()) { Serial.print(" TIMEOUT"); }
  Serial.println();
  //Data rate could  be as low as 33 ms or 66 ms for better accuracy.
  //Fount that a delay of 100 made reading the LCD display difficult because
  //10s and units value changed so fast could not read, so changed to 250 ms
  // or 4Hz
  delay(250); 
 } //if cnt
  lcd.setCursor (0,1); // go to data start of 2nd line position 0 
  lcd.print("               "); //clear the data area,chr 0-15 
 }

void loop(){
  Serial.println("Done");
  lcd.setCursor (0,1);
  lcd.print("Done");
  //Because manually stopping and starting, do not want many extraneous
  //readings of "Done" so increase delay after 1000 cnts reached.
  delay(2000);
  } 
