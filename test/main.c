#define  _MAIN_C
#include "config.h"
#include "timer_ISR.h"
#include "motor.h"
#include "Delay.h"
#include "main.h"

/* 按键扫描函数 (保留原来的) */
void keyscan()
{
	for(;;)	//死循环
	{
		if(KEY_S2 == 0)// 实时检测S2按键是否被按下
		{
			Delay1ms(5); //软件消抖
			if(KEY_S2 == 0)//再检测S2是否被按下
			{
				while(!KEY_S2);//松手检测
				BUZZER_on;	//开启蜂鸣器
				Delay1ms(200);//200毫秒延时
				BUZZER_off;	//关闭蜂鸣器
				break;		//退出FOR死循环
			}
		}
	}	
}

/* 主函数：直行 -> 闪灯左转 -> 右转 -> 鸣笛 -> 后退 */
void main()
{
	unsigned char i; // ★ 必须放在最前面，因为 Keil C89 不允许在 for 里面定义变量 ★

	IntRegInit(); // 中断寄存器初始化
	Timer0Init(); // PWM调速初始化
	
	SmartCarStops(); // 初始先停车，防止失控
	keyscan();       // 按S2按键启动
	
	// ---------- 1. 直行 ----------
	// 左轮速度130，右轮速度160（你之前测试的差速）
	SmartCarForward(130, 160);
	Delay1ms(2000); // 直行 2 秒
	
	// ---------- 2. 闪灯 + 左转 ----------
	// 左转：左轮停，右轮前进
	SmartCarLeft(0, 180);
	
	// 闪灯 3 次（每次亮200ms，灭200ms，总计1.2秒）
	for(i = 0; i < 3; i++) 
	{
		LED = 0; // 灯亮 (假设低电平亮，如果你的灯是反的，把 0 和 1 互换)
		Delay1ms(200);
		LED = 1; // 灯灭
		Delay1ms(200);
	}
	
	// ---------- 3. 右转 ----------
	// 右转：左轮前进，右轮停
	SmartCarRight(180, 0);
	Delay1ms(1200); // 右转 1.2 秒
	
	// ---------- 4. 停车 + 鸣笛 ----------
	SmartCarStops(); // 先停车
	Delay1ms(200);   // 缓冲一下
	
	BUZZER_on;       // 鸣笛 1
	Delay1ms(300);
	BUZZER_off;
	Delay1ms(100);
	
	BUZZER_on;       // 鸣笛 2
	Delay1ms(300);
	BUZZER_off;
	
	Delay1ms(500);   // 停顿半秒
	
	// ---------- 5. 后退 ----------
	// 两轮全速倒车
	SmartCarBack(180, 180);
	Delay1ms(2000); // 后退 2 秒
	
	// ---------- 彻底停车 ----------
	SmartCarStops();
	
	while(1); // 停机死循环，程序停在这里
}