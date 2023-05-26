#include "stdbool.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "i2c.h"
#include "math.h"

typedef struct
{
  portTickType delay;
  int ledNo;
} TaskParams_t;

typedef struct
{
	int16_t x;
	int16_t y;
} Coords_t;

void ReadMagnetometer( void * pvParameters )
{
	extern QueueHandle_t xQueue;
	TaskParams_t     * pData = (TaskParams_t*) pvParameters;
  	const portTickType delay = pData->delay;

	const uint8_t OUT_XH = 0x29;
	const uint8_t OUT_XL = 0x28;
	const uint8_t OUT_YH = 0x2B;
	const uint8_t OUT_YL = 0x2A;

	uint8_t xH;
	uint8_t xL;
	uint8_t yH;
	uint8_t yL;

	int16_t x;
	int16_t y;

	while (true)
	{
		if (I2C_ReadRegister(OUT_XH, &xH) && I2C_ReadRegister(OUT_XL, &xL) && I2C_ReadRegister(OUT_YH, &yH) && I2C_ReadRegister(OUT_YL, &yL))
		{
			x = (xH << 8) | xL;
			y = (yH << 8) | yL;

			Coords_t coordsToSend = { x, y };

			xQueueSend( xQueue, ( void * ) &coordsToSend, ( TickType_t ) 0 );

			vTaskDelay(delay);
		}
	}
}
