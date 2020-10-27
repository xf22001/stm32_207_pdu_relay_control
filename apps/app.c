

/*================================================================
 *
 *
 *   文件名称：app.c
 *   创 建 者：肖飞
 *   创建日期：2019年10月11日 星期五 16时54分03秒
 *   修改日期：2020年10月27日 星期二 09时24分32秒
 *   描    述：
 *
 *================================================================*/
#include "app.h"
#include "app_platform.h"
#include "cmsis_os.h"
#include "main.h"

#include "os_utils.h"

#include "test_serial.h"
#include "relay_board.h"
#include "channel_config.h"
#include "relay_board_communication.h"

//#define LOG_NONE
#include "log.h"

#include <string.h>

extern IWDG_HandleTypeDef hiwdg;
extern TIM_HandleTypeDef htim3;

void app(void const *argument)
{
	channel_info_config_t *channel_info_config = get_channel_info_config(0);

	HAL_GPIO_WritePin(usart_con_GPIO_Port, usart_con_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(led_fault_GPIO_Port, led_fault_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(led_can_GPIO_Port, led_can_Pin, GPIO_PIN_SET);

	if(channel_info_config == NULL) {
		app_panic();
	}

	{
		uart_info_t *uart_info = get_or_alloc_uart_info(channel_info_config->huart_debug);

		if(uart_info == NULL) {
			app_panic();
		}

		set_log_uart_info(uart_info);

		osThreadDef(uart_debug, task_uart_debug, osPriorityNormal, 0, 128 * 2 * 2);
		osThreadCreate(osThread(uart_debug), uart_info);
	}

	debug("===========================================start app============================================\n");

	update_relay_board_id();

	start_adc1();

	//{
	//	uart_info_t *uart_info = get_or_alloc_uart_info(&huart3);

	//	if(uart_info == NULL) {
	//		app_panic();
	//	}

	//	osThreadDef(task_test_serial, task_test_serial, osPriorityNormal, 0, 128);
	//	osThreadCreate(osThread(task_test_serial), uart_info);
	//}

	{
		relay_board_com_info_t *relay_board_com_info;
		osThreadDef(task_relay_board_com_request, task_relay_board_com_request, osPriorityNormal, 0, 128 * 2);
		osThreadDef(task_relay_board_com_response, task_relay_board_com_response, osPriorityAboveNormal, 0, 128 * 2);

		relay_board_com_info = get_or_alloc_relay_board_com_info(channel_info_config);

		if(relay_board_com_info == NULL) {
			app_panic();
		}

		osThreadCreate(osThread(task_relay_board_com_request), relay_board_com_info);
		osThreadCreate(osThread(task_relay_board_com_response), relay_board_com_info);
	}

	while(1) {
		osDelay(1000);
	}
}

typedef enum {
	PWM_COMPARE_COUNT_UP = 0,
	PWM_COMPARE_COUNT_DOWN,
	PWM_COMPARE_COUNT_KEEP,
} compare_count_type_t;

static void update_work_led(void)
{
	static compare_count_type_t type = PWM_COMPARE_COUNT_UP;
	static uint16_t duty_cycle = 0;
	static uint16_t keep_count = 0;
	//计数值小于duty_cycle,输出1;大于duty_cycle输出0

	switch(type) {
		case PWM_COMPARE_COUNT_UP: {

			if(duty_cycle < 1000) {
				duty_cycle += 5;
			} else {
				type = PWM_COMPARE_COUNT_KEEP;
			}
		}
		break;

		case PWM_COMPARE_COUNT_DOWN: {
			if(duty_cycle > 0) {
				duty_cycle -= 5;
			} else {
				type = PWM_COMPARE_COUNT_UP;
			}

		}
		break;

		case PWM_COMPARE_COUNT_KEEP: {
			if(keep_count < duty_cycle) {
				keep_count += 10;
			} else {
				keep_count = 0;
				type = PWM_COMPARE_COUNT_DOWN;
			}

		}
		break;

		default:
			break;
	}

	duty_cycle = 0;
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, duty_cycle);
}

void idle(void const *argument)
{
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);

	while(1) {
		HAL_IWDG_Refresh(&hiwdg);
		update_work_led();
		osDelay(1);
	}
}
