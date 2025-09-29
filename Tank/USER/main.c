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
    gun.fire = values[4]; // ���
    printf("Converted : [ %lf , %lf , %lf , %lf , %lf ]\r\n",values[0],values[1],values[2],values[3],values[4]);
}

////////// ������ /////////////
int main(void)
{
    USART1_Debug_Init(BAUD_LOW);
    ESP01S_Init(BAUD_LOW);

    // ��ʼ��ϵͳģ��
    motor = Motor_Init(1679, 0);
    gun = Gun_Init(839, 2);
    tcpserver = TcpServer_Init("STM32-Tank-Service", "88888888", "192.168.6.6", "255.255.255.0", 8080);
    TcpServer_Build(&tcpserver);

    // ��ʼ������ʱ
    RunTime_Init();

    // 60FPS - �����ȼ��߳�,ʵʱ��Ҫ���
    RunTime_MonoBehaviour(60, 2, Motor_Update, &motor, Gun_Update, &gun);
    // 25FPS - �����ȼ��߳�,ʵʱ��Ҫ���е�
    RunTime_MonoBehaviour(25, 0);
    // 1FPS - �����ȼ��߳�,ʵʱ��Ҫ���
    RunTime_MonoBehaviour(1, 0);

    // ��������ʱ
    RunTime_Run();

    return 0;
}
