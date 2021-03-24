#include <stdio.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

/* Priorities at which the tasks are created. */
#define TASK_A_PRIORITY			( tskIDLE_PRIORITY + 1 )
#define	TASK_B_PRIORITY			( tskIDLE_PRIORITY + 2 )
/* Task stack sizes*/
#define TASK_MY_TASK_STACK				( configMINIMAL_STACK_SIZE )
#define	TASK_MY_SECOND_TASK_STACK		( configMINIMAL_STACK_SIZE )

/* Task Handles */
TaskHandle_t _taskSecondHandle = NULL;

SemaphoreHandle_t  syncSemaphore;

void opA() {
	puts("OpA");
}

void opB() {
	puts("OpB");
}

// --------------------------------------------------------------------------------------
void taskA(void* pvParameters)
{
	// Remove compiler warnings.
	(void)pvParameters;

	for (;;)
	{
		xSemaphoreTake(syncSemaphore, portMAX_DELAY);
		opA();
		vTaskDelay(1000);
	}
}

// --------------------------------------------------------------------------------------
void taskB(void* pvParameters)
{
	// Remove compiler warnings.
	(void)pvParameters;

	for (;;)
	{
		opB();
		xSemaphoreGive(syncSemaphore);
		vTaskDelay(1000);
	}
}

// --------------------------------------------------------------------------------------
void main(void)
{
	/* Create the task, not storing the handle. */
	xTaskCreate(
		taskA,       /* Function that implements the task. */
		"MyTask",          /* Text name for the task. */
		TASK_MY_TASK_STACK,      /* Stack size in words, not bytes. */
		(void*)1,    /* Parameter passed into the task. */
		TASK_A_PRIORITY,/* Priority at which the task is created. */
		NULL);      /* Used to pass out the created task's handle. */

		/* Create the task, storing the handle. */
	xTaskCreate(
		taskB,       /* Function that implements the task. */
		"MySecondTask",          /* Text name for the task. */
		TASK_MY_SECOND_TASK_STACK,      /* Stack size in words, not bytes. */
		(void*)2,    /* Parameter passed into the task. */
		TASK_B_PRIORITY,/* Priority at which the task is created. */
		&_taskSecondHandle);      /* Used to pass out the created task's handle. */

	syncSemaphore = xSemaphoreCreateBinary();

	// Let the operating system take over :)
	vTaskStartScheduler();
}