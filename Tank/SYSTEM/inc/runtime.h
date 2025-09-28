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

/// @brief 初始化运行时
/// @param context 全局上下文
void RunTime_Init(void *context);

/// @brief 以固定帧率执行任务组
/// @param fps 帧率
/// @param context 任务组上下文
/// @param taskCount 任务总数
/// @param ... 可变参数格式 <taskPointer>
void RunTime_MonoBehaviour(uint8_t fps, void *context, uint32_t taskCount, ...);

/// @brief 开始运行
void RunTime_Run(void);

/// @brief 计算帧率对应的优先级
/// @param fps 帧率
/// @return FreeRTOS优先级
UBaseType_t CalculatePriorityFromFPS(uint8_t fps);

#endif
