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

### Jumpers
**SJ1 (5V converter** If you do not need 5V output, you can close SJ2 to disable the TPS6109

**SJ2 (ARef)** Normally the reference voltage is the same as the chip logic voltage (3.3V) but if you need an alternative analog reference, close SJ5 and select the external AREF in your firmware. Can't go higher than 3.3V!

```
analogReference(AR_EXTERNAL)
```
(See exemple)

**SJ3 & SJ4**
SJ2 and SJ3 are I2C pull up. Both jumpers have to be closed, excepted if SCL and SDA are pull up from another PCB.

**SJ5**
SJ5 must be open, excepted if you need to programm the ATSAMD21G18 ARM Cortex M0

## GPIO port extender
Coming soon

## 1-Wire
If you want to use a 1-Wire sensor as the DS18B20 (Waterproof temperature sensor), you can use the J7 connector which is close to 8 pin analogic connector. You need to enable the GPIO port extender. J7 is connected to P4 and a A5. We use P4 to power the DS18B20 when we need to take a mesure and the mesure is done through A5. Exemple will follow soon.
