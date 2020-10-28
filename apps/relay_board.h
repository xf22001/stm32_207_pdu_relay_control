

/*================================================================
 *   
 *   
 *   文件名称：relay_board.h
 *   创 建 者：肖飞
 *   创建日期：2020年07月06日 星期一 17时29分45秒
 *   修改日期：2020年10月28日 星期三 14时10分50秒
 *   描    述：
 *
 *================================================================*/
#ifndef _RELAY_BOARD_H
#define _RELAY_BOARD_H
#ifdef __cplusplus
extern "C"
{
#endif

#include "cmsis_os.h"
#include "app_platform.h"

#include "os_utils.h"

#ifdef __cplusplus
}
#endif

#define ADC_VALUES_GROUPS 10
typedef struct {
	uint16_t value1;
	uint16_t value2;
} adc1_values_t;

typedef struct {
	uint16_t value : 12;
	uint16_t unused : 4;
} adc1_value_t;

typedef union {
	adc1_value_t s;
	uint16_t v;
} u_adc1_value_t;

void update_relay_board_id(void);
uint8_t get_relay_board_id(void);
int relay_board_set_config(uint8_t config);
uint8_t relay_board_get_config(void);
uint8_t relay_board_get_status(void);
void start_adc1(void);
adc1_values_t *get_adc1_value(void);
int get_ntc_temperature(uint32_t ref_resistor, uint16_t adc_voltage, uint16_t adc_max);

#endif //_RELAY_BOARD_H
