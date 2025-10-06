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
#include "tcpserver.h"
#include "converter.h"

///////// ȫ�������� //////////
MotorContext motor;
GunContext gun;
TcpServerContext tcpserver;

////////// ������λ������ /////////////
void ESP01S_DataHandler(uint16_t data)
{
    ParseTankData(data);
}
void ProcessTankValues(double values[], uint8_t count)
{
    motor.vl = values[0]; // ����Ĵ��ٶ�
    motor.vr = values[1]; // �Ҳ��Ĵ��ٶ�
    gun.ah = values[2];   // ˮƽ����Ƕ�
    gun.av = values[3];   // ��ֱ����Ƕ�
    gun.fire = values[4]; // ���俪��
    tcpserver.counter = 20; // ����������Ч����ʱ
#if DEBUG_CONVERTER
    printf("Updated [ vl:%lf , vr:%lf , ah:%lf ]\r\n", values[0], values[1], values[2]);
#endif
}

///////// ����������ʩ /////////
void WIFI_OnDisconnect(void *context);

////////// ������ /////////////
int main(void)
{
    // �豸��ʼ��
    USART1_Debug_Init(BAUD_LOW);
    ESP01S_Init(BAUD_LOW);
    motor = Motor_Init(1679, 0);
    gun = Gun_Init(1999, 839);
    Motor_RST(&motor);
    Gun_RST(&gun);
    tcpserver = TcpServer_Init("Axvser", "888888881", "192.168.6.6", "192.168.6.1", "255.255.255.0", 8080);
    TcpServer_Build_STA(&tcpserver);

    // ����ʱ��ʼ��
    RunTime_Init();

    // 60FPS - ʵʱ��Ҫ���
    RunTime_MonoBehaviour(60, 2, Motor_Update, &motor, Gun_Update, &gun);
    // 20FPS - ʵʱ��Ҫ���е�
    RunTime_MonoBehaviour(20, 1, WIFI_OnDisconnect, &tcpserver);
    // 1FPS - ʵʱ��Ҫ���
    RunTime_MonoBehaviour(1, 0);

    // ��������ʱ
    RunTime_Run();

    return 0;
}

void WIFI_OnDisconnect(void *context)
{
    TcpServerContext *tcp = (TcpServerContext *)context;
    if ((0 == context) || ((--tcp->counter) < 0))
    {
        Gun_RST(&gun);
        Motor_RST(&motor);
    }
}
