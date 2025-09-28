///////// �������� /////////
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "FreeRTOS.h"
#include "task.h"
#include "bitband.h"
#include "debug.h"
#include "runtime.h"

///////// ҵ���װ //////////
#include "motor.h"
#include "gun.h"
#include "converter.h"

//////// ϵͳ������ /////////
typedef struct
{
    MotorContext motor;
    GunContext gun;
} SystemContext;

// ����ϵͳ�����ĵ�ȫ�ֱ���
SystemContext system;

////////// ������λ������ /////////////
void ESP01S_DataHandler(uint16_t data)
{
    ParseTankData(data);
}
void ProcessTankValues(double values[], uint8_t count)
{
    if (5 != count)
        return;
    system.motor.vl = values[0]; // ����Ĵ��ٶ�
    system.motor.vr = values[1]; // �Ҳ��Ĵ��ٶ�
    system.gun.ah = values[2];   // ˮƽ����Ƕ�
    system.gun.av = values[3];   // ��ֱ����Ƕ�
    system.gun.fire = values[4]; // ���
}

void LED_INIT(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    // 1. ʹ��GPIOAʱ��
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

    // 2. ����PA6��PA7
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // 3. ��ʼ״̬���ر�LED
    GPIO_ResetBits(GPIOA, GPIO_Pin_6 | GPIO_Pin_7);
}

typedef struct
{
    uint8_t state;
} LEDContext;

void LED_TEST(void *context)
{
    LEDContext *ctx = (LEDContext *)context;

    while (1)
    {
        if (ctx->state)
        {
            GPIO_SetBits(GPIOA, GPIO_Pin_6);
            GPIO_ResetBits(GPIOA, GPIO_Pin_7);
        }
        else
        {
            GPIO_ResetBits(GPIOA, GPIO_Pin_6);
            GPIO_SetBits(GPIOA, GPIO_Pin_7);
        }
        ctx->state = !ctx->state;
        vTaskDelay(2000);
    }
}

////////// ������ /////////////
int main(void)
{
    LEDContext ct;
    LED_INIT();
    // ���ô��ڴ�ӡ
    USART1_Debug_Init(115200);

    // ��ʼ��ϵͳ������
    system.motor = Motor_Init(1679, 0);
    system.gun = Gun_Init(839, 2);

    // ��ʼ������ʱ
    RunTime_Init(&system);

    // 60FPS - ���߳�,����ʵʱ��Ҫ��[��]������
    RunTime_MonoBehaviour(60, &system, 2, Motor_Update, Gun_Update);
    // 25FPS - ���߳�,����ʵʱ��Ҫ��[��]������
    RunTime_MonoBehaviour(25, &ct, 0);
    // 1FPS - ���߳�,����ʵʱ��Ҫ��[��]������
    RunTime_MonoBehaviour(1, &ct, 1, LED_TEST);

    // ��������ʱ
    RunTime_Run();
}
