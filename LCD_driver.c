// Include files.
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <inttypes.h>
#include <avr/interrupt.h>


#include "button.h"
#include "LCD_driver.h"

#define BOOL    char
#define FALSE   0
#define TRUE    (!FALSE)

// Used to indicate when the LCD interrupt handler should update the LCD
volatile char LCD_Update_Required = FALSE;

volatile char LCD_BLINK = FALSE;

// LCD display buffer (for double buffering).
unsigned char LCD_Data[LCD_REGISTER_COUNT];

// The gFlashTimer is used to determine the on/off
// timing of flashing characters
volatile char gLcdFlashTimer = 0;

volatile unsigned int gLcdGlobalTimer = 0;

// Turns on/off the colons on the LCD
char gLcdColon = 0;
char gLcdHeating = 0;
char gLcdDegree = 0;


// Look-up table used when converting ASCII to
// LCD display data (segment control)
// mt __flash unsigned int LCD_character_table[] =

const unsigned char LCD_SegTable[] PROGMEM =
{
	0x7B, //0
	0x12, //1
	0x67, //2
	0x37, //3
	0x1E, //4
	0x3D, //5
	0x7D, //6
	0x13, //7
	0x7F, //8
	0x3F, //9
	0x00, // 
	0x54, //11 - n
	0x4D, //12 - f
	0x6D, //13 - E
	0x44, //14 - r
	0x0f, //15 - degree
	0x64, //16 - c
	
};


/*****************************************************************************
*
*   Function name : LCD_Init
*
*   Returns :       None
*
*   Parameters :    None
*
*   Purpose :       Initialize LCD_displayData buffer.
*                   Set up the LCD (timing, contrast, etc.)
*
*****************************************************************************/
void LCD_Init (void)
{
    LCD_AllSegments(FALSE);                     // Clear segment buffer.

    //LCD_CONTRAST_LEVEL(LCD_INITIAL_CONTRAST);    //Set the LCD contrast level

	/* Use 32 kHz crystal oscillator */
	/* 1/3 Bias and 1/4 duty, SEG15:SEG24 is used as port pins */
	LCDCRB = (1<<LCDCS) | (1<<LCDMUX1)| (1<<LCDMUX0) | (1<<LCDPM0);
	/* Using 16 as prescaler selection and 7 as LCD Clock Divide */ 
	/* gives a frame rate of 51.2 Hz */
	LCDFRR = (1<<LCDCD2);
	/* Set segment drive time to 300 
	?
	s and output voltage to 3.0 V*/
	LCDCCR = (1<<LCDCC3);
	
	//LCDCCR |= (1<<LCDDC2) | (1<<LCDDC1) | (1<<LCDDC0);
	/* Enable LCD, default waveform and interrupt enabled */
	LCDCRA = (1<<LCDEN) | (1<<LCDIE);

    //LCDCRA = (1<<LCDEN) | (1<<LCDIE) | (1<<LCDAB);           // Enable LCD and set low power waveform

    LCD_Update_Required = FALSE;
	
}


/*****************************************************************************
*
*   Function name : LCD_WriteDigit(char c, char digit)
*
*   Returns :       None
*
*   Parameters :    Inputs
*                   c: The symbol to be displayed in a LCD digit
*                   digit: In which digit (0-5) the symbol should be displayed
*                   Note: Digit 0 is the first used digit on the LCD,
*                   i.e LCD digit 2
*
*   Purpose :       Stores LCD control data in the LCD_displayData buffer.
*                   (The LCD_displayData is latched in the LCD_SOF interrupt.)
*
*****************************************************************************/
void LCD_WriteDigit(char c, char digit)
{

    unsigned char seg = 0x00; // Holds the segment pattern
	char mask1 = 0x02;	
    char mask2 = 0x07;
	
	unsigned char BuffPtr[3];
	
    unsigned char *ptr = (LCD_Data + LCD_START_REG);
    unsigned char i;

    if ( !digit || digit > 5 ) {// Skip if digit is illegal
        return;
	}
	if ( digit > 2 ) {
		ptr += 1;
	}

	seg = (unsigned char) pgm_read_word(&LCD_SegTable[(unsigned char)c]); 
	
	BuffPtr[0] = (seg<<1) & mask1;
	BuffPtr[1] = (seg>>1) & mask2;
	BuffPtr[2] = (seg>>4) & mask2;

	if (digit & 0x01) {// Digit 1, 3
		mask1 <<= 4;
		mask2 <<= 4;
	}

	for (i = 0; i < 3; i++)
	{
		if (digit & 0x01) // Digit 1, 3
			BuffPtr[i] <<= 4;		
		
		if (i) {
			*ptr = (*ptr & ~mask2) | BuffPtr[i];
		}
		else {
			*ptr = (*ptr & ~mask1) | BuffPtr[i];			
		}		
		
		ptr += 5;
	}

}



void LCD_AllSegments(char show)
{
    unsigned char i;

    if (show)
        show = 0xFF;

    // Set/clear all bits in all LCD registers
    for (i=0; i < LCD_REGISTER_COUNT; i++)
        *(LCD_Data + i) = show;
}

void LCD_Clear(void)
{
    unsigned char i;

    //clear all bits in all LCD registers
    for (i=0; i < LCD_REGISTER_COUNT; i++)
        *(LCD_Data + i) = 0x00;
}


/*****************************************************************************
*
*   LCD Interrupt Routine
*
*   Returns :       None
*
*   Parameters :    None
*
*   Purpose: Latch the LCD_displayData and Set LCD_status.updateComplete
*
*****************************************************************************/

ISR(LCD_vect, ISR_NOBLOCK)
{
	unsigned char i;

	if (LCD_Update_Required) {	

		if (gLcdFlashTimer < (LCD_FLASH_SEED >> 1)) //LCD_FLASH_SEED/2
            LCD_BLINK = 0;
        else
            LCD_BLINK = 1;
		
		for (i = 0; i < LCD_REGISTER_COUNT; i++)
            *(LCDREG + i) = *(LCD_Data+i);
			
			
	if (gLcdColon)
            *(LCDREG + 15) |= 0x01;
        else
            *(LCDREG + 15) |= 0x00;	
			
	/*		
		if (gLcdHeating)
            *(LCDREG + 16) |= 0x01;
		else
			*(LCDREG + 16) &= ~0x01;
			

		if (gLcdDegree)
            *(LCDREG + 16) |= 0x20;
		else
			*(LCDREG + 16) &= ~0x20;	
			
	*/	
		if (gLcdFlashTimer == LCD_FLASH_SEED)
			gLcdFlashTimer= 0;
		else
			gLcdFlashTimer++;

        gLcdGlobalTimer++;
			
		LCD_Update_Required  = FALSE; // Clear LCD management flags, LCD update is complete
		
	}
	
	CheckLongPushButton();
	

/*
	static char LCD_timer = LCD_TIMER_SEED;
    char c;
    char c_flash;
    char flash;

    char EOL;
    unsigned char i;
	c_flash=0; // mt


    LCD_timer--;                    // Decreased every LCD frame
	
    if (gScrollMode)
    {
        // If we are in scroll mode, and the timer has expired,
        // we will update the LCD
        if (LCD_timer == 0)
        {
            if (gLCD_Start_Scroll_Timer == 0)
            {
                gLCD_Update_Required = TRUE;
            }
            else
                gLCD_Start_Scroll_Timer--;
        }
    }
    else    
    {   // if not scrolling,
        // disble LCD start of frame interrupt
//        cbi(LCDCRA, LCDIE);   //DEBUG
        gScroll = 0;
    }


    EOL = FALSE;
	
    if (gLCD_Update_Required == TRUE)
    {
		
        // Duty cycle of flashing characters
        if (gFlashTimer < (LCD_FLASH_SEED >> 1))
            flash = 0;
        else
            flash = 1;

        // Repeat for the six LCD characters
        for (i = 0; i < 6; i++)
        {
            if ((gScroll+i) >= 0 && (!EOL))
            {
                // We have some visible characters
                c = gTextBuffer[i + gScroll];
                c_flash = c & 0x80 ? 1 : 0;
                c = c & 0x7F;

                if (c == '\0')
                    EOL = i+1;      // End of character data
            }
            else
                c = ' ';

            // Check if this character is flashing

            if (c_flash && flash)
                LCD_WriteDigit(' ', i);
            else
                LCD_WriteDigit(c, i);
        }

        // Copy the segment buffer to the real segments
        for (i = 0; i < LCD_REGISTER_COUNT; i++)
            *(pLCDREG + i) = *(LCD_Data+i);

        // Handle colon
        if (gColon)
            *(pLCDREG + 8) = 0x01;
        else
            *(pLCDREG + 8) = 0x00;

        // If the text scrolled off the display,
        // we have to start over again.
        if (EOL == 1)
            gScroll = -6;
        else
            gScroll++;

        // No need to update anymore
        gLCD_Update_Required = FALSE;
    }


    // LCD_timer is used when scrolling text
    if (LCD_timer == 0)
    {

            LCD_timer = LCD_TIMER_SEED;
    }

    // gFlashTimer is used when flashing characters
    if (gFlashTimer == LCD_FLASH_SEED)
        gFlashTimer= 0;
    else
        gFlashTimer++;
*/
}

