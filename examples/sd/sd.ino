/*
 * EcoSensors - SD, BME280 (I2C)
 * The script is distributed WITHOUT WARRANTY.
 */

 /*
 * The script is being rewriting
 */

/* Libraries */
#include <Ecoboard.h>               // Include the EcoBoard library (lib)

/* Interval */
int32_t lastMeasure = 0;
#define INTERVAL 5000               // Define an interval of 5sec between each loop
unsigned long scheduler;

/* SD */
//The SD library is included from the Ecoboard library
bool isSdEnable = true;             // Enable SD card or not
bool debug = true;                  // Display the event from the library

Ecoboard Eco;    // The SD card is initiate in the cpp file. See Eco.begin bellow
SdFat sd;

void setup(void)
{
  Serial.begin(9600);

  delay(8000);                      // Give a delay to open the terminal

  Serial.println(" RTC");
  Serial.println("---------------------------------");
  Serial.println("");

  Eco.begin();                      // Begin the EcoBoard library
  

  /* 
  * SD Card
  */
  byte c=1;
  bool isSdReady = false; 
  int carddetect = 7;   
  int chipselect = 4;                                                 // used to count the attend to start the SD crard
  pinMode(carddetect, INPUT_PULLUP);                           // Define the pin mode

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
      isSdReady = true;                                         // Change the status. The SD is ready, then change the value of _isSdReady to true
    }
  }while(isSdReady == false && c < 4);                          // If the _isSdReady is always False, loop only 3 timne then, exit and continue
  
  if(!isSdReady)                                       // If the card is not ready
  {
    isSdEnable = false;                                        // Consider the card as disable
    Serial.println(F("KO SD could not be initiazed"));          // Errors here is mostly due to the SD card. Is it inserted?
    Serial.println(F(".. Is the SD card enabled"));
    Serial.println(F(".. Did you insert a card?"));
  }
  else
  {
    Serial.println(F("OK SD Card"));
    sd.chdir();                                                // If the card is ready, chdir to the root
  }

  Serial.println(F(""));
  Serial.println(F("LET'S GO!"));
}
 
void loop(){
  
  if (millis() > scheduler)             // Interval with ATSAMD21 clock
  {
    scheduler = millis() + INTERVAL;    // schedule take the millis of the moment + the INTERVAL defined above


    /*
    * Developpement in progress
    */

  } 
}
