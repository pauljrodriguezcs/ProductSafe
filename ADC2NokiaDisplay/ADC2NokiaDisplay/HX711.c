#include <stdbool.h>
#include <stdint.h>
#include <avr/io.h>
#include <avr/cpufunc.h>
#include <util/delay.h>
#include "HX711.h"

void HX711_init()
{
    PD_SCK_SET_OUTPUT;
    DOUT_SET_INPUT;
}

char HX711_isReady()
{
	if ((DOUT_INPUT & (1 << DOUT_PIN)) == 0){
		return 0;
	}
	
	return 1;
}

void HX711_setGain(uint8_t gain)
{
	switch (gain)
	{
		case 128:		// channel A, gain factor 128
			GAIN = 1;
			break;
		case 64:		// channel A, gain factor 64
			GAIN = 3;
			break;
		case 32:		// channel B, gain factor 32
			GAIN = 2;
			break;
	}

	PD_SCK_SET_LOW;
}

int32_t HX711_read()
{
    // Will hold the 24-bit value
    int32_t Count = 0;

    // wait for the chip to become ready
	while (!HX711_isReady());

    for (uint8_t i=0; i<24; i++)
    {
        PD_SCK_SET_HIGH;
        Count = Count << 1;
        PD_SCK_SET_LOW;

        if(DOUT_READ)
        {
            Count++;
        }
    }

    // Set the gain for next time
    for (uint8_t i = 0; i < GAIN; i++)
    {
        PD_SCK_SET_HIGH;
        PD_SCK_SET_LOW;
    }

    // XOR the count
    Count ^= 0x800000;

    return(Count);
}

int32_t HX711_readAverage(uint8_t times)
{
	int32_t sum = 0;
	for (uint8_t i = 0; i < times; i++)
	{
		sum += HX711_read();
	}
	return sum / times;
}

void HX711_tare(uint8_t times)
{
	uint32_t sum = HX711_readAverage(times);
	HX711_setOffset(sum);
}

int32_t HX711_getValue(uint8_t times)
{
    // Invert the sign. Pushing on the scale increases the return value now
	return -(HX711_readAverage(times) - OFFSET);
}

double HX711_getUnits(uint8_t times)
{
	return HX711_getValue(times) / SCALE;
}

void HX711_setScale(float scale)
{
	SCALE = scale;
}

void HX711_setOffset(uint32_t offset)
{
    OFFSET = offset;
}

void HX711_powerDown()
{
	PD_SCK_SET_LOW;
	PD_SCK_SET_HIGH;
	_delay_us(70);
}

void HX711_powerUp()
{
	PD_SCK_SET_LOW;
}
