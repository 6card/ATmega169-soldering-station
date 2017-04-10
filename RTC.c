#include <avr/io.h>
#include <avr/interrupt.h>

#include "RTC.h"
#include "Alarm.h"

volatile time gRtc; 


void RTC_init(void) {
	// Отключаем прерывания Таймера 2.
    TIMSK2 &= ~((1<<TOIE2) | (1<<OCIE2A)); 
    // Переводим Таймер 2 в асинхронный режим (тактирование от
    // часового кварцевого резонатора).
    ASSR = (1<<AS2);	
	//Устанавливаем коэффициент деления равным 128.
	TCCR2A = (1<<CS22) | (1<<CS20); 
    // Разрешаем прерывание от Таймера 2.
    TIMSK2 |= (1<<TOIE2);

		
	gRtc.wday = 4;
	gRtc.hour = 15;
	gRtc.minute = 00;
	gRtc.second  = 50;
 
}

char not_leap(void)
{
    if (!(gRtc.year%100))
        return (char)(gRtc.year%400);
    else
        return (char)(gRtc.year%4);
}


ISR(TIMER2_OVF_vect) //Прерывания Таймера 2 по переполнению
{ 	
	
	if (++gRtc.second==60)
    {
        gRtc.second=0;
 
		if (++gRtc.minute==60) 
        {
            gRtc.minute=0;
 
            if (++gRtc.hour==24)
            {
                gRtc.hour=0;
				
				if (++gRtc.wday==8)
				{
					gRtc.wday=1;
				}				
 
                if (++gRtc.date==32)
                {
                    gRtc.mounth++;
                    gRtc.date=1;
                }
                else if (gRtc.date==31) 
                { 
                    if ((gRtc.mounth==4) || (gRtc.mounth==6) || (gRtc.mounth==9) || (gRtc.mounth==11)) 
                    {
                        gRtc.mounth++;
                        gRtc.date=1;
                    }
                }
                else if (gRtc.date==30)
                {
                    if(gRtc.mounth==2)
                    {
                        gRtc.mounth++;
                        gRtc.date=1;
                    }
                } 
                else if (gRtc.date==29) 
                {
                    if((gRtc.mounth==2) && (not_leap()))
                    {
                        gRtc.mounth++;
                        gRtc.date=1;
                    } 
                } 
 
                if (gRtc.mounth==13)
                {
                    gRtc.mounth=1;
                    gRtc.year++;
                }
            }
        }
		
		CheckAlarm();
    }

} 