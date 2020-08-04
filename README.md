# EcoBoard
EcoBoard is new microcontroler based on the processor ATSAMD21G18 ARM Cortex M0 at 48Mhz with 3V3 logic, as the Arduino Zero.
The chip has **256K of FLASH and 32K of RAM**. It's fully compatible with Arduino and Adafruit libraries.

We add several useful features
* 64Kb EEPROM
* A MicroSD card
* A Real Time Clock
* 5V Output
* GPIO Port Expander (P0-P4 outpouts and P5 & P6 are connected to a LED (D5 & D6)
* A I2C connector
* A UART connector
* A 1-Wire connector 
* A debuger/programmer connector
* A reset buton
* A programable buton
* A switch ON/OFF Button

The EcoBoard board will be available during september 2020 

> The following page is being writting 

## Examples
We provide several [examples](https://github.com/ecosensors/Ecoboard/tree/master/examples) to quickly start with the EcoBoard. All examples are done for the EcoBoard, but you can easly use it for other Arduino board, with may be, a few modification as the pin definition. Note, the examples are basic and you will probabely improve it for your application. Feel you free to propose correction or improvement, scpecialy if you noticed some errors, or a better way to use the examples. 
(More examples will be added soon)

The board will be available beginning of August 2020, with exemples of Arduino code to easly first use it.

## Pintout

### Header J1 (2x20)
This header is compatible with Raspberry Header and [EcoRadio board](https://github.com/ecosensors/ecoradio)

Pin | Output
--- | ---
1 | 3V3
2 | 5V
3 | SDA 
4 | 5V 
5 | SCL 
6 | GND 
7 | NC 
8 | Tx 
9 | GND 
10 | Rx 
11 | NC 
12 | NC 
13 | NC 
14 | GND 
15 | NC 
16 | NC 
17 | 3V3 
18 | NC 
19 | MOSI 
20 | GND 
21 | MISO 
22 | RST 
23 | SCK 
24 | NC 
25 | GND 
26 | CS (CE1 / DIO0) 
27 | NC
28 | NC
29 | IRQ
30 | GND 
31 | NC 
32 | DIO1 
33 | D13 (also connected to the LED D13 and to the push button BTN)
34 | GND
35 | P6 (expander port) 
36 | DIO2
37 | NC
38 | P5 (expander port)
39 | GND
40 | NC 

### Header J2 (Rx/Tx)
You can connect a 3V3 sensor or a 5V sensor. By welding the Jumper SJ8, you can select the voltage you want to apply at the UART device.
By default, SJ6 is wilded to 3V3.

Pin | Output
--- | ---
1 | GND 
2 | 3V3 or 5V
3 | Rx 
4 | Tx 

### Header J3 (Port expander)
Header J3 is connected to PCF8574 chip

Pin | Output (PCF8574)
--- | ---
1 | GND
2 | 3V3
3 | P0 (max 500mA)
4 | P1 (max 500mA)
5 | P2 (max 500mA)
6 | P3 (max 500mA)
7 | P4 (aslo connected to J11 at pin 2)
9 | P5 (also connected to the LED D5 and to J1 at pin 38)

(P6 is connected to the LED D6 and J1 at pin 35);

**Be carefull**
The maximum output of the PCF8574 is 25mA. That's reason why, we add a MOSFET at P0, P1, P2 and P3 in order to have max 500mA. 
Have a look at section **GPIO I/O expander port**, bellow, to learn how to use P0 to P6.

### Hader J4 (analog)
Pin | Output
--- | ---
1 | GND
2 | 3V3
3 | A0
4 | A1
5 | A2
6 | A3
7 | A4
8 | A5

### Header J5 (I2C)
You can connect a 3V3 sensor or a 5V sensor. By welding the Jumper SJ6, you can select the voltage you want to apply at the I2C device.
By default, SJ6 is wilded to 3V3.

Pin | Output
--- | ---
1 | GND
2 | 3V3 or 5V
3 | SCL
4 | SDA
5 | D3 (ATSAMD21G18)
6 | D2 (ATSAMD21G18)

### Header J6 (programmation)
You can use Header J6 to program/debug the ATSAMD21G18 chip. we use it only to flash the boot loader. More info can be found [here](https://learn.adafruit.com/programming-an-m0-using-an-arduino/overview)

Pin | Output
--- | ---
1 | GND
2 | 3V3
3 | SWCLK
4 | SWDIO
5 | RESET (Close SJ5 to connect pin 5 to RESET pin of the ATSAMD21)
6 | NC

You ma yhave to close the jumper SJ5 to connect pin5 to pin40 of the AZSAMD21G18 (RESET)

### Header J11
J11 is connected to P4 and to A5.
You can use J11 as 1-Wire bus. I am used to connected a DS18B20 sensor (Waterproof temperature sensor). P4 power the sensor when a measure is needed, and A5 read the measure. If you use a 1-Wire sensor A5 need to have pull-up resitor. We added a 4.7K resistor with a Jumper SJ10 which is closed by default. If you do not need the pull-up resistor, you can unsolder SJ10.

Pin | Output
--- | ---
1 | A5 (connected the Jumper SJ10 and to a 4.7K pull-up resistor)
2 | P4
3 | NC 

### Header J8 (OLED)
You can connect a I2C 128x64 OLED display

Pin | Output
--- | ---
1 | GND
2 | 3V3
3 | SCL
4 | SDA

### Pin Header SP1 (Analog)
You can connect a 3V3 sensor or a 5V sensor. By welding the Jumper SJ9, you can select the voltage you want to apply at the analog device.
By default, SJ9 is wilded to 3V3. You might need to solder a 3 pin header.

Pin | Output
--- | ---
1 | GND
2 | 3V3 or 5V
3 | A0

A0 is connected to the Jumper SJ7. If you close it, A0 will be pull-down with a 10K resistor. We also connected a blue LED with a 1K resistor.

If you open SJ7, you can use an anenometer. If you keep closed SJ7, you can use a rain gauge, and the LED will bright each time a drop go throught the rain gauage.

## Jumpers
**SJ1 (5V converter)** If you do not need 5V output, you can close SJ2 to disable the TPS6109

**SJ2 (ARef)** Normally the reference voltage is the same as the chip logic voltage (3.3V) but if you need an alternative analog reference. Can't go higher than 3.3V!

For the EcoBoard as for all MO boards, close the SJ2 jumber and add the following in setup()

```
#if defined(ARDUINO_SAMD_ZERO) || defined(__arm__)
    analogReadResolution(10);
    analogReference(AR_EXTERNAL);
#endif
```

**SJ3 & SJ4 (I2C pullup)**
SJ3 and SJ4 are I2C pull up. Both jumpers have to be closed, excepted if SCL and SDA are pull up from another devise.

**SJ5**
SJ5 must be open, excepted if you need to programm the ATSAMD21G18 ARM Cortex M0. SJ5 is connected to ATSAMD21 at RESET pin

**SJ6**
SJ6 allow you to power a I2C sensor with either 3V3 or 5V. The I2C sensor can be connected to J5. Note, J5 is connected to D2 and D3 of the ATSAMD21.

**SJ7**
SJ7 is connected to A0. You can connect a Analog sensor to the pin header JP1.
If SJ7 is opened, you can use A0 with an anenometer for example. If you close SJ7, you can use A0 with **a rain gauge**. A 10K resistor is connected to SJ7 and the GND. Also, a 1K resistor in serie with a blue LED are in parallel with 10K resistor. The blue LED will bright, each time A0 is up, while the 10K restistor will make sure to have a GND at A0 (pull-down) when the rain gauge is waiting for the watter. Also, see SJ9 to power the analog sensor (A0) with 3V3 or 5V.

**SJ8**
SJ8 allow you to power an UART sensor with either 3V3 or 5V. The UART sensor can be connected to J2.

**SJ9**
SJ9 allowto power the analog sensor (A0) with 3V3 or 5V. The analog sensor can be connected to SP1.

**SJ10**
SJ10 allow you to pull-up A5 with a 4.7K resistor. It can be usefull, if you connect a 1-Wire sensor at J11. J11 has 3 pin as the following

Pin | Output
--- | ---
1 | A5 (connected the Jumper SJ10 and to a 4.7K pull-up resistor)
2 | P4 (Use to power the 1-Wire sensor, for example)
3 | NC 

## SD Card 
We add an SD card to log the EcoBoard activity or to save some parameters or other values. The MicroSD card is not provided with the board

Example is coming soon ...

## RTC (Real Time Clock) & Low Power
The Real Time Clock is working with the DS3231 chip. The DS3231 is a low-cost, extremely accurate I2C real-time clock (RTC) with an integrated temperature compensated. You will need a **CR1225** cell coin battery.
The EcoBoard can be programmed to sleep in order to save cunsomption.

See some examples [here](https://github.com/ecosensors/Ecoboard/blob/master/examples/rtc-lowpower/rtc-lowpower.ino)

## EEPROM
EcoBoard has a 64KB EEPROM  (74LC64) to store relatively small amounts of data as keys or parameters

See simple example [here](https://github.com/ecosensors/Ecoboard/tree/master/examples/eeprom)

## 1-Wire & GPIO expander port
### GPIO I/O expander port
EcoBoard allow you to use 6 additonal GPIO with the PCF8574 ([Datasheeet](https://www.ti.com/product/PCF8574) ). The outputs P1 to P6, will allow you to control the status of another device or an LED. However, the outputs can only provide 25mA. In some cases, this power may not be sufficient. It's the reason why, we added three MOSFET-P (IRML2244) on the outputs P0, P1, P2 and P3. Thus, the outputs P0, P1, P2 and P3 of the J3 connector can now supply up to 500mA.

**Note**
* You need to change P0 to LOW state in order to have Pin 3 powered by VCC (max 500mA).
* Change P1 to LOW in order to have Pin 4 powered by VCC (max 500mA)
* Change P2 to LOW in order to have Pin 5 powered by VCC (max 500mA)
* Change P3 to LOW in order to have Pin 6 powered by VCC (max 500mA)
* Change P0, P1, P2 or P3 to HIGH to have Pin 3, 4, 5 or unpowered (GND).

P4 can be used to power the 1-Wire sensor on J11

P5 and P6 are connected to a LED, as well.
* Change P5 to HIGH to bright the LED D5 (White)
* Change P6 to HIGH to bright the LED D6 (Blue)

See example [here](https://github.com/ecosensors/Ecoboard/blob/master/examples/expander-1wire/expander-1wire.ino#L76)


### 1-Wire (DS18B20)
In the example, we will show you how to prepare and use a Waterproof temperature sensor (DS18B20) working with 1-Wire bus.
If you want to use a 1-Wire sensor, you can use the J7 connector. You need to enable the GPIO port expander. J7 is connected to P4 and a A5. We use P4 to power the DS18B20 when we need to take a mesure and the mesure is done through A5. Exemple will follow soon.

#### Connection
Wires | ?
--- | ---
Red | 3V3
Blue | GND
Yellow | read

### Arduino

See example [here](https://github.com/ecosensors/Ecoboard/blob/master/examples/expander-1wire/expander-1wire.ino#L117)

Resumé:
```
#include <OneWire.h>                // Include for 1-Wire sensor
#define P4 4 // pin P4              // To power 1-Wire sensor before reading
const byte ONEWIRE_READ = 19;       // (19 = A5)  To read 1-Wire sensor
// See more in the example
```




