#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "croutine.h"

#include "em_chip.h"
#include "bsp.h"
#include "bsp_trace.h"

#include "sleep.h"

#include "i2c.h"
#include "ReadingTask.h"
#include "ProcessDataTask.h"
#include "WritingTask.h"

typedef struct
{
  portTickType delay;
  int ledNo;
} TaskParams_t;

typedef struct
{
	uint16_t x;
	uint16_t y;
} Coords_t;

#define STACK_SIZE_FOR_TASK (configMINIMAL_STACK_SIZE + 10)
#define TASK_PRIORITY (tskIDLE_PRIORITY + 1)
#define QUEUE_LENGTH    10
#define ITEM_SIZE       sizeof( Coords_t )

QueueHandle_t xQueue;
QueueHandle_t xQueue2;

int _write(int file, const char *ptr, int len)
{
  int x;
  for (x = 0; x < len; x++)
  {
    ITM_SendChar(*ptr++);
  }
  return (len);
}

int main(void)
{
  CHIP_Init();

  BSP_TraceProfilerSetup();

  BSP_I2C_Init(0x3C);
  int result = I2C_Test();
  if (!result)
  {
    printf("Who I Am INCORRECT/WRONG!\n");
    return -1;
  }

  if (!I2C_WriteRegister(0x20, 0x30))
  {
    return -1;
  }

  if (!I2C_WriteRegister(0x21, 0x00))
  {
    return -1;
  }

  if (!I2C_WriteRegister(0x22, 0x00))
  {
    return -1;
  }

  xQueue = xQueueCreate( QUEUE_LENGTH,
                               ITEM_SIZE);

  /* pxQueueBuffer was not NULL so xQueue should not be NULL. */
  configASSERT( xQueue );

  xQueue2 = xQueueCreate( QUEUE_LENGTH,
                               sizeof(float));

  configASSERT( xQueue2 );

  BaseType_t xReturnedReader;
  TaskHandle_t xHandleReader = NULL;
  static TaskParams_t parametersToTask1 = { pdMS_TO_TICKS(200), 0 };

  /* Create the task, storing the handle. */
  xReturnedReader = xTaskCreate(
		  	  	  ReadMagnetometer,       /* Function that implements the task. */
				  (const char*) "ReadMagnetometer",          /* Text name for the task. */
				  STACK_SIZE_FOR_TASK,      /* Stack size in words, not bytes. */
				  &parametersToTask1,    /* Parameter passed into the task. */
				  tskIDLE_PRIORITY,/* Priority at which the task is created. */
				  &xHandleReader );      /* Used to pass out the created task's handle. */

  BaseType_t xReturnedProcessor;
  TaskHandle_t xHandleProcessor = NULL;
  xReturnedProcessor = xTaskCreate(
		  	  	  ProcessData,       
				  (const char*) "ProcessData",          
				  STACK_SIZE_FOR_TASK,      
				  &parametersToTask1,    
				  tskIDLE_PRIORITY,
				  &xHandleProcessor );      

  BaseType_t xReturnedWriter;
  TaskHandle_t xHandleWriter = NULL;
  xReturnedWriter = xTaskCreate(
				  WriteOrientation,
				  (const char*) "WriteOrientation",
				  STACK_SIZE_FOR_TASK,      
				  &parametersToTask1,    
				  tskIDLE_PRIORITY,
				  &xHandleWriter );   

  if( xReturnedReader == pdPASS && xReturnedProcessor == pdPASS && xReturnedWriter == pdPASS )
  {
	  vTaskStartScheduler();
  }else{
	  /* The task was created.  Use the task's handle to delete the task. */
	  vTaskDelete( xHandleReader );
	  vTaskDelete( xHandleProcessor );
	  vTaskDelete( xReturnedWriter );
  }


  return 0;
}
