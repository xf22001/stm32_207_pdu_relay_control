

/*================================================================
 *
 *
 *   文件名称：relay_board_communication.c
 *   创 建 者：肖飞
 *   创建日期：2020年07月06日 星期一 17时08分54秒
 *   修改日期：2021年06月18日 星期五 16时26分52秒
 *   描    述：
 *
 *================================================================*/
#include "relay_board_communication.h"

#include <string.h>
#include <stdlib.h>

#include "app.h"
#include "main.h"
#include "object_class.h"
#include "relay_board.h"
#include "can_data_task.h"

#include "relay_board_command.h"

#define LOG_NONE
#include "log.h"

typedef struct {
	uint32_t src_id : 8;//src
	uint32_t dst_id : 8;//dest 0xff
	uint32_t unused : 8;
	uint32_t flag : 5;//0x12
	uint32_t unused1 : 3;
} com_can_tx_id_t;

typedef union {
	com_can_tx_id_t s;
	uint32_t v;
} u_com_can_tx_id_t;

typedef struct {
	uint32_t dst_id : 8;//dest 0xff
	uint32_t src_id : 8;//src
	uint32_t unused : 8;
	uint32_t flag : 5;//0x12
	uint32_t unused1 : 3;
} com_can_rx_id_t;

typedef union {
	com_can_rx_id_t s;
	uint32_t v;
} u_com_can_rx_id_t;

#define RESPONSE_TIMEOUT 300

#define add_des_case(e) \
	case e: { \
		des = #e; \
	} \
	break

static char *get_relay_board_cmd_des(relay_board_cmd_t cmd)
{
	char *des = "unknow";

	switch(cmd) {
			add_des_case(RELAY_BOARD_CMD_RELAY_BOARD_HEARTBEAT);
			add_des_case(RELAY_BOARD_CMD_RELAY_BOARDS_HEARTBEAT);
			add_des_case(RELAY_BOARD_CMD_RELAY_BOARDS_CONFIG);
			add_des_case(RELAY_BOARD_CMD_RELAY_BOARD_STATUS);

		default: {
		}
		break;
	}

	return des;
}

typedef struct {
	//data buffer
	relay_board_heartbeat_t relay_board_heartbeat;
	relay_boards_heartbeat_t relay_boards_heartbeat;
	relay_board_config_t relay_board_config;
	relay_board_status_t relay_board_status;
} data_ctx_t;

typedef int (*request_callback_t)(relay_board_com_info_t *relay_board_com_info);
typedef int (*response_callback_t)(relay_board_com_info_t *relay_board_com_info);

typedef struct {
	relay_board_cmd_t cmd;
	uint32_t request_period;
	request_callback_t request_callback;
	response_callback_t response_callback;
} command_item_t;

//准备请求数据 发
static int prepare_tx_request(relay_board_com_info_t *relay_board_com_info, uint8_t cmd, uint8_t *data, uint8_t data_size)
{
	int ret = -1;
	command_status_t *cmd_ctx = relay_board_com_info->cmd_ctx + cmd;
	can_com_cmd_common_t *can_com_cmd_common = (can_com_cmd_common_t *)relay_board_com_info->can_tx_msg.Data;

	ret = can_com_prepare_tx_request(cmd_ctx, can_com_cmd_common, data, data_size);

	return ret;
}

//请求后，处理响应数据 收
static int process_rx_response(relay_board_com_info_t *relay_board_com_info, uint8_t cmd, uint8_t data_size)
{
	int ret = -1;
	command_status_t *cmd_ctx = relay_board_com_info->cmd_ctx + cmd;
	can_com_cmd_response_t *can_com_cmd_response = (can_com_cmd_response_t *)relay_board_com_info->can_rx_msg->Data;

	ret = can_com_process_rx_response(cmd_ctx, can_com_cmd_response, data_size);

	return ret;
}

//收到请求后,准备响应数据 发
static int prepare_tx_response(relay_board_com_info_t *relay_board_com_info, uint8_t cmd, uint8_t data_size)
{
	int ret = -1;
	command_status_t *cmd_ctx = relay_board_com_info->cmd_ctx + cmd;
	can_com_cmd_response_t *can_com_cmd_response = (can_com_cmd_response_t *)relay_board_com_info->can_tx_msg.Data;

	ret = can_com_prepare_tx_response(cmd_ctx, can_com_cmd_response, data_size);

	return ret;
}

//处理请求数据 收
static int process_rx_request(relay_board_com_info_t *relay_board_com_info, uint8_t cmd, uint8_t *data, uint8_t data_size)
{
	int ret = -1;
	command_status_t *cmd_ctx = relay_board_com_info->cmd_ctx + cmd;
	can_com_cmd_common_t *can_com_cmd_common = (can_com_cmd_common_t *)relay_board_com_info->can_rx_msg->Data;

	ret = can_com_process_rx_request(cmd_ctx, can_com_cmd_common, data, data_size);

	return ret;
}

static void update_relay_board_heartbeat(relay_board_com_info_t *relay_board_com_info)
{
	int i;
	data_ctx_t *data_ctx = (data_ctx_t *)relay_board_com_info->relay_board_com_data_ctx;

	for(i = 0; i < sizeof(relay_board_heartbeat_t); i++) {
		data_ctx->relay_board_heartbeat.buffer[i] = i;
	}
}

static int request_relay_board_heartbeat(relay_board_com_info_t *relay_board_com_info)
{
	int ret = -1;

	command_status_t *cmd_ctx = relay_board_com_info->cmd_ctx + RELAY_BOARD_CMD_RELAY_BOARD_HEARTBEAT;
	data_ctx_t *data_ctx = (data_ctx_t *)relay_board_com_info->relay_board_com_data_ctx;

	if(cmd_ctx->index == 0) {
		update_relay_board_heartbeat(relay_board_com_info);

		//_hexdump("relay_board_heartbeat",
		//         (const char *)&data_ctx->relay_board_heartbeat,
		//         sizeof(relay_board_heartbeat_t));
	}

	ret = prepare_tx_request(relay_board_com_info,
	                         RELAY_BOARD_CMD_RELAY_BOARD_HEARTBEAT,
	                         (uint8_t *)&data_ctx->relay_board_heartbeat,
	                         sizeof(relay_board_heartbeat_t));

	return ret;
}

static int response_relay_board_heartbeat(relay_board_com_info_t *relay_board_com_info)
{
	int ret = -1;

	ret = process_rx_response(relay_board_com_info,
	                          RELAY_BOARD_CMD_RELAY_BOARD_HEARTBEAT,
	                          sizeof(relay_board_heartbeat_t));

	return ret;
}

static command_item_t command_item_relay_board_heartbeat = {
	.cmd = RELAY_BOARD_CMD_RELAY_BOARD_HEARTBEAT,
	.request_period = 500,
	.request_callback = request_relay_board_heartbeat,
	.response_callback = response_relay_board_heartbeat,
};

static int request_relay_boards_heartbeat(relay_board_com_info_t *relay_board_com_info)
{
	int ret = -1;
	//data_ctx_t *data_ctx = (data_ctx_t *)relay_board_com_info->relay_board_com_data_ctx;
	can_com_cmd_response_t *can_com_cmd_response = (can_com_cmd_response_t *)relay_board_com_info->can_tx_msg.Data;

	ret = prepare_tx_response(relay_board_com_info, RELAY_BOARD_CMD_RELAY_BOARDS_HEARTBEAT, sizeof(relay_boards_heartbeat_t));

	if(can_com_cmd_response->response_status == CAN_COM_RESPONSE_STATUS_DONE) {
		//int i;

		//for(i = 0; i < sizeof(relay_boards_heartbeat_t); i++) {
		//	if(data_ctx->relay_boards_heartbeat.buffer[i] != i) {
		//		debug("relay_boards_heartbeat data[%d] == %d", i, data_ctx->relay_boards_heartbeat.buffer[i]);
		//		break;
		//	}
		//}

		//_hexdump("relay_boards_heartbeat",
		//         (const char *)&data_ctx->relay_boards_heartbeat,
		//         sizeof(relay_boards_heartbeat_t));
	}

	return ret;
}

static int response_relay_boards_heartbeat(relay_board_com_info_t *relay_board_com_info)
{
	int ret = -1;
	data_ctx_t *data_ctx = (data_ctx_t *)relay_board_com_info->relay_board_com_data_ctx;

	ret = process_rx_request(relay_board_com_info,
	                         RELAY_BOARD_CMD_RELAY_BOARDS_HEARTBEAT,
	                         (uint8_t *)&data_ctx->relay_boards_heartbeat,
	                         sizeof(relay_boards_heartbeat_t));

	return ret;
}

static command_item_t command_item_relay_boards_heartbeat = {
	.cmd = RELAY_BOARD_CMD_RELAY_BOARDS_HEARTBEAT,
	.request_period = 0,
	.request_callback = request_relay_boards_heartbeat,
	.response_callback = response_relay_boards_heartbeat,
};

static int request_relay_board_config(relay_board_com_info_t *relay_board_com_info)
{
	int ret = -1;
	data_ctx_t *data_ctx = (data_ctx_t *)relay_board_com_info->relay_board_com_data_ctx;
	can_com_cmd_response_t *can_com_cmd_response = (can_com_cmd_response_t *)relay_board_com_info->can_tx_msg.Data;

	ret = prepare_tx_response(relay_board_com_info, RELAY_BOARD_CMD_RELAY_BOARDS_CONFIG, sizeof(relay_board_config_t));

	if(can_com_cmd_response->response_status == CAN_COM_RESPONSE_STATUS_DONE) {
		relay_board_config_t *relay_board_config = (relay_board_config_t *)&data_ctx->relay_board_config;

		relay_board_set_config(relay_board_config->config);
	}

	return ret;
}

static int response_relay_board_config(relay_board_com_info_t *relay_board_com_info)
{
	int ret = -1;
	data_ctx_t *data_ctx = (data_ctx_t *)relay_board_com_info->relay_board_com_data_ctx;

	ret = process_rx_request(relay_board_com_info,
	                         RELAY_BOARD_CMD_RELAY_BOARDS_CONFIG,
	                         (uint8_t *)&data_ctx->relay_board_config,
	                         sizeof(relay_board_config_t));

	return ret;
}

static command_item_t command_item_relay_board_config = {
	.cmd = RELAY_BOARD_CMD_RELAY_BOARDS_CONFIG,
	.request_period = 0,
	.request_callback = request_relay_board_config,
	.response_callback = response_relay_board_config,
};

static int request_relay_board_status(relay_board_com_info_t *relay_board_com_info)
{
	int ret = -1;

	command_status_t *cmd_ctx = relay_board_com_info->cmd_ctx + RELAY_BOARD_CMD_RELAY_BOARD_STATUS;
	data_ctx_t *data_ctx = (data_ctx_t *)relay_board_com_info->relay_board_com_data_ctx;

	if(cmd_ctx->index == 0) {
		relay_board_status_t *relay_board_status = &data_ctx->relay_board_status;
		adc1_values_t *adc1_values = get_adc1_value();

		relay_board_status->config = relay_board_get_config();
		relay_board_status->temperature1 = get_ntc_temperature(10000, adc1_values->value1, 4095);
		relay_board_status->temperature2 = get_ntc_temperature(10000, adc1_values->value2, 4095);
		relay_board_status->fault.fault = relay_board_get_status();
		relay_board_status->fault.over_temperature = (relay_board_status->temperature1 >= 75) ? 1 : 0;

		debug("relay_board_status->config:%08x", u8_bin(relay_board_status->config));
		debug("relay_board_status->temperature1:%d", relay_board_status->temperature1);
		debug("relay_board_status->temperature2:%d", relay_board_status->temperature2);
		debug("relay_board_status->fault.fault:%d", relay_board_status->fault.fault);
		debug("relay_board_status->fault.over_temperature:%d", relay_board_status->fault.over_temperature);
	}

	ret = prepare_tx_request(relay_board_com_info,
	                         RELAY_BOARD_CMD_RELAY_BOARD_STATUS,
	                         (uint8_t *)&data_ctx->relay_board_status,
	                         sizeof(relay_board_status_t));

	return ret;
}

static int response_relay_board_status(relay_board_com_info_t *relay_board_com_info)
{
	int ret = -1;

	ret = process_rx_response(relay_board_com_info,
	                          RELAY_BOARD_CMD_RELAY_BOARD_STATUS,
	                          sizeof(relay_board_status_t));

	return ret;
}

static command_item_t command_item_relay_board_status = {
	.cmd = RELAY_BOARD_CMD_RELAY_BOARD_STATUS,
	.request_period = 500,
	.request_callback = request_relay_board_status,
	.response_callback = response_relay_board_status,
};


static command_item_t *relay_board_com_command_table[] = {
	//&command_item_relay_board_heartbeat,
	&command_item_relay_boards_heartbeat,
	&command_item_relay_board_config,
	&command_item_relay_board_status,
};

static void relay_board_com_set_connect_state(relay_board_com_info_t *relay_board_com_info, uint8_t state)
{
	update_connect_state(&relay_board_com_info->connect_state, state);
}

uint8_t relay_board_com_get_connect_state(relay_board_com_info_t *relay_board_com_info)
{
	return get_connect_state(&relay_board_com_info->connect_state);
}

static void relay_board_com_request_periodic(relay_board_com_info_t *relay_board_com_info)
{
	int i;
	uint32_t ticks = osKernelSysTick();

	if(ticks_duration(ticks, relay_board_com_info->periodic_stamp) < 50) {
		return;
	}

	relay_board_com_info->periodic_stamp = ticks;

	for(i = 0; i < ARRAY_SIZE(relay_board_com_command_table); i++) {
		command_item_t *item = relay_board_com_command_table[i];
		command_status_t *cmd_ctx = relay_board_com_info->cmd_ctx + item->cmd;

		if(cmd_ctx->state == COMMAND_STATE_RESPONSE) {
			if(ticks_duration(ticks, cmd_ctx->send_stamp) >= RESPONSE_TIMEOUT) {//超时
				relay_board_com_set_connect_state(relay_board_com_info, 0);
				debug("cmd %d(%s) index %d timeout, connect state:%d",
				      item->cmd,
				      get_relay_board_cmd_des(item->cmd),
				      cmd_ctx->index,
				      relay_board_com_get_connect_state(relay_board_com_info));
				cmd_ctx->state = COMMAND_STATE_REQUEST;
			}
		}

		if(item->request_period == 0) {
			continue;
		}

		if(cmd_ctx->available == 0) {
			continue;
		}

		if(cmd_ctx->state != COMMAND_STATE_IDLE) {
			continue;
		}

		if(ticks_duration(ticks, cmd_ctx->stamp) >= item->request_period) {
			cmd_ctx->stamp = ticks;

			//debug("start cmd %d(%s)", item->cmd, get_relay_board_cmd_des(item->cmd));
			cmd_ctx->index = 0;
			cmd_ctx->state = COMMAND_STATE_REQUEST;
		}
	}

}

static void flicker_can_led(uint8_t live)
{
	static uint32_t live_ticks = 0;
	uint32_t ticks = osKernelSysTick();

	if(live != 0) {
		live_ticks = ticks;
	}

	if(ticks_duration(ticks, live_ticks) >= 30) {
		HAL_GPIO_WritePin(led_can_GPIO_Port, led_can_Pin, GPIO_PIN_SET);
	} else {
		HAL_GPIO_WritePin(led_can_GPIO_Port, led_can_Pin, GPIO_PIN_RESET);
	}
}

static void relay_board_com_request(relay_board_com_info_t *relay_board_com_info)
{
	int ret = 0;
	int i;

	for(i = 0; i < ARRAY_SIZE(relay_board_com_command_table); i++) {
		uint32_t ticks = osKernelSysTick();
		command_item_t *item = relay_board_com_command_table[i];
		command_status_t *cmd_ctx = relay_board_com_info->cmd_ctx + item->cmd;
		can_com_cmd_common_t *can_com_cmd_common = (can_com_cmd_common_t *)relay_board_com_info->can_tx_msg.Data;
		u_com_can_tx_id_t *u_com_can_tx_id = (u_com_can_tx_id_t *)&relay_board_com_info->can_tx_msg.ExtId;

		flicker_can_led(0);
		relay_board_com_request_periodic(relay_board_com_info);

		if(cmd_ctx->state != COMMAND_STATE_REQUEST) {
			continue;
		}

		//osDelay(5);

		u_com_can_tx_id->v = 0;
		u_com_can_tx_id->s.flag = 0x12;
		u_com_can_tx_id->s.dst_id = 0xff;
		u_com_can_tx_id->s.src_id = get_relay_board_id();

		relay_board_com_info->can_tx_msg.IDE = CAN_ID_EXT;
		relay_board_com_info->can_tx_msg.RTR = CAN_RTR_DATA;
		relay_board_com_info->can_tx_msg.DLC = 8;

		//debug("request cmd %d(%s), index:%d", item->cmd, get_relay_board_cmd_des(item->cmd), can_com_cmd_common->index);

		memset(relay_board_com_info->can_tx_msg.Data, 0, 8);

		can_com_cmd_common->cmd = item->cmd;

		flicker_can_led(1);
		ret = item->request_callback(relay_board_com_info);

		if(ret != 0) {
			debug("process request cmd %d(%s) error!", item->cmd, get_relay_board_cmd_des(item->cmd));
			continue;
		}

		cmd_ctx->send_stamp = ticks;

		ret = can_tx_data(relay_board_com_info->can_info, &relay_board_com_info->can_tx_msg, 10);

		if(ret != 0) {//发送失败
			cmd_ctx->state = COMMAND_STATE_REQUEST;

			debug("send request cmd %d(%s) error!", item->cmd, get_relay_board_cmd_des(item->cmd));
			relay_board_com_set_connect_state(relay_board_com_info, 0);
		}
	}
}

static void relay_board_com_response(relay_board_com_info_t *relay_board_com_info)
{
	int ret = 0;
	int i;
	uint8_t relay_board_id = get_relay_board_id();

	u_com_can_rx_id_t *u_com_can_rx_id;

	relay_board_com_info->can_rx_msg = can_get_msg(relay_board_com_info->can_info);

	u_com_can_rx_id = (u_com_can_rx_id_t *)&relay_board_com_info->can_rx_msg->ExtId;

	if(relay_board_id != u_com_can_rx_id->s.src_id) {
		debug("relay_board_id:%d, u_com_can_rx_id->s.src_id:%d", relay_board_id, u_com_can_rx_id->s.src_id);
		return;
	}

	for(i = 0; i < ARRAY_SIZE(relay_board_com_command_table); i++) {
		command_item_t *item = relay_board_com_command_table[i];
		can_com_cmd_common_t *can_com_cmd_common = (can_com_cmd_common_t *)relay_board_com_info->can_rx_msg->Data;

		if(can_com_cmd_common->cmd == item->cmd) {
			//debug("response cmd %d(%s), index:%d", item->cmd, get_relay_board_cmd_des(item->cmd), can_com_cmd_common->index);

			flicker_can_led(1);
			relay_board_com_set_connect_state(relay_board_com_info, 1);

			ret = item->response_callback(relay_board_com_info);

			if(ret != 0) {//收到响应
				debug("process response cmd %d(%s) error!", item->cmd, get_relay_board_cmd_des(item->cmd));
			}

			break;
		}

	}

	flicker_can_led(0);
}

static void can_data_request(void *fn_ctx, void *chain_ctx)
{
	relay_board_com_info_t *relay_board_com_info = (relay_board_com_info_t *)fn_ctx;

	if(relay_board_com_info == NULL) {
		return;
	}

	relay_board_com_request(relay_board_com_info);
}

static void can_data_response(void *fn_ctx, void *chain_ctx)
{
	relay_board_com_info_t *relay_board_com_info = (relay_board_com_info_t *)fn_ctx;

	if(relay_board_com_info == NULL) {
		return;
	}

	relay_board_com_response(relay_board_com_info);
}

static object_class_t *relay_board_com_class = NULL;

void free_relay_board_com_info(relay_board_com_info_t *relay_board_com_info)
{
	if(relay_board_com_info == NULL) {
		return;
	}

	if(relay_board_com_info->cmd_ctx != NULL) {
		os_free(relay_board_com_info->cmd_ctx);
	}

	if(relay_board_com_info->relay_board_com_data_ctx != NULL) {
		os_free(relay_board_com_info->relay_board_com_data_ctx);
	}

	os_free(relay_board_com_info);
}

static int relay_board_com_info_set_channel_config(relay_board_com_info_t *relay_board_com_info, channel_info_config_t *channel_info_config)
{
	int ret = -1;
	can_info_t *can_info;
	command_status_t *cmd_ctx;
	data_ctx_t *data_ctx;
	com_can_rx_id_t *com_can_rx_id;
	com_can_rx_id_t *com_can_rx_mask_id;
	can_data_task_info_t *can_data_task_info;

	cmd_ctx = (command_status_t *)os_alloc(sizeof(command_status_t) * RELAY_BOARD_CMD_TOTAL);

	if(cmd_ctx == NULL) {
		return ret;
	}

	memset(cmd_ctx, 0, sizeof(command_status_t) * RELAY_BOARD_CMD_TOTAL);

	relay_board_com_info->cmd_ctx = cmd_ctx;

	relay_board_com_info->cmd_ctx[RELAY_BOARD_CMD_RELAY_BOARD_STATUS].available = 1;

	data_ctx = (data_ctx_t *)os_alloc(sizeof(data_ctx_t));

	if(data_ctx == NULL) {
		return ret;
	}

	memset(data_ctx, 0, sizeof(data_ctx_t));

	relay_board_com_info->relay_board_com_data_ctx = data_ctx;

	can_info = get_or_alloc_can_info(channel_info_config->hcan_com);

	if(can_info == NULL) {
		return ret;
	}

	com_can_rx_id = (com_can_rx_id_t *)&can_info->can_config->filter_id;
	com_can_rx_mask_id = (com_can_rx_id_t *)&can_info->can_config->filter_mask_id;

	com_can_rx_id->src_id = get_relay_board_id();
	com_can_rx_mask_id->src_id = 0xff;

	com_can_rx_id->dst_id = 0xff;
	com_can_rx_mask_id->dst_id = 0xff;

	com_can_rx_id->flag = 0x12;
	com_can_rx_mask_id->flag = 0x1f;

	debug("can_info->can_config->filter_id:%08x", can_info->can_config->filter_id);
	debug("can_info->can_config->filter_mask_id:%08x", can_info->can_config->filter_mask_id);

	can_init(can_info);

	relay_board_com_info->can_info = can_info;

	can_data_task_info = get_or_alloc_can_data_task_info(relay_board_com_info->can_info->hcan);

	if(can_data_task_info == NULL) {
		app_panic();
	}

	relay_board_com_info->can_data_request_cb.fn = can_data_request;
	relay_board_com_info->can_data_request_cb.fn_ctx = relay_board_com_info;
	relay_board_com_info->can_data_response_cb.fn = can_data_response;
	relay_board_com_info->can_data_response_cb.fn_ctx = relay_board_com_info;

	add_can_data_task_info_request_cb(can_data_task_info, &relay_board_com_info->can_data_request_cb);
	add_can_data_task_info_response_cb(can_data_task_info, &relay_board_com_info->can_data_response_cb);

	ret = 0;
	return ret;
}

relay_board_com_info_t *alloc_relay_board_com_info(channel_info_config_t *channel_info_config)
{
	relay_board_com_info_t *relay_board_com_info = NULL;

	relay_board_com_info = (relay_board_com_info_t *)os_alloc(sizeof(relay_board_com_info_t));

	if(relay_board_com_info == NULL) {
		return relay_board_com_info;
	}

	memset(relay_board_com_info, 0, sizeof(relay_board_com_info_t));

	relay_board_com_info->channel_info_config = channel_info_config;

	if(relay_board_com_info_set_channel_config(relay_board_com_info, channel_info_config) != 0) {
		goto failed;
	}

	return relay_board_com_info;
failed:

	free_relay_board_com_info(relay_board_com_info);

	relay_board_com_info = NULL;

	return relay_board_com_info;
}

static int object_filter(void *o, void *ctx)
{
	int ret = -1;
	relay_board_com_info_t *relay_board_com_info = (relay_board_com_info_t *)o;
	channel_info_config_t *channel_info_config = (channel_info_config_t *)ctx;

	if(relay_board_com_info->channel_info_config == channel_info_config) {
		ret = 0;
	}

	return ret;
}

relay_board_com_info_t *get_or_alloc_relay_board_com_info(channel_info_config_t *channel_info_config)
{
	relay_board_com_info_t *relay_board_com_info = NULL;

	os_enter_critical();

	if(relay_board_com_class == NULL) {
		relay_board_com_class = object_class_alloc();
	}

	os_leave_critical();

	relay_board_com_info = (relay_board_com_info_t *)object_class_get_or_alloc_object(relay_board_com_class, object_filter, channel_info_config, (object_alloc_t)alloc_relay_board_com_info, (object_free_t)free_relay_board_com_info);

	return relay_board_com_info;
}

