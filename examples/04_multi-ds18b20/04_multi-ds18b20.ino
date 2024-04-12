/* Eco Sensors
 * ==============
 * 
 * Here we will show you how to use multiple DS18B20 (1-Wire)
 *
 * The script is distributed WITHOUT WARRANTY and it has not been fully tested.
 * The script will to be updated and improved following your needs
 * 
 * - ATSAMD21G18 ARM Corted MO 48Mhz
 * - 256K Flash
 * - 32 of RAM
 * (Same as Arduino Zero)
 * with 4 1-Wire connectors. 
 * 
 * Connection
 * ==========
 * Red => VCC
 * Blue or black => GND
 * Yellow or white => Signal
 * 
 * Important: additionally, you have to solder a 4.7kHom between the signal and the VCC.
 * 
 * In that exercise, you will have to connect the red wires of the DS18B20 together, 
 * the GND wires (black or Blue) together and the signal (white or yellow) wires together as well. 
 *
 *
 * We connected the signal wire to A5.
 * In order to power the DS18B20 only when we need to take a measure, we connected it the red wire to a pin
 * In that exercise, we named that pin: P4
 * 
 * If you measure the tempearture in a wet environement, it's not recommanded to keep the sensors powered while
 * no measure are taken.

 * We also considered , you already installed the lbrarires
 * - OneWire
 *
 *
 * Observation
 * ===========
 * 
 * As far I investigate, the script can return Addresses of a DS10B20 and its measures.
 * If you insert a DS18B20 sensor into the soil, the second at 10cm and the third at 1m above the ground, the script will 
 * search all DS18B20 in the 1-WIRE bus and return its value. But you will not be able to know whcih of the sensor
 * the value belong to, excepted if you know the ADDR. If you change a defect sensor, you will have to know the
 * ADD of the new sensor. The script is not intelligent enough to know the new ADDR remplace the ADDR of the defect 
 * sensor.
 * 
 * If you want to make sure, which value belong to which sensor, you should better connect (for example) the signal
 * pin of the first sensor to A1, the signal pin of the second sensor to A2, the third signal pin to A3. 
 * In order word, a signal pin should not been connected together on the same analog pin, as we do here.
 * 
 * Here, we will scan the DS18B20 connected to the same reading pin, same the addresses into an array.
 * 
 * I will improve my next PCB in order to have a different pin to read the signal of the DS18B20
 * 
 */

 byte led_d8 = 5;                   // White led on EcoLORA Board

 /*
 * DS18B20
 */
 byte sensorsAddr[4][8];                // arrayto store the DS18B20 addresses. Max 4 sensors
 int numberOfConnectedSensors = 0;      // Store the number of connected sensors. Max 4 sensors
 byte ds18b20_read = 19;             // (19 = A5) Pin to read values (onewire)

  #include <OneWire.h>  
  // Another solution for one 1-Wire: https://www.carnetdumaker.net/articles/mesurer-une-temperature-avec-un-capteur-1-wire-ds18b20-et-une-carte-arduino-genuino/
  //Broche du bus 1-Wire
  const byte BROCHE_ONEWIRE = ds18b20_read;

  // Code de retour de la fonction getTemperature()
  enum DS18B20_RCODES {
    READ_OK,  // Lecture ok
    NO_SENSOR_FOUND,  // Pas de capteur
    INVALID_ADDRESS,  // Adresse reçue invalide
    INVALID_SENSOR  // Capteur invalide (pas un DS18B20)
  };

  OneWire ds(BROCHE_ONEWIRE);           // Setup a oneWire instance to communicate with any OneWire devices


  /*
  * Replace and save here the ADDR you found. 
  */
  /*
  // Addresses of 3 DS18B20s
  byte SENSOR_ADDRESS_2[] = { 0x28, 0x5C, 0xC9, 0x79, 0x97, 0x13, 0x03, 0xAE };  // Connected to the first connector
  byte SENSOR_ADDRESS_1[] = { 0x28, 0xDE, 0x8D, 0x79, 0x97, 0x13, 0x03, 0xDC };  // Connected to the 2nd connector
  byte SENSOR_ADDRESS_3[] = { 0x28, 0x61, 0x64, 0x12, 0x3F, 0xFD, 0x80, 0xC6 };  // Connected to the 3rd connector
  byte SENSOR_ADDRESS_4[] = { 0x28, 0x61, 0x64, 0x12, 0x3F, 0xFD, 0x80, 0xC6 };  // Connected to the 4th connector
*/

  /*
   * PCF8574 Port extander
   * =====================
   * 
   * EcoLORA and Eco BOARD contain a Port extander, which allow you to have additonal pin.
   * Here we use P0 of the PCF8574 to power the DS18B20 when we need to take a measurement of temprature.
   * If you do not use a PCF8574 (what I beleive :) )
   * Ignore this part, but you will need to connect the RED wire of the DS18B20 to a pin of your microcontrolller
   * pinMode(ds18b20_pwd, OUTPUT);      // Define the pin of your microcontroller
   * digitalWrite(ds18b20_pwd, LOW);   // Set it to LOW state
   * 
   * If you power the DS18B20 with VCC, ignoe this part as well and adapt my script
   */

  #include <Adafruit_PCF8574.h>                  // Required for PCF857
  Adafruit_PCF8574 expander;
  int ds18b20_pwd = 0;                  // 0 belong to pin P0 of the PCF8574.
                                        // If you do not use a PCF854, replace this value
                                        // with a pin of you microcontroller


void setup()
{
  Serial.begin(9600);                   // start serial port
  delay(9000);

  Serial.println("");
  Serial.println("     EcoLORA      ");
  Serial.println("  Multi DS18B20   ");
  Serial.println("------------------");
  Serial.println("");
  Serial.println(F("Exemple with Multi DS18B20"));

  pinMode(ds18b20_read, INPUT_PULLUP);    // Define the pin of the ATSAM21 MO Cortec to read the value
  pinMode(led_d8, OUTPUT);                // Define the pin to control the led
  digitalWrite(led_d8, LOW);
  
  /*
   * Start and test PCF8574
   * Ignore this part if you do not use it
   */
  Serial.println(F("\r\n# Start PCF8574")); 
  expander.begin(0x27);
  expander.pinMode(0,OUTPUT);
  expander.pinMode(1,OUTPUT);
  expander.pinMode(2,OUTPUT);
  expander.pinMode(3,OUTPUT);
  expander.pinMode(4,OUTPUT);
  expander.pinMode(5,OUTPUT);
  expander.pinMode(6,OUTPUT);
  
  Serial.println(F("Testing PCF"));
    
  for(int p=0; p<7;p++){
    expander.digitalWrite(p,LOW);
    delay(50);
    //delayMicroseconds(50000);
    Serial.print(F("\tP")); Serial.print(p); Serial.print(F(":"));
    Serial.println(expander.digitalRead(p) ? "HIGH" : "LOW");
    delay(500);
    //delayMicroseconds(50000);
  }

  // Wih EcoLORA and EcoBOARD the pin PO, P1, P2 and P3 are behind a MOSFET-P (IRLML2244) which will allow to power
  // the devise with the VCC of the circuit and not from the PCF8574. LOW poser the sensor/devise, HIGH turn off the sensor.
  expander.digitalWrite(ds18b20_pwd, HIGH);   // Turn off the sensor connected to P0
  expander.digitalWrite(1, HIGH);             // Turn off the sensor connected to P1
  expander.digitalWrite(2, HIGH);             // Turn off the sensor connected to P2
  expander.digitalWrite(3, HIGH);             // Turn off the sensor connected to P3
  // to power a devise connected to P4 to P6 like a LED, LOW: turn OFF, HIGH state turn it on.
  Serial.print(F("\ds18b20_pwd (")); Serial.print(ds18b20_pwd); Serial.print(F("): "));
  Serial.println(expander.digitalRead(ds18b20_pwd) ? "HIGH" : "LOW"); // Check the status of the pin
  
  Serial.println(F("[OK] PCF8574"));
  Serial.println(F(""));

   /* 
   *  To locate devices on the bus
   *  =========================
   *  In order to know which ADDR belong to which sensor connected to a PicoBlade connector,
   *  I first run the setup script with one DS18B20 connected and I record the ADDR.
   *  I connect the second DS18B20 to the second connector, I run and record the DDR
   *  Same for the first and same for the fourth.
  */
  expander.digitalWrite(ds18b20_pwd,LOW);   // Power on the devise. As a MOSFET-P ise behind the DS18B20, LOW state will power it on
  // digitalWrite(ds18b20_pwd, HIGH);       // If you connected the RED wire of the DS18B20, uncomment that line and comment the previous line
  delay(1000);                              // Need a delay of 750. I choosed 1000 instead.

  Serial.println("# Scan 1-Wire bus...");
  int s = 0;
  do
  {
    byte address[8];
     /* search a 1-Wire in the bus */
    if (!ds.search(address))
    {
      ds.reset_search();
      numberOfConnectedSensors = s;
      Serial.println(F("End of Scan."));
      Serial.print(F("Found "));
      Serial.print(numberOfConnectedSensors);
      Serial.println(F(" sensor(s)"));
      s=4;  // Leave the loop
    }
    else
    {
      /* DS18B20 has been found */
      Serial.print(F("Found: "));
      for(byte i = 0; i < 8; ++i)
      {
        if (address[i] < 0x10) Serial.write('0');
        Serial.print(address[i], HEX);
        sensorsAddr[s][i]=address[i];             // Save into an array to use it later
        Serial.write(' ');
      }

      /* Check if the address belong to a DS18B20 */
      if (OneWire::crc8(address, 7) != address[7])
      {
        Serial.print(F("(CRC invalid)"));
      }
      s++;
      /* END */
      Serial.println();
    }
  }while(s < 4);
  expander.digitalWrite(ds18b20_pwd, HIGH);     // Turn off the DS18B20 (must be HIGH to turn off)
  // digitalWrite(ds18b20_pwd, LOW);            // If you connected the RED wire of the DS18B20, uncomment that line and comment the previous line
  
  Serial.println(F("------------- Done ------------------"));
  Serial.println(F(""));
}

void loop()
{
  if(numberOfConnectedSensors < 1)
  {
    Serial.println(F("No sensors"));
    delay(1000);
  }
  else
  {

    expander.digitalWrite(ds18b20_pwd, LOW);
    // digitalWrite(ds18b20_pwd, HIGH);       // If you connected the RED wire of the DS18B20, uncomment that line and comment the previous line
    delay(1000);                              // Min value is 750

    float temperature[numberOfConnectedSensors];

    Serial.println(F("# Staring reading measures"));
    Serial.println(F("Reading by searching the sensors in the bus"));
    /* 
    *  Read the temperature by searching the sensor in the bus
    */
    bool rst = true;
    for (uint8_t i = 0; i < numberOfConnectedSensors; i++)
    {
      if(i==0)
        rst = true;
      else
        rst = false;
      
      if (getTemperatureSearch(&temperature[i], rst) != READ_OK) { // The first measure of the first sensor, 2nd parameter must be true
        Serial.print(F("Error to read sensor "));
        Serial.println(i);
      }
      delay(100);
    }
  
    for (uint8_t i = 0; i < numberOfConnectedSensors; i++)
    {
      /* Print température */
      Serial.print(F("Temperature ["));
      Serial.print(i);
      Serial.print(F("] : "));
      Serial.print(temperature[i], 2);
      Serial.println(F("C"));
    }
    Serial.println();


    /* 
    *  Read the temperature by address of the sensor
    */
    Serial.println(F("Reading the sensors by address"));
    for (uint8_t i = 0; i < numberOfConnectedSensors; i++)
    {
      temperature[i] = getTemperatureAddr(sensorsAddr[i]);
      
      Serial.print(F("Temperature ["));
      Serial.print(i);
      Serial.print(F("] : "));
      Serial.print(temperature[i], 2);
      Serial.println(F("C")); 
    }
    
 
    expander.digitalWrite(ds18b20_pwd, HIGH);
    // digitalWrite(ds18b20_pwd, LOW);       // If you connected the RED wire of the DS18B20, uncomment that line and comment the previous line

    delay(3000);
  }
}

/*
 * Functions
 */
// Read temperature by address
float getTemperatureAddr(const byte addr[]) {
  byte data[9];
  // data[] : Données lues depuis le scratchpad
  // addr[] : Adresse du module 1-Wire détecté
  
  /* Reset le bus 1-Wire et sélectionne le capteur */
  ds.reset();
  ds.select(addr);
  
  /* Lance une prise de mesure de température et attend la fin de la mesure */
  ds.write(0x44, 1);
  delay(800);
  
  /* Reset le bus 1-Wire, sélectionne le capteur et envoie une demande de lecture du scratchpad */
  ds.reset();
  ds.select(addr);
  ds.write(0xBE);
 
 /* Lecture du scratchpad */
  for (byte i = 0; i < 9; i++) {
    data[i] = ds.read();
  }
   
  /* Calcul de la température en degré Celsius */
  return (int16_t) ((data[1] << 8) | data[0]) * 0.0625; 
}

// Read temperature by searching the sensors
byte getTemperatureSearch(float *temperature, byte reset_search) {
  byte data[9], addr[8];
  // data[] : Données lues depuis le scratchpad
  // addr[] : Adresse du module 1-Wire détecté
  
  /* Reset the bus 1-Wire if necessary (requiered for the read ofthe first sensor) */
  if (reset_search) {
    ds.reset_search();
  }
 
  /* Search the next sensor 1-Wire available */
  if (!ds.search(addr)) {
    // No sensor any more
    return NO_SENSOR_FOUND;
  }
  
  /* Vérifie que l'adresse a été correctement reçue */
  if (OneWire::crc8(addr, 7) != addr[7]) {
    // Adresse invalide
    return INVALID_ADDRESS;
  }
 
  /* Check if the sensor is a DS18B20 */
  if (addr[0] != 0x28) {
    // Mauvais type de capteur
    return INVALID_SENSOR;
  }
 
  /* Reset bus 1-Wire and select the sensor */
  printAddress(addr);
  ds.reset();
  ds.select(addr);
  
  /* Take a measure and wait for the end */
  ds.write(0x44, 1);
  delay(800);
  
  /* Reset le bus 1-Wire, sélectionne le capteur et envoie une demande de lecture du scratchpad */
  ds.reset();
  ds.select(addr);
  ds.write(0xBE);
 
 /* Lecture du scratchpad */
  for (byte i = 0; i < 9; i++) {
    data[i] = ds.read();
  }
   
  /* Calcul de la température en degré Celsius */
  *temperature = (int16_t) ((data[1] << 8) | data[0]) * 0.0625; 
  
  // Pas d'erreur
  return READ_OK;
}

void printAddress(byte *deviceAddress)
{ 
  for (uint8_t i = 0; i < 8; i++)
  {
    Serial.print("0x");
    if (deviceAddress[i] < 0x10) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
    if (i < 7) Serial.print(", ");
  }
  Serial.println("");
}
