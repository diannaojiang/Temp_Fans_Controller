#include "app_main.h"
static void data_print_task(void *arg)
{
	xSemaphoreTake(sync_print_task, portMAX_DELAY);
	while(1)
	{
		//printf("%d\n", (uint32_t)arg);
		printf("v0: %d\tv6: %d\tVerf_5V: %fmV\tTemp: %f℃\n", voltage_0, voltage_6, Verf_5V, Temp);
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}