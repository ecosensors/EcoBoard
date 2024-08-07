/*
 * EcoSensors - RTC / SD / BME280
 * The script is distributed WITHOUT WARRANTY.
 *
 * Feel free to collaborate and share suggestions for improvement
 *
 * To calibrate the RTC clock, uncomment //#define RTC_CALIBRATE  and define the date and time
 */
 

/* 
* Libraries 
*/
#include <Wire.h>                       // Need for I2C Bus
#include <Adafruit_BME280.h>            // include the library for the BME280 sensor
#include <RTClib.h>                     // Include the RTC Clock library
#include <ArduinoJson.h>                // Include the JSON library             
#include <FreeStack.h>                  // needed for freeMemory()

//#include <BufferedPrint.h>
//#include <MinimumSerial.h>
//#include <RingBuf.h>
#include <SdFat.h>
//#include <SdFatConfig.h>
#include <sdios.h>


/* 
* Global variables 
*/
bool debug = false;                      // Display the event from the library
bool debug_rtc = false;                 // Used to debug the RtcInterval() function or the print RTC time
bool debug_sd = false;                   // Used to debug the SD writing/reading
// Interval
#define TX_INTERVAL 10                  // Define an interval between each loop (seocnds). 
                                        // Do not use an interval < 1 second


/* 
* JSON 
*/
//JsonDocument json;                    // The object is created bellow
const char * fileName = "log.jsonl";    // jsonl stand for JSON Line (or ndJSON): https://jsonlines.org/


/* 
* BME280 
*/
#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BME280 bme;                    // Create an object


/* 
* RTC Clock 
*/
RTC_DS3231 rtc;                         // Create an object
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
int16_t y,m,d,h,mn,s;                   // Used for RTC time
int32_t unix_time;                      // Used for unix time
int32_t lastTx;                         // Used to record the time of the last measure (unix format)
char date_time[20];                     //1010-10-10 11:11:11
/*
 * If it's the first time you run the DS3231 or if you want to calibrate the time, 
 * uncomment #define RTC_CALIBRATE.
 * Then you can define here, the time below RTC_CALIBRATE.
 * 1. Upload the script and run once the script on board once (the script will stop after the new time has been recorded).
 * 2. Sitch off the board
 * 3. Re-comment #define RTC_CALIBRATE
 * 4. Upload the script
 */
//#define RTC_CALIBRATE                 // Uncomment to calibrate/set the time 
int16_t annee = 2024;                   // year 
int16_t mois = 7;                       // month
int16_t jour = 7;                      // day
int16_t heure = 0;                     // hour
int16_t minutes = 10;                   // minutes
int16_t secondes = 0;                  // seconds


/* 
SD card 
*/
bool isSdReady = false; 
const int carddetect = 7;   
const int chipselect = 4;
char sd_pathLog[18];                    // /log/2024/07/04


#define SD_FAT_TYPE 3                   // SD_FAT_TYPE = 0 for SdFat/File as defined in SdFatConfig.h,
                                        // 1 for FAT16/FAT32, 2 for exFAT, 3 for FAT16/FAT32 and exFAT.
#if SD_FAT_TYPE == 0
  SdFat sd;
  File file;
  File root;
#elif SD_FAT_TYPE == 1
  SdFat32 sd;
  File32 file;
  File32 root;
#elif SD_FAT_TYPE == 2
  SdExFat sd;
  ExFile file;
  ExFile root;
#elif SD_FAT_TYPE == 3
  SdFs sd;
  FsFile file;
  FsFile root;
#endif  // SD_FAT_TYPE

void setup(void)
{
  Serial.begin(9600);
  delay(8000);                            // Give a delay to open the terminal

  Serial.println(F(""));
  Serial.println(F(" ECOBOARD - RTC / SD / BME280"));
  Serial.println(F("-----------------------------"));
  Serial.println(F(""));

  /* Begin the BME280 sensor */
  Serial.println(F("#  Init BME280"));
  bool status;
  status = bme.begin();                   // 0x77 (default addr)
                                          // You can change the I2C addresss as the following: 
                                          // bme.begin(0x76)
  if (!status) {
    Serial.println(F(".. BME280 FAILED!"));
    Serial.println(F(".. Could not find a valid BME280 sensor"));
    Serial.println(F(".. check the wiring or the sensor address!"));
    Serial.println(F(".. the script stoped"));
    while (true);
  }
  else
  {
    // For different settings, see advancedsetting.ino file in BME280 library examples
    Serial.println(F(".. forced mode, 1x temperature / 1x humidity / 1x pressure oversampling,"));
    Serial.println(F(".. filter off"));
    bme.setSampling(Adafruit_BME280::MODE_FORCED,
      Adafruit_BME280::SAMPLING_X1,         // temperature
      Adafruit_BME280::SAMPLING_X1,         // pressure
      Adafruit_BME280::SAMPLING_X1,         // humidity
      Adafruit_BME280::FILTER_OFF);
        //,
        //Adafruit_BME280::STANDBY_MS_1000
      Serial.println(F(">> BME280 OK!"));
      Serial.println(F(""));
  }     

  /* Begin the RTC Clock */
  Serial.println(F("#  Start RTC Clock"));
  if(!rtc.begin())
  {
    Serial.println(F(".. Could not start RTC"));
    Serial.println(F(".. check the wiring or the sensor address!"));
    Serial.println(F(".. The DS3231 has a default I2C address of 0x68"));
    Serial.println(F(".. the script stoped"));
    while(true);
  }
  Serial.println(F(">> RTC Clock OK"));

  // If it's the first time you run the DS3231 to calibrate the clock
  #ifdef RTC_CALIBRATE
    Serial.println(F("# RTC Clock calibration")); 
    rtc.adjust(DateTime(annee, mois, jour, heure, minutes, secondes));
    Serial.println(F(">> The RTC has been calibrated at "));
    Serial.print(F(".. "));
    Serial.print(annee);
    Serial.print(F("-"));
    Serial.print(mois);
    Serial.print(F("-"));
    Serial.print(jour);
    Serial.print(F(" "));
    Serial.print(heure);
    Serial.print(F(":"));
    Serial.print(minutes);
    Serial.print(F(":"));
    Serial.println(secondes);
    Serial.println(F(".. Switch off the board, comment #define RTC_CALIBRATE, upload the code."));
    while(true);
  #endif

  if (rtc.lostPower())
  {
    Serial.println(F(">> RTC lost power, lets set the time!"));
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  // Here we want to get time
  DateTime now = rtc.now();
  y = now.year();                           // Save the year
  m = now.month();                          // Save the month
  d = now.day();                            // Save the day
  h = now.hour();                           // Save the hour
  mn = now.minute();                        // Save the minutes
  s = now.second();                         // Save the seconds 
  unix_time = now.unixtime();               // Save the unix time

  // Print the time
  Serial.println(F("#  Time"));
  Serial.print(F(">> "));
  Serial.print(y);
  Serial.print(F("/"));
  Serial.print(m);
  Serial.print(F("/"));
  Serial.print(d);
  Serial.print(F(" ("));
  Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
  Serial.print(F(") "));
  Serial.print(F(" "));
  Serial.print(h);
  Serial.print(F(":"));
  Serial.print(mn);
  Serial.print(F(":"));
  Serial.println(s);
  Serial.print(F("   Since midnight 1/1/1970 = "));
  Serial.println(unix_time);

  sprintf(date_time,"%i-%i-%i %i:%i:%i",y,m,d,h,mn,s); // Concatanate into date_time (char)
  Serial.print(F(".  ISO 8601: "));
  Serial.println(date_time);

  lastTx = unix_time;
  
  /* Begin SD Card */
  byte c=1;                                                     // used to count the attend to start the SD crard
  pinMode(carddetect, INPUT_PULLUP);                            // Define the pin mode

  Serial.println(F("\r\n#  Begin SD"));                                                
  do
  { 
    if (!sd.begin(chipselect, SD_SCK_MHZ(12)))                  // INITIALIZE and check the SD card
    {
      Serial.println(F(".. Attending to detect the SD card"));
      isSdReady = false;                                        // The status must remind false
      c++;                                                      // Increment the lopping count
      delay(1000);                                              // Give a delay of 1 sec
    }
    else                                                        // the cond return true
    {
      isSdReady = true;                                         // Change the status. The SD is ready, then change the value of _isSdReady to true
    }
  }while(isSdReady == false && c < 4);                          // If the _isSdReady is always False, loop only 3 timne then, exit and continue
  
  if(!isSdReady)                                                // If the card is not ready
  {
    Serial.println(F(">> SD could not be initiazed"));          // Errors here is mostly due to the SD card. Is it inserted?
    Serial.println(F(".. Is the SD card enabled"));
    Serial.println(F(".. Did you insert a card?"));
    while(1);                                                   // Stop the script
  }
  else
  {
    //sd.ls("/", LS_R);                                         // List the SD card content
    //Serial.println(F("\n#. List of files on the SD"));
    int d = checkVolumeWorkingDirectory();
    if(d==1)                       // the volume working directory is /log/year/month/day/
    {
      Serial.println(F(".. the VWD exists"));
    }
    else if(d==2)
    {
      Serial.println(F(".. the VWD has been created"));
    }
    else
    {
      Serial.println(F(".. an error occured will checking the VWD"));
    }; 
    Serial.println(F(">> OK SD Card"));
  }

  Serial.println(F(""));
  Serial.println(F("LET'S GO!"));
  Serial.println(F("================================================================================"));

  lastTx = now.unixtime() - (TX_INTERVAL * 2);                    //  Hook to start immediately the measures

  Serial.print(F("Time\t\t\t"));
  Serial.print(F("Temperature\t"));
  Serial.print(F("Pressure\t"));
  Serial.print(F("Aprox Alt.\t"));
  Serial.print(F("Humidity\t"));
  Serial.println(F("RAM"));
}
 
void loop(){

  // Check bool RtcInterval() function at the bottom
  if(RtcInterval(lastTx, TX_INTERVAL, debug_rtc) == false)
  {
    /*
    * We need to wait the next interval until RtcInterval return true
    * Note: RtcInterval() update the global variables  y,m,d,h,mn,s and unix_time at each loop
    */
    delay(100);
  }
  else
  {
  
    lastTx = unix_time;                                           // Save the lastest measures (unix_time is updated in RtcInterval())
    
    JsonDocument jdoc;                                            // Creating a JSON object

    jdoc["module"] = "demo";                                      // Adding an name and a value to the object
   
    sprintf(date_time,"%i-%i-%i %i:%i:%i",y,m,d,h,mn,s);          // Concatanate into date_time (char)
                                                                  // (y,m,d,hare global variable and their are updated in RtcInterval())
    Serial.print(date_time);                                      // Print the date and the time
    Serial.print(F("\t"));
    jdoc["time"] = date_time;                                     // Adding an name and a value to the object

    float f_temperature, f_pressure, f_altitude, f_humidity;

    // Only needed in forced mode! In normal mode, you can remove the next line.
    bme.takeForcedMeasurement(); // has no effect in normal mode
    f_temperature = bme.readTemperature();
    Serial.print(f_temperature, 1);                               // Print the temperature
    Serial.print(F(" *C \t"));
    jdoc["sensors"][0]["temperature"] = f_temperature;            // Adding an name and a value to the object
  
    f_pressure = bme.readPressure() / 100.0F;
    Serial.print(f_pressure, 0);                                  // Print the pressure
    Serial.print(F(" hPa\t\t"));
    jdoc["sensors"][1]["pressure"] = f_pressure;                  // Adding an name and a value to the object
  
    f_altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
    Serial.print(f_altitude, 0);                                  // Print the altitude
    Serial.print(F(" m\t\t"));
    jdoc["sensors"][2]["altitude"] = f_altitude;                  // // Adding an name and a value to the object
  
    f_humidity = bme.readHumidity();
    Serial.print(f_humidity, 0);                                  // Print the humidity
    Serial.print(F(" %\t\t"));
    jdoc["sensors"][3]["humidity"] = f_humidity;                  // Adding an name and a value to the object

    int ram = freeMemory();
    Serial.println(ram);                                          // Print the reminding SRAM
    jdoc["SRAM"] = ram;                                           // Adding an name and a value to the object
    
    // Prepare to write
    gotToVolumeWorkingDirectory();                                // Got to the volume working firectory /log/yaer/month/day/

    File sd_log;  
    sd_log = sd.open(fileName, O_RDWR | O_CREAT | O_AT_END);      // Open the file log.jsonl
    serializeJson(jdoc,sd_log);                                   // serialize and write te directly to the file
    sd_log.write('\n');                                           // Add a \n
    sd_log.close();                                               // Colse the file

  } 
}

int gotToVolumeWorkingDirectory()
{
  return volumeWorkingDirectory(true);
}

int checkVolumeWorkingDirectory()
{
  return volumeWorkingDirectory(false);
}

int volumeWorkingDirectory(bool gotToVWD){ 
  /*
  The logs are saved into the folder /log/year/month/day/log.jsonl
  The volumeWorkingDirectory() function  
  1. checks if the folder where the log are saved, exists, if goToVWD is false
  2. create the folder where are saved the log, if goToVWD is true

  returned values:
  -3 = Failed to mkdir
  -2 = Failed the VWD
  -1 = Failed to create the volume working directory
  0 = Can not chdir root
  1 = the volume working directory exists
  2 = the volume working directory has been created
  */

  sprintf(sd_pathLog,"/LOG/%i/%i/%i",y,m,d);                      // sd_pathLog is a global variable 
  
  if(sd.chdir()) // go to root
  {
    if(debug)
    {
      Serial.print(".. Path: ");
      Serial.println(sd_pathLog);                                   // sd_pathLog is a global variable and defined in defineVolumeWorkingDirectory()
    }

    if(!sd.exists(sd_pathLog))
    {
      if(!sd.mkdir(sd_pathLog,true))                              // Create missing parent directories if true
      {
        Serial.println(F(".. Failed to mkdir"));
        return -3;
      }
      else
      {
        Serial.print(F(".. "));
        Serial.print(F(sd_pathLog));
        Serial.println(F(" hass been created"));
        return 2;
      }
    }
    else
    {
      if(debug)
      {
        Serial.print(F(".. "));
        Serial.print(F(sd_pathLog)); 
        Serial.println(F(" already exists"));
      }
      if(gotToVWD)
      {
        bool b = sd.chdir(sd_pathLog);
        if(b == false)
        {
          if(debug)
          {
            Serial.print(F(".. "));
            Serial.println(F("failed to chdir the VWD"));
          }
          return -2;
        }
      }
      return 1;
    }
  }
  else
  {
    Serial.println(F("Could not chdir root"));
    return 0;
  }
}

/*
* This function need the RTC module. It calculates the interval between two measurements according to TX_INTERVAL
*
* 'lastTx' is the time when the last measurement is taken
* 'tx_interval' is the interval between two measurements
* 'degug' display messages
*
* If the different between lastTx and the time being is greater than tx_interval, RtcInterval() return true
*/
bool RtcInterval(int32_t lastTx, int32_t tx_interval, bool debug)
{ 
  int32_t nextTx = lastTx + tx_interval;
  int32_t unix_t;  
    
  DateTime t = rtc.now();
  y = t.year();                             // Save the year
  m = t.month();                            // Save the month
  d = t.day();                              // Save the day
  h = t.hour();                             // Save the hour
  mn = t.minute();                          // Save the minutes
  s = t.second();                           // Save the seconds 

  //Serial.println(now.unixtime());
  unix_t = t.unixtime();                    // Save the unix time
  unix_time = unix_t;                       // save the value into the global variable
  sprintf(date_time,"%i-%i-%i %i:%i:%i",y,m,d,h,mn,s); // Concatanate into date_time (char)
    
  if(debug==true){
    Serial.println(F("#  DEBUG"));
    Serial.print(F(".. UNIX time: "));
    Serial.print(unix_t);
    Serial.print(F(" - Next Tx: "));
    Serial.println(nextTx);
    Serial.print(F(".. Time: "));
    Serial.println(date_time);
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

/*
* Print SRAM
*/
int freeMemory() {
  char top;
  #ifdef __arm__
    return &top - reinterpret_cast<char*>(sbrk(0));
  #elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
    return &top - __brkval;
  #else  // __arm__
    return __brkval ? &top - __brkval : &top - __malloc_heap_start;
  #endif  // __arm__
}