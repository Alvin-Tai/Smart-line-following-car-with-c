/******************************************************************************
 * @file    app.c
 * @brief   С��Ӧ�ò㣺����ѭ�� + ѹ��LED��ʾ + �ٶȼ���������
 * @note    ���������ԣ�ѹ�� = 1���հ� = 0
 ******************************************************************************/

#include "config.h"
#include "timer_ISR.h"
#include "Delay.h"
#include "motor.h"
#include "app.h"
#include "main.h"

/* =====================================================================
 * ���� �����ٶȼ��е�����������ֻ������� ����
 * ��ֵ��Χ��0 ~ 255����ֵԽ���ٶ�Խ�죩
 * ===================================================================== */
#define SPEED_FORWARD_LEFT   70     /* ֱ�����ֻ����ٶ� */
#define SPEED_FORWARD_RIGHT  70     /* ֱ�����ֻ����ٶ� */

#define SPEED_TURN_LEFT      240    /* ��תʱ��ࣨ���֣��ƽ��ٶ� */
#define SPEED_TURN_RIGHT     240    /* ��תʱ��ࣨ���֣��ƽ��ٶ� */
/* ===================================================================== */

/*====================================
������  ��IR_Tracking
����    ������ѭ������������Ƶ��Ӧ��
����ֵ  ����
��ע    �����������ԡ���ѹ��=1���հ�=0
====================================*/
void IR_Tracking()
{
    /* ����һ��ѹ�ߣ���������� 1 = ��⵽���ߣ� */
    if (TrackSensorLeft == 1 || TrackSensorRight == 1)
    {
        // BUZZER_on;       /* ��ע�ͣ�ѹ�߲����ѣ�����ҹ�������� */
        LED = 0;            /* LED �����͵�ƽ������ʾѹ��״̬�� */
    }
    else
    {
        BUZZER_off;         /* �رշ����� */
        LED = 1;            /* LED �� */
    }

    /* ---- Ѳ�߶����߼���ʹ�ñ��ļ��������ٶȲ����� ---- */
    if (TrackSensorLeft == 0 && TrackSensorRight == 0)
    {
        /* ���߶��հ� �� ��ֱ���ٶ�ƽ��ǰ�� */
        SmartCarForward(SPEED_FORWARD_LEFT, SPEED_FORWARD_RIGHT);
    }
    else if (TrackSensorLeft == 1 && TrackSensorRight == 0)
    {
        /* �����ѹ�� �� ��ת�������ڲ����� 0��������ָ����ƽ��� */
        SmartCarLeft(0, SPEED_TURN_LEFT);
    }
    else if (TrackSensorLeft == 0 && TrackSensorRight == 1)
    {
        /* ���Ҳ�ѹ�� �� ��ת������������ָ����ƽ����ڲ����� 0�� */
        SmartCarRight(SPEED_TURN_RIGHT, 0);
    }
    else
    {
        /* ���߶�ѹ�ߣ�11���� ��ֱ���ٶ�ǰ�� */
        SmartCarForward(SPEED_FORWARD_LEFT, SPEED_FORWARD_RIGHT);
    }
}