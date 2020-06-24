/*
* That file is underdevelopment. Do not use it for now
*/
#include "Arduino.h"
#include "Ecoboard.h"

Ecoboard::Ecoboard()
{
	
}

int Ecoboard::begin()
{
	Serial.println(F("*******************"));
	Serial.println(F("*     Welcome     *"));
	Serial.println(F("* with EcoSensors *"));
	Serial.println(F("*******************"));
	Serial.println("");
}


