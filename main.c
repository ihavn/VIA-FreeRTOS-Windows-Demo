#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "message_buffer.h"

/* Priorities at which the tasks are created. */
#define TASK_MY_TASK_PRIORITY			( tskIDLE_PRIORITY + 1 )
#define	TASK_MY_SECOND_TASK_PRIORITY	( tskIDLE_PRIORITY + 2 )
/* Task stack sizes*/
#define TASK_MY_TASK_STACK				( configMINIMAL_STACK_SIZE )
#define	TASK_MY_SECOND_TASK_STACK		( configMINIMAL_STACK_SIZE )

/* Task Handles */
TaskHandle_t _taskSecondHandle = NULL;

/* Message buffer handles */
MessageBufferHandle_t _messBuf_handle = NULL;
#define MESS_BUF_SIZE	(500)

// --------------------------------------------------------------------------------------
void taskMyTask(void* pvParameters)
{
	// Remove compiler warnings.
	(void)pvParameters;

	char txBuf[40];
	char* someText[] = { "Peter Pan", "Julia Roberts", "James Dean", "Frodo", "Gary Grant" };

	for (;;)
	{
		vTaskDelay(pdMS_TO_TICKS(200));
		for (int i = 0; i < 5; i++)
		{
			sprintf(txBuf, "Message no %d: %s", i, someText[i] );
			xMessageBufferSend(_messBuf_handle, txBuf, strlen(txBuf) + 1, portMAX_DELAY);
		}
	}
}

// --------------------------------------------------------------------------------------
void taskMySeccondTask(void* pvParameters)
{
	// Remove compiler warnings.
	(void)pvParameters;

	char rxBuf[40];
	int rxLen = 0;

	for (;;)
	{
		rxLen = xMessageBufferReceive(_messBuf_handle, rxBuf, 40, portMAX_DELAY);
		printf("Received %d bytes: %s\n", rxLen, rxBuf);
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

	_messBuf_handle = xMessageBufferCreate(MESS_BUF_SIZE);

	// Let the operating system take over :)
	vTaskStartScheduler();
}