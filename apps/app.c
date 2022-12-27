

/*================================================================
 *
 *
 *   文件名称：app.c
 *   创 建 者：肖飞
 *   创建日期：2019年10月11日 星期五 16时54分03秒
 *   修改日期：2021年08月10日 星期二 10时24分49秒
 *   描    述：
 *
 *================================================================*/
#include "app.h"

#include <string.h>

#include "main.h"
#include "iwdg.h"
#include "os_utils.h"
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

	HAL_GPIO_WritePin(usart_con_GPIO_Port, usart_con_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(led_fault_GPIO_Port, led_fault_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(led_can_GPIO_Port, led_can_Pin, GPIO_PIN_SET);

	channel_info_config = get_channel_info_config(0);
	OS_ASSERT(channel_info_config != NULL);

	get_or_alloc_uart_debug_info(channel_info_config->huart_debug);
	add_log_handler((log_fn_t)log_uart_data);

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
		OS_ASSERT(relay_board_com_info != NULL);
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
