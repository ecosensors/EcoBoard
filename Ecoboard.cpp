/*
* EcoBoard © 2022 by Pierre Amey is licensed under CC BY-NC-SA 4.0
* That file is under development.
*
* ! Feel free to collaborate and make it better !
*
* The follwing code can be used without garranty.
*
*/


/*TODO
* Check and do all _save_config
*/
#include "Arduino.h"
#include "Ecoboard.h"
#include "RTClib.h"


Ecoboard::Ecoboard()
{
  Ecoboard(false, false, true, false);
}

Ecoboard::Ecoboard(bool isSdEnable, bool isRTCEnable, bool print, bool debug)
{
	// GENERAL
  _debug = debug;
  _print = print;
  _isRTCEnable = isRTCEnable;                 // used to store the RTS status
  _isSdEnable = isSdEnable;                  // used to store the status of the card
  _isSdReady = false;                        // used to check if the card is raedy or not. If the SD crad is not inserted, the value is Not Ready (dalse)
	
  // SD CARD
  _carddetect = 7;                           // used as the MicroSD card CD (card detect)
  _chipselect = 4;                           // used as the MicroSD card CS (chip select) pin
}


void Ecoboard::begin()
{
	Serial.println(F("Welcome with EcoSensors"));
	Serial.println("");

  
  if(!_isRTCEnable && _debug)
    Serial.println(F("RTC is disable"));

}

// ===========================================
// SD
// ===========================================

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
  
  if(!_isSdEnable && _debug)
    Serial.println(F("SD is disable"));


  byte c=1;                                                     // used to count the attend to start the SD crard
  do
  { 
    if (!_sd.begin(_chipselect, SD_SCK_MHZ(12)))                                // INITIALIZE and check the SD card
    {
      if(_debug)
      {
        Serial.print(c);
        Serial.println(F(": Attending to detect the SD card"));
      }

      _isSdReady = false;                                       // The status must remind false
      c++;                                                      // Increment the lopping count
      delay(1000);                                              // Give a delay of 1 sec
    }
    else                                                        // the cond return true
    {
      _isSdReady = true;                                         // Change the status. The SD is ready, then change the value of _isSdReady to true
    }

  }while(_isSdReady == false && c <= 3);                          // If the _isSdReady is always False, loop only 3 timne then, exit and continue
  
  if(_isSdReady == false)                                       // If the card is not ready
  {
    _isSdEnable = false;                                        // Consider the card as disabke
    _logger = false;                                            // No log into the card is possible

    return _isSdReady;
  }
  else
  {
    _sd.chdir();                                                // If the card is ready, chdir to the root
  }

  if (_debug)
      Serial.println(F("SD is ready"));

  return _isSdReady;
}


/*
* Init the card to log the activities of the board
* You need a RTC clock
*
* 3: SD is enable but not the RTC
* 2: SD disable
* 1: OK
* 0: Failed chdir root
* -1: Could not create LOG folder
* -2: Failed chdir log
* -3: Failed mkdir year
* -4: Failed chdir year
* -5: Failed mkdir month
* -6: Failed chdir month
* -7: Failed mkdir day
* -8: Failed chdir day
*/


int Ecoboard::sd_init_log(int16_t y, int16_t m, int16_t d, int16_t h, int16_t mn, int16_t s)
{
  if(_isSdEnable == false)
    return 2;

  if(_debug)
    Serial.println(F("# Init the log file..."));

  if( _isRTCEnable == false){
    if(_debug)
      Serial.println(F("\t RTC is not enable. The log file can not initilized"));

    return 3;
  }

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
    Serial.println(F("\tPrepare the log file..."));

  if(!_sd.chdir())
  {
    Serial.println(F("\tCould not chdir root"));
    return 0;
  }

  if(!_sd.exists("/LOG"))
  {
    if(!_sd.mkdir("/LOG"))
    {
      Serial.println(F("\tFolder /LOG Could't be created"));
      return -1;
    }
  }

  if (!_sd.chdir("/LOG"))
  {
    Serial.println(F("\tFailed chdir LOG"));
    return -2;
  }
  
  // YEAR
  if(!_sd.exists(yy))
  {

    if(!_sd.mkdir(yy))
    {
      Serial.print(F("\tFailed mkdir year dir "));
      Serial.println(yy);
      return -3;
    }
  }

  // Change volume working directory
  if (!_sd.chdir(yy)) {
    Serial.print(F("\tFailed chdir the year dir "));
    Serial.println(yy);
    return -4;
  }

  // MONTH
  if(!_sd.exists(mm))
  {
    if(!_sd.mkdir(mm))
    {
      Serial.print(F("\tFailed mkdir month dir "));
      Serial.println(mm);
      return -5;
    }
  }

  // Change volume working directory
  if (!_sd.chdir(mm)) {
    Serial.print(F("\tFailed chdir month dir "));
    Serial.println(mm);
    return -6;
  }

  // DAY
  if(!_sd.exists(dd))
  {
    if(!_sd.mkdir(dd))
    {
      Serial.print(F("\tFailed mkdir day dir "));
      Serial.println(dd);
      return -7;
    }
  }
  // Change volume working directory.
  if (!_sd.chdir(dd))
  {
    Serial.print(F("\tFailed chdir day dir "));
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
    Serial.println(F("\t Could not chdir ROOT after creating the struct"));
    return 0;
  }

  //_sd.chdir(); ??

  if(!_sd.chdir(_sd_pathLog))
  {
    if(_debug){
      Serial.print(F("\tCould not chdir /LOG"));
      Serial.print(F(yy));
      Serial.print(F("/"));
      Serial.print(F(mm));
      Serial.print(F("/"));
      Serial.print(F(dd));
      Serial.println(F("/"));
    }
    return 0;
  }

  Serial.print(F("\tLog Folder: "));
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
  	
   	// When the SD card is removed digitalRead(CARDDETECT) does not return 0 for a unknow reason 
   	// The script is temporarely stopped (because of while(1)).
   	// When the reason will be know and the problem solved, change this part of code

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

/*
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
*/

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

  	if(isWrite == true)
    	return 1;
  	else
    	return 0;
}



// ===========================================
// RTC
// ===========================================
void Ecoboard::RtcGetTime(int16_t &y, int16_t &m, int16_t &d, int16_t &h, int16_t &mn, int16_t &s, char * date_time)
{
  int32_t last_tx;
  RtcGetTime(y, m, d, h, mn, s, last_tx, date_time, true);
}
void Ecoboard::RtcGetTime(int16_t &y, int16_t &m, int16_t &d, int16_t &h, int16_t &mn, int16_t &s, int32_t &unix_time, char * date_time)
{
  RtcGetTime(y, m, d, h, mn, s, unix_time, date_time, true);
}
void Ecoboard::RtcGetTime(int16_t &y, int16_t &m, int16_t &d, int16_t &h, int16_t &mn, int16_t &s, int32_t &unix_time, char * date_time, bool debug)
{
  /* TODO if the day changed we have to change the log file (See: sd_init_logFile)
  */
  DateTime now = rtc.now();

  y = now.year();
  y = y-2000;
  m = now.month();
  d = now.day();
  h = now.hour();
  mn = now.minute();
  s = now.second();

  sprintf(date_time,"%i-%i-%i %i:%i:%i",y,m,d,h,mn,s);
   
  if(debug==true)
  {
  sprint(y,0);
  sprint(F("/"),0);
    
  sprint(m,0);
  sprint(F("/"),0);
    
  sprint(d, 0);
  sprint(F(" ("),0);
  sprint(daysOfTheWeek[now.dayOfTheWeek()],0);
  sprint(F(") "),0);
  sprint(F(" "),0);
    
  sprint(h, 0);
  sprint(F(":"),0);
    
  sprint(mn, 0);
  sprint(F(":"),0);
    
  sprintln(s, 0);

  sprint(F("Since midnight 1/1/1970 = "),0);
  }
  unix_time = now.unixtime();
  if(debug==true)
  {
  sprintln(unix_time,0);
  }
}



void Ecoboard::RtcGetUnixTime(int32_t &unix_time)
{
  DateTime now_u = rtc.now();
  unix_time = now_u.unixtime();
}


bool Ecoboard::RtcBegin()
{
  if(rtc.begin())
  {
    /*
    _sd.chdir();
    
    if (!_sd.exists(_config_rtc_file))
    {
      Serial.print(_config_rtc_file);
      Serial.print(F(" does not exist"));
      _config_rtc.year = 0;
      _config_rtc.month = 0;
      _config_rtc.day = 0;
      _config_rtc.hour = 0;
      _config_rtc.minute = 0;
      _config_rtc.second = 0;

      if(_save_Config_rtc(_config_rtc_file, false) == 1)
      {
        sprint(_config_rtc_file,0);
        sprintln(F(" created."),0);
      }
      else
      {
        sprint(_config_rtc_file,0);
        sprintln(F(" failed to be created...."),0);
        return false; 
      }
    }
    */
    
    return true;
  }
  else
  {
    return false;
  }

}

int Ecoboard::RtcCalibrate()
{
  if(_isSdEnable == false)
    return 2;

//  if(_load_Config_rtc(_config_rtc_file) == 1)
//  {
    //(We can not use the second paramet with the value of 2 sptint(,2), because this action is done before the SD is ready)
    sprint(F("\tYear: "),0);
    sprintln(_config_rtc.year,0);
    sprint(F("\tMonth: "),0);
    sprintln(_config_rtc.month,0);
    sprint(F("\tDay: "),0);
    sprintln(_config_rtc.day,0);
    sprint(F("\tHour: "),0);
    sprintln(_config_rtc.hour,0);
    sprint(F("\tMinute: "),0);
    sprintln(_config_rtc.minute,0);
    sprint(F("\tSecond: "),0);
    sprintln(_config_rtc.second,0);
    
    if(_config_rtc.year == 0 && _config_rtc.month == 0 && _config_rtc.day == 0 && _config_rtc.hour == 0 && _config_rtc.minute == 0 && _config_rtc.second == 0)
    {
      return -2;
    }
    else if(_config_rtc.year != 1970 || _config_rtc.month != 1 || _config_rtc.day != 1 || _config_rtc.hour != 0 || _config_rtc.minute != 0 || _config_rtc.second != 0)
    {
      sprintln(F("Calibrating the RTS DS3231"),0);
      rtc.adjust(DateTime(_config_rtc.year, _config_rtc.month, _config_rtc.day, _config_rtc.hour, _config_rtc.minute, _config_rtc.second));
      delay(100);

      //TODO
      //_save_Config_rtc(_config_rtc_file, true); // True init the file to 1970-1-1 0:0:0 
      delay(100);
      //TODO
      //_load_Config_rtc(_config_rtc_file);
      
      sprint(F("\tYear: "),0);
      sprintln(_config_rtc.year,0);
      sprint(F("\tMonth: "),0);
      sprintln(_config_rtc.month,0);
      sprint(F("\tDay: "),0);
      sprintln(_config_rtc.day,0);
      sprint(F("\tHour: "),0);
      sprintln(_config_rtc.hour,0);
      sprint(F("\tMinute: "),0);
      sprintln(_config_rtc.minute,0);
      sprint(F("\tSecond: "),0);
      sprintln(_config_rtc.second,0);

      return 1;
    }
    else
    {
      return 0;
    }
  //} return -1;
}


bool Ecoboard::RtcCalibrate(int16_t &y, int16_t &m, int16_t &d, int16_t &h, int16_t &mn, int16_t &s)
{
  sprintln(F("Calibrating the RTS DS3231 (int16_t)"),0);
  rtc.adjust(DateTime(y, m, d, h, mn, s));
  return true;
}

bool Ecoboard::RtcLostPower()
{
  if (rtc.lostPower())
  {
    sprintln(F("RTC lost power, lets set the time!"),0);
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    return true;
  }
  else
  {
    return false;
  }
}

bool Ecoboard::RtcInterval(int32_t lastTx, int32_t tx_interval, bool datetime_active, int debug)
{ 
    if(debug==1){
      Serial.print(F("******\r\nlastTx: "));
      Serial.println(lastTx);
      Serial.print(F("tx_int: "));
      Serial.println(tx_interval);
      Serial.println();
    }
    
    int32_t nextTx = lastTx + tx_interval;
    
    int32_t unix_t;

    //Serial.print(F("NextTx:"));
    //Serial.println(nextTx);
    
    if(datetime_active)
    {
      DateTime t = rtc.now();
      unix_t = t.unixtime();
    }else{
      unix_t = millis()/1000;
    }
    /*
    Serial.print(F("unix_t:"));
    Serial.println(unix_t);
    Serial.println(F("***** Done *****\r\n"));
    */
    
    if(debug==1){
      Serial.print("unix_t: "); Serial.print(unix_t);
      Serial.print(F(" - "));
      Serial.print("Next tx: "); Serial.println(nextTx);
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


// ===========================================
// PRINTS
// ===========================================

void Ecoboard::sprint(int message, int base, int logToSd)
{
  if (logToSd == 0 || logToSd == 2)
  {
    if(_print)
    {
      Serial.print(message,base);
    }
  }

  if (logToSd == 1 || logToSd == 2)
  {
    if(_logger)
    {
      //Serial.print("Check here:");
      char mess[10];
      snprintf(mess,sizeof(mess), "%d", message);
      //Serial1.print(message, 'HEX');
      _sd_write(_logFile, mess);
    }
  }
}


void Ecoboard::sprint(int message, int logToSd)
{
  if (logToSd == 0 || logToSd == 2)
  {
    if(_print)
    { 
      Serial.print(message);
    }
  }

  if (logToSd == 1 || logToSd == 2)
  {
    if(_logger)
    {
      char mess[10];
      snprintf(mess, sizeof(mess), "%d", message);
      //Serial1.print(message, 'DEC');
      _sd_write(_logFile, mess);
   }
  }
}

void Ecoboard::sprint(uint32_t message, int logToSd)
{
  if (logToSd == 0 || logToSd == 2)
  {
    if(_print)
    {
      Serial.print(message);
    }
  }

  if (logToSd == 1 || logToSd == 2)
  {
    if(_logger)
    {
      char mess[15 ];
      snprintf(mess, sizeof(mess), "%d", message);
      _sd_write(_logFile, mess);
    }
  }
}

void Ecoboard::sprint(int32_t message, int logToSd)
{
  if (logToSd == 0 || logToSd == 2)
  {
    if(_print)
    {
      Serial.print(message);
    }
  }

  if (logToSd == 1 || logToSd == 2)
  {
    if(_logger){
      char mess[15 ];
      snprintf(mess, sizeof(mess), "%d", message);
      _sd_write(_logFile, mess);
    }
  }
}

void Ecoboard::sprint(uint16_t message, int logToSd)
{
  if (logToSd == 0 || logToSd == 2)
  {
    if(_print)
    {
      Serial.print(message);
    }
  }

  if (logToSd == 1 || logToSd == 2)
  {
    if(_logger)
    {
      char mess[15];
      snprintf(mess, sizeof(mess), "%d", message);
      _sd_write(_logFile, mess);
    }
  }
}

void Ecoboard::sprint(int16_t message, int logToSd)
{
  if (logToSd == 0 || logToSd == 2)
  {
    if(_print)
    {
      Serial.print(message);
    }
  }

  if (logToSd == 1 || logToSd == 2)
  {
    if(_logger)
    {  
      char mess[15];
      snprintf(mess, sizeof(mess), "%d", message);
      _sd_write(_logFile, mess);
    }
  }
}

void Ecoboard::sprint(uint8_t message, int logToSd)
{
  if (logToSd == 0 || logToSd == 2)
  {
    if(_print)
    { 
      Serial.print(message);
    }
  }

  if (logToSd == 1 || logToSd == 2)
  {
    if(_logger)
    {
      char mess[10];
      snprintf(mess, sizeof(mess), "%d", message);
      _sd_write(_logFile, mess);
    }
  }
}



void Ecoboard::sprint(int8_t message, int logToSd)
{

  if (logToSd == 0 || logToSd == 2)
  {
    if(_print)
    {
      Serial.print(message);
    }
  }


  if (logToSd == 1 || logToSd == 2)
  {
    if(_logger)
    {
      char mess[10];
      snprintf(mess, sizeof(mess), "%d", message);
      _sd_write(_logFile, mess);
    }
  }
}

void Ecoboard::sprint(const __FlashStringHelper * message, int logToSd)
{
  if (logToSd == 0 || logToSd == 2)
  {
    if(_print)
    { 
      Serial.print(message);
    }
  }

  if (logToSd == 1 || logToSd == 2)
  {
    if(_logger)
    {
      _sd_write(_logFile, message);
    }
  }
}

void Ecoboard::sprint(char const * message, int logToSd)
{
  if (logToSd == 0 || logToSd == 2)
  {
    if(_print)
    {
      Serial.print(message);
    }
  }

  if (logToSd == 1 || logToSd == 2)
  {
    if(_logger)
    {
      _sd_write(_logFile, message);
    }
  }
}

void Ecoboard::sprintln(double message, int logToSd)
{

  if (logToSd == 0 || logToSd == 2)
  {
    if(_print)
    {
      Serial.println(message);
    }
  }

  if (logToSd == 1 || logToSd == 2)
  {
    if(_logger)
    {
      char mess[15];
      snprintf(mess, sizeof(mess), "%d", message);
      _sd_writeln(_logFile, mess);
    }
  }
}


void Ecoboard::sprint(double message, int logToSd)
{

  if (logToSd == 0 || logToSd == 2)
  {
    if(_print)
    { 
      Serial.print(message);
    }
  }

  if (logToSd == 1 || logToSd == 2)
  {
    if(_logger)
    {
      char mess[15];
      snprintf(mess, sizeof(mess), "%d", message);
      _sd_write(_logFile, mess);
    }
  }
}

void Ecoboard::sprintln(int message, int base, int logToSd)
{
  if (logToSd == 0 || logToSd == 2)
  {
    if(_print)
    {  
      Serial.println(message,base);
    }
  }

  if (logToSd == 1 || logToSd == 2)
  {
    if(_logger)
    {
      //Serial.print("Check here:");
      char mess[10];
      snprintf(mess, sizeof(mess), "%d", message);
      //Serial1.print(message, 'HEX');
      _sd_writeln(_logFile, mess);
    }
  }
}


void Ecoboard::sprintln(int message, int logToSd)
{

  if (logToSd == 0 || logToSd == 2)
  {
    if(_print)
    { 
      Serial.println(message);
    }
  }

  if (logToSd == 1 || logToSd == 2)
  {
    if(_logger)
    {
      char mess[10];
      snprintf(mess, sizeof(mess), "%d", message);
      _sd_writeln(_logFile, mess);
    }
  }
}

void Ecoboard::sprintln(uint32_t message, int logToSd)
{

  if (logToSd == 0 || logToSd == 2)
  {
    if(_print)
    { 
      Serial.println(message);
    }
  }

  if (logToSd == 1 || logToSd == 2)
  {
    if(_logger)
    {   
      char mess[15];
      snprintf(mess, sizeof(mess), "%d", message);
      _sd_writeln(_logFile, mess);
    }
  }
}

void Ecoboard::sprintln(uint16_t message, int logToSd)
{

  if (logToSd == 0 || logToSd == 2)
  {
    if(_print)
    { 
      Serial.println(message);
    }
  }

  if (logToSd == 1 || logToSd == 2)
  {
    if(_logger)
    {
      char mess[15];
      snprintf(mess,sizeof(mess), "%d", message);
      _sd_writeln(_logFile, mess);
    }
  }
}

void Ecoboard::sprintln(int16_t message, int logToSd)
{

  if (logToSd == 0 || logToSd == 2)
  {
    if(_print)
    {  
      Serial.println(message);
    }
  }

  if (logToSd == 1 || logToSd == 2)
  {
    if(_logger)
    {
      char mess[10];
      snprintf(mess, sizeof(mess), "%d", message);
      _sd_writeln(_logFile, mess);
    }
  }
}

void Ecoboard::sprintln(int32_t message, int logToSd)
{

  if (logToSd == 0 || logToSd == 2)
  {
    if(_print)
    {  
      Serial.println(message);
    }
  }

  if (logToSd == 1 || logToSd == 2)
  {
    if(_logger)
    {
      char mess[10];
      snprintf(mess, sizeof(mess), "%d", message);
      _sd_writeln(_logFile, mess);
    }
  }
}

void Ecoboard::sprintln(uint8_t message, int logToSd)
{

  if (logToSd == 0 || logToSd == 2)
  {
    if(_print)
    { 
      Serial.println(message);
    }
  }

  if (logToSd == 1 || logToSd == 2)
  {
    if(_logger)
    {
      char mess[10];
      snprintf(mess, sizeof(mess), "%d", message);
      _sd_writeln(_logFile, mess);
    }
  }
}


void Ecoboard::sprintln(char * message, int logToSd)
{

  if (logToSd == 0 || logToSd == 2)
  {
    if(_print)
    { 
      Serial.println(message);
    }
  }

  if (logToSd == 1 || logToSd == 2)
  {
    if(_logger)
    {
      _sd_writeln(_logFile, message);
    }
  }
}


void Ecoboard::sprintln(const __FlashStringHelper * message, int logToSd)
{

  if (logToSd == 0 || logToSd == 2)
  {
    if(_print)
    { 
      Serial.println(message);
    }
  }

  if (logToSd == 1 || logToSd == 2)
  {
      if(_logger)
    {
      _sd_writeln(_logFile, message);
    }
  }
}

void Ecoboard::sprintln(char const * message, int logToSd)
{

  if (logToSd == 0 || logToSd == 2)
  {
    if(_print)
    { 
      Serial.println(message);
    }
  }

  if (logToSd == 1 || logToSd == 2)
  {
    if(_logger)
    {
      _sd_writeln(_logFile, message);
    }
  }
}


void Ecoboard::printFloat(float value, int places) {

  // this is used to cast digits
  int digit;
  float tens = 0.1;
  int tenscount = 0;
  int i;
  float tempfloat = value;

  // make sure we round properly. this could use pow from <math.h>, but doesn't seem worth the import
  // if this rounding step isn't here, the value  54.321 prints as 54.3209

  // calculate rounding term d:   0.5/pow(10,places)
  float d = 0.5;
  if (value < 0)
    d *= -1.0;
  // divide by ten for each decimal place
  for (i = 0; i < places; i++)
    d /= 10.0;
  // this small addition, combined with truncation will round our values properly
  tempfloat += d;

  // first get value tens to be the large power of ten less than value
  // tenscount isn't necessary but it would be useful if you wanted to know after this how many chars the number will take

  if (value < 0)
    tempfloat *= -1.0;
  while ((tens * 10.0) <= tempfloat) {
    tens *= 10.0;
    tenscount += 1;
  }


  // write out the negative if needed
  if (value < 0) {
    sprint("-", 0);
  }

  if (tenscount == 0)
    sprint(0, 0);

  for (i = 0; i < tenscount; i++) {
    digit = (int) (tempfloat / tens);
    sprint(digit, 0);
    tempfloat = tempfloat - ((float)digit * tens);
    tens /= 10.0;
  }

  // if no places after decimal, stop now and return
  if (places <= 0)
    return;

  // otherwise, write the point and continue on
  sprint(".", 0);

  // now write out each decimal place by shifting digits one by one into the ones place and writing the truncated value
  for (i = 0; i < places; i++) {
    tempfloat *= 10.0;
    digit = (int) tempfloat;
    sprint(digit, 0);
    // once written, subtract off that digit
    tempfloat = tempfloat - (float) digit;
  }

}



// ===========================================
// Other function
// ===========================================