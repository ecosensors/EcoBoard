/*
 * EcoSensors
 * 

 */

#include <Ecoboard.h>
Ecoboard Eco;
#include <Wire.h>           // Need for I2C Bus


void setup(void)
{
  Serial.begin(9600);
  Wire.begin();  

  delay(8000);

  Eco.begin();
  
  Serial.println("     ECOBOARD     ");
  Serial.println("      SD card     ");
  Serial.println("------------------");
  Serial.println("");

 // Coming soon

}
 
void loop(){
}
