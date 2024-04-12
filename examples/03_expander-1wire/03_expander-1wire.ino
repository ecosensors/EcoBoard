/*
 * EcoSensors
 * The script is distributed WITHOUT WARRANTY.
 *
 * Read rhe README file for more information about he GPIO port expender and the 1.Wire D18B20 sensor
 */

#include <Wire.h>                     // Required for I2C communication
#include <Adafruit_PCF8574.h>         // Required for PCF8574
//#include <Adafruit_PCF8575.h>
//#include "PCF8574.h"                // Required for PCF8574 (not used)
#include <OneWire.h>                  // Requiered for 1-Wire sensor

#define P1 1    // pin P1
#define P2 2    // pin P2
#define P3 3    // pin P3
#define P4 4    // pin P4               // In the example, P4 will be used to power the 1-Wire sensors (DS18B20 sensor)
#define LED5 5  // pin P5
#define LED6 6  // pin P6

// For 1-Wire sensor & DS18B20
float temperature;
const byte ONEWIRE_READ = 19;           // (18 = Aç)  A4 will be use to read the DS18B20 sensor value
enum DS18B20_RCODES {                   // Status code
  READ_OK,                              // Lecture ok
  NO_SENSOR_FOUND,                      // Pas de capteur
  INVALID_ADDRESS,                      // Adresse reçue invalide
  INVALID_SENSOR                        // Capteur invalide (pas un DS18B20)
};
bool reset_search = true;


//PCF8574 expander;                     // Create PCF8574 object
Adafruit_PCF8574 expander;              // Create PCF8574 object
OneWire ds(ONEWIRE_READ);               // Create OneWire object
  

void setup() {
  Serial.begin(9600);
  Wire.begin();                         // Start I2C

  delay(4000);
  
  Serial.println("ECOBOARD - GPIO port Expender");
  Serial.println("and 1-Wire DS18B20 sensor");
  Serial.println("------------------------------");
  Serial.println("");

  Serial.println("# Starting with PCF8574");

  expander.begin(0x27);                 // Define the I2C address
  expander.pinMode(1,OUTPUT);           
  expander.pinMode(2,OUTPUT);
  expander.pinMode(3,OUTPUT);
  expander.pinMode(4,OUTPUT);           // define the pin mode as Output
  expander.pinMode(5,OUTPUT);
  expander.pinMode(6,OUTPUT);

  for(int p=1; p<7;p++){
    expander.digitalWrite(p,LOW);                                   // Set all outputs LOW
    delay(50);
    Serial.print(F("\tP")); Serial.print(p); Serial.print(F(":"));  // Print the output status
    Serial.println(expander.digitalRead(p) ? "HIGH" : "LOW");
    delay(50);
  }
   
}

void loop() {
  Serial.println("# Testing the LEDs with P5 and P6");
  Serial.println(".. Turn on LED 5");
  expander.digitalWrite(LED5, HIGH);
  delay(1000);
  Serial.println(".. Turn off LED 5");
  expander.digitalWrite(LED5, LOW);
  delay(1000);
  Serial.println(".. Turn on LED 6");
  expander.digitalWrite(LED6, HIGH);
  delay(1000);
  Serial.println(".. Turn off LED 6");
  expander.digitalWrite(LED6, LOW);
  delay(1000);
  Serial.println("");

  /*
  * 1-Wire
  */
  Serial.println(F("# 1-WIRE DS18B20"));
  Serial.println(F(".. Power the DS18B20 sensor with P4"));
  expander.digitalWrite(P4,HIGH);
  delay(100);
  Serial.println(F(".. Read a value at A4"));

  byte data[9], addr[8];
  // data[] : Data read from scratchpad
  // addr[] : Adress of the deteted 1-Wire  
  if (reset_search)                             // Reset the 1-Wire bus (needed for the read of the first sensot)
  {
    ds.reset_search();
  }
  
  if (!ds.search(addr))                         // Serach for next 1-Wire sensor
  {
    expander.digitalWrite(P4,LOW);              // No 1-Wire found, power off DS18B20
    Serial.println(F("No 1-Wire sensor found"));
    //return NO_SENSOR_FOUND;
  }   
      
  if (OneWire::crc8(addr, 7) != addr[7])        // Check the 1-Wire address
  {
    expander.digitalWrite(P4,LOW);              // Address is not valid, power off DS18B20   
    Serial.println(F("Address not valid"));
    //return INVALID_ADDRESS;
  }
 
  if (addr[0] != 0x28)                          // Check if the 1-Wire sensor is a DS18B20
  {
    expander.digitalWrite(P4,LOW);              // Wrong sensor, power off DS18B20 
    Serial.println("Wrong sensor");
    //return INVALID_SENSOR;
  }

  ds.reset();                                   // Reset 1-Wire bus
  ds.select(addr);                              // Select the sensor  
  ds.write(0x44, 1);                            // Take a measure and wait the end of the task
  delay(800);
    
  ds.reset();                                   // Reset 1-Wire bus
  ds.select(addr);                              // Select the sensor
  ds.write(0xBE);                               // Request the read of the scraptcha
   
  for (byte i = 0; i < 9; i++)                  // Raed the scratchpad
  {
    data[i] = ds.read();
  }
     
  
  temperature = ((data[1] << 8) | data[0]) * 0.0625; // Calculate the value Celsius 
 
  // No erros
  expander.digitalWrite(P4,LOW);                // Power off the 1-Wire sensor 
  Serial.print(">> Temperature: ");
  Serial.println(temperature);
  // return READ_OK;
  Serial.println("");

}
