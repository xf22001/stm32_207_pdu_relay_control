

/*================================================================
 *
 *
 *   文件名称：uart_debug_handler.c
 *   创 建 者：肖飞
 *   创建日期：2020年05月13日 星期三 13时18分00秒
 *   修改日期：2020年08月07日 星期五 11时29分56秒
 *   描    述：
 *
 *================================================================*/
#include "uart_debug_handler.h"

#include <stdio.h>
#include "relay_board.h"

#define LOG_UART
#include "log.h"

static void fn1(char *arguments)
{
	debug("arguments:\'%s\'\n", arguments);
}

static void fn2(char *arguments)
{
}

static void fn3(char *arguments)
{
}

void get_mem_info(size_t *total_size, size_t *max_size, size_t *count);

static void fn5(char *arguments)
{
	int size = xPortGetFreeHeapSize();
	uint8_t *os_thread_info;
	uint8_t is_app = 0;
	uint32_t ticks = osKernelSysTick();
	size_t total_size;
	size_t max_size;
	size_t count;

#if defined(USER_APP)
	is_app = 1;
#endif
	get_mem_info(&total_size, &max_size, &count);

	debug("os free heap size:%d\n", size);
	debug("app heap total_size:%d, max_size:%d, count:%d\n", total_size, max_size, count);
	debug("current ticks:%lu\n", ticks);
	debug("%lu day %lu hour %lu min %lu sec\n",
	        ticks / (1000 * 60 * 60 * 24),//day
	        (ticks % (1000 * 60 * 60 * 24)) / (1000 * 60 * 60),//hour
	        (ticks % (1000 * 60 * 60)) / (1000 * 60),//min
	        (ticks % (1000 * 60)) / (1000)//sec
	       );
	update_relay_board_id();
	debug("relay_board id:%d\n", get_relay_board_id());

	if(size < 4 * 1024) {
		return;
	}

	size = 1024;

	os_thread_info = (uint8_t *)os_alloc(size);

	if(os_thread_info == NULL) {
		return;
	}

	osThreadList(os_thread_info);

	_puts((const char *)os_thread_info);

	os_free(os_thread_info);

	if(is_app) {
		debug("in app!\n");
	} else {
		debug("in bootloader!\n");
	}
}

static uart_fn_item_t uart_fn_map[] = {
	{1, fn1},
	{2, fn2},
	{3, fn3},
	{5, fn5},
};

uart_fn_map_info_t uart_fn_map_info = {
	.uart_fn_map = uart_fn_map,
	.uart_fn_map_size = sizeof(uart_fn_map) / sizeof(uart_fn_item_t),
};
