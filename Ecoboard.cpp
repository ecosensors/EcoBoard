/*
* EcoBoard © 2022 by Pierre Amey is licensed under CC BY-NC-SA 4.0
* That file is under development.
*
* The follwing code can be used without garranty.
*
*/

#include "Arduino.h"
#include "Ecoboard.h"

Ecoboard::Ecoboard()
{
  Ecoboard(false, false, false);
}

Ecoboard::Ecoboard(bool isSdEnable, bool isRTCEnable, bool rebug)
{
	// GENERAL
  _debug;
  _isRTCEnable = isRTCEnable;                 // used to store the RTS status
	_isSdEnable = isSdEnable;                  // used to store the status of the card
  _isSdReady = false;                        // used to check if the card is raedy or not. If the SD crad is not inserted, the value is Not Ready (dalse)
	
  // SD CARD
  _carddetect = 7;                           // used as the MicroSD card CD (card detect)
	_chipselect = 4;                           // used as the MicroSD card CS (chip select) pin
}


void Ecoboard::begin()
{
	Serial.println(F("*******************"));
	Serial.println(F("*     Welcome     *"));
	Serial.println(F("* with EcoSensors *"));
	Serial.println(F("*******************"));
	Serial.println("");

  if(!_isSdEnable && _debug)
    Serial.println(F("SD is disable"));
  if(!_isRTCEnable && _debug)
    Serial.println(F("RTC is disable"));
}


/*
* INITIALIZE THE SD CARD
*
* False = SD is not ready
* True = SD is ready
*/
bool Ecoboard::sd_begin()
{
  pinMode(_carddetect, INPUT_PULLUP);                           // Define the pin mode
	if (_debug)
      Serial.println(F("# Begin SD"));
 
  byte c=1;                                                     // used to count the attend to start the SD crard
  do
  { 
    if (!_sd.begin(_chipselect))                                // INITIALIZE and check the SD card
    {
      if(_debug)
        Serial.println(F("..Attending to detect the SD card"));
      _isSdReady = false;                                       // The status must remind false
      c++;                                                      // Increment the lopping count
      delay(1000);                                              // Give a delay of 1 sec
    }
    else                                                        // the cond return true
    {
      _isSdReady = true;                                         // Change the status. The SD is ready, then change the value of _isSdReady to true
    }
  }while(_isSdReady == false && c < 4);                          // If the _isSdReady is always False, loop only 3 timne then, exit and continue
  
  if(_isSdReady == false)                                       // If the card is not ready
  {
    _isSdEnable = false;                                        // Consider the card as disabke
    //_logger = false;                                          // No log into the card is possible

    return _isSdReady;
  }
  else
  {
    _sd.chdir();                                                // If the card is ready, chdir to the root
  }

  return _isSdReady;
}


/*
* Init the card to log the activities of the board
* You need a RTC clock
*
* 2: SD disable
* 1: OK
* 0: Failed chdir root
* -1: Could not create LOG folder
* -2: Failed chdir log
* -3: Failed mkdir year
* -4: Failed chdir yes
* -5: Failed mkdir month
* -6: Failed chdir month
* -7: Failed mkdir day
* -8: Failed chdir day
*/
int Ecoboard::sd_init_logFile(int16_t y, int16_t m, int16_t d, int16_t h, int16_t mn, int16_t s)
{
  if(_isSdEnable == false)
    return 2;

  char yy[2];
  char mm[2];
  char dd[2];
  char hh[2];
  char min[2];
  char sec[2];
  snprintf(yy,3,"%i",y);
  snprintf(mm,3,"%i",m);
  snprintf(dd,3,"%i",d);
  snprintf(hh,3,"%i",h);
  snprintf(min,3,"%i",mn);
  snprintf(sec,3,"%i",s);

  if(_debug)
    Serial.println(F("Prepare the log file..."));

  if(!_sd.chdir())
  {
    Serial.println(F("Could not chdir root"));
    return 0;
  }

  if(!_sd.exists("/LOG"))
  {
    if(!_sd.mkdir("/LOG"))
    {
      Serial.println(F("Folder /LOG Could't be created"));
      return -1;
    }
  }

  if (!_sd.chdir("/LOG"))
  {
    Serial.println(F("Failed chdir LOG"));
    return -2;
  }
  
  // YEAR
  if(!_sd.exists(yy))
  {

    if(!_sd.mkdir(yy))
    {
      Serial.print(F("Failed (yy) mkdir "));
      Serial.println(yy);
      return -3;
    }
  }

  // Change volume working directory
  if (!_sd.chdir(yy)) {
    Serial.print(F("Failed chdir (yy) "));
    Serial.println(yy);
    return -4;
  }

  // MONTH
  if(!_sd.exists(mm))
  {
    if(!_sd.mkdir(mm))
    {
      Serial.print(F("Failed mkdir (mm) "));
      Serial.println(mm);
      return -5;
    }
  }

  // Change volume working directory
  if (!_sd.chdir(mm)) {
    Serial.print(F("Failed chdir "));
    Serial.println(mm);
    return -6;
  }

  // DAY
  if(!_sd.exists(dd))
  {
    if(!_sd.mkdir(dd))
    {
      Serial.print(F("Failed mkdir "));
      Serial.println(dd);
      return -7;
    }
  }
  // Change volume working directory.
  if (!_sd.chdir(dd))
  {
    Serial.print(F("Failed chdir "));
    Serial.println(dd);
    return -8;
  }

  sprintf(_sd_pathLog,"/LOG/%s/%s/%s/",yy,mm,dd);				// set the path to the log file
  sprintf(_logFile,"%s%s-%s-%s.txt",_sd_pathLog,hh,min,sec);	// set the log file name 
  //sprintf(_logData,"%sdatalog.txt",_sd_pathLog);

  _sd_write(_logFile, "LOG: ");									// Write first lines in the log file
  _sd_write(_logFile, _sd_pathLog);
  _sd_writeln(_logFile,_logFile);
  _sd_writeln(_logFile, "==================");
  
  delay(1000);

  if(!_sd.chdir())												// Put the cursor back to root of the card
  {
    Serial.println(F("Could not chdir ROOT after creating"));
    return 0;
  }

  _sd.chdir();

  if(!_sd.chdir(_sd_pathLog))
  {
    Serial.print(F("Could not chdir /LOG"));
    Serial.print(F(yy));
    Serial.print(F("/"));
    Serial.print(F(mm));
    Serial.print(F("/"));
    Serial.print(F(dd));
    Serial.println(F("/"));
    return 0;
  }

  Serial.print(F("Log Folder: "));
  Serial.println(_sd_pathLog);
  return 1;




  /*
  sprintf(_logFile,"%s",ADALOG);
  // Check if file exist sarting from ADALOG00.TXT
  for (uint8_t i = 0; i < 100; i++)
  {
    _logFile[6] = '0' + i/10;
    _logFile[7] = '0' + i%10;
    // create if does not exist, do not open existing, write, sync after write
    if (! _sd.exists(_logFile))
    {
      break;
    }
  }
  */


}
/*
// vwd() bug with the latest version of SdFat
// I need to update this part

void Ecoboard::listFiles(const char * folder){
  char fileNam[20];
  Serial.print(F("\nListing files in "));
  SdFile files;                             // for SDFAT
  if(folder == NULL)
  {
    Serial.println(F("root:"));
    if(!_sd.chdir())
    {
      Serial.println(F("Could not chdir root (listFile()"));
    }
    
  }
  else
  {
    Serial.print(folder);
    Serial.println(F(":"));
    if(!_sd.chdir(folder))
    {
      Serial.print(F("Could not chdir")); Serial.println(folder);
    }
  }
  
  _sd_showCwd();
    
  _sd.vwd()->rewind(); // on se remet au debut du repecoire courrant
  while (files.openNext(_sd.vwd(), O_READ))
  {
    files.printFileSize(&Serial);
    Serial.write(' ');
    files.printModifyDateTime(&Serial);
    Serial.write(' ');
    files.printName(&Serial);

    //file.getName(getNameFile, sizeof(getNameFile));
    
    if (files.isDir())
    {
      // Indicate a directory.
      Serial.write('/');
    }
    Serial.println();
    files.close();
  }
  Serial.println("listing Files Done!");

};
*/
/*
// vwd() bug with the latest version of SdFat
// I need to update this part
void Ecoboard::_sd_showCwd()
{
	char currentDirectory[20]; //Current Work Directory (cwd)
    _sd.vwd()->getName(currentDirectory,sizeof(currentDirectory ));
    Serial.print(F("Current directory: "));
    Serial.println(currentDirectory);
}
*/


bool Ecoboard::_sd_checkCard()
{
	bool isCardInserted = true;
  	Serial.println(F("Checking SD card"));
  	delay(5);
  	/*
   	* When the SD card is removed digitalRead(CARDDETECT) does not return 0 for a unknow reason 
   	* The script is temporarely stopped (because of while(1)).
   	* When the reason will be know and the problem solved, change this part of code
   	*/

  	//  while(!digitalRead(CARDDETECT))
  	while(1)
  	{
    	isCardInserted = false;
    	Serial.println(F("SD card is not ready. Check the card and reset the module"));
    	_isSdReady = false;
    
    	delay(1000);
  	}
  
  	if(isCardInserted == false)
      sd_begin();

  	isCardInserted = true;
  	// Serial.println(F("Sd card inserted"));
  	_isSdReady = true;

    
  	return isCardInserted;
}


int Ecoboard::sd_writeln(char const * text)
{
  return _sd_write(_logFile, text, true);
}

int Ecoboard::sd_write(char const * text)
{
  return _sd_write(_logFile, text, false);
}

int Ecoboard::sd_write(int16_t text)
{
  char mess[10];
  snprintf(mess,sizeof(mess), "%d", text);
  return _sd_write(_logFile, mess, false);
}

int Ecoboard::_sd_writeln(char * fileName, char const * text)
{
  return _sd_write(fileName, text, true);
}

int Ecoboard::_sd_write(const char * fileName, char const * text)
{
  return _sd_write(fileName, text, false);
}

int Ecoboard::_sd_write(char * fileName, const __FlashStringHelper * text)
{
  return _sd_write(fileName, text, false);
}

int Ecoboard::_sd_writeln(char * fileName, const __FlashStringHelper * text)
{
  return _sd_write(fileName, text, true);
}



int Ecoboard::_sd_write(const char * fileName, const __FlashStringHelper * text, bool ln)
{
  if(_isSdEnable == false)
    return 2;

  bool isWrite = false;
  
//#if defined(LOGGER) && defined(FEATHERWINGS)

    File writeFileFl;  
    if(_isSdReady == true)
    { 
      //digitalWrite(PIN_SDLED, HIGH);

      writeFileFl = _sd.open(fileName, O_RDWR | O_CREAT | O_AT_END);
      if(writeFileFl)
      {
        if(ln == true)
        {
          writeFileFl.println(text);
        }
        else
        {
          writeFileFl.print(text);
        }
        
        writeFileFl.close();
        isWrite = true;
      }
      else
      {
        Serial.println(F("\nError opening1 "));
        isWrite = false;
      }
      //digitalWrite(PIN_SDLED, LOW);
    }
    else
    {
      isWrite = false;
    }

    if(isWrite == false)
      _sd_checkCard();
/*
  #else
    sprint(F("LOGGER not defined"),0);
    isWrite = false;
  #endif
*/
  if(isWrite == true)
    return 1;
  else
    return 0;
}

int Ecoboard::_sd_write(const char * fileName, char const * text, bool ln)
{
  	if(_isSdEnable == false)
    	return 2;

 	bool isWrite = false;
  
	File writeFileCo;   //logFile
    if(_isSdReady == true)
    {
       writeFileCo = _sd.open(fileName, FILE_WRITE);
     	if(writeFileCo)
      	{
        	if(ln == true)
        	{
          		writeFileCo.println(text);
        	}
        	else
        	{
          		writeFileCo.print(text);
        	}
        	writeFileCo.close();
        	isWrite = true;
      	}
      	else
      	{
        	Serial.print(F("\nError opening2 ")); Serial.println(fileName);
        	isWrite = false;
      	}
    }
    else
    {
      	isWrite = false;
    }

    if(isWrite==false)
     	_sd_checkCard();
  /*  
  #else
    sprint(F("LOGGER not defined"),0);
    isWrite = false;
  #endif
  */

  	if(isWrite == true)
    	return 1;
  	else
    	return 0;
}