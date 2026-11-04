/******************************************************************************
 * @file    app.c
 * @brief   小车应用层：支持急弯大保持 + 直道防振荡（防画龙）过滤机制
 * @note    传感器极性：压线 = 1，空白 = 0
 ******************************************************************************/

#include "config.h"
#include "timer_ISR.h"
#include "Delay.h"
#include "motor.h"
#include "app.h"
#include "main.h"

/* =====================================================================
 * ★★★ 核心调速与参数配置区 ★★★
 * ===================================================================== */
#define SPEED_STRAIGHT   70     /* 直行基础速度（可以适当提速，因为直道稳了） */
#define SPEED_OUTSIDE    250    /* 转弯时外侧轮速度 */
#define SPEED_INSIDE     1      /* 转弯时内侧轮微速 */

#define TURN_HOLD_TIME   8000   /* 急弯盲打保持时间（你验证过完美的数值） */

/* 【新增加】直道防振荡参数 */
#define SHAKE_LIMIT      2      /* 左右交替达到4次，判定为直道画龙 */
#define STRAIGHT_LOCK_TIME 1000  /* 触发后强制直行锁定的循环次数（过滤毛刺） */
/* ===================================================================== */

static unsigned int hold_counter = 0;        /* 急弯盲打计数器 */
static unsigned int straight_lock_counter = 0; /* 直道防振荡锁定计数器 */
static uchar last_direction = 0;             /* 1=左转偏向，2=右转偏向 */
static uchar shake_counter = 0;              /* 左右横跳计数器 */

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
     * 2. 优先处理：如果正处于“直道防振荡锁定”期间
     * 强行让小车直行，忽略短暂的左右毛刺干扰，大幅提升直道速度！
     * ================================================================= */
    if (straight_lock_counter > 0)
    {
        straight_lock_counter--;
        SmartCarForward(SPEED_STRAIGHT, SPEED_STRAIGHT);
        return;
    }

    /* =================================================================
     * 3. 实时单侧纠偏判定（压线 = 1, 空白 = 0）
     * ================================================================= */
    if (TrackSensorLeft == 1 && TrackSensorRight == 0)
    {
        /* 左侧压线，右侧空白 -> 需要向左修正 */
        
        /* 检查是否发生“左右横跳（画龙）” */
        if (last_direction == 2) 
        {
            shake_counter++; // 上一次向右、这次向左，横跳次数+1
        }
        
        last_direction = 1;            
        hold_counter = TURN_HOLD_TIME; 

        /* 如果连续左右横跳次数达到上限，说明进入了毛刺直道，触发强制直行锁定！ */
        if (shake_counter >= SHAKE_LIMIT)
        {
            shake_counter = 0;
            straight_lock_counter = STRAIGHT_LOCK_TIME; /* 锁死直行一段时间 */
            SmartCarForward(SPEED_STRAIGHT, SPEED_STRAIGHT);
            return;
        }

        SmartCarLeft(SPEED_INSIDE, SPEED_OUTSIDE);   
        return;
    }
    else if (TrackSensorLeft == 0 && TrackSensorRight == 1)
    {
        /* 左侧空白，右侧压线 -> 需要向右修正 */
        
        /* 检查是否发生“左右横跳（画龙）” */
        if (last_direction == 1) 
        {
            shake_counter++; // 上一次向左、这次向右，横跳次数+1
        }
        
        last_direction = 2;            
        hold_counter = TURN_HOLD_TIME; 

        /* 如果连续左右横跳次数达到上限，触发强制直行锁定！ */
        if (shake_counter >= SHAKE_LIMIT)
        {
            shake_counter = 0;
            straight_lock_counter = STRAIGHT_LOCK_TIME; 
            SmartCarForward(SPEED_STRAIGHT, SPEED_STRAIGHT);
            return;
        }

        SmartCarRight(SPEED_OUTSIDE, SPEED_INSIDE);  
        return;
    }

    /* =================================================================
     * 4. 盲打保持期（针对真正的急弯丢线）
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
     * 5. 默认直行（重置状态）
     * ================================================================= */
    last_direction = 0;
    shake_counter = 0; // 长时间没横跳则清空振荡计数
    SmartCarForward(SPEED_STRAIGHT, SPEED_STRAIGHT);
}