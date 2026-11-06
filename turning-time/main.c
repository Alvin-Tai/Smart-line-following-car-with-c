/******************************************************************************
 * @file    main.c
 * @brief   传感器触发持续时间采集测试程序（带电机急停刹车版）
 * @note    极性：压线 = 1，空白 = 0
 ******************************************************************************/

#include <reg52.h>
#include "config.h"
#include "motor.h"
#include "Delay.h"

/* 数码管段码表和锁存器定义 */
unsigned char code smg_code[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};

sbit DU = P2^6; // 段选锁存器
sbit WE = P2^7; // 位选锁存器


#define SPEED_TEST 70   /* 测试时的直行基础速度 */


void main()
{
    unsigned int trigger_loops = 0;  // 核心：统计传感器触发期间主循环跑了多少轮
    unsigned char i;
    unsigned char num[4] = {0};      // 拆解后的 4 位数字
    unsigned int temp;

    
    // 1. 先直行等待，直到传感器“刚开始压线”（从 0 变成 1）
    while (TrackSensorLeft == 0 && TrackSensorRight == 0)
    {
        SmartCarForward(SPEED_TEST, SPEED_TEST);
    }

    // 2. 已经压线！立刻开始一边直行、一边疯狂计数，直到传感器离开（变回 0）
    trigger_loops = 0;
    while (TrackSensorLeft == 1 || TrackSensorRight == 1)
    {
        SmartCarForward(SPEED_TEST, SPEED_TEST);
        trigger_loops++; // 主循环每跑一次，计数器+1
    }

    // ==========================================================
    // 阶段三：传感器停止触发后，继续直行一小段（比如 300ms，防止过早刹车）
    // ==========================================================
    SmartCarForward(SPEED_TEST, SPEED_TEST);
    Delay1ms(300); 

    // ==========================================================
    // 阶段四：物理急刹车（倒车反向冲量 + 急停锁死）
    // ==========================================================
    SmartCarBack(80, 80);  // 瞬间给一个反向动力（倒车刹车抵消惯性）
    Delay1ms(100);         // 倒车刹车 100 毫秒
    SmartCarStops();       // 调用 motor.c 里的现成函数彻底锁死电机

    // ==========================================================
    // 阶段五：死循环用数码管静态/动态显示采集到的循环轮数数据
    // ==========================================================
    temp = trigger_loops;
    
    num[0] = temp / 1000 % 10;
    num[1] = temp / 100 % 10;
    num[2] = temp / 10 % 10;
    num[3] = temp % 10;

    while(1)
    {
        for(i = 0; i < 4; i++)
        {
            P0 = 0xFF; 
            WE = 1; WE = 0; 

            P0 = smg_code[num[i]]; 
            DU = 1; DU = 0; 

            P0 = ~(1 << i); 
            WE = 1; WE = 0; 

            Delay1ms(2);
        }
    }
}