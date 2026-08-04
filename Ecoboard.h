/*
* EcoBoard © 2022 by Pierre Amey is licensed under CC BY-NC-SA 4.0
* That file is underdevelopment.
*
* ! Feel free to collaborate and make it better !
*
* The follwing code can be used without garranty.
*
*/

#ifndef Ecoboard_h
#define Ecoboard_h
#include "Arduino.h"


//SD
#define SD_FAT_TYPE 3 // 1 for FAT16/FAT32, 2 for exFAT, 3 for FAT16/FAT32 and exFAT.
#include <FreeStack.h> // For SdFAT
#include <SdFat.h>
//#include <SdFatConfig.h>
#include <sdios.h>

#include "RTClib.h"

#define SD_PATHSIZE 20
#define FILESIZE 12

struct Config_rtc{
	int16_t year;
    int16_t month;
    int16_t day;
    int16_t hour;
    int16_t minute;
    int16_t second;
 };


class Ecoboard{
 	protected:
 		

 	private:
 		
 		bool _debug;
 		bool _print;

 		Config_rtc _config_rtc;

 		/* RTC */
 		bool _isRTCEnable;
		
 		/*SD Card */
 		#if SD_FAT_TYPE == 0
		  SdFat _sd;
		  File _file;
		  File _root;
		#elif SD_FAT_TYPE == 1
		  SdFat32 _sd;
		  File32 _file;
		  File32 _root;
		#elif SD_FAT_TYPE == 2
		  SdExFat _sd;
		  ExFile _file;
		  ExFile _root;
		#elif SD_FAT_TYPE == 3
		  SdFs _sd;
		  FsFile _file;
		  FsFile _root;
		#endif  // SD_FAT_TYPE

		bool _logger;
 		bool _isSdEnable;
 		bool _isSdReady;
 		int _carddetect;
		int _chipselect;
		char _sd_pathLog[SD_PATHSIZE];														// store the path to the log file
		char _logFile[SD_PATHSIZE+FILESIZE];	
													// store the logfile name
		//void _sd_showCwd();
		
		bool _sd_checkCard();
		int _sd_writeln(char * fileName, char const * text);								
		int _sd_write(const char * fileName, char const * text);							
		int _sd_write(char * fileName, const __FlashStringHelper * text);
		int _sd_writeln(char * fileName, const __FlashStringHelper * text);
		int _sd_write(const char * fileName, const __FlashStringHelper * text, bool ln);
		int _sd_write(const char * fileName, char const * text, bool ln);
	

	public:

		Ecoboard();
		Ecoboard(bool isSdEnable, bool isRTCEnable, bool print, bool debug);
		void begin();
		bool sd_begin();

		RTC_DS3231 rtc;
		bool RtcBegin();
    	void RtcGetTime(int16_t &y, int16_t &m, int16_t &d, int16_t &h, int16_t &mn, int16_t &s, char * date_time);
		void RtcGetTime(int16_t &y, int16_t &m, int16_t &d, int16_t &h, int16_t &mn, int16_t &s, int32_t &unix_time, char * datetime);
		void RtcGetTime(int16_t &y, int16_t &m, int16_t &d, int16_t &h, int16_t &mn, int16_t &s, int32_t &unix_time, char * datetime, bool debug);
		void RtcGetUnixTime(int32_t &unix_time);
		bool RtcInterval(int32_t lastTx, int32_t tx_interval, bool datetime_active, int debug);
		bool RtcLostPower();
		int RtcCalibrate();
		bool RtcCalibrate(int16_t &y, int16_t &m, int16_t &d, int16_t &h, int16_t &mn, int16_t &s);
		bool isDs3231;
		char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};



		// SD Card
		//void listFiles(const char * folder);
		
		int sd_init_log(int16_t y, int16_t m, int16_t d, int16_t h, int16_t mn, int16_t s);
		/*
		int sd_writeln(char const * text);
		int sd_write(char const * text);
		int sd_write(int16_t text);
		*/

		// RPINT
		void sprint(int message, int base, int logToSd);
		void sprint(int message, int logToSd);
		void sprint(uint32_t message, int logToSd);
		void sprint(int32_t message, int logToSd);
		void sprint(int16_t message, int logToSd);
		void sprint(uint16_t message, int logToSd);
		void sprint(uint8_t message, int logToSd);
		
		//void sprint(long message, int logToSd);
		
		void sprint(int8_t message, int logToSd);
		void sprint(const __FlashStringHelper * message, int logToSd);
		void sprint(char const * message, int logToSd);
		void sprintln(double message, int logToSd);
		//void sprint(float message, int logToSd);
		//void sprintln(float message, int logToSd);
		void sprint(double message, int logToSd);
		void sprintln(int message, int base, int logToSd);
		void sprintln(int message, int logToSd);
		void sprintln(uint32_t message, int logToSd);
		void sprintln(int32_t message, int logToSd);
		void sprintln(int16_t message, int logToSd);
		void sprintln(uint16_t message, int logToSd);
		void sprintln(uint8_t message, int logToSd);
		void sprintln(char * message, int logToSd);
		void sprintln(const __FlashStringHelper * message, int logToSd);
		void sprintln(char const * message, int logToSd);
		void printFloat(float value, int places);
 };
 #endif