/******************************************************************************
 * @file    app.c
 * @brief   小车应用层：具备历史记忆的“自适应动态保持”版本
 * @note    传感器极性：压线 = 1，空白 = 0
 ******************************************************************************/

#include "config.h"
#include "timer_ISR.h"
#include "Delay.h"
#include "motor.h"
#include "app.h"
#include "main.h"

/* =====================================================================
 * ★★★ 核心调速与自适应参数配置区 ★★★
 * ===================================================================== */
#define SPEED_STRAIGHT   85     /* 直行基础速度 */
#define SPEED_OUTSIDE    250    /* 转弯外侧速度 */
#define SPEED_INSIDE     1     /* 转弯内侧微速 */

#define TURN_HOLD_NORMAL 8000   /* 正常标准大保持时间 */
#define TURN_HOLD_SHORT  6500   /* 经历超长弯后，下一个弯的精简保持时间 */

/* 【自适应判定阈值】如果单方向连续转弯的循环次数超过这个值，就定义为“超长弯” */
#define LONG_TURN_THRESHOLD 8000 
/* ===================================================================== */

static unsigned int hold_counter = 0;          /* 当前实际生效的保持计数 */
static unsigned int current_turn_duration = 0; /* 统计当前方向连续转弯的时长 */
static uchar last_direction = 0;               /* 1=左转偏向，2=右转偏向 */
static uchar reduce_next_hold_flag = 0;        /* 标志位：指示下一个弯是否需要缩短保持 */

void IR_Tracking()
{
    /* 1. LED 指示 */
    if (TrackSensorLeft == 1 || TrackSensorRight == 1)
    {
        LED = 0;            /* 压线：LED亮 */
    }
    else
    {
        LED = 1;            /* 丢线：LED灭 */
    }

    /* =================================================================
     * 2. 实时单侧纠偏判定（压线 = 1, 空白 = 0）
     * ================================================================= */
    if (TrackSensorLeft == 1 && TrackSensorRight == 0)
    {
        /* 左侧压线，右侧空白 -> 向左修正 */
        
        /* 如果方向发生切换（比如刚才是右转，现在变成左转） */
        if (last_direction != 1 && last_direction != 0)
        {
            /* 结算上一个方向：如果上一个方向持续时间太长，触发降档标志 */
            if (current_turn_duration > LONG_TURN_THRESHOLD)
            {
                reduce_next_hold_flag = 1; 
            }
            current_turn_duration = 0; // 重置计数器
        }

        last_direction = 1;            
        current_turn_duration++;       // 累加当前方向的持续时间

        /* 动态决定这次的保持时间：如果触发了降档，用短保持；否则用标准大保持 */
        if (reduce_next_hold_flag)
        {
            hold_counter = TURN_HOLD_SHORT;
        }
        else
        {
            hold_counter = TURN_HOLD_NORMAL;
        }

        SmartCarLeft(SPEED_INSIDE, SPEED_OUTSIDE);   
        return;
    }
    else if (TrackSensorLeft == 0 && TrackSensorRight == 1)
    {
        /* 左侧空白，右侧压线 -> 向右修正 */
        
        /* 如果方向发生切换 */
        if (last_direction != 2 && last_direction != 0)
        {
            if (current_turn_duration > LONG_TURN_THRESHOLD)
            {
                reduce_next_hold_flag = 1; 
            }
            current_turn_duration = 0; 
        }

        last_direction = 2;            
        current_turn_duration++;       

        /* 动态决定这次的保持时间 */
        if (reduce_next_hold_flag)
        {
            hold_counter = TURN_HOLD_SHORT;
        }
        else
        {
            hold_counter = TURN_HOLD_NORMAL;
        }

        SmartCarRight(SPEED_OUTSIDE, SPEED_INSIDE);  
        return;
    }

    /* =================================================================
     * 3. 盲打保持期（执行当前生效的 hold_counter）
     * ================================================================= */
    if (hold_counter > 0)
    {
        hold_counter--; 
        current_turn_duration++; // 盲打期间也算作转弯持续时间
        
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
     * 4. 彻底回正/进入直道区域：结算并清空状态
     * ================================================================= */
    /* 当小车平稳直行且没有触发转弯时，检查上一个弯是不是长弯 */
    if (current_turn_duration > LONG_TURN_THRESHOLD)
    {
        reduce_next_hold_flag = 1; // 标记下一个弯减半
    }
    else
    {
        reduce_next_hold_flag = 0; // 短弯或直行，下一个弯恢复正常
    }

    current_turn_duration = 0;
    last_direction = 0;
    
    SmartCarForward(SPEED_STRAIGHT, SPEED_STRAIGHT);
}