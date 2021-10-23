#include "app_main.h"
static void data_print_task(void *arg)
{
	xSemaphoreTake(sync_print_task, portMAX_DELAY);
	while(1)
	{
		//printf("%d\n", (uint32_t)arg);
		printf("Temp: %f℃\tDuty: %f%%\n", Temp, pwm_duty);
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}