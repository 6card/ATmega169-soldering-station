//Button definitions

#define KEY_NULL    0
#define KEY_ENTER   1
#define KEY_RIGHT   2
#define KEY_LEFT    3
#define KEY_UP    	4
#define KEY_DOWN   	5
#define KEY_ENTER_LONG   6

#define JOYSTICK_PIN PINE
#define LEFT_PIN 0
#define RIGHT_PIN 1
#define UP_PIN 3
#define DOWN_PIN 5
#define PUSH_PIN 6

#define ENCODER_PIN PINE
#define ENCODER_A 4
#define ENCODER_B 5

#define POWER_PIN 7
#define POWER_LOST !(JOYSTICK_PIN & (1<<POWER_PIN))

#define LEFT_DIRECTION !(JOYSTICK_PIN & (1<<LEFT_PIN))
#define RIGHT_DIRECTION !(JOYSTICK_PIN & (1<<RIGHT_PIN))
#define UP_DIRECTION !(JOYSTICK_PIN & (1<<UP_PIN))
#define DOWN_DIRECTION !(JOYSTICK_PIN & (1<<DOWN_PIN))
#define PUSH_DIRECTION !(JOYSTICK_PIN & (1<<PUSH_PIN))

#define DEBOUNCE_TIME 4

#define LONG_PUSH_TIME 100

#ifndef FALSE
#define FALSE   0
#define TRUE    (!FALSE)
#endif

extern volatile char KEY;
extern volatile char KEY_PUSH;
extern volatile char KEY_VALID;

extern volatile unsigned char debounce_timer;
extern volatile unsigned int voltage;

void Buttons_Init(void);
void PinChangeInterrupt(void);
char getkey(void);

void EncoderChangeInterrupt(void);

void CheckLongPushButton(void);