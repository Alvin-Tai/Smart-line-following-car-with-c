#ifndef _MAIN_H
#define _MAIN_H

#include "config.h"

/* 全局速度配置变量（在 main.c 中定义，供 app.c 引用） */
extern uchar SpeedForwardLeft;   /* 直行左轮速度 */
extern uchar SpeedForwardRight;  /* 直行右轮速度 */
extern uchar SpeedTurnLeft;      /* 左转时外侧（右轮）速度 */
extern uchar SpeedTurnRight;     /* 右转时外侧（左轮）速度 */

#endif