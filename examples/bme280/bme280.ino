/*
 * EcoSensors - BME280 (I2C)
 * 
 * WARNING: This script is presented for informational purposes only and no warranty is given as to its suitability for 
 * your environment
 *
 */

#include <Wire.h>               // Need for I2C Bus

/* Interval */
int32_t lastMeasure = 0;
#define INTERVAL 5000           // = 5sec
unsigned long scheduler;

/* BME280 */
#define SEALEVELPRESSURE_HPA (1013.25)
#include <Adafruit_BME280.h>    // Import the library
Adafruit_BME280 bme;            // Create an instence for I2C bus


void setup(void)
{
  Serial.begin(9600);
  Wire.begin();                 // I2C

  delay(8000);                  // Give a delay giving the time to open the terminak

  Serial.println("ECOBOARD - BME280");
  Serial.println("-----------------");
  Serial.println("");

  /*
  * Initiate BME280 sensor
  */
  Serial.println(F("# Init BME280"));
  bool status;
  status = bme.begin();                 // 0x77 (default addr)
                                          // You can change the I2C addresss as the following: 
                                          // bme.begin(0x76)

  Serial.println(status);

  if (!status) {
    Serial.println(F("# BME280 FAILED!"));
    Serial.println(F("Could not find a valid BME280 sensor, check wiring!"));
    while (1);
  }
  else
  {
    // For different settings, see advancedsetting.ino file in BME280 library examples
    Serial.println(F("\tforced mode, 1x temperature / 1x humidity / 1x pressure oversampling,"));
    Serial.println(F("\tfilter off"));
    bme.setSampling(Adafruit_BME280::MODE_FORCED,
      Adafruit_BME280::SAMPLING_X1, // temperature
      Adafruit_BME280::SAMPLING_X1, // pressure
      Adafruit_BME280::SAMPLING_X1, // humidity
      Adafruit_BME280::FILTER_OFF);
        //,
        //Adafruit_BME280::STANDBY_MS_1000

      Serial.println(F("BME280 DONE!"));
      Serial.println(F(""));
  }
  Serial.println(F("Start measuring!"));
  Serial.println(F("==============="));
}
 
void loop(){
  
  if (millis() > scheduler)               // Interval with ATSAMD21 clock
  {
    scheduler = millis() + INTERVAL;

    // Only needed in forced mode! In normal mode, you can remove the next line.
    bme.takeForcedMeasurement(); // has no effect in normal mode
    Serial.print(F("Temperature = "));
    Serial.print(bme.readTemperature());
    Serial.println(F(" *C"));
  
    Serial.print(F("Pressure = "));
  
    Serial.print(bme.readPressure() / 100.0F);
    Serial.println(F(" hPa"));
  
    Serial.print(F("Approx. Altitude = "));
    Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
    Serial.println(F(" m"));
  
    Serial.print(F("Humidity = "));
    Serial.print(bme.readHumidity());
    Serial.println(F(" %"));
  }
 
}