#include "config.h"
#include "motor.h"

volatile uchar LeftMotorSpeed;
volatile uchar RightMotorSpeed;

void SmartCarForward(uchar LeftSpeedVal,uchar RightSpeedVal)
{
    LeftMotorSpeed  = LeftSpeedVal;
    RightMotorSpeed = RightSpeedVal;
    LeftMotorGo;
    RightMotorGo;
}

void SmartCarBack(uchar LeftSpeedVal,uchar RightSpeedVal)
{
    LeftMotorSpeed  = LeftSpeedVal;
    RightMotorSpeed = RightSpeedVal;
    LeftMotorBack;
    RightMotorBack;
}

void SmartCarLeft(uchar LeftSpeedVal,uchar RightSpeedVal)
{
    LeftMotorSpeed  = LeftSpeedVal;
    RightMotorSpeed = RightSpeedVal;
    LeftMotorStop;
    RightMotorGo;
}

void SmartCarLeftTurn(uchar LeftSpeedVal,uchar RightSpeedVal)
{
    LeftMotorSpeed  = LeftSpeedVal;
    RightMotorSpeed = RightSpeedVal;
    LeftMotorBack;
    RightMotorGo;
}

void SmartCarRight(uchar LeftSpeedVal,uchar RightSpeedVal)
{
    LeftMotorSpeed  = LeftSpeedVal;
    RightMotorSpeed = RightSpeedVal;
    LeftMotorGo;
    RightMotorStop;
}

void SmartCarRightTurn(uchar LeftSpeedVal,uchar RightSpeedVal)
{
    LeftMotorSpeed  = LeftSpeedVal;
    RightMotorSpeed = RightSpeedVal;
    LeftMotorGo;
    RightMotorBack;
}

void SmartCarStops(void)
{
    LeftMotorStop;
    RightMotorStop;
    LeftMotorSpeed  = 0;   // ★ 顺手清掉速度，PWM 不再翻转使能脚
    RightMotorSpeed = 0;
}