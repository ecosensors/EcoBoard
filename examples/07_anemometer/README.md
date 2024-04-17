# ECOBOARD - ANEMOMETER

Read the README of the [EcoAnemo library](https://github.com/ecosensors/EcoAnemo), particulary about [the wiring and the calibration](https://github.com/ecosensors/EcoAnemo?tab=readme-ov-file#calibration) of wind direction sensor.

## Libraries

Download the [EcoAnemo library](https://github.com/ecosensors/EcoAnemo) into SKETSCHBOOK/Libraries/EcoAnemo

Close and reopen Arduino IDE

## Wire

* Connect the connector with 3 wires the anemometer to J0
* Solder JP_3 to 3V3 to permanently power the sensor
* Connect the connector with 1 wire to J1 (A1)
* Solder JP3 to 3V3 (4.7K pullup)

Anemo | Pin | Connector
--- | --- | --
Yellwo| 3V3 | J0
Red | GND | J0
Black | A0 | J0 (Wind spped)
GREEN | A1 | J1 (Wind direction) with a 4.7K pullup

## Sketch
You can play with the [example file](https://github.com/ecosensors/EcoAnemo/blob/main/examples/calibration.ino) of the EcoAnemo library to calibrate the wind direction sensor. While calibrating the sensor, do not forget to update the first colum of the [WDdirection array](https://github.com/ecosensors/EcoAnemo/blob/main/EcoAnemo.h#L24) with the measured values.


All should work fine, but feel free to report errors or improvement. :) 