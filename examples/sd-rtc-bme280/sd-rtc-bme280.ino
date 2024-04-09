/*
 * EcoSensors - SD, BME280 (I2C)
 * The script is distributed WITHOUT WARRANTY.
 */

/* Libraries */
#include <Ecoboard.h>               // Include the EcoBoard library (lib)
#include <Wire.h>                   // Need for I2C Bus
#include <RTClib.h>                 // Include the RTC lib for the clock
#include <Adafruit_BME280.h>        // Include the BME280 lib for the barometer
// Note: You do not need to include the SD lib, because the lib is included from the EcoSensors lib

/* Interval */
int32_t lastMeasure = 0;
#define INTERVAL 5000               // Define an interval of 5sec between each loop
unsigned long scheduler;

/* BME280 */
 #define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BME280 bme;                // Create an instence for I2C bus

/* RTC */
RTC_DS3231 rtc;                     // Intialize the RTC Clock library
int16_t y,m,d,h,mn,s;               // Used for RTC time (y:year, m:month, d:day, h:hour, m:minutes, s:second)
int32_t unix_time;                  // Used to store the time in unix format
char date_time[18];                 // Used to store the time as the following: 20-6-10 11:11:11

/* SD */
//The SD library is included from the Ecoboard library
bool isSdEnable = true;             // Enable SD card or not
bool debug = true;                  // Display the event from the library
bool isRTCEnable = true;            // Enable the clock or no
Ecoboard Eco(isSdEnable, isRTCEnable, debug);    // The SD card is initiate in the cpp file. See Eco.begin bellow


void setup(void)
{
  Serial.begin(9600);
  Wire.begin();  

  delay(8000);                      // Give a delay to open the terminal

  Serial.println(" RTC / BME280 / SD card - BME280 ");
  Serial.println("---------------------------------");
  Serial.println("");

  Eco.begin();                     // Begin the EcoBoard library

  /* 
  * RTC 
  */
  if(!rtc.begin())                  // Start Real Time Clock (RTC)
  {
    Serial.println(F("OK RTC Clock"));

    DateTime now = rtc.now();         // Get the time
    y = now.year();                   // Get the year
    y = y-2000;                       // Change 2024 to 24
    m = now.month();                  // Get the mounth
    d = now.day();                    // Get the day
    h = now.hour();                   // Get the hour
    mn = now.minute();                // Get the minutes
    s = now.second();                 // Get the second
    unix_time = now.unixtime();       // Get the unix time

    sprintf(date_time,"%i-%i-%i %i:%i:%i",y,m,d,h,mn,s);  // Save the value into the char date_time

    Serial.print(F("Now is: "));      // Print the date/time
    Serial.println(date_time);
  }
  else
  {
    Serial.println(F("KO Could not initialize RTC"));
  }
  

  /* 
  * SD Card
  */
  if(Eco.sd_begin())                                              // Initiate the SD card
  {  
    Serial.println(F("OK SD Card"));
  }
  else
  {
    Serial.println(F("KO SD could not be initiazed or is inactive"));          // Errors here is mostly due to the SD card. Is it inserted?
    Serial.println(F(".. Did you insert a card?"));
    isSdEnable = false;
  }
  
  
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

   
  } 
}
