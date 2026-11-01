#include "config.h"
#include "timer_ISR.h"
#include "motor.h"

uchar PWM_T;

void IntRegInit(void)
{
    ET0 = 1;
    EA  = 1;
}

void Timer0Init(void)       //100微秒@11.0592MHz
{
    TMOD |= 0x02;
    TH0 = 0xA4;
    TL0 = 0xA4;
    TR0 = 1;
}

void tm0_isr() interrupt 1 using 1
{
    PWM_T++;
    if(PWM_T == 255)
    {
        PWM_T = 0;
    }
    /* ★ 用三元写法更简洁，效果和原来四个 if 完全一样 */
    RightMotor_EN = (RightMotorSpeed >= PWM_T) ? 1 : 0;
    LeftMotor_EN  = (LeftMotorSpeed  >= PWM_T) ? 1 : 0;
}