#include <stdio.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>

/* Priorities at which the tasks are created. */
#define TASK_A_TASK_PRIORITY			( tskIDLE_PRIORITY + 1 )
#define	TASK_B_TASK_PRIORITY			( tskIDLE_PRIORITY + 2 )
/* Task stack sizes*/
#define TASK_A_TASK_STACK				( configMINIMAL_STACK_SIZE )
#define	TASK_B_TASK_STACK				( configMINIMAL_STACK_SIZE )

/* Queue handles */
static QueueHandle_t _intQueue = NULL;

/* printf Mutex */
static SemaphoreHandle_t _printfMutex = NULL;

// --------------------------------------------------------------------------------------
void taskBTask(void* pvParameters)
{
	// Remove compiler warnings.
	(void)pvParameters;

	int recValue;

	for (;;)
	{
			xQueueReceive(_intQueue, &recValue, portMAX_DELAY);

			xSemaphoreTake(_printfMutex, portMAX_DELAY);
			printf("Received Task B: %d\n", recValue);
			xSemaphoreGive(_printfMutex);
	}
}

// --------------------------------------------------------------------------------------
void taskATask(void* pvParameters)
{
	// Remove compiler warnings.
	(void)pvParameters;

	int counter = 0;

	for (;;)
	{
		for (int i = 0; i < 5; i++)
		{
			xSemaphoreTake(_printfMutex, portMAX_DELAY);
			puts("Send From Task A");
			xQueueSendToBack(_intQueue, &counter, portMAX_DELAY);
			xSemaphoreGive(_printfMutex);
			counter++;
		}
		vTaskDelay(pdMS_TO_TICKS(200));
	}
}


// --------------------------------------------------------------------------------------
void main(void)
{
	/* Create the task, not storing the handle. */
	xTaskCreate(
		taskATask,       /* Function that implements the task. */
		"Task A",          /* Text name for the task. */
		TASK_A_TASK_STACK,      /* Stack size in words, not bytes. */
		(void*)1,    /* Parameter passed into the task. */
		TASK_A_TASK_PRIORITY,/* Priority at which the task is created. */
		NULL);      /* Used to pass out the created task's handle. */

		/* Create the task, storing the handle. */
	xTaskCreate(
		taskBTask,       /* Function that implements the task. */
		"Task B",          /* Text name for the task. */
		TASK_B_TASK_STACK,      /* Stack size in words, not bytes. */
		(void*)2,    /* Parameter passed into the task. */
		TASK_B_TASK_PRIORITY,/* Priority at which the task is created. */
		NULL);      /* Used to pass out the created task's handle. */

	// Create Queue that can hold 10 integers
	_intQueue = xQueueCreate(10, sizeof(int));

	_printfMutex = xSemaphoreCreateMutex();

	// Let the operating system take over :)
	vTaskStartScheduler();
}