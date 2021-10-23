#include "pwm_fan.h"
static void duty_calcu_task(void *arg)
{
	xSemaphoreTake(sync_calcu_task, portMAX_DELAY);
	
	//Continuously sample ADC1
	while (1) {
		static float X,A,B,C,D;
		X = Temp;
		A = 125.009487154138;
		B = -13.6199522004459;
		C = 27.3023955125344;
		D = -11.0410570365692;
		if (X<23.0)
		{
			pwm_duty = 0.0;
		}else if(X>30)
		{
			pwm_duty = 100.0;
		}else
		{
			pwm_duty = (A - D) / (1.0 + pow(X/C,B)) + D;
		}
		
		vTaskDelay(pdMS_TO_TICKS(1000));

	}
}