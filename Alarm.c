#include <avr/io.h>
#include <avr/interrupt.h>

#include "Alarm.h"
#include "main.h"
#include "RTC.h"
#include "button.h"
#include "LCD_driver.h"
#include "functions.h"



volatile alarm gAlarm; 


void Alarm_init(void)
{
    // initial time and date setting
    gAlarm.minute  = 01;
    gAlarm.hour    = 15;
	gAlarm.playing = FALSE;
	gAlarm.mode    = ALARM_MODE_OFF;
}


void CheckAlarm(void)
{
	if(gAlarm.mode != ALARM_MODE_OFF && !gAlarm.playing && gRtc.second == 0 && gRtc.minute == gAlarm.minute && gRtc.hour == gAlarm.hour)
	{
		char day1 = -1;
		char day2 = -1;
		
		switch(gAlarm.mode)
		{			
			case ALARM_MODE_1_5:
				day1 = 1;
				day2 = 5;
				break;
			case ALARM_MODE_6_7:
				day1 = 6;
				day2 = 7;
				break;
			default:
				return;
		} 
		
		char day = gRtc.wday;
		
		if(day >= day1 && day <= day2)
		{
			gAlarm.playing = TRUE;		
			//PlayAlarm();
		}
	}
}

char OnAlarm(char input)
{
	LCD_ShowMinutes(88);
	
	if (LCD_BLINK)
        PORTB |= (1<<PB0);
    else
        PORTB &= ~(1<<PB0);
	
	*(LCD_Data + LCD_START_REG) |= 0x55;
	*(LCD_Data + LCD_START_REG + 1) |= 0x54;
	
	LCD_Update_Required = 1;
	
	if (input != KEY_NULL)
		gAlarm.playing = FALSE;
	
  
    if (!gAlarm.playing) {
		PORTB &= ~(1<<PB0);
        return ST_TIME;
	}
	else	
		return ST_ON_ALARM;
}



char SetAlarmHour(char input)
{
    static char enter_function = 1;
    static unsigned char hour;

    if (enter_function) {
        hour = gAlarm.hour;
		enter_function = 0;
	}
	if (input ==  KEY_RIGHT) {		
		if (++hour == 24)
			hour = 0;
	}
	else if (input ==  KEY_LEFT) {
		if (hour == 0)
			hour = 23;
		else
			hour--;
	}

    if (input != KEY_NULL) {
        LCD_FlashReset();
	}	
	
	LCD_ShowMinutes(gAlarm.minute);
	
	if (LCD_BLINK) {
		LCD_WriteDigit(10, 1);
		LCD_WriteDigit(10, 2);
	}
	else {		
		LCD_ShowHours(hour);
	}

    LCD_Update_Required = 1;

    if (input == KEY_ENTER)
    {
        // store the temporary adjusted values to the global variables
        cli(); // mt __disable_interrupt();
        gAlarm.hour = hour;
        sei(); // mt __enable_interrupt();
		enter_function = 1;
        return ST_ALARM_MINUTE_ADJUST;
    }
	else {
		enter_function = 0;
		return ST_ALARM_HOUR_ADJUST;
	}
}


char SetAlarmMinute(char input)
{
    static char enter_function = 1;
    static unsigned char minute;

    if (enter_function) {
        minute = gAlarm.minute;
		enter_function = 0;
	}
	if (input ==  KEY_RIGHT) {		
		if (++minute == 60)
			minute = 0;
	}
	else if (input ==  KEY_LEFT) {
		if (minute == 0)
			minute= 59;
		else
			minute--;
	}

    if (input != KEY_NULL) {
        LCD_FlashReset();
	}

	LCD_ShowHours(gAlarm.hour);
	
	if (LCD_BLINK) {
		LCD_WriteDigit(10, 3);
		LCD_WriteDigit(10, 4);
	}
	else {		
		LCD_ShowMinutes(gAlarm.minute);
	}

    LCD_Update_Required = 1;

    if (input == KEY_ENTER)
    {
        // store the temporary adjusted values to the global variables
        cli(); // mt __disable_interrupt();
        gAlarm.minute = minute;
        sei(); // mt __enable_interrupt();
		enter_function = 1;
        return ST_ALARM_MODE_ADJUST;
    }
	else {
		enter_function = 0;
		return ST_ALARM_MINUTE_ADJUST;
	}
}

char SetAlarmMode(char input)
{
	if (input == KEY_RIGHT) {
		if (++gAlarm.mode == 3)
			gAlarm.mode = 2;
    }
	else if (input == KEY_LEFT) {
        if (gAlarm.mode == 0)
			gAlarm.mode = 0;
		else
			gAlarm.mode--;
	}

    switch(gAlarm.mode)
	{
		case ALARM_MODE_OFF:
			LCD_WriteDigit(10, 1);
			LCD_WriteDigit(0, 2);	
			LCD_WriteDigit(12, 3);
			LCD_WriteDigit(12, 4);	
			*(LCD_Data + LCD_START_REG) &= ~0x55;
			if (LCD_BLINK) 
				*(LCD_Data + LCD_START_REG + 1) &= ~0x55;
			else
				*(LCD_Data + LCD_START_REG + 1) |= 0x01;
			break;
		case ALARM_MODE_1_5:
			LCD_WriteDigit(10, 1);
			LCD_WriteDigit(10, 2);	
			LCD_WriteDigit(0, 3);
			LCD_WriteDigit(11, 4);	
			if (LCD_BLINK) {
				*(LCD_Data + LCD_START_REG) &= ~0x55;
				*(LCD_Data + LCD_START_REG + 1) &= ~0x55;
			}
			else {
				*(LCD_Data + LCD_START_REG) |= 0x55;
				*(LCD_Data + LCD_START_REG + 1) |= 0x40 | 0x01;
			}
			break;
		case ALARM_MODE_6_7:
			LCD_WriteDigit(10, 1);
			LCD_WriteDigit(10, 2);	
			LCD_WriteDigit(0, 3);
			LCD_WriteDigit(11, 4);	
			
			if (LCD_BLINK) {
				*(LCD_Data + LCD_START_REG) &= ~0x55;
				*(LCD_Data + LCD_START_REG + 1) &= ~0x55;
			}
			else {
				*(LCD_Data + LCD_START_REG + 1) |= 0x14 | 0x01;
			}
			
			break;
	}    

    LCD_Update_Required = 1;

	if (input != KEY_NULL)
        LCD_FlashReset();
    
		
    if (input == KEY_ENTER)
		return ST_BRIGHT_ADJUST;
	else
		return ST_ALARM_MODE_ADJUST;
}

