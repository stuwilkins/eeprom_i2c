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

EEPROM_I2C::EEPROM_I2C(uint8_t addr)
{
  _addr = addr;
}

int EEPROM_I2C::begin(void)
{
  Wire.begin();
  Wire.setClock(400000);
  _chunk = 4;
  return OK;
}

int EEPROM_I2C::writeIfDiff(uint16_t offset, uint8_t *data, int size, bool crc, bool verify)
{
  if(size > EEPROM_BUFFER_SIZE)
  {
    return BUFFER_ERROR;
  }

  uint8_t _buffer[EEPROM_BUFFER_SIZE];
  read(offset, _buffer, size, crc);

  if(memcmp(_buffer, data, size))
  {
    if(verify)
    {
      writeAndVerify(offset, data, size, crc);
    } else {
      write(offset, data, size, crc);
    }
  } else {
    return NO_WRITE;
  }

  return OK;
}

int EEPROM_I2C::writeAndVerify(uint16_t offset, uint8_t *data, int size, bool crc)
{
  if(size > EEPROM_BUFFER_SIZE)
  {
    return BUFFER_ERROR;
  }

  write(offset, data, size, crc);

  uint8_t _buffer[EEPROM_BUFFER_SIZE];
  read(offset, _buffer, size, crc);

  if(memcmp(_buffer, data, size))
  {
    return VERIFY_FAILED;
  }

  return OK;

}

void EEPROM_I2C::_write(uint16_t offset, uint8_t *data, int chunk)
{
    Wire.beginTransmission(_addr);
    Wire.write((int)(offset >> 8));
    Wire.write((int)(offset & 0xFF));

    for(int j=0;j<chunk;j++)
    {
      Wire.write((int)data[j]);
    }
    Wire.endTransmission();
    delay(5);
}

int EEPROM_I2C::write(uint16_t offset, uint8_t *data, int size, bool crc)
{
  uint8_t *ptr = data;
  uint16_t _offset = offset;

  for(int i = 0;i<size;i+=_chunk){
    int _c = (size - i) > _chunk ? _chunk : (size - i);
    _write(_offset, ptr, _c);
    _offset += _c;
    ptr += _c;
  }

  if(crc)
  {
    uint16_t c_crc = uCRC16Lib::calculate((char*)data, size); 
    _write(_offset, (uint8_t*)(&c_crc), sizeof(c_crc));
  }

  return OK;
}

void EEPROM_I2C::_read(uint16_t offset, uint8_t* data, int chunk)
{
    Wire.beginTransmission(_addr);
    Wire.write(offset >> 8);
    Wire.write(offset & 0xFF);
    Wire.endTransmission();

    Wire.requestFrom(_addr, chunk);
    
    int j=0;
    while(Wire.available())
    {
      data[j++] = Wire.read();
    }
}

int EEPROM_I2C::read(uint16_t offset, uint8_t* data, int size, bool crc)
{

  uint8_t *data_ptr = data;
  uint16_t _offset = offset;

  for(int i = 0;i<size;i+=_chunk)
  {
    int _c = (size - i) > _chunk ? _chunk : (size - i);
    _read(_offset, data_ptr, _c);
    data_ptr += _c;
    _offset += _c;
  }

  if(crc)
  {
    uint16_t c_crc, r_crc;
    _read(_offset, (uint8_t*)(&r_crc), sizeof(r_crc));
    c_crc = uCRC16Lib::calculate((char*)data, size); 

    if(c_crc != r_crc)
    {
      return VERIFY_FAILED;
    }
  }
  
  return OK;
}
