/******************************************************************************
 * @file    app.c
 * @brief   小车应用层：多级动态自适应保持 + 动态内侧轮速度版本
 * @note    传感器极性：压线 = 1，空白 = 0
 ******************************************************************************/

#include "config.h"
#include "timer_ISR.h"
#include "Delay.h"
#include "motor.h"
#include "app.h"
#include "main.h"

/* =====================================================================
 * ★★★ 核心调速与多级动态自适应参数配置区（所有参数统一在此调整） ★★★
 * ===================================================================== */
#define SPEED_STRAIGHT   132     /* 直行基础速度 */
#define SPEED_OUTSIDE    255    /* 转弯外侧速度 */

/* --- 5级阶梯式弯道参数表（含：触发判定限值、盲打保持时间、内侧轮动态速度） --- */

/* 级别 1：瞬时急弯 / 折线 */
#define T_LV1_LIMIT      990
#define HOLD_LV1         8000   /* 满血大保持 */
#define SPEED_IN_LV1     0      /* 急弯内侧速：极低或停转，死死切弯 */

/* 级别 2：中急弯 */
#define T_LV2_LIMIT      1250
#define HOLD_LV2         7000   
#define SPEED_IN_LV2     15     

/* 级别 3：中等弯道 */
#define T_LV3_LIMIT      1510
#define HOLD_LV3         4500   
#define SPEED_IN_LV3     66  

/* 级别 4：标准缓弯 */
#define T_LV4_LIMIT      1780
#define HOLD_LV4         2800   
#define SPEED_IN_LV4     100    

/* 级别 5：超长缓弯（超过 Level 4 的超长回环） */
#define HOLD_LV5         2500   
#define SPEED_IN_LV5     0     /* 缓弯内侧速：较高速度，防止转向过猛转过头 */
/* ===================================================================== */

static unsigned int hold_counter = 0;     /* 当前实际生效的动态保持计数 */
static unsigned int trigger_duration = 0; /* 统计传感器连续压线的时长 */
static uchar last_direction = 0;          /* 1=左转偏向，2=右转偏向 */
static uchar current_inside_speed = 35;   /* 当前动态生效的内侧轮速度 */

void IR_Tracking()
{
    /* 1. LED 指示 */
    if (TrackSensorLeft == 1 || TrackSensorRight == 1)
    {
        LED = 0; /* 压线：LED亮 */
    }
    else
    {
        LED = 1; /* 丢线：LED灭 */
    }

    /* =================================================================
     * 2. 第一优先级：实时单侧纠偏与多级动态参数计算（保持时间 + 内侧速度）
     * ================================================================= */
    if (TrackSensorLeft == 1 && TrackSensorRight == 0)
    {
        /* 左侧压线，右侧空白 -> 向左修正 */

        if (last_direction != 1)
        {
            trigger_duration = 0; // 方向切换，清空压线计时
        }

        last_direction = 1;
        trigger_duration++;       // 累加当前连续压线轮数

        /* --- 5级阶梯式自适应动态映射 --- */
        if (trigger_duration < T_LV1_LIMIT)
        {
            hold_counter = HOLD_LV1; 
            current_inside_speed = SPEED_IN_LV1;
        }
        else if (trigger_duration < T_LV2_LIMIT)
        {
            hold_counter = HOLD_LV2; 
            current_inside_speed = SPEED_IN_LV2;
        }
        else if (trigger_duration < T_LV3_LIMIT)
        {
            hold_counter = HOLD_LV3; 
            current_inside_speed = SPEED_IN_LV3;
        }
        else if (trigger_duration < T_LV4_LIMIT)
        {
            hold_counter = HOLD_LV4; 
            current_inside_speed = SPEED_IN_LV4;
        }
        else
        {
            hold_counter = HOLD_LV5; 
            current_inside_speed = SPEED_IN_LV5; // 超长缓弯提升内侧速度，让车身流畅过渡
        }

        SmartCarLeft(current_inside_speed, SPEED_OUTSIDE);
        return;
    }
    else if (TrackSensorLeft == 0 && TrackSensorRight == 1)
    {
        /* 左侧空白，右侧压线 -> 向右修正 */

        if (last_direction != 2)
        {
            trigger_duration = 0;
        }

        last_direction = 2;
        trigger_duration++;

        /* --- 5级阶梯式自适应动态映射（右转同步） --- */
        if (trigger_duration < T_LV1_LIMIT)
        {
            hold_counter = HOLD_LV1;
            current_inside_speed = SPEED_IN_LV1;
        }
        else if (trigger_duration < T_LV2_LIMIT)
        {
            hold_counter = HOLD_LV2;
            current_inside_speed = SPEED_IN_LV2;
        }
        else if (trigger_duration < T_LV3_LIMIT)
        {
            hold_counter = HOLD_LV3;
            current_inside_speed = SPEED_IN_LV3;
        }
        else if (trigger_duration < T_LV4_LIMIT)
        {
            hold_counter = HOLD_LV4;
            current_inside_speed = SPEED_IN_LV4;
        }
        else
        {
            hold_counter = HOLD_LV5;
            current_inside_speed = SPEED_IN_LV5;
        }

        SmartCarRight(SPEED_OUTSIDE, current_inside_speed);
        return;
    }

    /* =================================================================
     * 3. 第二优先级：盲打保持期（动态保持生效中）
     * ================================================================= */
    if (hold_counter > 0)
    {
        hold_counter--;
        trigger_duration++; // 盲打期间也算作压线延续

        if (last_direction == 1)
        {
            SmartCarLeft(current_inside_speed, SPEED_OUTSIDE);
        }
        else if (last_direction == 2)
        {
            SmartCarRight(SPEED_OUTSIDE, current_inside_speed);
        }
        return;
    }

    /* =================================================================
     * 4. 第三优先级：完全回正/安全直行区
     * ================================================================= */
    trigger_duration = 0;
    last_direction = 0;
    SmartCarForward(SPEED_STRAIGHT, SPEED_STRAIGHT);
}