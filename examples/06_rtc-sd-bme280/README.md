# RTC / SD / BME280

> The BME280 and the RTC colck are ready. I am now writtting the script to log the measure into the SD card.

In this exercice, You are going to log the measures of the temperature, the humidity, altitude and the pressure (BME250) into a SD card. The data will be saved in **a JSON Line format**. You are going to use a RTC clock chip to log the measures in a regular time into a SD card.

The log files will saved into the folder '/log/year/month/day/'

The exercice has been written ans tested for the EcoBoard, but you can use for almost all MO and Arduino board.

The examples are distributed WITHOUT WARRANTY

**Feel free to collaborate and share suggestions for improvement** :)

> NOTE: JSON Lines (also called newline-delimited JSON) is a convenient format for storing structured data that may be processed one record at a time. It works well with unix-style text processing tools and shell pipelines. It's a great format for log files. It's also a flexible format for passing messages between cooperating processes. 

## Prequisite
* Look at the [01_bme280](https://github.com/ecosensors/EcoBoard/tree/master/examples/01_bme280) example
* look at the [05_rtc_lowpower](https://github.com/ecosensors/EcoBoard/tree/master/examples/05_rtc-lowpower) example

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
* Arduinojson by Benoit Blanchon

### Dependencies
The libraries need some dependencies:

* Adafruit BusIO
* Adafruit Unified Sensor


Close the Arduino IDE and reopen it.

## Sketch
* **IMPORTANT** It's necessary to change the SPI clock speed `SD_SCK_MHZ(50)` to `SD_SCK_MHZ(12)`
* Note: The DS3231 has a default I2C address of 0x68

## Wire

Connect the BME280 and the RTC boards into the two I2C connectors

Board | BME280 | RTC
--- | --- | ---
3V3 | VIN | Vin
GND | GND | GND
SDA | SDI | SDA
SLC | SCK | SLC