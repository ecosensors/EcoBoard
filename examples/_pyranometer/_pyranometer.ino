/*
 * EcoSensors - Davis Pyranometer 6450
 * https://www.meteo-shopping.fr/Station-meteo/Pyranometre-pour-mesure-du-rayonnement-solaire-6450-Davis-Instruments#caracteristique
 * 
 * Wire:
 * Green wire . . . . . . .	Output (0 to +3VDC); 1.67 mV per W/m2
 * Red & Black wires . . .	Ground
 * Yellow wire . . . . . .	+3 VDC ±10%; 1mA (typical)
 *
 * Range . . . . . . .  . .	0 to 1800 W/M2
 *
 * The script is distributed WITHOUT WARRANTY.
 * Feel free to collaborate and share suggestions for improvement
 * 
 * Not tested yet
 * TODO: sun_offset
 * 
 */

int analogResolution = 1023;
int sensor_power_pin = 2;                			// PCF8574 pins (0 to 3) or Arduino pin
int sensor_read_pin = 16;                			// A2
int16_t sun_offset = 0;							// I will clarify that later


void setup() {

	Serial.begin(9600);
	delay(8000);                            			// Give a delay to open the 

	Serial.println(F(""));
	Serial.println(F(" ECOBOARD - Davis Pyranometer 6450"));
	Serial.println(F("----------------------------------"));
	Serial.println(F(""));
 
 	// https://www.arduino.cc/reference/fr/language/functions/analog-io/analogreference/
 	analogReference(AR_DEFAULT);          			// 3.3V (10-12)
 	// analogReference(AR_INTERNAL);      			// 2.23V
 	// analogReference(AR_INTERNAL2V23);  			// 2.23V
 	// analogReference(AR_EXTERNAL);      			// the voltage applied to the AREF pin is used as the reference

	int ar = 12;										// Choose 10 or 12
	analogReadResolution(ar);

	switch(ar){
		case 10:
        	analogResolution = 1023;    				// analogResolution will be need to calculate the analog value    
        	break;
      	case 12:
        	analogResolution = 4096;
        	break;
      	default:
        	analogResolution = 1023;
        	break;
	}

	pinMode(sensor_power_pin, OUTPUT);
	digitalWrite(sensor_power_pin, HIGH);      		// Power the sensor
	pinMode(sensor_read_pin, INPUT_PULLDOWN);

}

void loop() {
	void get_sunOffset();
	//void get_sun();
}

void get_sunOffset(){

	/* TODO */
	Serial.println(F("\r\nOffset value"));
	Serial.println(F("******************************"));
	/*
	* Run the function 20mn into a black room and record the average value
	*/

	float f_offset = analogRead(sensor_read_pin);     
  	f_offet = analogRead(sensor_read_pin);
  	delay(10);
  	f_offset = 0; //Ignore the two first reading

  	// Raed 20 time the analog value and keep the average value
  	for (int o = 0; o < 20; o++) {
    	f_offet += analogRead(sensor_read_pin);
    	Serial.print(o); Serial.print(F(". ")); Serial.println(f_sunAnalogValue);
    	delay(10);
  	}
  	f_offet = f_offet / 20;           				// Calculate the average

  	Serial.print(F("DEBUG > Offset : ")); Serial.println(f_offset);
 	delay(1000);

}

void get_sun()
{
	Serial.println(F("\r\nSun Radiation"));
	Serial.println(F("******************************"));
	int16_t sunAnalogValueMinusOffset = 0;
	float sunValueWm2 = 0;
	int16_t sunAnalogValue;
	int16_t sunValue;

	//https://www.meteo-shopping.fr/Station-meteo/Pyranometre-pour-mesure-du-rayonnement-solaire-6450-Davis-Instruments#caracteristique
	float f_sunAnalogValue = analogRead(sensor_read_pin);
	f_sunAnalogValue = analogRead(sensor_read_pin);
	delay(10);
	f_sunAnalogValue = 0; 							//Ignore the two first reading

 	// Raed 20 time the analog value and keep the average value
  	for (int o = 0; o < 20; o++) {
    	f_sunAnalogValue += analogRead(sensor_read_pin);
    	Serial.print(o); Serial.print(F(". ")); Serial.println(f_sunAnalogValue);
    	delay(10);
  	}

  	f_sunAnalogValue = f_sunAnalogValue / 20;          // Calculate the average
  
  	// Convert to int (and remove the decimal)
  	sunAnalogValue = (int16_t)f_sunAnalogValue;

  	Serial.print(F("DEBUG > sunAnalogValue (")); Serial.print(sensor_read_pin); Serial.print(F("): ")); Serial.println(sunAnalogValue);

  	// See get_sunOffset()
  	if (sunAnalogValue < sun_offset)
  	{
    	// If the analog value is bellow the offset, the analogValue take the offset value
    	sunAnalogValue = sun_offset;
  	}

  	Serial.print(F("DEBUG > offset: ")); Serial.println(sun_offset);

  	sunAnalogValueMinusOffset = sunAnalogValue - sun_offset;

  	Serial.print(F("DEBUG > sunAnalogValue-offset: ")); Serial.println(sunAnalogValueMinusOffset);

  	float v = (3.3 / analogResolution) * sunAnalogValueMinusOffset;
  	int mv = v*1000;  // Convert to mV
  	Serial.print(F("DEBUG > Volt: ")); Serial.print(mv); Serial.println(F("mV"));

  	sunValueWm2 = mv / 1.67;  						// calculate the sun radiation (1.67 mV per W/m2) 

  	Serial.print(F("\rDEBUG > Sun radiation: ")); Serial.print((int16_t)sunValueWm2); Serial.println(F("W/m2\r"));

  	delay(5000);

}