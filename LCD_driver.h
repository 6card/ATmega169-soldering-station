/************************************************************************/
// Definitions
/************************************************************************/
#define LCD_INITIAL_CONTRAST    0x0F

#define LCD_FLASH_SEED 30 
#define LCD_REGISTER_COUNT      20
#define TEXTBUFFER_SIZE         32

#define LCD_START_REG 0

/************************************************************************/
//MACROS
/************************************************************************/


// DEVICE SPECIFIC!!! (ATmega169)
#define LCDREG ((unsigned char*)(0xEC))

// DEVICE SPECIFIC!!! (ATmega169) First LCD segment register
#define LCD_CONTRAST_LEVEL(level) LCDCCR=(0x0F & level)


#define T1T4_MASK = 0x55;
#define T5T7_MASK = 0x54;


/************************************************************************/
// Global variables
/************************************************************************/
extern volatile char LCD_Update_Required;
extern unsigned char LCD_Data[LCD_REGISTER_COUNT];
extern volatile char LCD_BLINK;

extern volatile char gLcdFlashTimer;
extern volatile unsigned int gLcdGlobalTimer;
extern char gLcdColon;
extern char gLcdHeating;
extern char gLcdDegree;



/************************************************************************/
// Global functions
/************************************************************************/
void LCD_Init (void);
void LCD_WriteDigit(char input, char digit);
void LCD_AllSegments(char show);
void LCD_Clear(void);
