# RTC / SD / BME280

In that exercice, you will se how to use a RTC clock (DS3231), a SD card and barometer sensor (BME280).

You are going to take some measures with BME280 sensor and you are going to log the value into a SD card with date and the time.

If you do not have a RTC clock, you will be able to inactive it and save the measures into the SD card without the time.

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

## Wire

Board | BME280
--- | ---
3V3 | VCC
GND | GND
SDA | SDA
SLC | SLC 

