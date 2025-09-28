///////// 基础依赖 /////////
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "FreeRTOS.h"
#include "task.h"
#include "bitband.h"
#include "debug.h"
#include "runtime.h"

///////// 业务封装 //////////
#include "motor.h"
#include "gun.h"
#include "converter.h"

//////// 系统上下文 /////////
typedef struct
{
    MotorContext motor;
    GunContext gun;
} SystemContext;

// 创建系统上下文的全局变量
SystemContext system;

////////// 处理上位机数据 /////////////
void ESP01S_DataHandler(uint16_t data)
{
    ParseTankData(data);
}
void ProcessTankValues(double values[], uint8_t count)
{
    if (5 != count)
        return;
    system.motor.vl = values[0]; // 左侧履带速度
    system.motor.vr = values[1]; // 右侧履带速度
    system.gun.ah = values[2];   // 水平舵机角度
    system.gun.av = values[3];   // 垂直舵机角度
    system.gun.fire = values[4]; // 火控
}

void LED_INIT(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    // 1. 使能GPIOA时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

    // 2. 配置PA6和PA7
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // 3. 初始状态：关闭LED
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

////////// 主函数 /////////////
int main(void)
{
    LEDContext ct;
    LED_INIT();
    // 启用串口打印
    USART1_Debug_Init(115200);

    // 初始化系统上下文
    system.motor = Motor_Init(1679, 0);
    system.gun = Gun_Init(839, 2);

    // 初始化运行时
    RunTime_Init(&system);

    // 60FPS - 主线程,处理实时性要求[高]的数据
    RunTime_MonoBehaviour(60, &system, 2, Motor_Update, Gun_Update);
    // 25FPS - 副线程,处理实时性要求[中]的数据
    RunTime_MonoBehaviour(25, &ct, 0);
    // 1FPS - 次线程,处理实时性要求[低]的数据
    RunTime_MonoBehaviour(1, &ct, 1, LED_TEST);

    // 启动运行时
    RunTime_Run();
}
