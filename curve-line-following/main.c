#define  _MAIN_C
#include "config.h"
#include "timer_ISR.h"
#include "motor.h"
#include "Delay.h"
#include "app.h"
#include "main.h"

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
				BUZZER_on;	//启动开机提示音（按键一次响一声）
				Delay1ms(200);//200毫秒延时
				BUZZER_off;	//关闭蜂鸣器
				break;		//退出FOR死循环
			}
		}
	}	
}

void main()
{
	IntRegInit();

	Timer0Init();

	keyscan();

	while(1)
	{
		IR_Tracking();
	}
}