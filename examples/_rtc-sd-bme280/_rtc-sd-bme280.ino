/*
 * EcoSensors - RTC / SD / BME280
 * The script is distributed WITHOUT WARRANTY.
 */

 /*
 * The script is being rewriting
 */

/* Libraries */
#include <Wire.h>                       // Need for I2C Bus
#include <Adafruit_BME280.h>            // include the library for the BME280 sensor
#include <RTClib.h>                     // Include the RTC Clock library

#include <BufferedPrint.h>
#include <FreeStack.h>
#include <MinimumSerial.h>
#include <RingBuf.h>
#include <SdFat.h>
#include <SdFatConfig.h>
#include <sdios.h>


/* Global variables */
bool debug = true;                  // Display the event from the library
bool debug_rtc = true;              // Used to debug the RtcInterval() function or the print RTC time
// Interval
int32_t lastMeasure = 0;
#define TX_INTERVAL 10            // Define an interval between each loop (seocnds). 
                                  // Do not use an interval < 1 second
unsigned long scheduler;          // Used incase RTC is disable


/* BME280 */
#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BME280 bme;                // Create an object


/* RTC Clock */
RTC_DS3231 rtc;                    // Create an object
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
int16_t y,m,d,h,mn,s;               // Used for RTC time
int32_t unix_time;                  // Used for unix time
int32_t lastTx;                     // Used to record the time of the last measure (unix format)
char date_time[18];                 //1010-10-10 11:11:11
/*
 * If it's the first time you run the DS3231 or if you want to modify the time, 
 * Uncomment #define RTC_CALIBRATE to calibrate/set the time
 * and define here the time below RTC_CALIBRATE.
 * 1. Upload the script and run once the script on board once.
 * 2. Sitch off the board
 * 3. Re-comment #define RTC_CALIBRATE
 * 4. Upload the script
 */
//#define RTC_CALIBRATE             // Uncomment to calibrate/set the time 
int16_t annee = 2024;               // year 
int16_t mois = 4;                   // month
int16_t jour = 23;                  // day
int16_t heure = 22;                 // hour
int16_t minutes = 42;               // minutes
int16_t secondes = 00;              // seconds


/* SD */
bool isSdReady = false; 
const int carddetect = 7;   
const int chipselect = 4;  

#define SD_FAT_TYPE 0               // SD_FAT_TYPE = 0 for SdFat/File as defined in SdFatConfig.h,
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
  delay(5000);                      // Give a delay to open the terminal

  Serial.println("");
  Serial.println(" ECOBOARD - RTC / SD / BME280");
  Serial.println("-----------------------------");
  Serial.println("");

  /* Begin the BME280 sensor */
  Serial.println(F("# Init BME280"));
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
  Serial.println(F("# Start RTC Clock"));
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
  Serial.println(F("# Time"));
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
  Serial.print(F(".. Since midnight 1/1/1970 = "));
  Serial.println(unix_time);

  sprintf(date_time,"%i-%i-%i %i:%i:%i",y,m,d,h,mn,s); // Concatanate into date_time (char)
  Serial.println(F(",, ISO 8601: "));
  Serial.println(date_time);

  lastTx = unix_time;
  

  /* 
  * SD Card
  */
  byte c=1;                                               // used to count the attend to start the SD crard
  pinMode(carddetect, INPUT_PULLUP);                      // Define the pin mode

  Serial.println(F("# Begin SD"));                                                
  do
  { 
    if (!sd.begin(chipselect))                                // INITIALIZE and check the SD card
    {
      Serial.println(F("..Attending to detect the SD card"));
      isSdReady = false;                                       // The status must remind false
      c++;                                                      // Increment the lopping count
      delay(1000);                                              // Give a delay of 1 sec
    }
    else                                                        // the cond return true
    {
      isSdReady = true;                                   // Change the status. The SD is ready, then change the value of _isSdReady to true
    }
  }while(isSdReady == false && c < 4);                    // If the _isSdReady is always False, loop only 3 timne then, exit and continue
  
  if(!isSdReady)                                          // If the card is not ready
  {
    Serial.println(F("KO SD could not be initiazed"));    // Errors here is mostly due to the SD card. Is it inserted?
    Serial.println(F(".. Is the SD card enabled"));
    Serial.println(F(".. Did you insert a card?"));
    while(1);                                             // Stop the script
  }
  else
  {
    Serial.println(F("OK SD Card"));
    Serial.println(F("\nList of files on the SD.\n"));
    sd.ls("/", LS_R);
    sd.chdir();                                                // If the card is ready, chdir to the root
    Serial.println(F("\nList of files on the SD.\n"));
    sd.ls("/", LS_R);
  }

  Serial.println(F(""));
  Serial.println(F("LET'S GO!"));
}
 
void loop(){
  
  if(RtcInterval(lastTx, TX_INTERVAL, debug_rtc) == false)
  {
    // We need to wait the next interval until RtcInterval return true
    delay(500);
  }
  else
  {
  
    /*
    if (millis() > scheduler)             // Interval with ATSAMD21 clock
    {
      scheduler = millis() + TX_INTERVAL;    // schedule take the millis of the moment + the INTERVAL defined above
    */
    if (sd.exists("Folder1") || sd.exists("Folder1/file1.txt") || sd.exists("Folder1/File2.txt")) {
      Serial.println(F("Please remove existing Folder1, file1.txt, and File2.txt"));
    }



    /*
    * Developpement in progress
    */

  } 
}

/*
* That function calculates the interval according to TX_INTERVAL
*/
bool RtcInterval(int32_t lastTx, int32_t tx_interval, bool debug)
{ 
  int32_t nextTx = lastTx + tx_interval;
  int32_t unix_t;  
    
  DateTime t = rtc.now();
  y = t.year();                           // Save the year
  m = t.month();                          // Save the month
  d = t.day();                            // Save the day
  h = t.hour();                           // Save the hour
  mn = t.minute();                        // Save the minutes
  s = t.second();                         // Save the seconds 

  //Serial.println(now.unixtime());
  unix_t = t.unixtime();                    // Save the unix time
    
  if(debug==true){
    Serial.println(F("DEBUG"));
    Serial.print(F(".. UNIX time: "));
    Serial.print(unix_t);
    Serial.print(F(" - Next Tx: "));
    Serial.println(nextTx);
    
    Serial.print(F(".. Time: "));
    sprintf(date_time,"%i-%i-%i %i:%i:%i",y,m,d,h,mn,s); // Concatanate into date_time (char)
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
