#include "runtime.h"
#include "string.h"
#include "stdio.h"

RuntimeContext g_runtimeContext;

void RunTime_Init(void *context)
{
    memset(&g_runtimeContext, 0, sizeof(RuntimeContext));
    g_runtimeContext.globalContext = context;
    g_runtimeContext.groupCount = 0;
    printf("Runtime System Initialized\n");
}

UBaseType_t CalculatePriorityFromFPS(uint8_t fps)
{
    UBaseType_t maxPriority = configMAX_PRIORITIES - 1;
    UBaseType_t calculatedPriority = (fps * maxPriority) / 60;

    if (calculatedPriority < 1)
        calculatedPriority = 1;
    if (calculatedPriority > maxPriority)
        calculatedPriority = maxPriority;

    printf("FPS: %d -> Priority: %lu\n", fps, calculatedPriority);
    return calculatedPriority;
}

void TaskGroup_Executor(void *pvParameters)
{
    TaskGroup *group = (TaskGroup *)pvParameters;
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xPeriod = group->periodTicks;

    printf("Task Group Started: FPS=%d, Period=%lu ticks\n",
           group->fps, group->periodTicks);

    for (;;)
    {
        // 执行所有任务
        for (uint32_t i = 0; i < group->taskCount; i++)
        {
            if (group->tasks[i].task)
            {
                group->tasks[i].task(group->context);
            }
        }

        group->frameCounter++;

        // 使用vTaskDelayUntil确保精确的周期执行
        vTaskDelayUntil(&xLastWakeTime, xPeriod);
    }
}

void RunTime_MonoBehaviour(uint8_t fps, void *context, uint32_t taskCount, ...)
{
    if (g_runtimeContext.groupCount >= MAX_TASK_GROUPS)
    {
        printf("Error: Maximum task groups exceeded!\n");
        return;
    }

    if (taskCount > MAX_TASKS_PER_GROUP)
    {
        printf("Error: Too many tasks for group!\n");
        return;
    }

    TaskGroup *group = &g_runtimeContext.taskGroups[g_runtimeContext.groupCount];
    group->fps = fps;
    group->priority = CalculatePriorityFromFPS(fps);
    group->periodTicks = pdMS_TO_TICKS(1000 / fps); // 转换为FreeRTOS ticks
    group->context = context;
    group->taskCount = taskCount;
    group->frameCounter = 0;

    // 处理可变参数
    va_list args;
    va_start(args, taskCount);

    for (uint32_t i = 0; i < taskCount; i++)
    {
        RunTimeTask task = va_arg(args, RunTimeTask);
        group->tasks[i].task = task;
        snprintf(group->tasks[i].taskName, sizeof(group->tasks[i].taskName),
                 "Task_%d_%lu", fps, i);
    }

    va_end(args);

    printf("Registered Task Group: FPS=%d, Tasks=%lu\n", fps, taskCount);
    g_runtimeContext.groupCount++;
}

void RunTime_Run(void)
{
    printf("Starting Runtime System with %d task groups...\n", g_runtimeContext.groupCount);

    // 为每个任务组创建FreeRTOS任务
    for (uint8_t i = 0; i < g_runtimeContext.groupCount; i++)
    {
        TaskGroup *group = &g_runtimeContext.taskGroups[i];
        char name[32];
        sprintf(name,"RT_Item%d",i);

        BaseType_t result = xTaskCreate(
            TaskGroup_Executor,
            name,
            configMINIMAL_STACK_SIZE * 4,
            (void *)group,
            group->priority,
            &group->taskHandle);

        if (result != pdPASS)
        {
            printf("Failed to create task for FPS=%d\n", group->fps);
        }
        else
        {
            printf("Created task: FPS=%d, Priority=%lu\n", group->fps, group->priority);
        }
    }

    printf("Starting FreeRTOS Scheduler...\n");
    vTaskStartScheduler();

    printf("ERROR: FreeRTOS scheduler failed to start!\n");
    while (1)
        ;
}
