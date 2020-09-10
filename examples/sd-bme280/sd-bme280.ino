/*
 * EcoSensors - SD, RTC, BME280 (I2C)
 * 
 * THAT EXEMPLE HAS NOT BEEN TEST YET, and the script is under development
 * 
 * In that example, we will see how to create briefly a log file into the MicroSD card.
 * We will use a I2C barometer sensor (BME280), to record some value in the log file
 * 
 * We need a RTC module to get the time and to save the value into folders a the following
 * /log/20/06/24/212222.txt (/log/year/month/day/time.txt)
 * 
 * We will need to use the library Ecosensors (Ecosensors.ccp / Ecosensors.h)
 * I will try to provide comments and description, but you will need some knowledge to improve it, 
 * for you application
 *
 * WARNING: Before doing that example, we consider you alraedy calibrated the RTC as show here:
 * https://github.com/ecosensors/Ecoboard/blob/master/examples/rtc-lowpower/rtc-lowpower.ino
 * If you have not, make sure your board print am acceptable time and date
 *
 */

#include <Ecoboard.h>
#include <Wire.h>           // Need for I2C Bus
/*
 * RTC
 */
#include <RTClib.h>         // RTC is needed to get the time to create and to name the log file
RTC_DS3231 rtc;
int16_t y,m,d,h,mn,s;       // Used for RTC time
int32_t unix_time;
char date_time[18];         // 20-6-10 11:11:11
bool isSdEnable = true;     // Enable SD card

/* Interval */

int32_t lastMeasure = 0;
#define INTERVAL 5000 // = 5sec
unsigned long scheduler;


/*
 * BME280
 */
 #define SEALEVELPRESSURE_HPA (1013.25)
#include <Adafruit_BME280.h>  // Import the library
Adafruit_BME280 bme;          // Create an instence for I2C bus

/*
 * SD is defined in the Ecoboard library
 */

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
  Serial.println(" SD card - BME280 ");
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
   //Eco.sd_write("Add new line, ");    // Modify that line following your neeed
   //Eco.sd_writeln("add a new word at the same line and move the cursor to the next line");

   Serial.println(F("DONE!"));

   /*
    * Later I will show how to add a text in another file than the log file, but you can
    * easly do it, working around those function (see /libraries/Ecoboard/Ecoboard.ccp):
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


    /*
    * Initiate BME280 sensor
    */

    Serial.println(F("Init BME280"));
    bool status;
    status = bme.begin(); // 0x77 (default addr)
    // You can change the I2C addresss as the following: 
    // bme.begin(0x76)
    if (!status) {
      Serial.println(F("BME280 FAILED!"));
      Serial.println(F("Could not find a valid BME280 sensor, check wiring!"));
      while (1);
    }
    else
    {
      // For different settings, see advancedsetting.ino file in BME280 library examples
      Serial.println(F("\tforced mode, 1x temperature / 1x humidity / 1x pressure oversampling,"));
      Serial.println(F("\tfilter off"));
      bme.setSampling(Adafruit_BME280::MODE_FORCED,
        Adafruit_BME280::SAMPLING_X1, // temperature
        Adafruit_BME280::SAMPLING_X1, // pressure
        Adafruit_BME280::SAMPLING_X1, // humidity
        Adafruit_BME280::FILTER_OFF);
        //,
        //Adafruit_BME280::STANDBY_MS_1000

      Serial.println(F("BME280 DONE!"));
      Serial.println(F(""));
  }
  Serial.println(F("Start measuring!"));
  Serial.println(F("==============="));
  Eco.sd_writeln("START MEASURING");
}
 
void loop(){
  // Interval with ATSAMD21 clock
  if (millis() > scheduler)
  {
    scheduler = millis() + INTERVAL;

  // Interval with RTC module
  //if(rtcInterval(lastMeasure, INTERVAL, 0) == true){

    // Only needed in forced mode! In normal mode, you can remove the next line.
    bme.takeForcedMeasurement(); // has no effect in normal mode
    Serial.print(F("Temperature = "));
    Serial.print(bme.readTemperature());
    Serial.println(F(" *C"));
  
    Serial.print(F("Pressure = "));
  
    Serial.print(bme.readPressure() / 100.0F);
    Serial.println(F(" hPa"));
  
    Serial.print(F("Approx. Altitude = "));
    Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
    Serial.println(F(" m"));
  
    Serial.print(F("Humidity = "));
    Serial.print(bme.readHumidity());
    Serial.println(F(" %"));

    /*
     * In progress
    */


    /*
     // Uncomment this, if you want to use interval with RTC
      DateTime now = rtc.now();
      lastMeasure = now.unixtime();
    */
  }
  
}


bool rtcInterval(int32_t lastTx, int32_t tx_interval, int debug)
{ 
    int32_t nextTx = lastTx + tx_interval;
    int32_t unix_t;  
    
    DateTime t = rtc.now();

    //Serial.println(now.unixtime());
    unix_t = t.unixtime();
    
    if(debug==1){
      Serial.print(unix_t);
      Serial.print(F(" - "));
      Serial.println(nextTx);
    }

    if(unix_t > nextTx)
    {
      return true;
    }
    else
    {
      return false;
    }
}
