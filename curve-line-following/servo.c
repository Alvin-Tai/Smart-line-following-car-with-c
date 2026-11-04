#include "config.h"
#include "Delay.h"
#include "Servo.h"

void ServoFront(void)
{
    char i;
    EA_off;
    for(i=0;i<10;i++)
    {
        ServoPin = 1;
        Delay1550us();
        ServoPin = 0;
        Delay18450us();
    }
    EA_on;
    Delay1ms(800);
}

void ServoLeft(void)
{
    char i;
    EA_off;
    for(i=0;i<10;i++)
    {
        ServoPin = 1;
        Delay2500us();
        ServoPin = 0;
        Delay17500us();
    }
    EA_on;
    Delay1ms(800);
}

void ServoRight(void)
{
    char i;
    EA_off;
    for(i=0;i<10;i++)
    {
        ServoPin = 1;
        Delay600us();
        ServoPin = 0;
        Delay19400us();
    }
    EA_on;
    Delay1ms(800);
}