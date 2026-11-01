#ifndef _MOTOR_H
#define _MOTOR_H

#include <reg52.h>
#include "config.h"

extern unsigned char LeftMotorSpeed;
extern unsigned char RightMotorSpeed;

void SmartCarForward(uchar LeftSpeedVal,uchar RightSpeedVal);
void SmartCarBack(uchar LeftSpeedVal,uchar RightSpeedVal);
void SmartCarLeft(uchar LeftSpeedVal,uchar RightSpeedVal);
void SmartCarRight(uchar LeftSpeedVal,uchar RightSpeedVal);
void SmartCarLeftTurn(uchar LeftSpeedVal,uchar RightSpeedVal);
void SmartCarRightTurn(uchar LeftSpeedVal,uchar RightSpeedVal);
void SmartCarStops(void);

#endif