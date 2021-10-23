#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/gpio.h"
#include <math.h>


static SemaphoreHandle_t sync_temp_task;
static SemaphoreHandle_t sync_calcu_task;
static SemaphoreHandle_t sync_print_task;
static SemaphoreHandle_t sync_adc_task;

static float Temp;


static float pwm_duty;