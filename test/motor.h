/******************************************************************************
 * @file    motor.h
 * @brief   小车电机动作封装 —— 接口声明
 ******************************************************************************/

#ifndef _MOTOR_H
#define _MOTOR_H

#include <reg52.h>
#include "config.h"

/* ★ 中断(PWM比较)与主循环共享，必须 volatile，与 motor.c 定义保持一致 ★ */
extern volatile unsigned char LeftMotorSpeed;
extern volatile unsigned char RightMotorSpeed;

/* 双轮前进 / 后退：两轮同向，可分别设定左右速度（差速补偿用） */
void SmartCarForward(uchar LeftSpeedVal, uchar RightSpeedVal);
void SmartCarBack(uchar LeftSpeedVal, uchar RightSpeedVal);

/* 单轮驱动转弯：一轮停、另一轮转，绕停转轮画弧（转弯半径大） */
void SmartCarLeft(uchar rightSpeed);    /* 左轮停，右轮前进 */
void SmartCarRight(uchar leftSpeed);    /* 右轮停，左轮前进 */

/* 原地自转：两轮反向，绕车体中心转（转弯半径小，更急） */
void SmartCarLeftTurn(uchar LeftSpeedVal, uchar RightSpeedVal);
void SmartCarRightTurn(uchar LeftSpeedVal, uchar RightSpeedVal);

/* 滑行停车：EN=0 输出高阻，方向线保持，电机惯性转停 */
void SmartCarStops(void);

#endif