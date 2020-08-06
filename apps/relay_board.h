

/*================================================================
 *   
 *   
 *   文件名称：relay_board.h
 *   创 建 者：肖飞
 *   创建日期：2020年07月06日 星期一 17时29分45秒
 *   修改日期：2020年07月15日 星期三 13时21分40秒
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

void update_relay_board_id(void);
uint8_t get_relay_board_id(void);
int relay_board_set_config(uint8_t config);
uint8_t relay_board_get_config(void);
uint8_t relay_board_get_status(void);

#endif //_RELAY_BOARD_H
