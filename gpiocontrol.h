#ifndef GPIOCONTROL_H
#define GPIOCONTROL_H
#include <iostream>
#include "wiringPi.h"

#define DEFAULTPINSTART 2
#define DEFAULTPINEND 6

using namespace std;
class GpioControl
{
public:
    GpioControl();
    GpioControl(int pin,int mode)
    {
        wiringPiSetup();
        this->pin = pin;
        pinMode(pin, mode);
    }
    void setallhigh(){
        cout <<__func__ <<endl;
        for(int i=DEFAULTPINSTART;i<=DEFAULTPINEND;i++)
           digitalWrite(i, HIGH);
    }
    void setalllow(){
        cout <<__func__ <<endl;
        for(int i=DEFAULTPINSTART;i<=DEFAULTPINEND;i++)
           digitalWrite(i, LOW);
    }
    int getvalue(int pin) {
        return digitalRead(pin);
    }

private:
    int pin;
};

#endif // GPIOCONTROL_H
