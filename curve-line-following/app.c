/******************************************************************************
 * @file    app.c
 * @brief   小车应用层：两档动态自适应（急弯倒转 vs 缓弯常规）+ 集中调参区
 * @note    传感器极性：压线 = 1，空白 = 0
 ******************************************************************************/

#include "config.h"
#include "timer_ISR.h"
#include "Delay.h"
#include "motor.h"
#include "app.h"
#include "main.h"

/* =====================================================================
 * ★★★ 核心调参集中营（所有参数统一在此调整） ★★★
 * ===================================================================== */

/* 1. 基础速度配置 */
#define SPEED_STRAIGHT 65 /* 直行基础速度 */
#define SPEED_OUTSIDE 255  /* 转弯时外侧轮速度 */

/* 2. 档位阈值分界线 */
/* 触发判定限值：连续压线循环次数小于此值的算作“急弯”，大于等于此值的算作“缓弯” */
#define TURN_MODE_LIMIT 1000

/* 3. 【档位一：急弯模式】参数配置（内侧轮倒转） */
#define HOLD_LV_SHARP 5000 /* 急弯时的转弯保持时间（防止急弯甩脱） */
#define SPEED_IN_SHARP 125  /* 急弯内侧轮反转速度（数值越大倒转越猛） */

/* 4. 【档位二：缓弯模式】参数配置（内侧轮不转或微速） */
#define HOLD_LV_GENTLE 7000 /* 缓弯时的转弯保持时间（防止长缓弯转过头） */
#define SPEED_IN_GENTLE 0   /* 缓弯内侧轮速度（0代表直接停转，不倒转） */

/* ===================================================================== */

static unsigned int hold_counter = 0;     /* 当前实际生效的动态保持计数 */
static unsigned int trigger_duration = 0; /* 统计传感器连续压线的时长（计数） */
static uchar last_direction = 0;          /* 1=左转偏向，2=右转偏向 */

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
     * 2. 第一优先级：实时单侧纠偏与两档动态判断
     * ================================================================= */
    if (TrackSensorLeft == 1 && TrackSensorRight == 0)
    {
        /* 左侧压线，右侧空白 -> 需要向左修正 */

        if (last_direction != 1)
        {
            trigger_duration = 0; // 方向切换，清空之前的压线计时
        }

        last_direction = 1;
        trigger_duration++; // 累加当前连续压线计数

        /* --- 两档弯道自适应判定 --- */
        if (trigger_duration < TURN_MODE_LIMIT)
        {
            /* 判定为：【急弯】 -> 内侧轮反转模式 */
            hold_counter = HOLD_LV_SHARP;
            SmartCarLeftTurn(SPEED_IN_SHARP, SPEED_OUTSIDE);
        }
        else
        {
            /* 判定为：【缓弯】 -> 内侧轮不转/微速模式 */
            hold_counter = HOLD_LV_GENTLE;
            SmartCarLeft(SPEED_IN_GENTLE, SPEED_OUTSIDE);
        }
        return;
    }
    else if (TrackSensorLeft == 0 && TrackSensorRight == 1)
    {
        /* 左侧空白，右侧压线 -> 需要向右修正 */

        if (last_direction != 2)
        {
            trigger_duration = 0;
        }

        last_direction = 2;
        trigger_duration++;

        /* --- 两档弯道自适应判定（右转同步） --- */
        if (trigger_duration < TURN_MODE_LIMIT)
        {
            /* 判定为：【急弯】 -> 内侧轮反转模式 */
            hold_counter = HOLD_LV_SHARP;
            SmartCarRightTurn(SPEED_OUTSIDE, SPEED_IN_SHARP);
        }
        else
        {
            /* 判定为：【缓弯】 -> 内侧轮不转/微速模式 */
            hold_counter = HOLD_LV_GENTLE;
            SmartCarRight(SPEED_OUTSIDE, SPEED_IN_GENTLE);
        }
        return;
    }

    /* =================================================================
     * 3. 第二优先级：盲打保持期（动态保持生效中）
     * ================================================================= */
    if (hold_counter > 0)
    {
        hold_counter--;
        trigger_duration++; // 保持期间延续计时

        if (last_direction == 1)
        {
            // 保持期间根据之前判定的档位继续动作
            if (trigger_duration < TURN_MODE_LIMIT)
                SmartCarLeftTurn(SPEED_IN_SHARP, SPEED_OUTSIDE);
            else
                SmartCarLeft(SPEED_IN_GENTLE, SPEED_OUTSIDE);
        }
        else if (last_direction == 2)
        {
            if (trigger_duration < TURN_MODE_LIMIT)
                SmartCarRightTurn(SPEED_OUTSIDE, SPEED_IN_SHARP);
            else
                SmartCarRight(SPEED_OUTSIDE, SPEED_IN_GENTLE);
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
