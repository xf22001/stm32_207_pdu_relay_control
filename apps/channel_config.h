

/*================================================================
 *   
 *   
 *   文件名称：channel_config.h
 *   创 建 者：肖飞
 *   创建日期：2020年04月30日 星期四 09时39分55秒
 *   修改日期：2020年06月22日 星期一 16时54分20秒
 *   描    述：
 *
 *================================================================*/
#ifndef _CHANNEL_CONFIG_H
#define _CHANNEL_CONFIG_H
#ifdef __cplusplus
extern "C"
{
#endif

#include "app_platform.h"
#include "cmsis_os.h"
#include "can_txrx.h"

#ifdef __cplusplus
}
#endif

typedef struct {
	uint32_t stamp;
	uint8_t state;
} charger_op_ctx_t;

typedef struct {
	uint8_t id;

	UART_HandleTypeDef *huart_debug;
	CAN_HandleTypeDef *hcan_com;
} channel_info_config_t;

channel_info_config_t *get_channel_info_config(uint8_t config_id);
#endif //_CHANNEL_CONFIG_H
