#ifndef FALSE
#define FALSE   0
#define TRUE    (!FALSE)
#endif

#define ALARM_MODE_OFF	0
#define	ALARM_MODE_1_5	1	// Mon. to Fri.
#define	ALARM_MODE_6_7	2	// Sat. to Sun.

typedef struct{ 
	unsigned char minute;
	unsigned char hour;
	unsigned char playing;
	unsigned char mode;
}alarm;

volatile extern alarm gAlarm; 

void Alarm_init(void);
void CheckAlarm(void);
char OnAlarm(char);

char SetAlarmHour(char);
char SetAlarmMinute(char);
char SetAlarmMode(char);
