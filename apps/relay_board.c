

/*================================================================
 *
 *
 *   文件名称：relay_board.c
 *   创 建 者：肖飞
 *   创建日期：2020年07月06日 星期一 17时29分03秒
 *   修改日期：2020年08月07日 星期五 13时19分25秒
 *   描    述：
 *
 *================================================================*/
#include "relay_board.h"

#include "main.h"

#define LOG_UART
#include "log.h"

static uint8_t relay_board_id = 0xff;

typedef union {
	uint8_bits_t s;
	uint8_t v;
} u_relay_board_id_t;

void update_relay_board_id(void)
{
	u_relay_board_id_t u_relay_board_id;
	u_relay_board_id.v = 0;


	u_relay_board_id.s.bit0 = (HAL_GPIO_ReadPin(d1_GPIO_Port, d1_Pin) == GPIO_PIN_RESET) ? 1 : 0;
	u_relay_board_id.s.bit1 = (HAL_GPIO_ReadPin(d2_GPIO_Port, d2_Pin) == GPIO_PIN_RESET) ? 1 : 0;
	u_relay_board_id.s.bit2 = (HAL_GPIO_ReadPin(d4_GPIO_Port, d4_Pin) == GPIO_PIN_RESET) ? 1 : 0;
	u_relay_board_id.s.bit3 = (HAL_GPIO_ReadPin(d8_GPIO_Port, d8_Pin) == GPIO_PIN_RESET) ? 1 : 0;

	relay_board_id = u_relay_board_id.v;

	debug("relay_board_id:%02x\n", relay_board_id);
}

uint8_t get_relay_board_id(void)
{
	return relay_board_id;
}

static uint8_t relay_board_config = 0x00;

int relay_board_set_config(uint8_t config)
{
	int ret = 0;
	u_uint8_bits_t *u_uint8_bits = (u_uint8_bits_t *)&config;

	HAL_GPIO_WritePin(kg1_GPIO_Port, kg1_Pin, u_uint8_bits->s.bit0);
	HAL_GPIO_WritePin(kg12_GPIO_Port, kg12_Pin, u_uint8_bits->s.bit0);

	HAL_GPIO_WritePin(kg2_GPIO_Port, kg2_Pin, u_uint8_bits->s.bit1);
	HAL_GPIO_WritePin(kg11_GPIO_Port, kg11_Pin, u_uint8_bits->s.bit1);

	HAL_GPIO_WritePin(kg3_GPIO_Port, kg3_Pin, u_uint8_bits->s.bit2);
	HAL_GPIO_WritePin(kg10_GPIO_Port, kg10_Pin, u_uint8_bits->s.bit2);

	HAL_GPIO_WritePin(kg4_GPIO_Port, kg4_Pin, u_uint8_bits->s.bit3);
	HAL_GPIO_WritePin(kg9_GPIO_Port, kg9_Pin, u_uint8_bits->s.bit3);

	HAL_GPIO_WritePin(kg5_GPIO_Port, kg5_Pin, u_uint8_bits->s.bit4);
	HAL_GPIO_WritePin(kg8_GPIO_Port, kg8_Pin, u_uint8_bits->s.bit4);

	HAL_GPIO_WritePin(kg6_GPIO_Port, kg6_Pin, u_uint8_bits->s.bit5);
	HAL_GPIO_WritePin(kg7_GPIO_Port, kg7_Pin, u_uint8_bits->s.bit5);

	debug("set relay_board_config %02x\n", relay_board_config);
	relay_board_config = config;

	return ret;
}

#define set_config_status_bits(index, state0, state1) do { \
	if((state0 == GPIO_PIN_SET) && (state1 == GPIO_PIN_SET)) { \
		u_uint8_bits->s.bit##index = 1; \
	} else { \
		u_uint8_bits->s.bit##index = 0; \
	} \
} while(0)

uint8_t relay_board_get_config(void)
{
	uint8_t config;
	u_uint8_bits_t *u_uint8_bits = (u_uint8_bits_t *)&config;
	GPIO_PinState state0;
	GPIO_PinState state1;
	u_uint8_bits->v = 0;

	state0 = HAL_GPIO_ReadPin(fb1_GPIO_Port, fb1_Pin);
	state1 = HAL_GPIO_ReadPin(fb12_GPIO_Port, fb12_Pin);
	set_config_status_bits(0, state0, state1);
	//debug("bit0 state0:%d state1:%d\n", state0, state1);

	state0 = HAL_GPIO_ReadPin(fb2_GPIO_Port, fb2_Pin);
	state1 = HAL_GPIO_ReadPin(fb11_GPIO_Port, fb11_Pin);
	set_config_status_bits(1, state0, state1);
	//debug("bit1 state0:%d state1:%d\n", state0, state1);

	state0 = HAL_GPIO_ReadPin(fb3_GPIO_Port, fb3_Pin);
	state1 = HAL_GPIO_ReadPin(fb10_GPIO_Port, fb10_Pin);
	set_config_status_bits(2, state0, state1);
	//debug("bit2 state0:%d state1:%d\n", state0, state1);

	state0 = HAL_GPIO_ReadPin(fb4_GPIO_Port, fb4_Pin);
	state1 = HAL_GPIO_ReadPin(fb9_GPIO_Port, fb9_Pin);
	set_config_status_bits(3, state0, state1);
	//debug("bit3 state0:%d state1:%d\n", state0, state1);

	state0 = HAL_GPIO_ReadPin(fb5_GPIO_Port, fb5_Pin);
	state1 = HAL_GPIO_ReadPin(fb8_GPIO_Port, fb8_Pin);
	set_config_status_bits(4, state0, state1);
	//debug("bit4 state0:%d state1:%d\n", state0, state1);

	state0 = HAL_GPIO_ReadPin(fb6_GPIO_Port, fb6_Pin);
	state1 = HAL_GPIO_ReadPin(fb7_GPIO_Port, fb7_Pin);
	set_config_status_bits(5, state0, state1);
	//debug("bit5 state0:%d state1:%d\n", state0, state1);
	
	debug("get config %02x\n", config);

	if(config != relay_board_config) {
		debug("config %02x, relay_board_config:%02x\n", config, relay_board_config);
	}

	return config;
}

#define set_status_bits(index, state0, state1) do { \
	if((state0 == GPIO_PIN_SET) || (state1 == GPIO_PIN_SET)) { \
		u_uint8_bits->s.bit##index = 1; \
	} else { \
		u_uint8_bits->s.bit##index = 0; \
	} \
} while(0)

uint8_t relay_board_get_status(void)
{
	uint8_t fault = 0;
	uint8_t status;
	u_uint8_bits_t *u_uint8_bits = (u_uint8_bits_t *)&status;
	GPIO_PinState state0;
	GPIO_PinState state1;
	u_uint8_bits->v = 0;

	state0 = HAL_GPIO_ReadPin(fb1_GPIO_Port, fb1_Pin);
	state1 = HAL_GPIO_ReadPin(fb12_GPIO_Port, fb12_Pin);
	set_status_bits(0, state0, state1);
	//debug("bit0 state0:%d state1:%d\n", state0, state1);

	state0 = HAL_GPIO_ReadPin(fb2_GPIO_Port, fb2_Pin);
	state1 = HAL_GPIO_ReadPin(fb11_GPIO_Port, fb11_Pin);
	set_status_bits(1, state0, state1);
	//debug("bit1 state0:%d state1:%d\n", state0, state1);

	state0 = HAL_GPIO_ReadPin(fb3_GPIO_Port, fb3_Pin);
	state1 = HAL_GPIO_ReadPin(fb10_GPIO_Port, fb10_Pin);
	set_status_bits(2, state0, state1);
	//debug("bit2 state0:%d state1:%d\n", state0, state1);

	state0 = HAL_GPIO_ReadPin(fb4_GPIO_Port, fb4_Pin);
	state1 = HAL_GPIO_ReadPin(fb9_GPIO_Port, fb9_Pin);
	set_status_bits(3, state0, state1);
	//debug("bit3 state0:%d state1:%d\n", state0, state1);

	state0 = HAL_GPIO_ReadPin(fb5_GPIO_Port, fb5_Pin);
	state1 = HAL_GPIO_ReadPin(fb8_GPIO_Port, fb8_Pin);
	set_status_bits(4, state0, state1);
	//debug("bit4 state0:%d state1:%d\n", state0, state1);

	state0 = HAL_GPIO_ReadPin(fb6_GPIO_Port, fb6_Pin);
	state1 = HAL_GPIO_ReadPin(fb7_GPIO_Port, fb7_Pin);
	set_status_bits(5, state0, state1);
	//debug("bit5 state0:%d state1:%d\n", state0, state1);

	debug("get status %02x\n", status);

	if(status != relay_board_config) {
		fault = 1;
	}

	if(fault == 0) {
		HAL_GPIO_WritePin(led_fault_GPIO_Port, led_fault_Pin, GPIO_PIN_SET);
	} else {
		HAL_GPIO_WritePin(led_fault_GPIO_Port, led_fault_Pin, GPIO_PIN_RESET);
	}

	return fault;
}
