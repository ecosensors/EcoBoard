/*
 * EcoSensors
 * 
 * In  that example, we will show you how to quickly start with real Time Clock (RTC) 
 * and the ARDUINOLOWPOWER to save battery. we also will show to wakeup the module with a press button
 * 
 * Do not forget to insert a CR1225 cell coin battery
 * 
 * Feel free to propose improvement
 */
 
#include <Ecoboard.h>
Ecoboard Eco;
#include <Wire.h>           // Need for I2C Bus

/*
 * LowPower
 */
 #include <ArduinoLowPower.h>
 const int timeSleep = 5000;    // ms for ARDUINOLOWPOWER
 const int led = 8;             // Pin where the LED is connected
 const int btn = 13;            // Pin where the button is connected
 volatile bool flagBtn = false; // Flag true when the button has been pressed


/*
 * RTC
 */
#include <RTClib.h>                 // Include library
 RTC_DS3231 rtc;
const int32_t interval = 30;      // Interval before sleeping (second)
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
int16_t y,m,d,h,mn,s;               // Used for RTC time
int32_t unix_time;
int32_t lastTx;                     // Record the time of the last send (unix format)
char date_time[18];                 //1010-10-10 11:11:11

/*
 * If it's the first time you run the DS3231 or if you want to modify the time 
 * Uncomment #define RTC_CALIBRATE to calibrate/set the time
 * and define here the time.
 * 1. Upload the script and run your board
 * 2. Sitch off the board
 * 3. Re-comment #define RTC_CALIBRATE
 * 4. Upload the script
 */
//#define RTC_CALIBRATE             // Uncomment to calibrate/set the time 
int16_t annee = 2020;               // year 
int16_t mois = 6;                   // month
int16_t jour = 23;                  // day
int16_t heure = 22;                 // hour
int16_t minutes = 42;               // minutes
int16_t secondes = 00;              // seconds

 
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Wire.begin();  

  delay(8000);
  Eco.begin();
  
  Serial.println("     ECOBOARD     ");
  Serial.println(" Rtc & Low power  ");
  Serial.println("------------------");
  Serial.println("");
  

  /*
   * RTC
   */
  Serial.println(F("START RTC"));

  if(!rtc.begin())
  {
    Serial.println(F("ERROR: Could not start RTC"));
    while(true);
  }

  // If it's the first time you run the DS3231
  #ifdef RTC_CALIBRATE   
      rtc.adjust(DateTime(annee, mois, jour, heure, minutes, secondes));
      Serial.println(F("The RTC has been calibrated at "));
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
      Serial.println(F("Switch off the board, comment #define RTC_CALIBRATE, upload the code."));
      while(true);
  #endif

  if (rtc.lostPower())
  {
    Serial.println(F("RTC lost power, lets set the time!"));
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  // Get time
  DateTime now = rtc.now();

  y = now.year();
  m = now.month();
  d = now.day();
  h = now.hour();
  mn = now.minute();
  s = now.second();

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

  Serial.print(F("Since midnight 1/1/1970 = "));
  unix_time = now.unixtime();
  lastTx = unix_time;
  Serial.println(unix_time);
  
  sprintf(date_time,"%i-%i-%i %i:%i:%i",y,m,d,h,mn,s);

  Serial.println("");
  Serial.print(F("Started at: "));
  Serial.println(date_time);


  /*
   * LOWPOWER
   */
    pinMode(led, OUTPUT);
    digitalWrite(led, LOW);
    LowPower.attachInterruptWakeup(btn, triggerBtn, RISING);  // Wakeup when btn is pressed

   //Uncomment this function if you wish to attach a function dummy when RTC wakes up the chip
   //LowPower.attachInterruptWakeup(RTC_ALARM_WAKEUP, dummy, CHANGE);
  
}

void loop() {
  Serial.println(F(""));
  Serial.print(F("The board is going to sleep for "));
  Serial.print(interval);
  Serial.println(F("sec"));
  Serial.println(F("As the board is going to sleep, you will not be able to print to the Serial."));
  Serial.println(F(""));
  Serial.print(F("The board need to wake up each "));
  Serial.print(timeSleep);
  Serial.println(F("ms, to check the unixtime time."));
  Serial.println(F("To monitor the asleep of the board, the LED #8 will flash once, when the board"));
  Serial.println(F("wakes up to check the RTC time, and the LED will flash three times after "));
  Serial.print(interval);
  Serial.println(F("sec of interval."));
  Serial.println(F("Finaly, the LED will flash 5 times, if you pressed the button to wake it up"));
  Serial.println(F(""));
  
  do{  
    LowPower.sleep(timeSleep);
    digitalWrite(led, HIGH);
    delay(100);
    digitalWrite(led, LOW); 
  }while(RtcInterval(lastTx,interval, false) == false && flagBtn == false);

  
  DateTime now = rtc.now();
  lastTx = now.unixtime();

  if(flagBtn)
  {
    delay(100);
    digitalWrite(led, HIGH);
    delay(100);
    digitalWrite(led, LOW); 
    delay(100);
    digitalWrite(led, HIGH);
    delay(100);
    digitalWrite(led, LOW); 
    flagBtn = false;
  }

  delay(100);
  digitalWrite(led, HIGH);
  delay(100);
  digitalWrite(led, LOW); 
  delay(100);
  digitalWrite(led, HIGH);
  delay(100);
  digitalWrite(led, LOW); 

  /*
   * Take some measures :)
   */
 
}

void triggerBtn(){
  flagBtn = true;  
}

bool RtcInterval(int32_t lastTx, int32_t tx_interval, bool debug)
{ 
  int32_t nextTx = lastTx + tx_interval;
  int32_t unix_t;  
    
  DateTime t = rtc.now();

  //Serial.println(now.unixtime());
  unix_t = t.unixtime();
    
  if(debug==true){
    Serial.print(unix_t);
    Serial.print(F(" - "));
    Serial.println(nextTx);
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
