#include <Wire.h>
#include "eeprom_i2c.h"

// EEPROM
EEProm_I2C eeprom = EEProm_I2C(0x50);

void setup(void)
{
	Serial.begin(115200);
	delay(5000);
	eeprom.begin();

}

void loop(void)
{
	uint8_t data_out[100];
	uint8_t data_in[100];

	data_out[0] = random(0, 255);
	eeprom.write(1, data_out, 1);
	eeprom.read(1, data_in, 1);

	Serial.print("Result = ");
	Serial.println(data_in[0]);

	delay(5000);

}
