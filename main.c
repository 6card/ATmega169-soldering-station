#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include "main.h"
#include "RTC.h"
#include "Alarm.h"
#include "LCD_driver.h"
#include "ADC.h"
#include "button.h"
#include "functions.h"
#include "menu.h"
#include "eeprom.h"

void Timer1_Init(void)
{
	//FAST PWM	
	TIMSK1 &= ~((1<<ICIE1) | (1<<TOIE1) | (1<<OCIE1B) | (1<<OCIE1A)); // Отключаем прерывания Таймера 1.
	TCCR1B |= (1<<WGM12); //Fast PWM, 8-bit
	TCCR1A |= (1<<COM1B1) | (1<<WGM10);// Non-inverted PWM
	TCCR1B |= (1<<CS10); //No pescaler 3906 Hz	

    DDRB |= (1<<PB6); ////Set OC1B PIN as output
	
	OCR1B = EEPROMLoadBright();
}

int main (void)
{
	char (*StateFunc)(char);
	unsigned char nextstate = ST_TIME;
	unsigned char state = ST_TIME;
	
	char input;
	
	unsigned char i, j;
			
	LCD_Init();		
	ADC_init();	
	RTC_init();	
	Alarm_init();	
	Timer1_Init();	
	Buttons_Init();	
	EEPROMInit();

	// Порт B на выход
	sbi(DDRB, LED_PIN); //LED
	sbi(DDRB, BUZZER_PIN); //BUZZER
	sbi(DDRB, HEATER_PIN); //HEATER

	// НАЧАЛЬНЫЕ ЗНАЧЕНИЯ
	sbi(PORTB, LED_PIN); //LED
	cbi(PORTB, BUZZER_PIN); //BUZZER
	sbi(PORTB, HEATER_PIN); //HEATER
	
	sei(); // Разрешаем прерывания глобально. 	
	
	if (UP_DIRECTION)
		StateFunc = CalibrationSoldering; // Settings on start
	else
		StateFunc = ShowRoomTemp;
	
	while(1) {
	
		input = getkey(); // Read buttons		
		
		if (input && input != KEY_ENTER_LONG)
			PlayClick();
		
		if (StateFunc) {		
			nextstate = StateFunc(input);		
		}

		if (nextstate != state)
		{
			state = nextstate;
			for (i=0; (j=pgm_read_byte(&menu_state[i].state)); i++)
			{
				if (j == state)
				{
					StateFunc = (PGM_VOID_P) pgm_read_word(&menu_state[i].Func);
					break;
				}
			}
		}
		
		if(gAlarm.playing)
		{
			nextstate = ST_ON_ALARM;
		} 

		if (gHeaterStatus) {
			HEATER_ON;
		}
		else {
			HEATER_OFF;
		}

	};
}

void Delay(unsigned int millisec)
{
	unsigned char i;

	while (millisec--) {
		for (i=0; i<125; i++) {
			asm volatile ("nop"::);
		}
	}
}

void PlayClick(void)
{
	unsigned char i;
	for (i = 0; i < 10; i++) {
		sbi(PORTB, BUZZER_PIN);
		Delay(1);
		cbi(PORTB, BUZZER_PIN);
		Delay(1);
	}
}

void Beep(void)
{
	sbi(PORTB, BUZZER_PIN);
	Delay(200);
	cbi(PORTB, BUZZER_PIN);
}


ISR(TIMER1_COMPB_vect) //compare interrupt vector
{ 
	//PORTB ^= (1<<PB0);
	
}
