#include <stdio.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

/* Priorities at which the tasks are created. */
#define TASK_MY_TASK_PRIORITY			( tskIDLE_PRIORITY + 1 )
#define	TASK_MY_SECOND_TASK_PRIORITY	( tskIDLE_PRIORITY + 2 )
/* Task stack sizes*/
#define TASK_MY_TASK_STACK				( configMINIMAL_STACK_SIZE )
#define	TASK_MY_SECOND_TASK_STACK		( configMINIMAL_STACK_SIZE )

/* Task Handles */
TaskHandle_t _taskSecondHandle = NULL;

/* Semaphors */
SemaphoreHandle_t _putsSem = NULL;

// --------------------------------------------------------------------------------------
void taskMyTask(void* pvParameters)
{
	// Remove compiler warnings.
	(void)pvParameters;

	for (;;)
	{
		if (xSemaphoreTake(_putsSem, portMAX_DELAY)) {
			puts("Hi from My Task");
			xSemaphoreGive(_putsSem);
		}
		vTaskDelay(pdMS_TO_TICKS(200));
	}
}

// --------------------------------------------------------------------------------------
void taskMySeccondTask(void* pvParameters)
{
	// Remove compiler warnings.
	(void)pvParameters;

	for (;;)
	{
		if (xSemaphoreTake(_putsSem, portMAX_DELAY)) {
			puts("Hi from My Second Task");
			xSemaphoreGive(_putsSem);
		}
		vTaskDelay(pdMS_TO_TICKS(1000));
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
		&_taskSecondHandle);      /* Used to pass out the created task's handle. */


	/* Create a binary semaphor and give it */
	_putsSem = xSemaphoreCreateBinary();
	xSemaphoreGive(_putsSem);

	// Let the operating system take over :)
	vTaskStartScheduler();
}