#include "stm32f10x.h"
#include "bitband.h"
#include "system.h"
#include "delay.h"
#include "runtime.h"

///////// ҵ���װ //////////
#include "gun.h"
#include "tcpserver.h"
#include "converter.h"

///////// ȫ�������� //////////
GunContext gun;
TcpServerContext tcpserver;

////////// ������λ������ /////////////
void ESP01S_DataHandler(uint16_t data)
{
    ParseTankData(data);
}
void ProcessTankValues(double values[], uint8_t count)
{
    gun.av = values[3];   // ��ֱ����Ƕ�
    gun.fire = values[4]; // ���ӿ�����ͣ
#if DEBUG_CONVERTER
    printf("Updated [ av:%lf , fire:%lf ]\r\n", values[3], values[4]);
#endif
}

int main(void)
{
    USART1_Debug_Init(BAUD_LOW);
    ESP01S_Init(BAUD_LOW);

    // ��̨ģ���ʼ��
    gun = Gun_Init(19999, 71);
    tcpserver = TcpServer_Init("Axvser", "888888881", "192.168.8.8", "192.168.8.1", "255.255.255.0", 8080);
    TcpServer_Build_STA(&tcpserver);

    // ��ʼ������ʱ
    RunTime_Init();

    // 60FPS - ʵʱ��Ҫ���
    RunTime_MonoBehaviour(60, 1, Gun_Update, &gun);
    // 20FPS - ʵʱ��Ҫ���е�
    RunTime_MonoBehaviour(20, 0);
    // 1FPS - ʵʱ��Ҫ���
    RunTime_MonoBehaviour(1, 0);

    // ��������ʱ
    RunTime_Run();

    while (1)
        ;
}
