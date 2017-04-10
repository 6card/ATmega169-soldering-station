#ifndef FALSE
#define FALSE   0
#define TRUE    (!FALSE)
#endif

#ifndef NULL
#define NULL ((void *)0)
#endif

#define BUZZER_PIN PB0
#define LED_PIN PB3
#define HEATER_PIN PB2


#define sbi(x,y) x |= (1 << y)
#define cbi(x,y) x &= ~(1<< y)
#define READ(x,y) ((0u == (x & (1<<y)))?0u:1u)
#define TOGGLE(x,y) (x ^= (1<<y))


//Soldering Macroses
#define HEATER_ON cbi(PORTB,HEATER_PIN)
#define HEATER_OFF sbi(PORTB,HEATER_PIN)


// Menu state machine states
#define ST_TIME                         		20
#define	ST_TEMP				21

#define ST_TIME_HOURS_ADJUST		23
#define ST_TIME_MINUTES_ADJUST	24
#define ST_TIME_WDAY_ADJUST		25

#define ST_ON_ALARM			30
#define ST_ALARM_HOUR_ADJUST		31
#define ST_ALARM_MINUTE_ADJUST	32
#define ST_ALARM_MODE_ADJUST		33
#define	ST_BRIGHT_ADJUST			34






char ShowInfo(char);
char ChangeHours(char);
char ChangeMinutes(char);
char ChangeWday(char);
void Delay(unsigned int);

void PlayClick(void);
void Beep(void);