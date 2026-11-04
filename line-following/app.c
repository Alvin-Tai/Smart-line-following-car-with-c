/******************************************************************************
 * @file    app.c
 * @brief   小车应用层：高电平有效（压线 = 1，空白 = 0）版本
 * @note    传感器极性：压线 = 1，空白 = 0
 ******************************************************************************/

#include "config.h"
#include "timer_ISR.h"
#include "Delay.h"
#include "motor.h"
#include "app.h"
#include "main.h"

/* =====================================================================
 * ★★★ 核心调速与保持参数配置区 ★★★
 * ===================================================================== */
#define SPEED_STRAIGHT   160     /* 直行基础速度 */
#define SPEED_OUTSIDE    250    /* 转弯时外侧轮速度 */
#define SPEED_INSIDE     1      /* 转弯时内侧轮微速 */

#define TURN_HOLD_TIME   7777   /* 保持计数（确保能死死咬住急弯） */
/* ===================================================================== */

static unsigned int hold_counter = 0;  
static uchar last_direction = 0;       /* 1=左转偏向，2=右转偏向 */

void IR_Tracking()
{
    /* 1. LED 指示（高电平 1 压线时 LED 亮） */
    if (TrackSensorLeft == 1 || TrackSensorRight == 1)
    {
        LED = 0;            /* 压线：LED亮 */
    }
    else
    {
        LED = 1;            /* 丢线：LED灭 */
    }

    /* =================================================================
     * 2. 第一优先级：明确的单侧偏离纠正（压线 = 1, 空白 = 0）
     * 极性已取反：左压线(1)右空白(0)；左空白(0)右压线(1)
     * ================================================================= */
    if (TrackSensorLeft == 1 && TrackSensorRight == 0)
    {
        /* 左侧压线，右侧空白 -> 需要向左修正 */
        last_direction = 1;            
        hold_counter = TURN_HOLD_TIME; 
        SmartCarLeft(SPEED_INSIDE, SPEED_OUTSIDE);   
        return;
    }
    else if (TrackSensorLeft == 0 && TrackSensorRight == 1)
    {
        /* 左侧空白，右侧压线 -> 需要向右修正 */
        last_direction = 2;            
        hold_counter = TURN_HOLD_TIME; 
        SmartCarRight(SPEED_OUTSIDE, SPEED_INSIDE);  
        return;
    }

    /* =================================================================
     * 3. 第二优先级：盲打保持期（绝对保护区！）
     * ================================================================= */
    if (hold_counter > 0)
    {
        hold_counter--; 
        
        if (last_direction == 1)
        {
            SmartCarLeft(SPEED_INSIDE, SPEED_OUTSIDE);   
        }
        else if (last_direction == 2)
        {
            SmartCarRight(SPEED_OUTSIDE, SPEED_INSIDE); 
        }
        return;
    }

    /* =================================================================
     * 4. 第三优先级：默认直行
     * 此时双侧都是 0（完全丢线且无保持），或者双侧都是 1（在直道上）
     * ================================================================= */
    last_direction = 0;
    SmartCarForward(SPEED_STRAIGHT, SPEED_STRAIGHT);
}