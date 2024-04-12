/*
 * EcoSensors
 * 
 * Here is a short and simple example to write and read a char into te EEPROM 24LC
 *
 * WARNING: This script is presented for informational purposes only and no warranty is given as to its suitability for 
 * your environment
 * The script will to be updated and improved following your needs
 *
 */

#include <Ecoboard.h>
bool isSdEnable = false;    // Disable SD card
Ecoboard Eco(isSdEnable);
#include <Wire.h>           // Need for I2C Bus
#define EEPROM_ADDR 0x50    //Address of 24LC256 eeprom chip
byte pos, pos1, pos2 = 0;   // Position of the text in the EEPROM

unsigned char rdata[100];   // Buffer for text read from the EEPROM

void setup(void)
{
  Serial.begin(9600);
  Wire.begin();  

  delay(8000);

  Eco.begin();
  
  Serial.println("     ECOBOARD     ");
  Serial.println("      EEPROM      ");
  Serial.println("------------------");
  Serial.println("");
  Serial.println(F("Simple example with 74LC"));
 
  char str_data[]={"Hello World!"}; 
  Serial.print(F("# Writing at pos ")); Serial.println(pos);
  writeEEPROM(EEPROM_ADDR,pos,str_data);                      // (Addr, position, text)
  Serial.print("# Reading from pos "); Serial.println(pos);
  readEEPROM(EEPROM_ADDR, (pos), rdata, strlen(str_data));
  Serial.write(rdata,strlen(str_data));
  Serial.println("");
  
  char str_data1[]={"How are you?"};
  pos1 = pos + strlen(str_data);                       // Moving the cursor for the next test
  Serial.print(F("# Writing at pos ")); Serial.println(pos1);
  writeEEPROM(EEPROM_ADDR,pos1,str_data1);
  Serial.print("# Reading from pos "); Serial.println(pos1);
  readEEPROM(EEPROM_ADDR, (pos1), rdata, strlen(str_data1));
  Serial.write(rdata,strlen(str_data1));
  Serial.println("");
  
  char str_data2[]={"Fine!!"};
  pos2 = pos1 + strlen(str_data1);
  Serial.print(F("# Writing at pos ")); Serial.println(pos2);
  writeEEPROM(EEPROM_ADDR,pos2,str_data2);
  Serial.print("# Reading from pos "); Serial.println(pos2);
  readEEPROM(EEPROM_ADDR, (pos2), rdata, strlen(str_data2));
  Serial.write(rdata,strlen(str_data2));


  Serial.println(F(""));

}
 
void loop(){
}
 
void writeEEPROM(int deviceaddress, unsigned int eeaddress, char* data) 
{
  // Uses Page Write for 24LC256
  // Allows for 64 byte page boundary
  // Splits string into max 16 byte writes
  unsigned char i=0, counter=0;
  unsigned int  address;
  unsigned int  page_space;
  unsigned int  page=0;
  unsigned int  num_writes;
  unsigned int  data_len=0;
  unsigned char first_write_size;
  unsigned char last_write_size;  
  unsigned char write_size;  
  
  // Calculate length of data
  do{ data_len++; } while(data[data_len]);   
   
  // Calculate space available in first page
  page_space = int(((eeaddress/64) + 1)*64)-eeaddress;

  // Calculate first write size
  if (page_space>16){
     first_write_size=page_space-((page_space/16)*16);
     if (first_write_size==0) first_write_size=16;
  }   
  else 
     first_write_size=page_space; 
    
  // calculate size of last write  
  if (data_len>first_write_size) 
     last_write_size = (data_len-first_write_size)%16;   
  
  // Calculate how many writes we need
  if (data_len>first_write_size)
     num_writes = ((data_len-first_write_size)/16)+2;
  else
     num_writes = 1;  
     
  i=0;   
  address=eeaddress;
  for(page=0;page<num_writes;page++) 
  {
     if(page==0) write_size=first_write_size;
     else if(page==(num_writes-1)) write_size=last_write_size;
     else write_size=16;
  
     Wire.beginTransmission(deviceaddress);
     Wire.write((int)((address) >> 8));   // MSB
     Wire.write((int)((address) & 0xFF)); // LSB
     counter=0;
     do{ 
        Wire.write((byte) data[i]);
        i++;
        counter++;
     } while((data[i]) && (counter<write_size));  
     Wire.endTransmission();
     address+=write_size;   // Increment address for next write
     
     delay(6);  // needs 5ms for page write
  }
}
 
void readEEPROM(int deviceaddress, unsigned int eeaddress,  
                 unsigned char* data, unsigned int num_chars) 
{
  unsigned char i=0;
  Wire.beginTransmission(deviceaddress);
  Wire.write((int)(eeaddress >> 8));   // MSB
  Wire.write((int)(eeaddress & 0xFF)); // LSB
  Wire.endTransmission();
 
  Wire.requestFrom(deviceaddress,num_chars);
 
  while(Wire.available()) data[i++] = Wire.read();

}
