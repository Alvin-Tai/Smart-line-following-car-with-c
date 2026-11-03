#define  _MAIN_C
#include "config.h"
#include "timer_ISR.h"
#include "motor.h"
#include "Delay.h"
#include "app.h"
#include "main.h"

/* =====================================================================
 * 全局速度变量定义
 * ===================================================================== */
uchar SpeedForwardLeft;   /* 直行左轮速度 */
uchar SpeedForwardRight;  /* 直行右轮速度 */
uchar SpeedTurnLeft;      /* 左转时外侧轮速度 */
uchar SpeedTurnRight;     /* 右转时外侧轮速度 */

void keyscan()
{
	for(;;)	//死循环
	{
		if(KEY_S2 == 0)// 实时检测S2按键是否被按下
		{
			Delay1ms(5); //消抖
			if(KEY_S2 == 0)//再确认S2是否被按下
			{
				while(!KEY_S2);//等待松手
				BUZZER_on;	//蜂鸣器响
				Delay1ms(200);//200毫秒延时
				BUZZER_off;	//关闭蜂鸣器
				break;		//退出FOR死循环
			}
		}
	}	
}

void main()
{
	/* =================================================================
	 * ★★★ 核心速度集中调节区（范围：0 ~ 255） ★★★
	 * 以后调速只在此处修改这四个数字！
	 * ================================================================= */
	SpeedForwardLeft  = 70;   /* 直行左轮基础速度（适当调小，防止大惯性冲弯） */
	SpeedForwardRight = 70;   /* 直行右轮基础速度 */
	
	SpeedTurnLeft     = 140;  /* 左转修正时外侧（右轮）速度（调大此值可大幅增强转弯力度） */
	SpeedTurnRight    = 140;  /* 右转修正时外侧（左轮）速度（调大此值可大幅增强转弯力度） */

	IntRegInit();

	Timer0Init();

	keyscan();

	while(1)
	{
		IR_Tracking();
	}
}