#define __USE_C99_MATH

#include <stdbool.h>
#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>
#include "HX711_1.h"
/*
void HX711_init(uint8_t gain);
int HX711_is_ready();
void HX711_set_gain(uint8_t gain);
int32_t HX711_read();
int32_t HX711_read_average(uint8_t times);
double HX711_get_value(uint8_t times);
float HX711_get_units(uint8_t times);
void HX711_tare(uint8_t times);
void HX711_set_scale(float scale);
float HX711_get_scale();
void HX711_set_offset(int32_t offset);
int32_t HX711_get_offset();
void HX711_power_down();
void HX711_power_up();
uint8_t shiftIn(void);
*/
void HX711_1_init(uint8_t gain)
{
    PD_SCK_SET_OUTPUT_1;
    DOUT_SET_INPUT_1;

    HX711_1_set_gain(gain);
}

int HX711_1_is_ready(void)
{
    return (DOUT_INPUT_1 & (1 << DOUT_PIN_1)) == 0;
}

void HX711_1_set_gain(uint8_t gain)
{
	switch (gain)
	{
		case 128:		// channel A, gain factor 128
			GAIN_1 = 1;
			break;
		case 64:		// channel A, gain factor 64
			GAIN_1 = 3;
			break;
		case 32:		// channel B, gain factor 32
			GAIN_1 = 2;
			break;
	}

	PD_SCK_SET_LOW_1;
	HX711_1_read();
}

uint32_t HX711_1_read(void)
{
	// wait for the chip to become ready
	while (!HX711_1_is_ready());

    unsigned long count; 
    unsigned char i;
	
	/*
    DOUT_SET_HIGH;
    _delay_us(1);
    PD_SCK_SET_LOW;
    _delay_us(1);
	*/
	
    count=0; 
    while(DOUT_READ_1); 
    for(i=0;i<24;i++)
    { 
        PD_SCK_SET_HIGH_1; 
        _delay_us(1);
        count=count<<1; 
        PD_SCK_SET_LOW_1; 
        _delay_us(1);
        if(DOUT_READ_1)
            count++; 
    }
	
	// following for loop was added by me
	for(i=0;i<GAIN_1;i++){
		PD_SCK_SET_HIGH_1;
		_delay_us(1);
		PD_SCK_SET_LOW_1;
		_delay_us(1);
	}
	
	/* 
    count = count>>6;
    PD_SCK_SET_HIGH; 
    _delay_us(1);
    PD_SCK_SET_LOW; 
    _delay_us(1);
	*/
	
	
    count ^= 0x800000;
    return(count);
}

uint32_t HX711_1_read_average(uint8_t times)
{
	uint32_t sum = 0;
	for (uint8_t i = 0; i < times; i++)
	{
		sum += HX711_1_read();
		// TODO: See if yield will work | yield();
	}
	return sum / times;
}

double HX711_1_get_value(uint8_t times)
{
	return HX711_1_read_average(times) - OFFSET_1;
}

float HX711_1_get_units(uint8_t times)
{
	return HX711_1_get_value(times) / SCALE_1;
}

void HX711_1_tare(uint8_t times)
{
	double sum = HX711_1_read_average(times);
	HX711_1_set_offset(sum);
}

void HX711_1_set_scale(float scale)
{
	SCALE_1 = scale;
}

float HX711_1_get_scale(void)
{
	return SCALE_1;
}

void HX711_1_set_offset(double offset)
{
    OFFSET_1 = offset;
}

double HX711_1_get_offset(void)
{
	return OFFSET_1;
}

void HX711_1_power_down(void)
{
	PD_SCK_SET_LOW_1;
	PD_SCK_SET_HIGH_1;
	_delay_us(70);
}

void HX711_1_power_up(void)
{
	PD_SCK_SET_LOW_1;
}

uint8_t shiftIn_1(void)
{
    uint8_t value = 0;

    for (uint8_t i = 0; i < 8; ++i)
    {
        PD_SCK_SET_HIGH_1;
        value |= DOUT_READ_1 << (7 - i);
        PD_SCK_SET_LOW_1;
    }
    return value;
}
