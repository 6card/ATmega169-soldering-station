// menu.h



typedef struct PROGMEM
{
    unsigned char state;
    unsigned char input;
    unsigned char nextstate;
} MENU_NEXTSTATE;


typedef struct PROGMEM
{
    unsigned char state;
    char (*Func)(char input);
} MENU_STATE;


const MENU_NEXTSTATE menu_nextstate[] PROGMEM = {
//  STATE                       INPUT       NEXT STATE
    {ST_TIME,                   KEY_ENTER,  ST_TIME_HOURS_ADJUST},
    //{ST_TIME,                   KEY_NEXT,   ST_TIME_CLOCK},


    {ST_TIME_HOURS_ADJUST,      KEY_ENTER,  ST_TIME_MINUTES_ADJUST},
	
	//{ST_TIME_MINUTES_ADJUST,    KEY_ENTER,  ST_TIME_WDAY_ADJUST},

	//{ST_TIME_WDAY_ADJUST,    	KEY_ENTER,  ST_TIME},   

    {0,                         0,       	0}

};

const MENU_STATE menu_state[] PROGMEM = {
//  STATE                               STATE_FUNC
    {ST_TIME,                           ShowClock},
	{ST_TEMP,                           ShowRoomTemp},
	
    {ST_TIME_HOURS_ADJUST,              ChangeHours},
	{ST_TIME_MINUTES_ADJUST,            ChangeMinutes},
	{ST_TIME_WDAY_ADJUST,            	ChangeWday},
	{ST_ON_ALARM,            			OnAlarm},
	{ST_ALARM_HOUR_ADJUST,				SetAlarmHour},
	{ST_ALARM_MINUTE_ADJUST,			SetAlarmMinute},
	{ST_ALARM_MODE_ADJUST,				SetAlarmMode},
	{ST_BRIGHT_ADJUST,                        ChangeBright},
	{0, 								0}
};