#include <Wire.h>
#include "eeprom_i2c.h"

// EEPROM
EEPROM_I2C eeprom = EEPROM_I2C(0x50);

void setup(void)
{
	Serial.begin(115200);
	delay(5000);
	eeprom.begin();

}

void loop(void)
{
	uint8_t data_out[200];
	uint8_t data_in[200];
	unsigned long a,b;

	Serial.println("Running tests ......");

	for(int i=0;i<200;i++)
	{
		data_out[i] = random(0, 255);
	}

	a = millis();
	Serial.println(eeprom.writeIfDiff(0, data_out, 200, 1, 1));
	b = millis() - a;
	Serial.println(b);


	a = millis();
	Serial.println(eeprom.read(0, data_in, 200, 1));
	b = millis() - a;
	Serial.println(b);

	for(int i=0;i<200;i++)
	{
		if(data_in[i] != data_out[i])
		{
			Serial.print(F("Error at "));
			Serial.println(i);
			break;
		}
	}

	Serial.println("Looping ......");

	delay(5000);

}
