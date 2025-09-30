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

/// @brief 初始化运行时
void RunTime_Init(void);

/// @brief 仅在运行时启动执行一次的任务
/// @param taskCount 任务总数
/// @param ... 可变参数格式 <taskPointer>,<contextPointer>
void RunTime_StartWith(uint32_t taskCount, ...);

/// @brief 以固定帧率执行任务组
/// @param fps 帧率
/// @param taskCount 任务总数
/// @param ... 可变参数格式 <taskPointer>,<contextPointer>
void RunTime_MonoBehaviour(uint8_t fps, uint32_t taskCount, ...);

/// @brief 开始运行
void RunTime_Run(void);

/// @brief 计算帧率对应的优先级
/// @param fps 帧率
/// @return FreeRTOS优先级
UBaseType_t CalculatePriorityFromFPS(uint8_t fps);

#endif
