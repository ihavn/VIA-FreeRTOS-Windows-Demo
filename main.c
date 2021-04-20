#include <stdio.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

/* Priorities at which the tasks are created. */
#define TASK_A_PRIORITY		( tskIDLE_PRIORITY + 3 )
#define	TASK_B_PRIORITY		( tskIDLE_PRIORITY + 1 )
#define	TASK_C_PRIORITY		( tskIDLE_PRIORITY + 2 )
/* Task stack sizes*/
#define TASK_A_STACK		( configMINIMAL_STACK_SIZE )
#define	TASK_B_STACK		( configMINIMAL_STACK_SIZE )
#define	TASK_C_STACK		( configMINIMAL_STACK_SIZE )

/* Task Handles */
TaskHandle_t _taskBHandle = NULL;

/* Semaphors */
SemaphoreHandle_t _aSem = NULL;
SemaphoreHandle_t _bSem = NULL;
SemaphoreHandle_t _cSem = NULL;

// --------------------------------------------------------------------------------------
void taskATask(void* pvParameters)
{
	// Remove compiler warnings.
	(void)pvParameters;

	for (;;)
	{
		puts("R");
		xSemaphoreGive(_bSem);
		xSemaphoreTake(_aSem, portMAX_DELAY);
		puts("OK");

		vTaskSuspend(NULL);
	}
}

// --------------------------------------------------------------------------------------
void taskBTask(void* pvParameters)
{
	// Remove compiler warnings.
	(void)pvParameters;

	for (;;)
	{
		xSemaphoreTake(_bSem, portMAX_DELAY);
		puts("I");
		xSemaphoreGive(_cSem);
		xSemaphoreTake(_bSem, portMAX_DELAY);
		puts("OK");

		vTaskSuspend(NULL);
	}
}

// --------------------------------------------------------------------------------------
void taskCTask(void* pvParameters)
{
	// Remove compiler warnings.
	(void)pvParameters;

	for (;;)
	{
		xSemaphoreTake(_cSem, portMAX_DELAY);
		puts("O");
		puts("OK");
		xSemaphoreGive(_aSem);
		xSemaphoreGive(_bSem);

		vTaskSuspend(NULL);
	}
}
// --------------------------------------------------------------------------------------
void main(void)
{
	/* Create the task, not storing the handle. */
	xTaskCreate(
		taskATask,       /* Function that implements the task. */
		"Task A",          /* Text name for the task. */
		TASK_A_STACK,      /* Stack size in words, not bytes. */
		(void*)1,    /* Parameter passed into the task. */
		TASK_A_PRIORITY,/* Priority at which the task is created. */
		NULL);      /* Used to pass out the created task's handle. */

		/* Create the task, storing the handle. */
	xTaskCreate(
		taskBTask,       /* Function that implements the task. */
		"Task B",          /* Text name for the task. */
		TASK_B_STACK,      /* Stack size in words, not bytes. */
		(void*)2,    /* Parameter passed into the task. */
		TASK_B_PRIORITY,/* Priority at which the task is created. */
		&_taskBHandle);      /* Used to pass out the created task's handle. */

			/* Create the task, storing the handle. */
	xTaskCreate(
		taskCTask,       /* Function that implements the task. */
		"Task C",          /* Text name for the task. */
		TASK_C_STACK,      /* Stack size in words, not bytes. */
		(void*)3,    /* Parameter passed into the task. */
		TASK_C_PRIORITY,/* Priority at which the task is created. */
		NULL);      /* Used to pass out the created task's handle. */


	/* Create binary semaphors */
	_aSem = xSemaphoreCreateBinary(void);
	_bSem = xSemaphoreCreateBinary(void);
	_cSem = xSemaphoreCreateBinary(void);

	// Let the operating system take over :)
	vTaskStartScheduler();
}