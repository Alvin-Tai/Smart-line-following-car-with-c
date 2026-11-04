/******************************************************************************
 * @file    app.c
 * @brief   小车应用层：传感器优先 + 可中断的转弯保持逻辑
 * @note    传感器极性：压线 = 1，空白 = 0
 ******************************************************************************/

#include "config.h"
#include "timer_ISR.h"
#include "Delay.h"
#include "motor.h"
#include "app.h"
#include "main.h"

/* =====================================================================
 * ★★★ 核心速度与保持参数配置区 ★★★
 * ===================================================================== */
#define SPEED_FORWARD_LEFT   60     
#define SPEED_FORWARD_RIGHT  60     

#define SPEED_TURN_LEFT      250    
#define SPEED_TURN_RIGHT     250    

/* 转弯保持周期（盲打保持时间，可根据过弯表现微调） */
#define TURN_HOLD_TIME       333     
/* ===================================================================== */

static uchar turn_hold_counter = 0;  
static uchar last_turn_direction = 0;

void IR_Tracking()
{
    /* 视觉提示：仅 LED 亮灭指示压线，不鸣笛（夜间静音） */
    if (TrackSensorLeft == 1 || TrackSensorRight == 1)
    {
        // BUZZER_on;       
        LED = 0;            /* 压线：LED亮 */
    }
    else
    {
        // BUZZER_off;      
        LED = 1;            /* 丢线：LED灭 */
    }

    /* =================================================================
     * 第一优先级：传感器实时状态（最高优先级！）
     * 只要传感器检测到黑线，立刻响应、更新方向、并刷新保持计数。
     * 绝不忽略任何一次灯亮！
     * ================================================================= */
    if (TrackSensorLeft == 1 && TrackSensorRight == 0)
    {
        last_turn_direction = 1;            /* 记录方向：左转 */
        turn_hold_counter = TURN_HOLD_TIME; /* 刷新保持倒计时 */
        SmartCarLeft(0, SPEED_TURN_LEFT);   /* 执行左转 */
        return;
    }
    else if (TrackSensorLeft == 0 && TrackSensorRight == 1)
    {
        last_turn_direction = 2;            /* 记录方向：右转 */
        turn_hold_counter = TURN_HOLD_TIME; /* 刷新保持倒计时 */
        SmartCarRight(SPEED_TURN_RIGHT, 0); /* 执行右转 */
        return;
    }
    else if (TrackSensorLeft == 1 && TrackSensorRight == 1)
    {
        /* 双侧压线：清空保持，按直行通过 */
        last_turn_direction = 0;
        turn_hold_counter = 0;
        SmartCarForward(SPEED_FORWARD_LEFT, SPEED_FORWARD_RIGHT);
        return;
    }

    /* =================================================================
     * 第二优先级：只有当传感器双双【丢线 (0,0)】时，
     * 才会启动“转弯保持（盲打）”，防止在急弯瞬间因丢线而直行冲出！
     * ================================================================= */
    if (turn_hold_counter > 0)
    {
        turn_hold_counter--; // 盲打倒计时递减
        
        if (last_turn_direction == 1)
        {
            SmartCarLeft(0, SPEED_TURN_LEFT);   /* 继续保持左转 */
        }
        else if (last_turn_direction == 2)
        {
            SmartCarRight(SPEED_TURN_RIGHT, 0); /* 继续保持右转 */
        }
        return;
    }

    /* =================================================================
     * 第三优先级：既没压线，也没有保持计数，恢复正常直行
     * ================================================================= */
    last_turn_direction = 0;
    SmartCarForward(SPEED_FORWARD_LEFT, SPEED_FORWARD_RIGHT);
}