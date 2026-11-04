#include "config.h"
#include "Delay.h"

void Delay1ms(uint ms)
{
    uint i;
    do{
        i = MAIN_Fosc / 96000;
        while(--i);
    }while(--ms);
}

/* ★ 下面 6 个函数原来是注释掉的，这里恢复，解决 UNRESOLVED EXTERNAL 警告 */
void Delay1550us(void)      //@11.0592MHz
{
    unsigned char i, j;
    i = 3;
    j = 196;
    do
    {
        while (--j);
    } while (--i);
}

void Delay18450us(void)     //@11.0592MHz
{
    unsigned char i, j;
    _nop_();
    i = 34;
    j = 16;
    do
    {
        while (--j);
    } while (--i);
}

void Delay600us(void)       //@11.0592MHz
{
    unsigned char i, j;
    _nop_();
    i = 2;
    j = 15;
    do
    {
        while (--j);
    } while (--i);
}

void Delay19400us(void)     //@11.0592MHz
{
    unsigned char i, j;
    _nop_();
    i = 35;
    j = 197;
    do
    {
        while (--j);
    } while (--i);
}

void Delay17500us(void)     //@11.0592MHz
{
    unsigned char i, j;
    i = 32;
    j = 93;
    do
    {
        while (--j);
    } while (--i);
}

void Delay2500us(void)      //@11.0592MHz
{
    unsigned char i, j;
    i = 5;
    j = 120;
    do
    {
        while (--j);
    } while (--i);
}