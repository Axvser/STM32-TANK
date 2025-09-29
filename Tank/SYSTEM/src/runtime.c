#include "runtime.h"
#include "string.h"

static RuntimeContext g_runtimeContext;

void RunTime_Init(void)
{
    memset(&g_runtimeContext, 0, sizeof(RuntimeContext));
    g_runtimeContext.groupCount = 0;
}

UBaseType_t CalculatePriorityFromFPS(uint8_t fps)
{
    UBaseType_t maxPriority = configMAX_PRIORITIES - 1;
    UBaseType_t calculatedPriority = (fps * maxPriority) / 60;

    if (calculatedPriority < 1)
        calculatedPriority = 1;
    if (calculatedPriority > maxPriority)
        calculatedPriority = maxPriority;

    return calculatedPriority;
}

static void TaskGroup_Executor(void *pvParameters)
{
    TaskGroup *group = (TaskGroup *)pvParameters;
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xPeriod = group->periodTicks;

    for (;;)
    {
        for (uint32_t i = 0; i < group->taskCount; i++)
        {
            if (group->tasks[i].task)
            {
                group->tasks[i].task(group->tasks[i].context);
            }
        }

        group->frameCounter++;
        vTaskDelayUntil(&xLastWakeTime, xPeriod);
    }
}

void RunTime_MonoBehaviour(uint8_t fps, uint32_t taskCount, ...)
{
    if (g_runtimeContext.groupCount >= MAX_TASK_GROUPS)
        return;

    if (taskCount > MAX_TASKS_PER_GROUP)
        return;

    TaskGroup *group = &g_runtimeContext.taskGroups[g_runtimeContext.groupCount];
    group->fps = fps;
    group->priority = CalculatePriorityFromFPS(fps);
    group->periodTicks = pdMS_TO_TICKS(1000 / fps);
    group->taskCount = taskCount;
    group->frameCounter = 0;

    va_list args;
    va_start(args, taskCount);

    for (uint32_t i = 0; i < taskCount; i++)
    {
        RunTimeTask task = va_arg(args, RunTimeTask);
        void *context = va_arg(args, void *);

        group->tasks[i].task = task;
        group->tasks[i].context = context;
    }

    va_end(args);
    g_runtimeContext.groupCount++;
}

void RunTime_Run(void)
{
    for (uint8_t i = 0; i < g_runtimeContext.groupCount; i++)
    {
        TaskGroup *group = &g_runtimeContext.taskGroups[i];
        char name[16];

        snprintf(name, sizeof(name), "RT_Grp%d", i);

        xTaskCreate(
            TaskGroup_Executor,
            name,
            configMINIMAL_STACK_SIZE * 4,
            (void *)group,
            group->priority,
            &group->taskHandle);
    }

    vTaskStartScheduler();
    while (1)
        ;
}
