#include <stdio.h>
#include <FreeRTOS.h>
#include <task.h>
#include <event_groups.h>

/* Priorities at which the tasks are created. */
#define TASK_MY_TASK_PRIORITY			( tskIDLE_PRIORITY + 1 )
#define	TASK_MY_SECOND_TASK_PRIORITY	( tskIDLE_PRIORITY + 2 )
/* Task stack sizes*/
#define TASK_MY_TASK_STACK				( configMINIMAL_STACK_SIZE )
#define	TASK_MY_SECOND_TASK_STACK		( configMINIMAL_STACK_SIZE )

/* Task Handles */
TaskHandle_t _taskSecondHandle = NULL;

/* Event Groups */
EventGroupHandle_t _myEventGroup = NULL;
#define BIT_TASK_A_READY (1 << 0)
#define BIT_TASK_B_READY (1 << 1)

// --------------------------------------------------------------------------------------
void taskMyTask(void* pvParameters)
{
	// Remove compiler warnings.
	(void)pvParameters;

	for (;;)
	{
		puts("Hi from My Task");
		vTaskDelay(pdMS_TO_TICKS(75)); // Change to more than 100 ms to get time-out
		xEventGroupSetBits(_myEventGroup, BIT_TASK_A_READY);

		xEventGroupWaitBits(
			_myEventGroup, /* The event group being tested. */
			BIT_TASK_B_READY, /* The bits to wait for. */
			pdTRUE, /* Bits will be cleared before return*/
			pdTRUE, /* Wait for bits to be set */
			portMAX_DELAY); /* Maximum time to wait*/
	}
}

// --------------------------------------------------------------------------------------
void taskMySeccondTask(void* pvParameters)
{
	// Remove compiler warnings.
	(void)pvParameters;
	EventBits_t _bits = 0;

	for (;;)
	{
		_bits = xEventGroupWaitBits(
			_myEventGroup, /* The event group being tested */
			BIT_TASK_A_READY, /* The bits to wait for */
			pdTRUE, /* Bits will be cleared before return */
			pdTRUE, /* Wait for all bits to be set */
			pdMS_TO_TICKS(100)); /* Maximum time to wait */

		// Test bits if using timeout
		if (BIT_TASK_A_READY != _bits)
		{
			/* Timeout!!
			xEventGroupWaitBits() returned because more than 100 ms has passed*/
			puts("!!!! Task A takes too long !!!!");
		}
		else
		{
			puts("Hi from My Second Task");
			xEventGroupSetBits(_myEventGroup, BIT_TASK_B_READY);
		}
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

	/* Create Event Groups */
	_myEventGroup = xEventGroupCreate();

	// Let the operating system take over :)
	vTaskStartScheduler();
}