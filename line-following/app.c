/******************************************************************************
 * @file    app.c
 * @brief   小车应用层：红外循迹 + 压线声光提示（无死锁、无阻塞高速版）
 * @note    传感器极性：压线 = 1，空白 = 0
 ******************************************************************************/

#include "config.h"
#include "timer_ISR.h"
#include "Delay.h"
#include "motor.h"
#include "app.h"
#include "main.h"

/*====================================
函数名  ：IR_Tracking
功能    ：红外循迹 + 任意一侧压线时声光提示
返回值  ：无
备注    ：传感器极性——压线=1，空白=0
====================================*/
void IR_Tracking()
{
    /* 任意一侧压线（传感器输出 1 = 检测到黑线） */
    if (TrackSensorLeft == 1 || TrackSensorRight == 1)
    {
        BUZZER_on;          /* 压线时蜂鸣器响 */
        LED = 0;            /* LED 亮（低电平亮） */
    }
    else
    {
        BUZZER_off;         /* 空白区域关闭蜂鸣器 */
        LED = 1;            /* LED 灭 */
    }

    /* ---- 巡线动作逻辑（直接调用 main.c 中设置的速度变量） ---- */
    if (TrackSensorLeft == 0 && TrackSensorRight == 0)
    {
        /* 两边都空白 → 按直行速度平稳前进 */
        SmartCarForward(SpeedForwardLeft, SpeedForwardRight);
    }
    else if (TrackSensorLeft == 1 && TrackSensorRight == 0)
    {
        /* 仅左侧压线 → 左转修正（内侧轮 0，外侧右轮使用 SpeedTurnLeft 快速推进） */
        SmartCarLeft(0, SpeedTurnLeft);
    }
    else if (TrackSensorLeft == 0 && TrackSensorRight == 1)
    {
        /* 仅右侧压线 → 右转修正（外侧左轮使用 SpeedTurnRight 快速推进，内侧轮 0） */
        SmartCarRight(SpeedTurnRight, 0);
    }
    else
    {
        /* 两边都压线（11）→ 按直行速度前进 */
        SmartCarForward(SpeedForwardLeft, SpeedForwardRight);
    }
}