/***************************************************************************//**
 * @file
 * @brief FreeRTOS Blink Demo for Energy Micro EFM32GG_STK3700 Starter Kit
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>

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

#define STACK_SIZE_FOR_TASK    (configMINIMAL_STACK_SIZE + 10)
#define TASK_PRIORITY          (tskIDLE_PRIORITY + 1)

/* Structure with parameters for LedBlink */
typedef struct {
  /* Delay between blink of led */
  portTickType delay;
  /* Number of led */
  int          ledNo;
} TaskParams_t;

/***************************************************************************//**
 * @brief Simple task which is blinking led
 * @param *pParameters pointer to parameters passed to the function
 ******************************************************************************/
static void LedBlink(void *pParameters)
{
  TaskParams_t     * pData = (TaskParams_t*) pParameters;
  const portTickType delay = pData->delay;

  for (;; ) {
    BSP_LedToggle(pData->ledNo);
    vTaskDelay(delay);
  }
}
int _write(int file, const char *ptr, int len){
	int x;
	for(x = 0; x < len; x++){
		ITM_SendChar(*ptr++);
	}
	return (len);
}

void BSP_I2C_Init(uint8_t addr);
bool I2C_Test();
bool I2C_ReadRegister(uint8_t reg, uint8_t *val);

/***************************************************************************//**
 * @brief  Main function
 ******************************************************************************/
int main(void)
{
  /*Initializing the semaphore*/
  /* Chip errata */
  CHIP_Init();

  /* If first word of user data page is non-zero, enable Energy Profiler trace */
  BSP_TraceProfilerSetup();

  BSP_I2C_Init(0x3C);
  int result = I2C_Test();
  if(!result){
	  printf("Who I Am INCORRECT/WRONG!\n");
	  return -1;
  }
  const uint8_t OUT_XH = 0x28;
  const uint8_t OUT_XL = 0x29;
  const uint8_t OUT_YH = 0x2A;
  const uint8_t OUT_YL = 0x2B;
  const uint8_t OUT_ZH = 0x2C;
  const uint8_t OUT_ZL = 0x2D;

  uint8_t xH;
  uint8_t xL;
  uint8_t yH;
  uint8_t yL;
  uint8_t zH;
  uint8_t zL;

  while(true){
    if (I2C_ReadRegister(OUT_XH, &xH) && I2C_ReadRegister(OUT_XL, &xL) 
        && I2C_ReadRegister(OUT_YH, &yH) && I2C_ReadRegister(OUT_YL, &yL) 
        && I2C_ReadRegister(OUT_ZH, &zH) && I2C_ReadRegister(OUT_ZL, &zL)){
      printf("xH: %d -- ", xH);
      printf("xL: %d -- ", xL);
      printf("yH: %d -- ", yH);
      printf("yL: %d -- ", yL);
      printf("zH: %d -- ", zH);
      printf("zL: %d\n", zL);
    }
  }

  /* Initialize LED driver */
  BSP_LedsInit();
  /* Setting state of leds*/
  BSP_LedSet(0);
  BSP_LedSet(1);

  /* Initialize SLEEP driver, no calbacks are used */
  SLEEP_Init(NULL, NULL);
#if (configSLEEP_MODE < 3)
  /* do not let to sleep deeper than define */
  SLEEP_SleepBlockBegin((SLEEP_EnergyMode_t)(configSLEEP_MODE + 1));
#endif

  /* Parameters value for taks*/
  static TaskParams_t parametersToTask1 = { pdMS_TO_TICKS(1000), 0 };
  static TaskParams_t parametersToTask2 = { pdMS_TO_TICKS(500), 1 };

  /*Create two task for blinking leds*/
  xTaskCreate(LedBlink, (const char *) "LedBlink1", STACK_SIZE_FOR_TASK, &parametersToTask1, TASK_PRIORITY, NULL);
  xTaskCreate(LedBlink, (const char *) "LedBlink2", STACK_SIZE_FOR_TASK, &parametersToTask2, TASK_PRIORITY, NULL);

  /*Start FreeRTOS Scheduler*/
  vTaskStartScheduler();

  return 0;
}
