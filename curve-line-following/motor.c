/******************************************************************************
 * @file    motor.c
 * @brief   电机驱动层：支持内侧轮 PWM 动态差速调速版本
 ******************************************************************************/

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
    LeftMotorBack;
}

void SmartCarLeft(uchar LeftSpeedVal,uchar RightSpeedVal)
{
    LeftMotorSpeed  = LeftSpeedVal;
    RightMotorSpeed = RightSpeedVal;
    
    /* 优化：如果内侧速度为 0 则停转，若大于 0 则允许 PWM 驱动其低速旋转 */
    if (LeftSpeedVal == 0)
    {
        LeftMotorStop;
    }
    else
    {
        LeftMotorGo;  // 恢复正转，交由 PWM 控制转速
    }
    
    RightMotorGo;
}

void SmartCarLeftTurn(uchar LeftSpeedVal,uchar RightSpeedVal)
{
    LeftMotorSpeed  = LeftSpeedVal;
    RightMotorSpeed = RightSpeedVal;
    LeftMotorBack;
    LeftMotorGo;
}

void SmartCarRight(uchar LeftSpeedVal,uchar RightSpeedVal)
{
    LeftMotorSpeed  = LeftSpeedVal;
    RightMotorSpeed = RightSpeedVal;
    
    LeftMotorGo;
    
    /* 优化：如果右侧内侧速度为 0 则停转，若大于 0 则允许 PWM 驱动 */
    if (RightSpeedVal == 0)
    {
        RightMotorStop;
    }
    else
    {
        RightMotorGo; // 恢复正转，交由 PWM 控制转速
    }
}

void SmartCarRightTurn(uchar LeftSpeedVal,uchar RightSpeedVal)
{
    LeftMotorSpeed  = LeftSpeedVal;
    RightMotorSpeed = RightSpeedVal;
    LeftMotorGo;
    LeftMotorBack;
}

void SmartCarStops(void)
{
    LeftMotorStop;
    RightMotorStop;
    LeftMotorSpeed  = 0;   /* 清掉速度，PWM 不再翻转使能脚 */
    RightMotorSpeed = 0;
}