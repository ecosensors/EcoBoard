/*
 * EcoSensors - BME280 (I2C)
 * 
 * The script is distributed WITHOUT WARRANTY.
 * Feel free to collaborate and share suggestions for improvement
 *
 */

#include <Wire.h>                       // Need for I2C Bus

/* BME280 */
#define SEALEVELPRESSURE_HPA (1013.25)
#include <Adafruit_BME280.h>            // include the library
Adafruit_BME280 bme;                    // Create an object

/* Interval */
int32_t lastMeasure = 0;
#define INTERVAL 5000                   // = 5sec
unsigned long scheduler;

void setup(void)
{
  Serial.begin(9600);
  Wire.begin();                         // I2C

  delay(8000);                          // Give a delay giving the time to open the terminak

  Serial.println("ECOBOARD - BME280");
  Serial.println("-----------------");
  Serial.println("");

  /*
  * Initiate BME280 sensor
  */
  Serial.println(F("# Init BME280"));
  bool status;
  status = bme.begin();                   // 0x77 (default addr)
                                          // You can change the I2C addresss as the following: 
                                          // bme.begin(0x76)


  if (!status) {
    Serial.println(F(".. BME280 FAILED!"));
    Serial.println(F("Could not find a valid BME280 sensor, check wiring!"));
    while (1);
  }
  else
  {
    // For different settings, see advancedsetting.ino file in BME280 library examples
    Serial.println(F("forced mode, 1x temperature / 1x humidity / 1x pressure oversampling,"));
    Serial.println(F("filter off"));
    bme.setSampling(Adafruit_BME280::MODE_FORCED,
      Adafruit_BME280::SAMPLING_X1,         // temperature
      Adafruit_BME280::SAMPLING_X1,         // pressure
      Adafruit_BME280::SAMPLING_X1,         // humidity
      Adafruit_BME280::FILTER_OFF);
        //,
        //Adafruit_BME280::STANDBY_MS_1000
      Serial.println(F("BME280 OK!"));
      Serial.println(F(""));
  }
  Serial.println(F("# Let's go"));

  Serial.print(F("Temperature\t"));
  Serial.print(F("Pressure\t"));
  Serial.print(F("Aprox Alt.\t"));
  Serial.println(F("Humidity\t"));
}
 
void loop(){
  
  if (millis() > scheduler)                 // Interval with ATSAMD21 clock
  {
    scheduler = millis() + INTERVAL;

    // Only needed in forced mode! In normal mode, you can remove the next line.
    bme.takeForcedMeasurement(); // has no effect in normal mode
    Serial.print(bme.readTemperature());
    Serial.print(F(" *C \t"));
  
    Serial.print(bme.readPressure() / 100.0F);
    Serial.print(F(" hPa\t"));
  
    Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
    Serial.print(F(" m\t"));

    Serial.print(bme.readHumidity());
    Serial.println(F(" %"));

  }
}