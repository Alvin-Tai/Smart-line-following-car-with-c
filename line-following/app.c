#include "config.h"
#include "timer_ISR.h"
#include "Delay.h"
#include "motor.h"
#include "app.h"

/* ==========================================
 * 速度参数调试区（建议范围 80~255）
 * ========================================== */
#define SPD_STRAIGHT_L    180   // 直线：左轮速度
#define SPD_STRAIGHT_R    180   // 直线：右轮速度

#define SPD_TURN_OUTER    160   // 弯道：外侧轮（速度快，负责拐弯）
#define SPD_TURN_INNER    100   // 弯道：内侧轮（速度慢，防止剧烈顿挫）

void IR_Tracking(void)
{
    uchar L = TrackSensorLeft;
    uchar R = TrackSensorRight;

    if (L == 0 && R == 0) 
    {
        // 1. 正常直行：两边都没看到黑线
        SmartCarForward(SPD_STRAIGHT_L, SPD_STRAIGHT_R); 
        BUZZER_off; // ★ 直行时不响 ★
    }
    else if (L == 0 && R == 1) 
    {
        // 2. 左侧压线：向左修正
        SmartCarForward(SPD_TURN_INNER, SPD_TURN_OUTER); 
        BUZZER_on;  // ★ 压线时鸣笛 ★
    }
    else if (L == 1 && R == 0) 
    {
        // 3. 右侧压线：向右修正
        SmartCarForward(SPD_TURN_OUTER, SPD_TURN_INNER); 
        BUZZER_on;  // ★ 压线时鸣笛 ★
    }
    else 
    {
        // 4. 十字路口/横线：全速冲过去
        SmartCarForward(SPD_STRAIGHT_L, SPD_STRAIGHT_R);
        BUZZER_off; // 十字路口算直行，不响
    }
}