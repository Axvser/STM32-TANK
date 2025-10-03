#include "stm32f10x.h"
#include "bitband.h"
#include "system.h"
#include "delay.h"
#include "runtime.h"

///////// ҵ���װ //////////
#include "tcpserver.h"
#include "converter.h"

////////// ������λ������ /////////////
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
    // 1. ����GPIOʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    // 2. ��ʼ��GPIO�ṹ��
    GPIO_InitTypeDef GPIO_InitStructure;
    // 3. ����GPIO����
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;        // LED���ӵ�����
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  // �������ģʽ
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // ����ٶ�50MHz    
    // 4. ��ʼ��GPIO
    GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	// ��ʼ������ʱ
    RunTime_Init();

    // 60FPS - ʵʱ��Ҫ���
    RunTime_MonoBehaviour(60, 1, Test,0);
    // 20FPS - ʵʱ��Ҫ���е�
    RunTime_MonoBehaviour(20, 0);
    // 1FPS - ʵʱ��Ҫ���
    RunTime_MonoBehaviour(1, 0);

    // ��������ʱ
    RunTime_Run();
    
    while(1);
}
