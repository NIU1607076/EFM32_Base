#include "stdbool.h"
#include <stdio.h>
#include <stdlib.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "i2c.h"

typedef struct
{
  portTickType delay;
  int ledNo;
} TaskParams_t;


void WriteOrientation( void * pvParameters )
{
	extern QueueHandle_t xQueue2;

  	float north = 0.0;
  	float east = 90.0;
  	float south = 180.0;
  	float west = 270.0;

  	float heading = 0;
  	char orientation;
	while (true)
	{
		if( xQueueReceive( xQueue2, &(heading),( TickType_t ) portMAX_DELAY  ) == pdPASS )
		{			
			if(heading < north + 45 && heading > north - 45){
				orientation = 'N';
			}
			else if(heading < east + 45 && heading > east - 45){
				orientation = 'E';
			}
			else if(heading < south + 45 && heading > south - 45){
				orientation = 'S';
			}
			else if(heading < west + 45 && heading > west - 45){
				orientation = 'W';
			}else{
				orientation = 'X';
			}		
		}

		printf("ORIENTATION: %c \n", orientation);
	}
}
