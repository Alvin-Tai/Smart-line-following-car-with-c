#define  _MAIN_C
#include "config.h"
#include "timer_ISR.h"
#include "motor.h"
#include "Delay.h"
#include "app.h"
#include "main.h"

void keyscan()
{
    for(;;) 
    {
        if(KEY_S2 == 0) 
        {
            Delay1ms(5); 
            if(KEY_S2 == 0) 
            {
                while(!KEY_S2); 
                BUZZER_on; 
                Delay1ms(200); 
                BUZZER_off; 
                break; 
            }
        }
    }   
}

void main()
{
    // 1. 系统初始化
    IntRegInit(); 
    Timer0Init(); 
    
    // 2. 等待按键启动
    keyscan(); 

    // 3. 主循环
    while(1)
    {
        // 核心寻迹逻辑（里面已经包含了鸣笛控制）
        IR_Tracking();
        
        // 维持极小的系统周期（2ms），让传感器反应达到极致
        Delay1ms(2);
    }
}