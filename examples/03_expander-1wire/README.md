# GPIO Port Expander & 1-Wire (DS18B20)

This is an example to use PCF8574 (GPIO port expander) with the EcoBoard
You can easly use that example for an other application, but the code and the explication below are written for
EcoBoard board.

## GPIO Port Expander

EcoBoard allow you to use 6 additonal GPIO with the PCF8574 ([Datasheeet](https://www.ti.com/product/PCF8574). The outputs P1 to P6, will allow you to control the status of another device or an LED. However, the outputs can only provide 25mA. In some cases, this power may not be sufficient. It's the reason why, we added three MOSFET-P (IRML2244) on the outputs P0, P1, P2 and P3. 
Thus, the outputs P0, P1, P2 and P3 of the J0, J1, J2 and J3 connectors can now supply up to 500mA. 
P4, P5 and P6 outputs depend on the PCF8574.
P4 and P5 are connected to a LED D5 and D6

You wil see how to turn on/off the two LED connected to the PCF8574 (p5 & P6)

P4 is used to power the 1-Wire sensor.

## 1-Wire - Ds18B20 sensor
We will play with the DS18B20 sensor (Waterproof temperature sensor).

You can check the [EcoDs18b20 library](https://github.com/ecosensors/EcoDs18b20), as well. There is a very interresting section about the compensated temperature value, that you can apply here.

### Datasheet
* Operating voltage : 3.0 - 5.5VDC
* Precision (-10°C - +85°C) : ±0.5°C.
* Measuring range : –55°C à +125°C

### Sensor wire
Color | Pin
--- | ---
RED | 3V3
BLUE | GND
YELLOW | READ (with a 4.7K pullup)

Note: The color can be different for some sensors. Look at the datasheet

## Requirement
* Solder the Jumper JP_4 to P4 (not to 3V3)
* Solder the Jumper JP4 to UP (not DOWN) to have a 4.7kOhm pullup resistence on the READ pin (A4)

## WIRE
Connect the sensor to J4

**Important:** I used the DS18B20 to measure the soil temprature. It's important to never permanetly power the sensor. In other word, the sensor has to be powered only during a measure. It's the reason why, P4 of the GPIO port expender will power (or not) the sensors when needed.


* The red wire (POWER) is connected to pin P4 of the PCF8574. It will power the sensor when we need to ake a measure
* The yellow wire (READ) is connected to A4 of the ATSAM21 microcontroller **With a 4.7K pullup resistence**


## Libraries
Make sure you have the needed libraries in SKETSCHBOOK/Libraries/

Open Arduino IDE, and then go to **Sketch -> Add Library -> Manage Libraries** 

Search and install

* Adafruit PCF8574 by Adafruit
* Adafruit BusIO (dependencies)
* OneWire

Close and reopen Arduino IDE
```
#include <Wire.h>                         // Need for I2C Bus
```

