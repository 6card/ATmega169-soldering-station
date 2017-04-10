#include <avr/io.h>
#include <util/delay.h>

void ADC_init(void)
{	
	ADMUX = (1<<REFS0); // ADC AVCC Voltage Reference		
	ADCSRA = (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0); // ADC Enable, prescaler 1MHz / 128 = 8kHz
}



int ADC_read(char pin)
{
    char i;
    int ADC_temp;

    unsigned int ADCr = 0;
	
	ADMUX = (ADMUX & 0xE0) | pin;
	
    ADCSRA |= (1<<ADEN);     // Enable the ADC
   
        
    for(i=0;i<8;i++)            // do the ADC conversion 8 times for better accuracy 
    {
        ADCSRA |= (1<<ADSC);        // do single conversion
        while(ADCSRA & (1<<ADSC));    // wait for conversion done, ADIF flag active
        
        ADC_temp = ADCL;            // read out ADCL register
        ADC_temp += (ADCH << 8);    // read out ADCH register        

        ADCr += ADC_temp;      // accumulate result (8 samples) for later averaging
    }

    //ADCr = ADCr >> 3;     // average the 8 samples
    
    ADCSRA &= ~(1<<ADEN);      // disable the ADC

    return ADCr;
}
