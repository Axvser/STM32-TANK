#include "system.h"
#include "FreeRTOS.h"
#include "task.h"
#include "stdarg.h"
#include "queue.h"

#ifndef __RUNTIME_H
#define __RUNTIME_H

#define MAX_TASK_GROUPS 10
#define MAX_TASKS_PER_GROUP 8

typedef void (*RunTimeTask)(void *);

typedef struct
{
    RunTimeTask task;
    char taskName[16];
} TaskDefinition;

typedef struct
{
    uint8_t fps;
    UBaseType_t priority;
    TickType_t periodTicks;
    TaskHandle_t taskHandle;
    void *context;
    uint32_t taskCount;
    TaskDefinition tasks[MAX_TASKS_PER_GROUP];
    TickType_t lastWakeTime;
    uint32_t frameCounter;
} TaskGroup;

typedef struct
{
    TaskGroup taskGroups[MAX_TASK_GROUPS];
    uint8_t groupCount;
    void *globalContext;
} RuntimeContext;

extern RuntimeContext g_runtimeContext;

/// @brief ��ʼ������ʱ
/// @param context ȫ��������
void RunTime_Init(void *context);

/// @brief �Թ̶�֡��ִ��������
/// @param fps ֡��
/// @param context ������������
/// @param taskCount ��������
/// @param ... �ɱ������ʽ <taskPointer>
void RunTime_MonoBehaviour(uint8_t fps, void *context, uint32_t taskCount, ...);

/// @brief ��ʼ����
void RunTime_Run(void);

/// @brief ����֡�ʶ�Ӧ�����ȼ�
/// @param fps ֡��
/// @return FreeRTOS���ȼ�
UBaseType_t CalculatePriorityFromFPS(uint8_t fps);

#endif
