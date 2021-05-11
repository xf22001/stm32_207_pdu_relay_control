

/*================================================================
 *
 *
 *   文件名称：app.c
 *   创 建 者：肖飞
 *   创建日期：2019年10月11日 星期五 16时54分03秒
 *   修改日期：2021年02月01日 星期一 14时59分53秒
 *   描    述：
 *
 *================================================================*/
#include "app.h"

#include <string.h>

#include "app_platform.h"
#include "cmsis_os.h"
#include "main.h"
#include "iwdg.h"
#include "os_utils.h"
#include "usart_txrx.h"
#include "uart_debug.h"
#include "test_serial.h"
#include "relay_board.h"
#include "channel_config.h"
#include "relay_board_communication.h"

//#define LOG_NONE
#include "log.h"

#include "duty_cycle_pattern.h"

extern IWDG_HandleTypeDef hiwdg;
extern TIM_HandleTypeDef htim3;

static os_signal_t app_event = NULL;

void app_init(void)
{
	app_event = signal_create(1);
}

void send_app_event(app_event_t event)
{
	signal_send(app_event, event, 0);
}

void app(void const *argument)
{
	channel_info_config_t *channel_info_config;

	add_log_handler((log_fn_t)log_uart_data);

	channel_info_config = get_channel_info_config(0);

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

	debug("===========================================start app============================================");

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
		relay_board_com_info_t *relay_board_com_info = get_or_alloc_relay_board_com_info(channel_info_config);

		if(relay_board_com_info == NULL) {
			app_panic();
		}
	}

	while(1) {
		uint32_t event;
		int ret = signal_wait(app_event, &event, 1000);

		if(ret == 0) {
			switch(event) {
				default: {
				}
				break;
			}
		}

		//handle_open_log();
	}
}

static pattern_state_t work_pattern_state = {
	.type = PWM_COMPARE_COUNT_UP,
	.duty_cycle = 0,
};

static void update_work_led(void)
{
	//计数值小于duty_cycle,输出1;大于duty_cycle输出0
	uint16_t duty_cycle = get_duty_cycle_pattern(&work_pattern_state, 1000, 0, 20);
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, duty_cycle);
}

void idle(void const *argument)
{
	MX_IWDG_Init();
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);

	while(1) {
		HAL_IWDG_Refresh(&hiwdg);
		update_work_led();
		osDelay(10);
	}
}
