						   #include <reg52.h>

// ================= 引脚定义 =================
sbit IN1 = P1^2;  
sbit IN2 = P1^3;  
sbit EN1 = P1^4;  // 左电机使能
sbit EN2 = P1^5;  // 右电机使能
sbit IN3 = P1^6;  
sbit IN4 = P1^7;  

// ================= 全局变量 =================
// 速度设定：范围 0 ~ 100 (0为停，100为满速)
// 根据你之前的测试，左轮慢一点，设为 75；右轮设为 100
unsigned char speed_L = 75; 
unsigned char speed_R = 100;

// 定时器中断里的计数器
unsigned char pwm_count = 0;

// ================= 基础延时函数 =================
// 仅用于主循环里的动作间隔（如停1秒），不参与PWM生成
void Delay_ms(unsigned int ms)
{
    unsigned int i, j;
    for(i = 0; i < ms; i++)
        for(j = 0; j < 112; j++); // 12MHz下约1ms
}

// ================= 定时器0初始化 =================
// 产生大约 100us 的中断，100次中断就是一个完整的 PWM 周期 (10ms)
void Timer0_Init()
{
    TMOD &= 0xF0;   // 清除定时器0模式位
    TMOD |= 0x01;   // 设定定时器0为16位模式 (模式1)
    TH0 = 0xFF;     // 12MHz晶振下约 100us 初值
    TL0 = 0x9C;
    ET0 = 1;        // 开启定时器0中断
    EA = 1;         // 开启总中断
    TR0 = 1;        // 启动定时器0
}

// ================= 定时器0中断服务函数 =================
// 这个函数每 100us 自动执行一次，完全由硬件控制，极其精准！
void Timer0_ISR() interrupt 1
{
    TH0 = 0xFF;     // 重新装载初值
    TL0 = 0x9C;
    
    pwm_count++;
    if(pwm_count >= 100) 
    {
        pwm_count = 0; // 满 100 次（10ms），计数器清零，开始下一个周期
    }
    
    // --- 左轮 PWM 控制 ---
    if(pwm_count < speed_L) 
        EN1 = 1;
    else 
        EN1 = 0;
        
    // --- 右轮 PWM 控制 ---
    if(pwm_count < speed_R) 
        EN2 = 1;
    else 
        EN2 = 0;
}

// ================= 动作函数 =================

// 前进
void Car_Forward(void)
{
    IN1 = 0;  IN2 = 1;  // 左轮正转
    IN3 = 1;  IN4 = 0;  // 右轮正转
    speed_L = 75;       // 恢复前进时的差速
    speed_R = 100;
}

// 后退 (不用差速，直接满速后退)
void Car_Backward(void)
{
    IN1 = 1;  IN2 = 0;  // 左轮反转
    IN3 = 0;  IN4 = 1;  // 右轮反转
    speed_L = 100;      // 后退时两轮都满速
    speed_R = 100;
}

// 停车
void Car_Stop(void)
{
    speed_L = 0;        // 速度为0，中断里会把 EN1 和 EN2 永远拉低
    speed_R = 0;
    // 把方向线也全拉低，彻底断电防发热
    IN1 = 0; IN2 = 0; IN3 = 0; IN4 = 0;
}

// ================= 主函数 =================
void main()
{
    Timer0_Init(); // 启动 PWM 引擎 (后台自动运行)
    
    // 1. 前进 2 秒
    Car_Forward();
    Delay_ms(2000); 
    
    // 2. 停车缓冲 1 秒
    Car_Stop();
    Delay_ms(1000);
    
    // 3. 后退 2 秒
    Car_Backward();
    Delay_ms(2000);
    
    // 4. 彻底停车
    Car_Stop();
    
    // 停机死循环，PWM中断还在跑，但因为 speed=0，电机保持停转
    while(1);
}