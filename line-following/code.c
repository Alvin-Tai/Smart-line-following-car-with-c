/******************************************************************************
 * @file    code.c
 * @brief   51单片机寻迹小车 - 硬件基础测试程序
 * @note    仅用于验证电机接线、方向、PWM调速是否正常
 ******************************************************************************/

#include <reg52.h>

/* ================= 引脚定义 ================= */
sbit IN1 = P1^2;   /* 左轮方向1 */
sbit IN2 = P1^3;   /* 左轮方向2 */
sbit EN1 = P1^4;   /* 左轮PWM使能 */
sbit EN2 = P1^5;   /* 右轮PWM使能 */
sbit IN3 = P1^6;   /* 右轮方向1 */
sbit IN4 = P1^7;   /* 右轮方向2 */

/* ================= 全局变量 ================= */
/* [OPT] 加 volatile：中断与主循环共享，防止编译器优化读到旧值 */
volatile unsigned char speed_L = 0;   /* 左轮速度 0~100 */
volatile unsigned char speed_R = 0;   /* 右轮速度 0~100 */
volatile unsigned char pwm_count = 0; /* PWM计数器 0~99 */

/* ================= 基础延时 ================= */
/* [OPT] 仅用于动作间隔，注释更准确（12MHz经验值，换晶振需重算） */
void Delay_ms(unsigned int ms)
{
    unsigned int i, j;
    for (i = 0; i < ms; i++)
        for (j = 0; j < 112; j++);
}

/* ================= 定时器0初始化 ================= */
/* 100us中断一次，100次 = 一个PWM周期 10ms (100Hz) */
void Timer0_Init(void)
{
    TMOD &= 0xF0;    /* 清除T0模式位 */
    TMOD |= 0x01;    /* T0 模式1：16位定时器 */
    TH0 = 0xFF;      /* 12MHz下约100us初值 */
    TL0 = 0x9C;
    ET0 = 1;         /* 使能T0中断 */
    EA  = 1;         /* 使能总中断 */
    TR0 = 1;         /* 启动T0 */
}

/* ================= 定时器0中断（PWM生成） ================= */
void Timer0_ISR(void) interrupt 1
{
    TH0 = 0xFF;      /* 软件重装初值 */
    TL0 = 0x9C;

    if (++pwm_count >= 100)
        pwm_count = 0;

    /* [OPT] 用三元表达式，逻辑更紧凑；
       占空比 = speed / 100 */
    EN1 = (pwm_count < speed_L) ? 1 : 0;
    EN2 = (pwm_count < speed_R) ? 1 : 0;
}

/* ================= 动作函数 ================= */

/* 前进（两轮正转） */
void Car_Forward(void)
{
    IN1 = 0;  IN2 = 1;   /* 左轮正转 */
    IN3 = 1;  IN4 = 0;   /* 右轮正转 */
    speed_L = 90;
    speed_R = 90;
}

/* 后退（两轮反转） */
void Car_Backward(void)
{
    IN1 = 1;  IN2 = 0;   /* 左轮反转 */
    IN3 = 0;  IN4 = 1;   /* 右轮反转 */
    speed_L = 90;
    speed_R = 90;
}

/* 停车（滑行：EN=0，方向线保持） */
/* [OPT] 只清速度即可，EN由中断自动拉低；不再清方向线，
   因为"IN全0"在H桥上不一定是断电，反而可能变成制动。
   测试程序里保持滑行更安全，避免频繁大电流冲击。 */
void Car_Stop(void)
{
    speed_L = 0;
    speed_R = 0;
}

/* [OPT] 新增：刹车（方向线全拉低，快速制动）
   与 Car_Stop 区分开，测试时可对比两种停车效果 */
void Car_Brake(void)
{
    speed_L = 0;
    speed_R = 0;
    IN1 = 0; IN2 = 0; IN3 = 0; IN4 = 0;
}

/* ================= 主函数 ================= */
void main(void)
{
    Timer0_Init();          /* 启动PWM引擎（后台自动运行） */

    /* 1. 前进 1秒 */
    Car_Forward();
    Delay_ms(1000);

    /* 2. 停车缓冲 1秒 */
    Car_Stop();
    Delay_ms(1000);

    /* 3. 后退 1秒 */
    Car_Backward();
    Delay_ms(1000);

    /* 4. 刹车停车 */
    Car_Brake();

    /* 停机死循环：PWM中断仍在跑，但speed=0，电机保持停转 */
    while (1);
}