/******************************************************************************
 * @file    app.c
 * @brief   小车循迹应用层：适配急弯赛道（内侧轮反转版）
 * @note    传感器极性：压线 = 1，空白 = 0
 ******************************************************************************/

#include "config.h"
#include "timer_ISR.h"
#include "Delay.h"
#include "motor.h"
#include "app.h"
#include "main.h"

/* =====================================================================
 * ★★★ 核心调速与保持参数配置区（可根据实际车况反复微调） ★★★
 * ===================================================================== */
#define SPEED_STRAIGHT     70   // 直行基础速度（数值越大直行越快，但过弯容易冲出去）
#define SPEED_OUTSIDE      250  // 过弯时外侧轮速度（负责提供主要的转弯驱动力）
#define SPEED_INSIDE       125   // 过弯时内侧轮反转速度（反转力度，数值越大原地掉头越猛）

#define TURN_HOLD_TIME     6500 // 盲打保持计数：传感器刚脱离后，继续维持转弯状态的循环次数
                                // 针对急弯图纸，如果发现过弯后转不过来可以适当调大；
                                // 如果发现转过头了（转圈圈），可以适当调小。
/* ===================================================================== */

static unsigned int hold_counter = 0;
static uchar last_direction = 0; /* 状态记录：1 = 上一次向左偏离，2 = 上一次向右偏离 */

void IR_Tracking()
{
    /* 1. LED 指示灯状态：高电平 1 压线时点亮 */
    if (TrackSensorLeft == 1 || TrackSensorRight == 1)
    {
        LED = 0; /* 压线：LED亮 */
    }
    else
    {
        LED = 1; /* 丢线：LED灭 */
    }

    /* =================================================================
     * 2. 第一优先级：实时传感器检测与急弯纠正（压线 = 1, 空白 = 0）
     * ================================================================= */
    if (TrackSensorLeft == 1 && TrackSensorRight == 0)
    {
        /* 左侧压线，右侧空白 -> 说明车向右偏了，需要向左急转弯修正 */
        last_direction = 1;
        hold_counter = TURN_HOLD_TIME; // 刷新保持计数
        
        // 调用内侧轮反转函数：左轮反转(SPEED_INSIDE)，右轮正转(SPEED_OUTSIDE)
        SmartCarLeftTurn(SPEED_INSIDE, SPEED_OUTSIDE);
        return;
    }
    else if (TrackSensorLeft == 0 && TrackSensorRight == 1)
    {
        /* 左侧空白，右侧压线 -> 说明车向左偏了，需要向右急转弯修正 */
        last_direction = 2;
        hold_counter = TURN_HOLD_TIME; // 刷新保持计数
        
        // 调用内侧轮反转函数：右轮反转(SPEED_INSIDE)，左轮正转(SPEED_OUTSIDE)
        SmartCarRightTurn(SPEED_OUTSIDE, SPEED_INSIDE);
        return;
    }

    /* =================================================================
     * 3. 第二优先级：盲打保持区（专门用来对付图中的死角和连续急弯）
     * 当传感器临时同时变0（压线瞬间的空隙或刚甩出赛道），靠记忆继续转弯
     * ================================================================= */
    if (hold_counter > 0)
    {
        hold_counter--;

        if (last_direction == 1)
        {
            SmartCarLeftTurn(SPEED_INSIDE, SPEED_OUTSIDE);
        }
        else if (last_direction == 2)
        {
            SmartCarRightTurn(SPEED_OUTSIDE, SPEED_INSIDE);
        }
        return;
    }

    /* =================================================================
     * 4. 第三优先级：默认直行
     * 传感器双侧都是 0（在直道正常行驶）或双侧都是 1
     * ================================================================= */
    last_direction = 0;
    SmartCarForward(SPEED_STRAIGHT, SPEED_STRAIGHT);
}