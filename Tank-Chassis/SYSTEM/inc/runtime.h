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
    void *context;
    char taskName[16];
} TaskDefinition;

typedef struct
{
    uint8_t fps;
    UBaseType_t priority;
    TickType_t periodTicks;
    TaskHandle_t taskHandle;
    uint32_t taskCount;
    TaskDefinition tasks[MAX_TASKS_PER_GROUP];
    TickType_t lastWakeTime;
    uint32_t frameCounter;
} TaskGroup;

typedef struct
{
    TaskGroup taskGroups[MAX_TASK_GROUPS];
    uint8_t groupCount;
} RuntimeContext;

/// @brief ��ʼ������ʱ
void RunTime_Init(void);

/// @brief ��������ʱ����ִ��һ�ε�����
/// @param taskCount ��������
/// @param ... �ɱ������ʽ <taskPointer>,<contextPointer>
void RunTime_StartWith(uint32_t taskCount, ...);

/// @brief �Թ̶�֡��ִ��������
/// @param fps ֡��
/// @param taskCount ��������
/// @param ... �ɱ������ʽ <taskPointer>,<contextPointer>
void RunTime_MonoBehaviour(uint8_t fps, uint32_t taskCount, ...);

/// @brief ��ʼ����
void RunTime_Run(void);

/// @brief ����֡�ʶ�Ӧ�����ȼ�
/// @param fps ֡��
/// @return FreeRTOS���ȼ�
UBaseType_t CalculatePriorityFromFPS(uint8_t fps);

#endif
