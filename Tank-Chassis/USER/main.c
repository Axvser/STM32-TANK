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
#include "tcpserver.h"
#include "converter.h"

///////// 全局上下文 //////////
MotorContext motor;
GunContext gun;
TcpServerContext tcpserver;

////////// 处理上位机数据 /////////////
void ESP01S_DataHandler(uint16_t data)
{
    ParseTankData(data);
}
void ProcessTankValues(double values[], uint8_t count)
{
    motor.vl = values[0]; // 左侧履带速度
    motor.vr = values[1]; // 右侧履带速度
    gun.ah = values[2];   // 水平舵机角度
    gun.av = values[3];   // 垂直舵机角度
    gun.fire = values[4]; // 火控
    gun.changed = 1;
#if DEBUG_CONVERTER
    printf("Updated [ vl:%lf , vr:%lf , ah:%lf , av:%lf , fire:%lf ]\r\n", values[0], values[1], values[2], values[3], values[4]);
#endif
}

////////// 主函数 /////////////
int main(void)
{
    USART1_Debug_Init(BAUD_LOW);
    ESP01S_Init(BAUD_LOW);

    // 初始化系统模块
    motor = Motor_Init(1679, 0);
    gun = Gun_Init(1999, 839);
    tcpserver = TcpServer_Init("Axvser", "888888881", "192.168.6.6", "192.168.6.1", "255.255.255.0", 8080);
    TcpServer_Build_STA(&tcpserver);

    // 初始化运行时
    RunTime_Init();

    // 60FPS - 实时性要求高
    RunTime_MonoBehaviour(60, 2, Motor_Update, &motor, Gun_Update, &gun);
    // 20FPS - 实时性要求中等
    RunTime_MonoBehaviour(20, 0);
    // 1FPS - 实时性要求低
    RunTime_MonoBehaviour(1, 0);

    // 启动运行时
    RunTime_Run();

    return 0;
}
