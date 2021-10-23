#include "app_main.h"
#include "temp_adc.c"
#include "data_print.c"
#include "pwm_fan.c"

void app_main(void)
{
	//Allow other core to finish initialization
	vTaskDelay(pdMS_TO_TICKS(100));
	
	temp_init();

	sync_temp_task = xSemaphoreCreateBinary();
	sync_calcu_task = xSemaphoreCreateBinary();
	sync_print_task = xSemaphoreCreateBinary();
	sync_adc_task = xSemaphoreCreateBinary();


	
	xTaskCreatePinnedToCore(get_adc_data_task, "adc", 4096, NULL, 2, NULL, 0);
	
	xTaskCreatePinnedToCore(get_temp_task, "temp", 4096, NULL, 2, NULL, tskNO_AFFINITY);

	xTaskCreatePinnedToCore(duty_calcu_task, "calcu", 4096, (void*)5, 2, NULL, 1);
	
	xTaskCreatePinnedToCore(data_print_task, "print", 4096, (void*)5, 2, NULL, 1);

	xSemaphoreGive(sync_adc_task);
	xSemaphoreGive(sync_temp_task);
	xSemaphoreGive(sync_print_task);
	xSemaphoreGive(sync_calcu_task);
	
	/***
	TaskHandle_t xHandle = NULL;
	// Use the handle to delete the task.
	if( xHandle != NULL )
	{
		vTaskDelete( xHandle );
	}
	***/
}
