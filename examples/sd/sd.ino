/*
 * EcoSensors
 * 
 * In that example, we will see how to create briefly a log file into the MicroSD card.
 * The log file will be saved into folders a the following
 * /log/20/06/24/212222.txt (/log/year/month/day/time.txt)
 * We will need to read the files Ecosensors.ccp and Ecosensors.h
 * I will try to provide comments and description, but you will need some knowledge to improve it for you application
 *
 * WARNING: Before doing that example, we consider you alraedy calibrated the RTC as show here:
 * https://github.com/ecosensors/Ecoboard/blob/master/examples/rtc-lowpower/rtc-lowpower.ino
 * If you have not, make sure your board print am acceptable time and date
 */

#include <Ecoboard.h>
#include <Wire.h>           // Need for I2C Bus
#include <RTClib.h>         // RTC is needed to get the time to create and to name the log file
RTC_DS3231 rtc;
int16_t y,m,d,h,mn,s;               // Used for RTC time
int32_t unix_time;
char date_time[18];                 //20-6-10 11:11:11
 
bool isSdEnable = true;     // Enable SD card

Ecoboard Eco(isSdEnable);

void setup(void)
{
  Serial.begin(9600);
  Wire.begin();  

  delay(8000);

  /*
   * If isSdEnable is true, Eco.begin will call the private function _sd_begin()
   * to initiate the SD card, and list the content of the card
   */
  if(Eco.begin()==0)
  {  
    Serial.println(F("[Error]"));          // Errors here is mostly due to the SD card. Is it inserted?
    while(1);
  }

  Serial.println("     ECOBOARD     ");
  Serial.println("      SD card     ");
  Serial.println("------------------");
  Serial.println("");

    /*
   * We need to get the time from DS3231, in orderto create a log file into the SD card, named with the curent date/time
   * To simplify, we consider you already calibrate the RTC, following that eample: https://github.com/ecosensors/Ecoboard/blob/master/examples/rtc-lowpower/rtc-lowpower.ino#L82
   * look at #RTC_CALIBRATE
   */
  if(!rtc.begin())                        // Start Real Time Clock (RTC)
  {
    Serial.println(F("[ERROR] Could not start RTC"));
    while(true);
  }

  DateTime now = rtc.now();             // Get time

  y = now.year();
  y = y-2000;
  m = now.month();
  d = now.day();
  h = now.hour();
  mn = now.minute();
  s = now.second();

  unix_time = now.unixtime();
  sprintf(date_time,"%i-%i-%i %i:%i:%i",y,m,d,h,mn,s);

  Serial.print(F("Now is: "));
  Serial.println(date_time);

  /*
   * Back to SD card preparation, now we are going to create the log file
   */
   if(Eco.sd_init_logFile(y,m,d,h,mn,s) !=  1)
  {
    while(1);
  }

  /*
   * Now the log file has been ceated.
   */
   Eco.sd_write("Add new line ");
   Eco.sd_writeln("and new word in the log file");

   Serial.println(F("DONE!));

   /*
    * Later I will show how to add a text in another file than the log file, but you can
    * easly do it, working around those function:
    * int Ecoboard::sd_writeln(char const * text)
    * {
    *   return _sd_write(_logFile, text, true); // _logFile is the path from root and name of the file
    * }
    *
    * int Ecoboard::sd_write(char const * text)
    * {
    *   return _sd_write(_logFile, text, false);
    * }
    * 
    * in Ecoboard.cpp and Ecoboard.h files
    */





  
 // Coming soon

}
 
void loop(){
}
