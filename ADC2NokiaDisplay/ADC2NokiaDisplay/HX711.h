#ifndef HX711_h
#define HX711_h

    #define PD_SCK_PORT         PORTB	                        // Power Down and Serial Clock Input Port
    #define PD_SCK_DDR          DDRB                            // Power Down and Serial Clock DDR
    #define PD_SCK_PIN          0x01                            // Power Down and Serial Clock Pin

    #define PD_SCK_SET_OUTPUT   PD_SCK_DDR |= (1<<PD_SCK_PIN)

    #define PD_SCK_SET_HIGH     PD_SCK_PORT |= (1<<PD_SCK_PIN); _NOP(); _NOP(); _NOP(); _NOP(); _NOP();
    #define PD_SCK_SET_LOW      PD_SCK_PORT &= ~(1<<PD_SCK_PIN); _NOP(); _NOP(); _NOP(); _NOP();

    #define DOUT_PORT           PORTA                           // Serial Data Output Port
    #define DOUT_DDR            DDRA                            // Serial Data Output DDR
    #define DOUT_INPUT          PINA                            // Serial Data Output Input
    #define DOUT_PIN            0x01  		                    // Serial Data Output Pin
    #define DOUT_READ           (DOUT_INPUT & (1<<DOUT_PIN))    // Serial Data Output Read Pin

    #define DOUT_SET_HIGH       DOUT_PORT |= (1<<DOUT_PIN)
    #define DOUT_SET_LOW        DOUT_PORT &= ~(1<<DOUT_PIN)
    #define DOUT_SET_INPUT      DOUT_DDR &= ~(1<<DOUT_PIN); DOUT_SET_HIGH
    #define DOUT_SET_OUTPUT     DOUT_DDR |= (1<<DOUT_PIN); DOUT_SET_LOW

    uint8_t GAIN;		                // amplification factor
    int32_t OFFSET;	                    // used for tare weight
    float SCALE;	                    // used to return weight in grams, kg, ounces, whatever

	void HX711_init(void);

	char HX711_isReady();

	/// Set the GAIN parameter for use in HX711_read()
	void HX711_setGain(uint8_t gain);

	/// Set the OFFSET parameter for use in HX711_tare()
	void HX711_setOffset(uint32_t offset);

	/// Set the SCALE parameter for use in HX711_getUnits()
	void HX711_setScale(float scale);

	/// Reads the raw ADC value
	int32_t HX711_read();

	/// Reads the raw ADC value n times and calculates the average
	int32_t HX711_readAverage(uint8_t times);

	/// Read the raw ADC value and subtract the tare value
	int32_t HX711_getValue(uint8_t times);

	/// Calculate the weight using the SCALE parameter. Outputs real units
	double HX711_getUnits(uint8_t times);

	/// Zero the scale
	void HX711_tare(uint8_t times);

	void HX711_powerDown();

	void HX711_powerUp();

#endif /* HX711_h */
