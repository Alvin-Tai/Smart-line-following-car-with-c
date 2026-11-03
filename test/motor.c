/******************************************************************************
 * @file    motor.c
 * @brief   小车电机动作封装
 * @note    方向宏(LeftMotorGo/Back/Stop 等)定义在 config.h 中
 ******************************************************************************/

#include "config.h"
#include "motor.h"

/* ★ 中断(PWM比较)与主循环共享，必须 volatile，防止编译器优化读到旧值 ★ */
volatile uchar LeftMotorSpeed;
volatile uchar RightMotorSpeed;

/* ----------------------------------------------------------------------------
 * 双轮前进：两轮均正转，可分别设定左右速度（用于直行差速补偿）
 * -------------------------------------------------------------------------- */
void SmartCarForward(uchar LeftSpeedVal, uchar RightSpeedVal)
{
    LeftMotorSpeed  = LeftSpeedVal;
    RightMotorSpeed = RightSpeedVal;
    LeftMotorGo;
    RightMotorGo;
}

/* ----------------------------------------------------------------------------
 * 双轮后退：两轮均反转
 * -------------------------------------------------------------------------- */
void SmartCarBack(uchar LeftSpeedVal, uchar RightSpeedVal)
{
    LeftMotorSpeed  = LeftSpeedVal;
    RightMotorSpeed = RightSpeedVal;
    LeftMotorBack;
    RightMotorBack;
}

/* ----------------------------------------------------------------------------
 * 单轮驱动左转：左轮停，右轮以 rightSpeed 前进（绕左轮画弧，转弯半径大）
 * @param rightSpeed 右轮速度 0~255
 * @note  左轮速度强制为 0，与"左轮停"的方向状态保持一致
 * -------------------------------------------------------------------------- */
void SmartCarLeft(uchar rightSpeed)
{
    LeftMotorSpeed  = 0;
    LeftMotorStop;
    RightMotorSpeed = rightSpeed;
    RightMotorGo;
}

/* ----------------------------------------------------------------------------
 * 原地左转：左轮反转 + 右轮正转（绕车体中心自转，转弯更急）
 * @param LeftSpeedVal  左轮反转速度 0~255
 * @param RightSpeedVal 右轮正转速度 0~255
 * @note  与 SmartCarLeft 的区别：SmartCarLeft 是单轮驱动画弧，
 *        本函数是双轮反向自转，转弯半径更小
 * -------------------------------------------------------------------------- */
void SmartCarLeftTurn(uchar LeftSpeedVal, uchar RightSpeedVal)
{
    LeftMotorSpeed  = LeftSpeedVal;
    RightMotorSpeed = RightSpeedVal;
    LeftMotorBack;
    RightMotorGo;
}

/* ----------------------------------------------------------------------------
 * 单轮驱动右转：右轮停，左轮以 leftSpeed 前进（绕右轮画弧，转弯半径大）
 * @param leftSpeed 左轮速度 0~255
 * @note  右轮速度强制为 0，与"右轮停"的方向状态保持一致
 * -------------------------------------------------------------------------- */
void SmartCarRight(uchar leftSpeed)
{
    LeftMotorSpeed  = leftSpeed;
    LeftMotorGo;
    RightMotorSpeed = 0;
    RightMotorStop;
}

/* ----------------------------------------------------------------------------
 * 原地右转：左轮正转 + 右轮反转（绕车体中心自转，转弯更急）
 * @param LeftSpeedVal  左轮正转速度 0~255
 * @param RightSpeedVal 右轮反转速度 0~255
 * @note  与 SmartCarRight 的区别：SmartCarRight 是单轮驱动画弧，
 *        本函数是双轮反向自转，转弯半径更小
 * -------------------------------------------------------------------------- */
void SmartCarRightTurn(uchar LeftSpeedVal, uchar RightSpeedVal)
{
    LeftMotorSpeed  = LeftSpeedVal;
    RightMotorSpeed = RightSpeedVal;
    LeftMotorGo;
    RightMotorBack;
}

/* ----------------------------------------------------------------------------
 * 滑行停车：EN=0 输出高阻，方向线保持（电机惯性转停）
 * @note  清速度后 PWM 中断里 EN 恒为 0，不再翻转使能脚；
 *        方向线不复位是刻意的——滑行状态 EN 已关断，方向线状态无害
 * -------------------------------------------------------------------------- */
void SmartCarStops(void)
{
    LeftMotorStop;
    RightMotorStop;
    LeftMotorSpeed  = 0;   /* PWM 中断里 EN 恒 0，不再翻转 */
    RightMotorSpeed = 0;
}