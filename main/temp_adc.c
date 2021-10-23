/* ADC1 Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include "temp_adc.h"



static void check_efuse(void)
{
#if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S3
	//Check if TP is burned into eFuse
	if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_TP) == ESP_OK) {
		printf("eFuse Two Point: Supported\n");
	} else {
		printf("eFuse Two Point: NOT supported\n");
	}
	//Check Vref is burned into eFuse
	if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_VREF) == ESP_OK) {
		printf("eFuse Vref: Supported\n");
	} else {
		printf("eFuse Vref: NOT supported\n");
	}
#elif CONFIG_IDF_TARGET_ESP32S2
	if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_TP) == ESP_OK) {
		printf("eFuse Two Point: Supported\n");
	} else {
		printf("Cannot retrieve eFuse Two Point calibration values. Default calibration values will be used.\n");
	}
#else
#error "This example is configured for ESP32/ESP32S2."
#endif
}

const float Rp=10000.0; //10K
const float T2 = (273.15+25.0);;//T2
const float Bx = 3950.0;//B
const float Ka = 273.15;
float Get_Temp(uint32_t Rt)
{
	float temp;
	//like this R=5000, T2=273.15+25,B=3470, RT=5000*EXP(3470*(1/T1-1/(273.15+25)),  
	temp = Rt/Rp;
	temp = log(temp);//ln(Rt/Rp)
	temp/=Bx;//ln(Rt/Rp)/B
	temp+=(1/T2);
	temp = 1/(temp);
	temp-=Ka;
	return temp;
} 

static void print_char_val_type(esp_adc_cal_value_t val_type)
{
	if (val_type == ESP_ADC_CAL_VAL_EFUSE_TP) {
		printf("Characterized using Two Point Value\n");
	} else if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF) {
		printf("Characterized using eFuse Vref\n");
	} else {
		printf("Characterized using Default Vref\n");
	}
}

static void temp_init(void)
{
	
	//Check if Two Point or Vref are burned into eFuse
	check_efuse();

	//Configure ADC
	if (unit == ADC_UNIT_1) {
		adc1_config_width(width);
		adc1_config_channel_atten(ADC_CHANNEL_6, atten);
		adc1_config_channel_atten(ADC_CHANNEL_7, ADC_ATTEN_DB_6);
	} else {
		adc2_config_channel_atten((adc2_channel_t)ADC_CHANNEL_6, atten);
	}

    adc_chars_5v = calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_value_t val_type_5v = esp_adc_cal_characterize(unit, ADC_ATTEN_DB_6, width, DEFAULT_VREF, adc_chars_5v);

	//Characterize ADC
	adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
	esp_adc_cal_value_t val_type = esp_adc_cal_characterize(unit, atten, width, DEFAULT_VREF, adc_chars);
	print_char_val_type(val_type);

}


static uint32_t get_adc_data(adc_channel_t channel)
{
	uint32_t adc_reading = 0;
		//Multisampling
	for (int i = 0; i < NO_OF_SAMPLES; i++) {
		if (unit == ADC_UNIT_1) {
			adc_reading += adc1_get_raw((adc1_channel_t)channel);
		} else {
			int raw;
			adc2_get_raw((adc2_channel_t)channel, width, &raw);
			adc_reading += raw;
		}
	}
	adc_reading /= NO_OF_SAMPLES;
	//Convert adc_reading to voltage in mV
	static uint32_t voltage;
	if(channel == ADC_CHANNEL_7){
		voltage = esp_adc_cal_raw_to_voltage(adc_reading, adc_chars_5v);
	}else{
		voltage = esp_adc_cal_raw_to_voltage(adc_reading, adc_chars);
	}
	return voltage;
}
static void get_adc_data_task(void *arg)
{
	xSemaphoreTake(sync_adc_task, portMAX_DELAY);
	
	//Continuously sample ADC1
	while (1) {
		voltage_7 = get_adc_data(ADC_CHANNEL_7);
		for (uint32_t i = 0; i < 10; i++)
		{
			voltage_6 = get_adc_data(ADC_CHANNEL_6);
			vTaskDelay(pdMS_TO_TICKS(100));
		}
		

	}
}

static void get_temp_task(void *arg)
{
	xSemaphoreTake(sync_temp_task, portMAX_DELAY);
	//Continuously sample ADC1
	while (1) {
		Verf_5V = voltage_7*3.35;
		static uint32_t voltage;
		voltage = voltage_6;
		float Res = 20000.0/(Verf_5V-voltage*1.0)*voltage;
		Temp = Get_Temp(Res);
		//printf("Raw: %d\tVerf_5V: %fmV\tVoltage: %dmV\tRes: %fΩ\tTemp: %f℃\n", adc_reading, Verf_5V, voltage, Res, Temp);
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}