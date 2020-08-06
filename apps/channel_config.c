

/*================================================================
 *
 *
 *   文件名称：channel_config.c
 *   创 建 者：肖飞
 *   创建日期：2020年04月30日 星期四 09时37分37秒
 *   修改日期：2020年07月13日 星期一 13时56分15秒
 *   描    述：
 *
 *================================================================*/
#include "channel_config.h"

#include "os_utils.h"
#include "main.h"

extern CAN_HandleTypeDef hcan1;
extern UART_HandleTypeDef huart1;

channel_info_config_t channel_info_config = {
	.id = 0,

	.huart_debug = &huart1,
	.hcan_com = &hcan1,
};

static channel_info_config_t *channel_info_config_sz[] = {
	&channel_info_config,
};

channel_info_config_t *get_channel_info_config(uint8_t id)
{
	int i;
	channel_info_config_t *channel_info_config = NULL;
	channel_info_config_t *channel_info_config_item = NULL;

	for(i = 0; i < ARRAY_SIZE(channel_info_config_sz); i++) {
		channel_info_config_item = channel_info_config_sz[i];

		if(channel_info_config_item->id == id) {
			channel_info_config = channel_info_config_item;
			break;
		}
	}

	return channel_info_config;
}

