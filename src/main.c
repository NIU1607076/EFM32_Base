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
bool I2C_WriteRegister(uint8_t reg, uint8_t data);

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


  if(!I2C_WriteRegister(0x20, 0x30)){//01110000
	  return -1;
  }

  if(!I2C_WriteRegister(0x21, 0x00)){
  	  return -1;
  }

  if(!I2C_WriteRegister(0x22, 0x00)){
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

  uint16_t x;
  uint16_t y;
  uint16_t z;

  float heading = 0;
  float mag_scale = 0.00014;
  float pi = 3.1415;

  while(true){
    if (I2C_ReadRegister(OUT_XH, &xH) && I2C_ReadRegister(OUT_XL, &xL) 
        && I2C_ReadRegister(OUT_YH, &yH) && I2C_ReadRegister(OUT_YL, &yL) 
        && I2C_ReadRegister(OUT_ZH, &zH) && I2C_ReadRegister(OUT_ZL, &zL)){

    	x = (xH << 8) | xL;
    	y = (yH << 8) | yL;
    	z = (zH << 8) | zL;

	    // printf("x: %d <||> ", x);
	    // printf("y: %d <||> ", y);
	    // printf("z: %d <||> \n", z);

	    heading = 180.0 / (pi * atan2(((float)x*mag_scale), ((float)y*mag_scale)));

	    printf("HEADING: %.2f \n", heading);
    }
  }

  return 0;
}
