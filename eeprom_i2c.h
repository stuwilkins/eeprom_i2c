/*
 * =====================================================================================
 *
 *       Filename:  eeprom_i2c.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/14/2018 13:05:38
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Stuart B. Wilkins (sbw), stuwilkins@mac.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef __EEPROM_I2C__
#define __EEPROM_I2C__

#ifndef EEPROM_BUFFER_SIZE
  #define EEPROM_BUFFER_SIZE 256
#endif 

class EEPROM_I2C
{
  public:
    EEPROM_I2C(uint8_t addr);
    int begin(void);
    int writeIfDiff(uint16_t offset, uint8_t *data, int size, bool crc = false, bool verify = false);
    int writeAndVerify(uint16_t offset, uint8_t* data, int size, bool crc = false);
    int write(uint16_t offset, uint8_t* data, int size, bool crc = false);
    int read(uint16_t offset, uint8_t* data, int size, bool crc = false);

    enum rtn_val {
      OK = 0,
      VERIFY_FAILED = 1,
      BUFFER_ERROR = 2,
      NO_WRITE = 3
    };

  private:
    uint8_t _addr;
    uint16_t _chunk;
    void _read(uint16_t offset, uint8_t* data, int chunk);
    void _write(uint16_t offset, uint8_t *data, int chunk);
};

#endif

