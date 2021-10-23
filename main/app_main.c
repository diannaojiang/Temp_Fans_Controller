#include "app_main.h"
#include "temp_adc.c"

void app_main(void)
{
    //Allow other core to finish initialization
    vTaskDelay(pdMS_TO_TICKS(100));
    
    temp_init();

    sync_temp_task = xSemaphoreCreateBinary();
    sync_calcu_task = xSemaphoreCreateBinary();
    sync_print_task = xSemaphoreCreateBinary();


    
    xTaskCreatePinnedToCore(get_temp_task, "stats", 4096, NULL, 2, NULL, tskNO_AFFINITY);
    xSemaphoreGive(sync_temp_task);
    
}
