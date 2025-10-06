#include "stm32f10x.h"
#include "bitband.h"
#include "system.h"
#include "delay.h"
#include "runtime.h"

///////// 业务封装 //////////
#include "gun.h"
#include "tcpserver.h"
#include "converter.h"

///////// 全局上下文 //////////
GunContext gun;
TcpServerContext tcpserver;

////////// 处理上位机数据 /////////////
void ESP01S_DataHandler(uint16_t data)
{
    ParseTankData(data);
}
void ProcessTankValues(double values[], uint8_t count)
{
    gun.av = values[3];   // 垂直舵机角度
    gun.fire = values[4]; // 电子开关启停
#if DEBUG_CONVERTER
    printf("Updated [ av:%lf , fire:%lf ]\r\n", values[3], values[4]);
#endif
}

int main(void)
{
    USART1_Debug_Init(BAUD_LOW);
    ESP01S_Init(BAUD_LOW);

    // 炮台模块初始化
    gun = Gun_Init(19999, 71);
    tcpserver = TcpServer_Init("Axvser", "888888881", "192.168.8.8", "192.168.8.1", "255.255.255.0", 8080);
    TcpServer_Build_STA(&tcpserver);

    // 初始化运行时
    RunTime_Init();

    // 60FPS - 实时性要求高
    RunTime_MonoBehaviour(60, 1, Gun_Update, &gun);
    // 20FPS - 实时性要求中等
    RunTime_MonoBehaviour(20, 0);
    // 1FPS - 实时性要求低
    RunTime_MonoBehaviour(1, 0);

    // 启动运行时
    RunTime_Run();

    while (1)
        ;
}
