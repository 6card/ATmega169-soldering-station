typedef struct{ 
	unsigned char second;
	unsigned char minute;
	unsigned char hour;
	unsigned char wday;
	unsigned char date;
	unsigned char mounth;
	unsigned char year;
}time;

volatile extern time gRtc;

void RTC_init(void);