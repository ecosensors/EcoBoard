# RTC and sleep mode

In this exercice, you will see how to use a RTC clock and how to make you board in sleep mode.

## Materials
* [RTC DS3132](https://www.adafruit.com/product/3013)

## Libraries
Make sure you have the needed libraries in SKETSCHBOOK/Libraries/

Open Arduino IDE, and then go to **Sketch -> Add Library -> Manage Libraries** 

Search and install the following libraries:

* RTClib by Adafruit
* Arduino Low Power by Arduino
* RTCZero (depemdemcy for Arduino Low Power )


Close the Arduino IDE and reopen it.

## Wire

Connect the BME280 and the RTC boards into the two I2C connectors

Board | RTC
--- | ---
3V3 | Vin
GND | GND
SDA | SDA
SLC | SLC