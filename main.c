#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"

/* Priorities at which the tasks are created. */
#define TASK_MY_TASK_PRIORITY			( tskIDLE_PRIORITY + 1 )
#define	TASK_MY_SECOND_TASK_PRIORITY	( tskIDLE_PRIORITY + 2 )
#define	TASK_MY_THIRD_TASK_PRIORITY		( tskIDLE_PRIORITY + 3 )
/* Task stack sizes*/
#define TASK_MY_TASK_STACK				( configMINIMAL_STACK_SIZE )
#define	TASK_MY_SECOND_TASK_STACK		( configMINIMAL_STACK_SIZE )
#define	TASK_MY_THIRD_TASK_STACK		( configMINIMAL_STACK_SIZE )

/* Queue handles */
QueueHandle_t intQueue = NULL;

xSemaphoreHandle sem = NULL;

// --------------------------------------------------------------------------------------
void taskMyTask(void* pvParameters)
{
	// Remove compiler warnings.
	(void)pvParameters;

	int recValue;

	for (;;)
	{
			xQueueReceive(intQueue, &recValue, portMAX_DELAY);
			xSemaphoreTake(sem, portMAX_DELAY);
			printf("Received: %d\n", recValue);
			xSemaphoreGive(sem);
	}
}

// --------------------------------------------------------------------------------------
void taskMySeccondTask(void* pvParameters)
{
	// Remove compiler warnings.
	(void)pvParameters;

	int counter = 0;

	for (;;)
	{
		for (int i = 0; i < 5; i++)
		{
			xSemaphoreTake(sem, portMAX_DELAY);
			puts("Send From Second");
			xQueueSendToBack(intQueue, &counter, portMAX_DELAY);
			xSemaphoreGive(sem);
			counter++;
		}
		vTaskDelay(pdMS_TO_TICKS(200));
	}
}

// --------------------------------------------------------------------------------------
void taskMyThirdTask(void* pvParameters)
{
	// Remove compiler warnings.
	(void)pvParameters;

	int counter = 1000;

	for (;;)
	{
		for (int i = 0; i < 2; i++)
		{
			xSemaphoreTake(sem, portMAX_DELAY);
			puts("Send From Third");
			xQueueSendToBack(intQueue, &counter, portMAX_DELAY);
			xSemaphoreGive(sem);
			counter++;
		}
		vTaskDelay(pdMS_TO_TICKS(300));
	}
}


// --------------------------------------------------------------------------------------
void main(void)
{
	/* Create the task, not storing the handle. */
	xTaskCreate(
		taskMyTask,       /* Function that implements the task. */
		"MyTask",          /* Text name for the task. */
		TASK_MY_TASK_STACK,      /* Stack size in words, not bytes. */
		(void*)1,    /* Parameter passed into the task. */
		TASK_MY_TASK_PRIORITY,/* Priority at which the task is created. */
		NULL);      /* Used to pass out the created task's handle. */

		/* Create the task, storing the handle. */
	xTaskCreate(
		taskMySeccondTask,       /* Function that implements the task. */
		"MySecondTask",          /* Text name for the task. */
		TASK_MY_SECOND_TASK_STACK,      /* Stack size in words, not bytes. */
		(void*)2,    /* Parameter passed into the task. */
		TASK_MY_SECOND_TASK_PRIORITY,/* Priority at which the task is created. */
		NULL);      /* Used to pass out the created task's handle. */

	/* Create the task, not storing the handle. */
	xTaskCreate(
		taskMyThirdTask,       /* Function that implements the task. */
		"MyThirdTask",          /* Text name for the task. */
		TASK_MY_THIRD_TASK_STACK,      /* Stack size in words, not bytes. */
		(void*)3,    /* Parameter passed into the task. */
		TASK_MY_THIRD_TASK_PRIORITY, /* Priority at which the task is created. */
		NULL);      /* Used to pass out the created task's handle. */

	// Create Queue that can hold 10 integers
	intQueue = xQueueCreate(10,	sizeof(int));

	sem = xSemaphoreCreateBinary();
	xSemaphoreGive(sem);

	// Let the operating system take over :)
	vTaskStartScheduler();
}