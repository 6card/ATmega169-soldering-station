#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/pgmspace.h>

#include "button.h"

volatile char KEY = 0;
volatile char KEY_PUSH = 0;
volatile char KEY_VALID = FALSE;

//volatile char KeyClickStatus = FALSE; // Variable to enable/disable keyclick

volatile unsigned char debounce_timer = 0;

void Buttons_Init(void)
{
	
	DDRE &= ~((1<<UP_PIN) | (1<<DOWN_PIN) | (1<<PUSH_PIN)); // Порт E на вход JOYSTICK
	DDRE &= ~((1<<ENCODER_A) | (1<<ENCODER_B)); // Порт E на вход ENCODER
	
	PORTE |= (1<<UP_PIN) | (1<<DOWN_PIN) | (1<<PUSH_PIN);	// включаем поддтяжки на порте E JOYSTICK
	PORTE |= (1<<ENCODER_A) | (1<<ENCODER_B);	// включаем поддтяжки на порте E ENCODER
    
    	EIMSK = (1<<PCIE0) | (1<<INT0); // включение прерыаний INT0 и PCINT7..0	
    	EICRA = (1<<ISC01)  |  (1<<ISC00); // Срабатывание INT0 по фронту
	
	PCMSK0 |= (1<<UP_PIN) | (1<<DOWN_PIN) | (1<<PUSH_PIN); // маска прерываний на порт E JOYSTICK
	PCMSK0 |= (1<<ENCODER_A) | (1<<ENCODER_B); // маска прерываний на порт E ENCODER

	
}


void PinChangeInterrupt(void)
{
		
	if (POWER_LOST) {
			//set_sleep_mode(SLEEP_MODE_PWR_SAVE);
			//sleep_mode();
			//sleep_enable();
	}

	if(PUSH_DIRECTION && !KEY_PUSH) {
		KEY_PUSH = KEY_ENTER;	
	}
	else if(UP_DIRECTION && !KEY_PUSH){
		KEY_PUSH = KEY_UP;	
	}
	else if(DOWN_DIRECTION && !KEY_PUSH){
		KEY_PUSH = KEY_DOWN;	
	}
	
	if(!PUSH_DIRECTION && (KEY_PUSH == KEY_ENTER))
    {	
		if (debounce_timer > DEBOUNCE_TIME) {
			KEY = KEY_ENTER;
			KEY_VALID = TRUE;
		}
		KEY_PUSH = KEY_NULL;		
	}
	else if(!UP_DIRECTION && (KEY_PUSH == KEY_UP))
    {	
		if (debounce_timer > DEBOUNCE_TIME) {
			KEY = KEY_UP;
			KEY_VALID = TRUE;
		}
		KEY_PUSH = KEY_NULL;		
	}
	else if(!DOWN_DIRECTION && (KEY_PUSH == KEY_DOWN))
    {	
		if (debounce_timer > DEBOUNCE_TIME) {
			KEY = KEY_DOWN;
			KEY_VALID = TRUE;
		}
		KEY_PUSH = KEY_NULL;		
	}
	//EIFR = (1<<PCIF0); // Delete pin change interrupt flags

}

char getkey(void)
{
    char k;

    cli();

    if (KEY_VALID)              // Check for unread key in buffer
    {
        k = KEY;
        KEY_VALID = FALSE;
    }
    else
        k = KEY_NULL;           // No key stroke available

    sei();

    return k;
}

void EncoderChangeInterrupt(void) {
	static unsigned char old_AB = 3;  //lookup table index
	unsigned char encport; //encoder port copy
	unsigned char direction;
	static const char enc_states[] PROGMEM = {0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0};  //encoder lookup table

	old_AB <<=2;  //remember previous state
	encport = (ENCODER_PIN & ((1<<ENCODER_A) | (1<<ENCODER_B))) >> 4; //read encoder
	old_AB |= encport;
	direction = pgm_read_byte(&(enc_states[( old_AB & 0x0f )]));
	/* post "Navigation forward/reverse" event */
	if( direction && ( encport == 3 )) { //check if at detent and transition is valid
		// post "Navigation forward/reverse" event

		if( direction == 1 ) {
			KEY = KEY_RIGHT;
			KEY_VALID = TRUE;
		}
		else {
			KEY = KEY_LEFT;
			KEY_VALID = TRUE;
		}
	}//if( direction && encport == 3...
	
	
}

void CheckLongPushButton(void) {
//long push button with timer
	if (KEY_PUSH) {
		debounce_timer++;
		if (debounce_timer > LONG_PUSH_TIME)		
			debounce_timer = LONG_PUSH_TIME;
	}
	else
		debounce_timer = 0;
		
	if(PUSH_DIRECTION && (KEY_PUSH == KEY_ENTER))
    {	
		if (debounce_timer == LONG_PUSH_TIME) {
			KEY = KEY_ENTER_LONG;
			KEY_VALID = TRUE;
			KEY_PUSH = KEY_NULL;
		}
				
	}
}

ISR(PCINT0_vect) {	
	PinChangeInterrupt();	
	EncoderChangeInterrupt();
	//EIFR = (1<<PCIF0); // Delete pin change interrupt flags
}

ISR(INT0_vect) {	

}