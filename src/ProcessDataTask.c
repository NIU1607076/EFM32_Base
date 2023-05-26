#include "stdbool.h"
#include <stdio.h>
#include <stdlib.h>

#include "FreeRTOS.h"
#include "queue.h"
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

float heading = 0;
float mag_scale = 0.00014;
float pi = 3.1415;

void ProcessData( void * pvParameters )
{
  	Coords_t coords;
	int16_t x;
	int16_t y;
	extern QueueHandle_t xQueue;
	extern QueueHandle_t xQueue2;

	while(true){
		if( xQueueReceive( xQueue, &(coords),( TickType_t ) portMAX_DELAY ) == pdPASS )
		{
			x = coords.x;
			y = coords.y;
		}else{
			x = 0;
			y = 0;
		}

		heading = (float)((atan2((float)y*mag_scale, (float)x*mag_scale) * 180) / pi);
		if (heading < 0)
		{
			heading = 360 + heading;
		}

		printf("HEADING: %.2f \n", heading);
		xQueueSend( xQueue2, ( void * ) &heading, ( TickType_t ) 0 );
	}
}
