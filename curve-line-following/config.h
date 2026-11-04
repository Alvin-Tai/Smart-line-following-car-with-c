#ifndef _CONFIG_H
#define _CONFIG_H
/****************************************************************
 通用头文件
****************************************************************/
#include <reg52.h>
#include <intrins.h>

#define MAIN_Fosc       11059200UL  //宏定义主时钟HZ
#define BAUD 9600 //UART 波特率

#define LeftMotorStop       IN1 = 0, IN2 = 0//左电机停止
#define RightMotorStop      IN3 = 0, IN4 = 0//右电机停止
#define LeftMotorGo         IN1 = 0, IN2 = 1//左电机正转
#define LeftMotorBack       IN1 = 1, IN2 = 0//左电机反转
#define RightMotorGo        IN3 = 1, IN4 = 0//右电机正转
#define RightMotorBack      IN3 = 0, IN4 = 1//右电机反转
#define Timer1On            TR1 = 1         //启动定时器1
#define Timer1Off           TR1 = 0         //关闭定时器1
#define EA_on               EA = 1          //开始总中断
#define EA_off              EA = 0          //关闭总中断
#define BUZZER_on           BUZZER = 0      //开启蜂鸣器
#define BUZZER_off          BUZZER = 1      //关闭蜂鸣器

/****************************************************************
已有数据类型重新定义
****************************************************************/
typedef signed char int8;
typedef signed int  int16;
typedef unsigned char uint8;
typedef unsigned char uchar;
typedef unsigned int  uint16;
typedef unsigned int  uint;
typedef unsigned long uint32;
typedef unsigned char BYTE;
typedef unsigned int  WORD;

/****************************************************************
 硬件接口定义
****************************************************************/
/*电机驱动IO定义*/
sbit IN1 = P1^2; //为1 左电机反转
sbit IN2 = P1^3; //为1 左电机正转
sbit IN3 = P1^6; //为1 右电机正转
sbit IN4 = P1^7; //为1 右电机反转
sbit LeftMotor_EN  = P1^4; //为1 左电机使能
sbit RightMotor_EN = P1^5; //为1 右电机使能
/*舵机接口*/
sbit ServoPin = P2^7;
/*超声波接口*/
sbit EchoPin = P2^0;
sbit TrigPin = P2^1;
/*4位独立按键引脚*/
sbit KEY_S2 = P3^0;
sbit KEY_S3 = P3^1;
sbit KEY_S4 = P3^2;
sbit KEY_S5 = P3^3;
/*LCD1602液晶引脚*/
#define LCD1602_DB  P0
sbit LCD1602_RS = P3^5;
sbit LCD1602_RW = P3^6;
sbit LCD1602_EN = P3^4;
/*5V有源蜂鸣器引脚*/
sbit BUZZER = P2^3;
/*传感器接口定义*/
sbit FollowSensorLeft   = P3^4;
sbit FollowSensorRight  = P3^5;
sbit AvoidSensorLeft    = P3^4;
sbit AvoidSensorRight   = P3^5;
sbit TrackSensorLeft    = P3^3;
sbit TrackSensorRight   = P3^2;

sbit UAvoidSensorLeft   = P1^0;
sbit UAvoidSensorRight  = P1^1;

sbit LED = P2^4;

#endif