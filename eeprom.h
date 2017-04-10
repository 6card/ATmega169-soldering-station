#define OPTION_BRIGHT 0x00 //
#define OPTION_CORRECTION 0x02 //
#define OPTION_STEP 0x01 //

extern char cCorrection;
extern char cStep;

// #define EEPROM_START 0x100 //the name will be put at this adress
void EEPROMInit(void);
void SaveECorrection(void);

void LoadEEPROM(char *pBuffer, char num_bytes, unsigned char *EE_START_ADR);
void StoreEEPROM(char *pBuffer, char num_bytes, unsigned char *EE_START_ADR);

char EEPROMLoadBright(void);
void SaveBright(char);