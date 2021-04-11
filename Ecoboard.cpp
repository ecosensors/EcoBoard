/*
* That file is underdevelopment.
*
* The follwing code can be used without garranty.
I will double check it later with EcoLora
*
*/
#include "Arduino.h"
#include "Ecoboard.h"

Ecoboard::Ecoboard(bool isSdEnable)
{
	/*
	* SD Card
	*/
	_isSdEnable = isSdEnable;
	_carddetect = 7;
	_chipselect = 4;
	
}

/*
* 1 : SD Cad OK
*/
int Ecoboard::begin()
{
	Serial.println(F("*******************"));
	Serial.println(F("*     Welcome     *"));
	Serial.println(F("* with EcoSensors *"));
	Serial.println(F("*******************"));
	Serial.println("");

	/*
	* Initiate SD card
	*/
	if(_isSdEnable)
  	{
		
		_isSdReady = _sd_begin();
    	
    	if(_isSdReady == true)
      	{
      		Serial.println("(SD card is Ready)" );
      		return 1;
    	}
    	else
      	{
      		Serial.println("(SD card is NOT Ready. Check if the card is OK)" );
      		return 0;
  		}
  	}
  	else
  	{
    	_isSdReady = false;
    	Serial.println("(SD card is disable)" );
    	return 2;
  	}

}


/*
* SD Card
*/
bool Ecoboard::_sd_begin()
{
    pinMode(_carddetect, INPUT_PULLUP);
	Serial.println(F("# Begin SD"));
	bool isOk = false;  
    do
    { 
    	if (!_sd.begin(_chipselect))
    	{
      		Serial.println(F("No card SD"));
        	isOk = false;
        	delay(1000);
      	}
    	else
    	{
        	isOk = true;
      	}
    }while(isOk == false);
    _sd.chdir();
    //DEBUG_L
    //listFiles(NULL);
    return isOk;
}

/*
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
    if(!_sd.chdir(folder)) // "/GPS/2016/"
    {
      Serial.print(F("Could not chdir")); Serial.println(folder);
    }
  }
  
  _sd_showCwd();
    
  _sd.vwd()->rewind(); // on se remte au debut du repecoire courrant
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


void Ecoboard::_sd_showCwd()
{
	char currentDirectory[20]; //Current Work Directory (cwd)
    _sd.vwd()->getName(currentDirectory,sizeof(currentDirectory ));
    Serial.print(F("Current directory: "));
    Serial.println(currentDirectory);
}

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
    	_sd_begin();

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