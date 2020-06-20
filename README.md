# EcoBoard
EcoBoard is new microcontroler based on the processor ATSAMD21G18 ARM Cortex M0 at 48Mhz with 3V3 logic, as the Arduino Zero.
The chip has 256K of FLASH and 32K of RAM. It's fully compatible with Arduino and Adafruit libraries

We add several useful features
* 1Kb EEPROM
* A MicroSD card
* A Real Time Clock
* 5V Output
* GPIO Port Extender (P0-P4 outpouts and P5 & P6 are connected to a LED (D5 & D6)
* A I2C connector
* A UART connector
* A 1-Wire connector
* A debuger/programmer connector
* A reset buton
* A programable buton
* A switch ON/OFF Button

The board will be available beginning of June, with exemples of Arduino code to easly first use it.
Description of the EcoBoard is coming soon.

## Pintout
Coming soon

### Header J1 (2x20)
This header is compatible with Adafruit Header and (EcoRadio board)[https://github.com/ecosensors/ecoradio]

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
35 | P6 (port extender) 
36 | DIO2
37 | NC
38 | P5 (port extender)
39 | GND
40 | NC 

### Header J2 (Rx/Tx)
1 => GND; 
2 => 3V3;
3 => Rx; 
4 => Tx; 
5 => NC; 
6 => NC 

### Header J3 (Port extender)
1 => GND;
2 => 3V3;
3 => P0;
4 => P1;
5 => P2;
6 => P3;
7 => P4 (aslo connected to J7);
9 => P5 (also connected to the LED D5);
(P6 is connected to the LED D6);

### Hader J4 (analog)
1 => GND;
2 => 3V3;
3 => A0;
4 => A1;
5 => A2;
6 => A3;
7 => A4;
8 => A5;

### Header J5 (I2C)
1 => GND;
2 => 3V3;
3 => SCL;
4 => SDA;
5 => D3 (ATSAMD21G18);
6 => D2 (ATSAMD21G18);

### Header J7
J7 is connected to P4 and to A5.
You can use it as 1Wire bus. I am used to connected a DS18B20 sensor (Waterproof temperature sensor). P4 power it when a measure is needed and A5 read the measure

### Header J8 (OLED)
1 => GND;
2 => 3V3;
3 => SCL;
4 => SDA;

### Jumpers
**SJ1 (5V converter)** If you do not need 5V output, you can close SJ2 to disable the TPS6109

**SJ2 (ARef)** Normally the reference voltage is the same as the chip logic voltage (3.3V) but if you need an alternative analog reference, close SJ5 and select the external AREF in your firmware. Can't go higher than 3.3V!

```
analogReference(AR_EXTERNAL)
```
(See exemple)

**SJ3 & SJ4 (I2C pullup)**
SJ2 and SJ3 are I2C pull up. Both jumpers have to be closed, excepted if SCL and SDA are pull up from another PCB.

**SJ5**
SJ5 must be open, excepted if you need to programm the ATSAMD21G18 ARM Cortex M0

## GPIO port extender
Coming soon

## 1-Wire
If you want to use a 1-Wire sensor as the DS18B20 (Waterproof temperature sensor), you can use the J7 connector which is close to 8 pin analogic connector. You need to enable the GPIO port extender. J7 is connected to P4 and a A5. We use P4 to power the DS18B20 when we need to take a mesure and the mesure is done through A5. Exemple will follow soon.
