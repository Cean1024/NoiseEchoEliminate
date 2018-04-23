#include "gpiocontrol.h"

GpioControl::GpioControl()
{
    wiringPiSetup();
    for(int i=DEFAULTPINSTART;i<=DEFAULTPINEND;i++){

        pinMode(i, OUTPUT);
    }


}
