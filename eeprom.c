#include <avr/io.h>
#include <avr/eeprom.h>

#include "eeprom.h"
#include "main.h"

char cCorrection = 0;
char cStep = 20;

void EEPROMInit(void) {
    cCorrection = eeprom_read_byte((const uint8_t *) OPTION_CORRECTION);
    cStep = eeprom_read_byte((const uint8_t *) OPTION_STEP);
}
char EEPROMLoadBright(void) {
    return eeprom_read_byte((const uint8_t *) OPTION_BRIGHT);
}
void SaveBright(char bright) {
    eeprom_write_byte((uint8_t *) OPTION_BRIGHT, bright);
    Beep();
}
void SaveECorrection(void){
    eeprom_write_byte((uint8_t *) OPTION_CORRECTION, cCorrection);
    eeprom_write_byte((uint8_t *) OPTION_STEP, cStep);
    Beep();
}



void StoreEEPROM(char *pBuffer, char num_bytes, unsigned char* EE_START_ADR)
{
    unsigned char i;
    for (i=0;i<num_bytes;i++) {
        eeprom_write_byte(EE_START_ADR, pBuffer[i]);
        EE_START_ADR++;
    }
}

void LoadEEPROM(char *pBuffer, char num_bytes, unsigned char* EE_START_ADR)
{
    unsigned char i;
    for (i=0;i<num_bytes;i++) {
        pBuffer[i]=eeprom_read_byte(EE_START_ADR);
        EE_START_ADR++;
    }
}