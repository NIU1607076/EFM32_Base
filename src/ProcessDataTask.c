#include "stdbool.h"
#include "FreeRTOS.h"
#include "queue.h"

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

float heading = 0;
float mag_scale = 0.00014;
float pi = 3.1415;

void ProcessData( void * pvParameters )
{
	TaskParams_t     * pData = (TaskParams_t*) pvParameters;
  	const portTickType delay = pData->delay;
  	Coords_t coords;
	uint16_t x;
	uint16_t y;
	extern QueueHandle_t xQueue;

	while(true){
		if( xQueueReceive( xQueue, &(coords),( TickType_t ) 10 ) == pdPASS )
		{
			x = coords.x;
			y = coords.y;
		}else{
			x = 0;
			y = 0;
		}

		heading = 180.0 / (pi * atan2(((float)x * mag_scale), ((float)y * mag_scale)));
		if (heading < 0)
		{
			heading = 360 + heading;
		}

		printf("HEADING: %.2f \n", heading); //TODO: Put the heading value in a queue

		xTaskDelay(delay);
	}
}
