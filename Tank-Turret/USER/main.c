#include "stm32f10x.h"
#include "bitband.h"
#include "system.h"
#include "delay.h"
#include "runtime.h"

///////// 业务封装 //////////
#include "tcpserver.h"
#include "converter.h"

////////// 处理上位机数据 /////////////
void ESP01S_DataHandler(uint16_t data)
{
    ParseTankData(data);
}
void ProcessTankValues(double values[], uint8_t count)
{
    
#if DEBUG_CONVERTER
    printf("Updated [ vl:%lf , vr:%lf , ah:%lf , av:%lf , fire:%lf ]\r\n", values[0], values[1], values[2], values[3], values[4]);
#endif
}

volatile uint8_t counter = 0;
volatile uint8_t now =0;

void Test(void *context)
{
	if(++counter > 60)
	{
		counter = 0;
		PCout(13) = (now = !now);
	}
}

int main(void)
{
    // 1. 开启GPIO时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    // 2. 初始化GPIO结构体
    GPIO_InitTypeDef GPIO_InitStructure;
    // 3. 配置GPIO参数
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;        // LED连接的引脚
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  // 推挽输出模式
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // 输出速度50MHz    
    // 4. 初始化GPIO
    GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	// 初始化运行时
    RunTime_Init();

    // 60FPS - 实时性要求高
    RunTime_MonoBehaviour(60, 1, Test,0);
    // 20FPS - 实时性要求中等
    RunTime_MonoBehaviour(20, 0);
    // 1FPS - 实时性要求低
    RunTime_MonoBehaviour(1, 0);

    // 启动运行时
    RunTime_Run();
    
    while(1);
}
