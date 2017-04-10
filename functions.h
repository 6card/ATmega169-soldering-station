#ifndef FALSE
#define FALSE   0
#define TRUE    (!FALSE)
#endif

#define SHOW_TEMPERATURE 0
#define SHOW_ADC 1
#define SHOW_SETTEMP 2

//Read table
#define TEMPERATURE_UNDER 0
#define TEMPERATURE_OVER 500
#define TEMPERATURE_TABLE_READ(i) pgm_read_word(&termo_table[i])

extern char gHeaterStatus;

void SetHeater(char) ;

void LCD_FlashReset(void);
void LCD_GlobalTimerReset(void);
void LCD_Colon(char);
void LCD_T9(char);

void LCD_ShowMinutes(unsigned char);
void LCD_ShowHours(unsigned char);
void LCD_TSymbol(unsigned char, unsigned char);
void LCD_ShowPsymbol(unsigned char,  unsigned char);

void LCD_ShowTemp(unsigned int);
void LCD_ShowDigits(unsigned int);


char ShowClock(char);
char ShowTemp(char);
char ChangeHours(char);
char ChangeMinutes(char);
char ChangeWday(char);
char ChangeBright(char);

char ShowRoomTemp(char);
char CalibrationSoldering (char );
