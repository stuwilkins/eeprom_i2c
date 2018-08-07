/*
 * =====================================================================================
 *
 *       Filename:  eeprom_i2c.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/14/2018 13:05:17
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Stuart B. Wilkins (sbw), stuwilkins@mac.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include <Wire.h>
#include <uCRC16Lib.h>
#include <eeprom_i2c.h>

EEProm_I2C::EEProm_I2C(uint8_t addr)
{
  _addr = addr;
}

bool EEProm_I2C::begin(void)
{
  Wire.begin();
  _chunk = 4;
  return true;
}

bool EEProm_I2C::store(uint16_t offset, uint8_t *data, int size)
{

 Serial.print(F("Write offset = "));
 Serial.print(offset);
 Serial.print(F(" size = "));
 Serial.println(size);

 // Calculate CRC
 uint16_t crc = uCRC16Lib::calculate((char*)data, size); 
 Serial.print(F("crc = "));
 Serial.println(crc, HEX);

 Serial.print(F("Data = "));
 for(int i=size-1;i>=0;i--)
 {
   Serial.print(data[i], HEX);
 }

 Serial.println("");
 write(offset, data, size);

 // Write the CRC
 write(offset + size, (uint8_t*)(&crc), sizeof(crc));

 return true;
}

bool EEProm_I2C::retrieve(uint16_t offset, uint8_t *data, int size)
{

 Serial.print(F("Retrieve offset = "));
 Serial.print(offset);
 Serial.print(F(" size = "));
 Serial.println(size);

 read(offset, data, size);

 Serial.print(F("Data = "));
 for(int i=size-1;i>=0;i--)
 {
   Serial.print(data[i], HEX);
 }
 Serial.println("");

 // Now check CRC
 
 uint16_t c_crc = uCRC16Lib::calculate((char*)data, size);
 uint16_t s_crc;
 read(offset+size, (uint8_t*)(&s_crc), sizeof(s_crc));

 Serial.print("s_crc = ");
 Serial.println(s_crc, HEX);
 Serial.print("c_crc = ");
 Serial.println(c_crc, HEX);

 if(s_crc != c_crc){
   Serial.println("CRC Mismach");
   return false;
 }

 return true;
}

bool EEProm_I2C::write(uint16_t offset, uint8_t *data, int size)
{
  uint8_t *ptr = data;
  uint16_t _offset = offset;

  for(int i = 0;i<size;i+=_chunk){
    Wire.beginTransmission(_addr);
    Wire.write((int)(_offset >> 8));
    Wire.write((int)(_offset & 0xFF));

    int _c = (size - i) > _chunk ? _chunk : (size - i);
    for(int j=0;j<_c;j++)
    {
      Wire.write((int)*ptr);
      ptr++;
    }
    Serial.print(Wire.endTransmission());
    _offset += _chunk;
    delay(5);
  }

  return true;
}

bool EEProm_I2C::read(uint16_t offset, uint8_t* data, int size)
{

  int j = 0;
  for(int i = 0;i<size;i+=_chunk)
  {
    uint16_t _offset = offset + i;

    Wire.beginTransmission(_addr);
    Wire.write(_offset >> 8);
    Wire.write(_offset & 0xFF);
    Wire.endTransmission(true);

    int _c = (size - i) > _chunk ? _chunk : (size - i);
    Wire.requestFrom(_addr, _c);
    while(Wire.available())
    {
      data[j++] = Wire.read();
      Serial.println(data[i], HEX);
    }
  }
  
  return true;
}
