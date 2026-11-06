#include <reg52.h>

// 共阴极段码表 (0~9)
unsigned char code smg_code[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};

sbit DU = P2^6; // 段选
sbit WE = P2^7; // 位选
sbit KEY = P3^0; // 用 S2 按键来清零计数

unsigned int loop_count = 0; // ★ 核心：记录主循环执行次数

void Delay1ms(unsigned int ms)
{
    unsigned int i, j;
    for(i = ms; i > 0; i--)
        for(j = 110; j > 0; j--);
}

// ★ 极其重要的消影函数 ★
void Clear_SMG()
{
    P0 = 0x00; // 段码全 0 (共阴极全灭)
    DU = 1; DU = 0; // 锁存段码
    P0 = 0xFF; // 位码全 1 (你的板子 0xFF 是全选通，相当于取消了所有位选)
    WE = 1; WE = 0; // 锁存位码
}

void main()
{
    unsigned char i;
    unsigned char num[4] = {0}; // 我们要显示前 4 位
    unsigned int temp;

    while(1)
    {
        // ==========================================
        // 1. 主循环逻辑：这里假装是你的寻迹代码在跑
        // ==========================================
        loop_count++; 
        Delay1ms(1); // 加 1ms 延时模拟寻迹时的计算时间，让数字涨得慢一点，肉眼能看清

        // 如果按下按键，清零
        if(KEY == 0)
        {
            Delay1ms(10); // 消抖
            if(KEY == 0)
            {
                while(KEY == 0); // 等待松手
                loop_count = 0; // 清零
            }
        }

        // 把 loop_count 拆解成 4 个数字，存入 num 数组
        temp = loop_count;
        num[0] = temp / 1000 % 10;
        num[1] = temp / 100 % 10;
        num[2] = temp / 10 % 10;
        num[3] = temp % 10;

        // ==========================================
        // 2. 动态扫描数码管（只扫前 4 位）
        // ==========================================
        for(i = 0; i < 4; i++)
        {
            // 第一步：先关闭所有位选，防止上一个数字的残影
            P0 = 0xFF; 
            WE = 1; WE = 0; 

            // 第二步：送段码（显示什么数字）
            P0 = smg_code[num[i]]; 
            DU = 1; DU = 0; 

            // 第三步：打开对应的位选（选中这一个管子）
            // 你的板子是给 1 选通，所以 1<<i 代表第 i 根管子亮
            P0 = ~(1 << i); // ★ 注意这里用了取反 ~ ★
            // （如果你之前全亮是 0xFF，那选中某一位很可能是 0 使它亮）
            WE = 1; WE = 0; 

            // 第四步：短暂延时让 LED 发光
            Delay1ms(2);
        }
    }
}