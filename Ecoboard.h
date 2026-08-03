/*
* EcoBoard © 2022 by Pierre Amey is licensed under CC BY-NC-SA 4.0
* That file is underdevelopment. Do not use it for now
*
* The follwing code can be used without garranty.
I will double check it later with EcoLora
*
*/

#ifndef Ecoboard_h
#define Ecoboard_h



//SD
#include <FreeStack.h> // For SdFAT
#include <SdFat.h>
//#include <SdFatConfig.h>
#include <sdios.h>

#include "Arduino.h"

#define SD_PATHSIZE 20
#define SD_FAT_TYPE 3 // 1 for FAT16/FAT32, 2 for exFAT, 3 for FAT16/FAT32 and exFAT.



#define FILESIZE 12


class Ecoboard{
 	protected:
 		

 	private:
 		
 		bool _debug;

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
		Ecoboard(bool isSdEnable, bool isRTCEnable, bool debug);
		void begin();
		bool sd_begin();

		/* SD Card */
		//void listFiles(const char * folder);
		
		int sd_init_logFile(int16_t y, int16_t m, int16_t d, int16_t h, int16_t mn, int16_t s);
		/*
		int sd_writeln(char const * text);
		int sd_write(char const * text);
		int sd_write(int16_t text);
		*/


 };
 #endif