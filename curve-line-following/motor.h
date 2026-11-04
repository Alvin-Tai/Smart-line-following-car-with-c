#ifndef _MOTOR_H
#define _MOTOR_H

#include <reg52.h>
#include "config.h"

/* 加上 volatile 确保 PWM 中断读取速度时无延迟 */
extern volatile unsigned char LeftMotorSpeed;
extern volatile unsigned char RightMotorSpeed;

void SmartCarForward(uchar LeftSpeedVal,uchar RightSpeedVal);
void SmartCarBack(uchar LeftSpeedVal,uchar RightSpeedVal);
void SmartCarLeft(uchar LeftSpeedVal,uchar RightSpeedVal);
void SmartCarRight(uchar LeftSpeedVal,uchar RightSpeedVal);
void SmartCarLeftTurn(uchar LeftSpeedVal,uchar RightSpeedVal);
void SmartCarRightTurn(uchar LeftSpeedVal,uchar RightSpeedVal);
void SmartCarStops(void);

#endif