# SD / RTC / BME280

In this exercice, you will see how to use a RTC clock (DS3231), a SD card and barometer sensor (BME280).

You are going to take some measures with BME280 sensor and you are going to log the value into a SD card with date and the time. The log files will order into folders '/2024/04/10/log.txt'

If you do not have a RTC clock, you will be able to inactive it and save the measures into the SD card without the time. The log file will order one folders '/log/log.txt'.

## Materials
* [RTC DS3132](https://www.adafruit.com/product/3013)
* [BME280](https://www.adafruit.com/product/2652)

## Libraries
Make sure you have the needed libraries in SKETSCHBOOK/Libraries/

Open Arduino IDE, and then go to **Sketch -> Add Library -> Manage Libraries** 

Search and install the following libraries:

* SdFat by Bill Germain
* RTClib by Adafruit
* Adafruit BME280 Library

### Dependencies
The libraries need some dependencies:

* Adafruit BusIO
* Adafruit Unified Sensor


Close the Arduino IDE and reopen it.

## Sketch
**IMPORTANT** It's necessary to change the SPI clock speed `SD_SCK_MHZ(50)` to `SD_SCK_MHZ(12)`

## Wire

Connect the BME280 and the RTC boards into the two I2C connectors

Board | BME280 | RTC
--- | --- | ---
3V3 | VIN | Vin
GND | GND | GND
SDA | SDI | SDA
SLC | SCK | SLC

