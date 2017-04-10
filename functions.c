#include <avr/io.h>
#include <avr/pgmspace.h>

#include "RTC.h"
#include "ADC.h"
#include "main.h"
#include "LCD_driver.h"
#include "button.h"
#include "eeprom.h"
#include "Alarm.h"

#include "functions.h"



char gHeaterStatus = FALSE;

void SetHeater(char status) {
	gHeaterStatus = status;
}


void LCD_FlashReset(void)
{
    gLcdFlashTimer = 0;
}

void LCD_GlobalTimerReset(void)
{
    gLcdGlobalTimer = 0;
}

void LCD_Colon(char show)
{
    gLcdColon = show;
}

void LCD_Degree(char show)
{
    gLcdDegree = show;
}

void LCD_T9(char show)
{
    gLcdHeating = show;
}

void LCD_ShowMinutes(unsigned char data) {
	unsigned char min, dmin;
	
	dmin = data / 10;	
	min = data % 10;
	LCD_WriteDigit(dmin, 3);
	LCD_WriteDigit(min, 4);
}

void LCD_ShowHours(unsigned char data) {
	unsigned char sec, dsec;
	
	dsec = data / 10;	
	sec = data % 10;
	LCD_WriteDigit(dsec, 1);
	LCD_WriteDigit(sec, 2);
}

void LCD_ShowTemp(unsigned int data) {
	unsigned char htemp, dtemp, temp;
	
	htemp = data / 100;	
	data = data % 100;	
	dtemp = data / 10;	
	temp = data % 10;
	
	LCD_WriteDigit(htemp, 1);
	LCD_WriteDigit(dtemp, 2);
	LCD_WriteDigit(temp, 3);
	LCD_WriteDigit(16, 4);
}

void LCD_ShowDigits(unsigned int data) {
	LCD_ShowMinutes((data % 100));
	LCD_ShowHours((data / 100));	
}

void LCD_TSymbol(unsigned char data, unsigned char clearT) {

	/*
		T1 = LCDDR0/6
		T2 = LCDDR0/4
		T3 = LCDDR0/2
		T4 = LCDDR0/0
		T5 = LCDDR1/6
		T6 = LCDDR1/4
		T7 = LCDDR1/2
		T8 = LCDDR16/2
		T9 = LCDDR16/1
	*/
	
	unsigned char *ptr = (LCD_Data + LCD_START_REG);

	if (data > 4 && data < 8) {	
		data = 2*(8-data);
		*ptr &= ~0x55;// clear T1-T4	
		ptr++;
		*ptr = (*ptr & ~0x54) | (1<<data);
		*(ptr + 15) &= ~0x06;// clear T8-T9
		
	}
	else if (data > 7) {
		data = (10 - data); //2
		*ptr &= ~0x55;// clear T1-T4
		ptr++;
		*ptr &= ~0x54;// clear T5-T7
		ptr +=15;
		*ptr = (*ptr & ~0x06) | (1<<data);
	}
	else {
		data = 2*(4-data);
		*ptr = (*ptr & ~0x55) | (1<<data);
		ptr++;
		*ptr &= ~0x54;// clear T5-T7
		*(ptr + 15) &= ~0x06;// clear T8-T9
	}	
	

	
	
	/*
	if (clearT) {
		*ptr &= ~0x55;
		*(ptr + 1) &= ~0x54;
		*(ptr + 16) &= ~0x06;
	}
	
	if (data < 8 && data > 4) { //data = 6
		data -= 4; // 2
		data = 2*(4-data); // 4
		if (clearT)  {
			*ptr &= ~0x55;
			*(ptr + 16) &= ~0x06;
		}
		ptr += 1; // LCDDR1
		*ptr |= (*ptr & ~0x54) | (1 << data);
	}
	else if (data < 5) { //data = 3
		data = 2*(4-data); // 2
		if (clearT)  {
			*(ptr + 1) &= ~0x54;
			*(ptr + 16) &= ~0x06;
		}
		*ptr |= (*ptr & ~0x55) | (1 << data);
		
	}
	else { //data = 8
		data = (10 - data); //2
		
		if (clearT)  {
			*ptr &= ~0x55;
			*(ptr + 1) &= ~0x54;
		}
		ptr += 16; // LCDDR16
		*ptr |=  (*ptr & ~0x06) | (1<<data);
	}

	*/
}

void LCD_ShowPsymbol(unsigned char data,  unsigned char clearP) {

	/*
		P1 = LCDDR15/6
		P2 = LCDDR15/5
		P3 = LCDDR15/4
		P4 = LCDDR15/2
		P5 = LCDDR15/1
		
		P6 = LCDDR16/6
		P7 = LCDDR16/4
		P8 = LCDDR1/0
		P9 = LCDDR16/0
	*/


	unsigned char *ptr = (LCD_Data + LCD_START_REG + 1);

	if (clearP) {
		*ptr &= ~0x01; //clear P8
		*(ptr + 14) &= ~0x76; //clear P1-P5
		*(ptr + 15) &= ~0x51; //clear P6, P7, P9
	}
	
	
	switch(data)
    {
		case 1: {*(ptr + 14)  |= 0x40; break;}
		case 2: {*(ptr + 14)  |= 0x20; break;}
		case 3: {*(ptr + 14)  |= 0x10; break;}
		case 4: {*(ptr + 14)  |= 0x04; break;}
		case 5: {*(ptr + 14)  |= 0x02; break;}
		case 6: {*(ptr + 15)  |= 0x40; break;}
		case 7: {*(ptr + 15)  |= 0x10; break;}
		case 8: {*ptr  |= 0x01; break;}
		case 9: {*(ptr + 15)  |= 0x01; break;}
	}
	

}


char ShowClock(char input) {

	LCD_ShowMinutes(gRtc.minute);
	LCD_ShowHours(gRtc.hour);
	LCD_TSymbol(gRtc.wday, TRUE);
	
	
	if (LCD_BLINK) {
        LCD_Colon(1);
    }
	else {
        LCD_Colon(0);
	}	
	if (PORTB & (1<<PB4)) {
		*(LCD_Data + LCD_START_REG + 16) |= 0x01;
	}
	else {
		*(LCD_Data + LCD_START_REG + 16) &= ~0x01;
	}
		
	LCD_Update_Required = 1;
	
	if (input ==  KEY_UP)
		return ST_TEMP;		
	else if (input ==  KEY_ENTER_LONG)
		return ST_TIME_HOURS_ADJUST;
	else
		return ST_TIME;
	
}




char ChangeHours(char input) {
	
	if (input ==  KEY_RIGHT) {		
		if (++gRtc.hour == 24)
			gRtc.hour = 0;
		LCD_FlashReset();
	}
	else if (input ==  KEY_LEFT) {
		if (gRtc.hour == 0)
			gRtc.hour = 23;
		else
			gRtc.hour--;
		LCD_FlashReset();
	}


	LCD_Colon(0);
	
	LCD_ShowMinutes(gRtc.minute);
	if (LCD_BLINK) {
		LCD_WriteDigit(10, 1);
		LCD_WriteDigit(10, 2);
	}
	else {		
		LCD_ShowHours(gRtc.hour);
	}
		
	LCD_TSymbol(gRtc.wday, TRUE);

	LCD_Update_Required = 1;
	
	if (input ==  KEY_ENTER)
		return ST_TIME_MINUTES_ADJUST;
	else
		return ST_TIME_HOURS_ADJUST;
}


char ChangeMinutes(char input) {

	if (input ==  KEY_RIGHT) {
		if (++gRtc.minute == 60)
			gRtc.minute = 0;
		LCD_FlashReset();
	}
	else if (input ==  KEY_LEFT) {
		if (gRtc.minute == 0)
			gRtc.minute = 59;
		else
			gRtc.minute--;
		LCD_FlashReset();
	}

	LCD_Colon(0);
	LCD_ShowHours(gRtc.hour);
	
	if (LCD_BLINK) {
		LCD_WriteDigit(10, 3);
		LCD_WriteDigit(10, 4);
	}
	else {		
		LCD_ShowMinutes(gRtc.minute);
	}
		
	LCD_TSymbol(gRtc.wday, TRUE);


	LCD_Update_Required = 1;
	
	if (input ==  KEY_ENTER)
		return ST_TIME_WDAY_ADJUST;
	else
		return ST_TIME_MINUTES_ADJUST;
}


char ChangeWday(char input) {

	if (input ==  KEY_RIGHT) {
		if (++gRtc.wday == 8)
			gRtc.wday = 1;
		LCD_FlashReset();
	}
	else if (input ==  KEY_LEFT) {
		if (gRtc.wday == 1)
			gRtc.wday = 7;
		else
			gRtc.wday--;
		LCD_FlashReset();
	}

	LCD_Colon(0);
	LCD_ShowHours(gRtc.hour);
	LCD_ShowMinutes(gRtc.minute);
	
	if (LCD_BLINK) {
		*(LCD_Data + LCD_START_REG) &= ~0x55;
		*(LCD_Data + LCD_START_REG + 1) &= ~0x54;
	}
	else {		
		LCD_TSymbol(gRtc.wday, TRUE);
	}	


	LCD_Update_Required = 1;
	
	if (input ==  KEY_ENTER)
		return ST_ALARM_HOUR_ADJUST;
	else
		return ST_TIME_WDAY_ADJUST;
}


char ChangeBright(char input) {

	
	if (input ==  KEY_LEFT) {
		OCR1B -= 5;
		if (OCR1B < 0)
			OCR1B = 0;
	}
	else if (input ==  KEY_RIGHT) {
		OCR1B += 5;
		if (OCR1B > 255)
			OCR1B = 255;
	}

	
	LCD_ShowDigits(OCR1B);
	
	LCD_Degree(0);

	LCD_Update_Required = TRUE;	

	if (input ==  KEY_UP) {
		SaveBright(OCR1B);
		return ST_TIME;
	}
	else
		return ST_BRIGHT_ADJUST;
	
}



char ShowTemp(char input) {
	//static char mux = 0b10000;
	static char tmp = 1;
	static int setTemp = 180;
	static int showSetTemp = 0;
	int ADCresult = 0;
	int temp;
	
	
	if (input ==  KEY_DOWN) {
		setTemp -= 5;
		showSetTemp = 1;
		LCD_FlashReset();
	}
	else if (input ==  KEY_UP) {
		setTemp += 5;
		showSetTemp = 1;
		LCD_FlashReset();
	}
	else if (input ==  KEY_ENTER) {
		tmp = !tmp;
	}
	
	
	//ADMUX = (ADMUX & 0b11100000) | mux;
	ADCresult = ADC_read(0);
	
	temp = (ADCresult*0.2)-285;
	
	//if (ADCresult < 8000) {
	
		if (!LCD_BLINK && showSetTemp) {
			LCD_WriteDigit((setTemp % 10), 3);
			LCD_WriteDigit(15, 4);
			LCD_ShowHours((setTemp / 10));
		}
		else {
			showSetTemp = 0;
			if (tmp) {
				LCD_WriteDigit((temp % 10), 3);
				LCD_WriteDigit(15, 4);
				LCD_ShowHours((temp / 10));
			}
			else {
				LCD_ShowDigits(ADCresult);
			}		
		}
	/*
	}
	
	else {
		LCD_WriteDigit(13, 1);
		LCD_WriteDigit(14, 2);
		LCD_WriteDigit(14, 3);
		LCD_WriteDigit(1, 4);
	}
	*/
	if (temp < setTemp) 
		PORTB |= (1<<PB6);
	else
		PORTB &= ~(1<<PB6);
		
	if (PORTB & (1<<PB6))
		LCD_T9(1);
	else
		LCD_T9(0);
	
	*(LCD_Data + LCD_START_REG) &= ~0x55;
	*(LCD_Data + LCD_START_REG + 1) &= ~0x54;	
	
	LCD_Colon(0);
	
	LCD_Update_Required = TRUE;
	

	
	if (input ==  KEY_UP)
		return ST_TIME;
	else	
	
		return ST_TEMP;
	
}

/* Чтение значений температуры из таблицы */
const unsigned int termo_table[] PROGMEM = {
    1703, 1916, 2115, 2302, 2478, 2643, 2799, 2947, 3087, 3219, 3345, 3465, 3578, 3687, 3790, 3889, 3984, 4074, 4160, 4243, 4323, 4400, 4473, 4544, 4612, 4678
};


int16_t calc_temperature(unsigned int adcsum, unsigned char TEMPERATURE_TABLE_START, unsigned char TEMPERATURE_TABLE_STEP) {
  unsigned char l = 0;
  unsigned char r = (sizeof(termo_table) / sizeof(termo_table[0])) - 1;
  unsigned int thigh = TEMPERATURE_TABLE_READ(r);
  
  
  if (adcsum >= thigh) {
    #ifdef TEMPERATURE_OVER
      if (adcsum > thigh) 
        return TEMPERATURE_OVER;
    #endif
    return TEMPERATURE_TABLE_STEP * r + TEMPERATURE_TABLE_START;
  }
  unsigned int tlow = TEMPERATURE_TABLE_READ(0);
  if (adcsum <= tlow) {
    #ifdef TEMPERATURE_UNDER
      if (adcsum < tlow)
        return TEMPERATURE_UNDER;
    #endif
    return TEMPERATURE_TABLE_START;
  }

  
  while ((r - l) > 1) {
    unsigned char m = (l + r) >> 1;
    unsigned int mid = TEMPERATURE_TABLE_READ(m);
    if (adcsum > mid) {
      l = m;
    } else {
      r = m;
    }
  }
  
  
  unsigned int vr = TEMPERATURE_TABLE_READ(r);//2400
  
  if (adcsum >= vr) {
    return l * TEMPERATURE_TABLE_STEP + TEMPERATURE_TABLE_START;
  }
  
  unsigned int vl = TEMPERATURE_TABLE_READ(l);//2200
  unsigned int vd = vr - vl;//200
  unsigned int res = TEMPERATURE_TABLE_START + l * TEMPERATURE_TABLE_STEP; //4 * 20 = 80

  if (vd) {
    res += ((TEMPERATURE_TABLE_STEP * (int32_t)(adcsum - vl) + (vd >> 1)) / vd); //80 + 15 = 95
  }
  return res;
  
}
/*
char ShowRoomTemp(char input) {
	static unsigned char pSymbol = 1;

	if (input ==  KEY_RIGHT) {
		if (++pSymbol == 10)
			pSymbol = 1;
	}
	else if (input ==  KEY_LEFT) {
		if (pSymbol == 1)
			pSymbol = 9;
		else
			pSymbol--;
	}
	LCD_WriteDigit(pSymbol, 1);
	LCD_TSymbol(pSymbol, TRUE);
	LCD_Update_Required = TRUE;

	return ST_TEMP;
}

*/
char ShowRoomTemp(char input) {
	static char tmp = SHOW_TEMPERATURE;
	static int settemp = 200;
	int ADCresult = 0;
	int temp;
	
	
	if (input ==  KEY_ENTER) {
		if (++tmp > SHOW_ADC) {
			tmp  = SHOW_TEMPERATURE;
		}
			
	}

	ADCresult = ADC_read(0);
	
	temp = calc_temperature(ADCresult, cCorrection, cStep);
	
	if ( temp > settemp ) {		
		SetHeater(FALSE);
	}
	else {
		SetHeater(TRUE);
	}
	if (gLcdGlobalTimer > 100 && tmp == SHOW_SETTEMP) {
		tmp = SHOW_TEMPERATURE;
	}

	if (input ==  KEY_RIGHT) {
		settemp += 5;
		if (settemp > 450) {
			settemp  = 450;
		}
		LCD_GlobalTimerReset(); 
		tmp = SHOW_SETTEMP;
	}
	else if (input ==  KEY_LEFT) {
		settemp -= 5;
		if (settemp < 0) {
			settemp  = 0;
		}
		LCD_GlobalTimerReset();
		tmp = SHOW_SETTEMP;
	}
	
	switch ( tmp ){

		case SHOW_ADC : 
			LCD_ShowDigits(ADCresult);	
		break;

		case SHOW_TEMPERATURE : 
			LCD_ShowTemp(temp);
		break;

		case SHOW_SETTEMP : 
			LCD_ShowTemp(settemp);
		break;

		default : 
		
		break;

	} 			
	
	LCD_Update_Required = TRUE;

	if (input ==  KEY_ENTER_LONG) {
		Beep();
	}
	
	if (input ==  KEY_UP)
		return ST_TIME;
	else	
	
		return ST_TEMP;
	
}

char CalibrationSoldering (char input) {
	static char step = 0;

	int ADCresult = 0;
	int temp = 0;

	if (input ==  KEY_ENTER) {
		if (++step > 1) {
			step  = 0;
		}	
	}

	ADCresult = ADC_read(0);

	if (step ==  0) {
		SetHeater(FALSE);

		if (input ==  KEY_RIGHT) {
			cCorrection++;
		}
		else if (input ==  KEY_LEFT) {
			cCorrection--;
		}
		//Show_hours(cCorrection);
	}
	else if (step ==  1) {
		if ( ADCresult > 3500 ) 	
			SetHeater(FALSE);			
		else
			SetHeater(TRUE);

		if (input ==  KEY_RIGHT) {
			cStep++;
		}
		else if (input ==  KEY_LEFT) {
			cStep--;
		}
		//Show_hours(cStep);
	}

	if (input ==  KEY_ENTER_LONG) {
		SaveECorrection();
	}
	
	temp = calc_temperature(ADCresult, cCorrection, cStep);

	LCD_ShowTemp(temp);
	LCD_Update_Required = TRUE;
	
	return 0;	
}
