/*
 * EcoSensors - SD
 * The script is distributed WITHOUT WARRANTY.
 */

 /*
 * The script is being rewriting
 */

/* Libraries */
#include <BufferedPrint.h>
#include <FreeStack.h>
#include <MinimumSerial.h>
#include <RingBuf.h>
#include <SdFat.h>
#include <SdFatConfig.h>
#include <sdios.h>

bool debug = true;                  // Display the event from the library

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


/* Interval */
int32_t lastMeasure = 0;
#define INTERVAL 5000               // Define an interval of 5sec between each loop
unsigned long scheduler;


void setup(void)
{
  Serial.begin(9600);
  delay(8000);                      // Give a delay to open the terminal

  Serial.println(" ECOBOARD - SD");
  Serial.println("---------------------------------");
  Serial.println("");

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
  
  if (millis() > scheduler)             // Interval with ATSAMD21 clock
  {
    scheduler = millis() + INTERVAL;    // schedule take the millis of the moment + the INTERVAL defined above

    if (sd.exists("Folder1") || sd.exists("Folder1/file1.txt") || sd.exists("Folder1/File2.txt")) {
      Serial.println(F("Please remove existing Folder1, file1.txt, and File2.txt"));
    }



    /*
    * Developpement in progress
    */

  } 
}
