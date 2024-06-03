#ifndef DABBLE_GAMEPAD_H
#define DABBLE_GAMEPAD_H

#include <Arduino.h>


//#define PRINT_DEBUG


#define UP 0x001
#define DOWN 0x002
#define LEFT 0x004
#define RIGHT 0x008
#define START 0x010
#define SELECT 0x020
#define TRIANGLE 0x040
#define CIRCLE 0x080
#define CROSS 0x100
#define SQUARE 0x200

extern int buttonvar;
extern float x_value;

class DabbleGamepad {
public:
    DabbleGamepad();
    void begin(long baudRate);
    void update(void);
    uint16_t isButtonPressed(uint16_t button);
    int16_t getJoystickX(void);
    int16_t getJoystickY(void);
    uint16_t getJoystickAngle(void);
    uint16_t getJoystickRadius(void);


private:
    char incomingData[16];
    void parseData();

    uint16_t buttonState;
    uint16_t crossState;
    float joystickX;
    float joystickY;
    uint16_t joystickAngle;
    uint16_t joystickRadius;
};



#endif // DABBLE_GAMEPAD_H
