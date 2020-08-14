

/*================================================================
 *
 *
 *   文件名称：probe_tool_handler.c
 *   创 建 者：肖飞
 *   创建日期：2020年03月20日 星期五 12时48分07秒
 *   修改日期：2020年08月14日 星期五 09时48分24秒
 *   描    述：
 *
 *================================================================*/
#include "probe_tool_handler.h"

#include <string.h>

#include "lwip/netdb.h"
#include "lwip/inet.h"
#include "lwip/netif.h"

#include "net_client.h"
#include "flash.h"
#include "iap.h"
#include "bms.h"
#include "net_client_callback.h"
#include "relay_board.h"

#define LOG_UDP
#include "log.h"

extern struct netif gnetif;

static void fn1(request_t *request)
{
	probe_server_chunk_sendto(request->payload.fn, (void *)0x8000000, 512);
}

static void fn2(request_t *request)
{
}

static void fn3(request_t *request)
{
}

static int p_host(struct hostent *ent)
{
	int ret = 0;
	char **cp;

	if(ent == NULL) {
		ret = -1;
		return ret;
	}

	_printf("\n");

	_printf("h_name:%s\n", ent->h_name);
	_printf("h_aliases:\n");
	cp = ent->h_aliases;

	while(*cp != NULL) {
		_printf("%s\n", *cp);
		cp += 1;

		if(*cp != NULL) {
			//_printf(", ");
		}
	}

	_printf("h_addrtype:%d\n", ent->h_addrtype);

	_printf("h_length:%d\n", ent->h_length);

	_printf("h_addr_list:\n");
	cp = ent->h_addr_list;

	while(*cp != NULL) {
		_printf("%s\n", inet_ntoa(**cp));
		cp += 1;

		if(*cp != NULL) {
			//_printf(", ");
		}
	}

	return ret;
}

static void get_host_by_name(char *content, uint32_t size)
{
	struct hostent *ent;
	char *hostname = (char *)os_alloc(RECV_BUFFER_SIZE);
	int ret;
	int fn;
	int catched;

	//_hexdump("content", (const char *)content, size);

	if(hostname == NULL) {
		return;
	}

	hostname[0] = 0;

	ret = sscanf(content, "%d %s%n", &fn, hostname, &catched);

	if(ret == 2) {
		_printf("hostname:%s!\n", hostname);
		ent = gethostbyname(hostname);
		p_host(ent);
	} else {
		_printf("no hostname!\n");
	}

	os_free(hostname);
}

static void fn4(request_t *request)
{
	_printf("local host ip:%s\n", inet_ntoa(gnetif.ip_addr));

	get_host_by_name((char *)(request + 1), request->header.data_size);
	memset(request, 0, RECV_BUFFER_SIZE);
}

static void fn5(request_t *request)
{
	int size = xPortGetFreeHeapSize();
	uint8_t *os_thread_info;
	uint8_t is_app = 0;
	uint32_t ticks = osKernelSysTick();

#if defined(USER_APP)
	is_app = 1;
#endif

	_printf("free heap size:%d\n", size);
	_printf("current ticks:%lu\n", ticks);
	_printf("%lu day %lu hour %lu min %lu sec\n",
	        ticks / (1000 * 60 * 60 * 24),//day
	        (ticks % (1000 * 60 * 60 * 24)) / (1000 * 60 * 60),//hour
	        (ticks % (1000 * 60 * 60)) / (1000 * 60),//min
	        (ticks % (1000 * 60)) / (1000)//sec
	       );
	update_relay_board_id();
	_printf("relay_board id:%d\n", get_relay_board_id());

	size = 1024;

	os_thread_info = (uint8_t *)os_alloc(size);

	if(os_thread_info == NULL) {
		return;
	}

	osThreadList(os_thread_info);

	_puts((const char *)os_thread_info);

	os_free(os_thread_info);

	if(is_app) {
		_printf("in app!\n");
	} else {
		_printf("in bootloader!\n");
	}
}

extern protocol_if_t protocol_if_tcp;
extern protocol_if_t protocol_if_udp;
static void fn6(request_t *request)
{
}

static void fn7(request_t *request)
{
}

static void fn8(request_t *request)
{
}

#include "test_https.h"
static void fn9(request_t *request)
{
	//test_https();
	_printf("sizeof(brm_data_multi_t):%d\n", sizeof(brm_data_multi_t));
	_printf("sizeof(bcp_data_multi_t):%d\n", sizeof(bcp_data_multi_t));
	_printf("sizeof(bcs_data_t):%d\n", sizeof(bcs_data_t));
	_printf("sizeof(bsm_data_t):%d\n", sizeof(bsm_data_t));
	_printf("sizeof(bst_data_t):%d\n", sizeof(bst_data_t));
	_printf("sizeof(bsd_data_t):%d\n", sizeof(bsd_data_t));
}

void set_connect_enable(uint8_t enable);
uint8_t get_connect_enable(void);
static void fn10(request_t *request)
{
}

static server_item_t server_map[] = {
	{1, fn1},
	{2, fn2},
	{3, fn3},
	{4, fn4},
	{5, fn5},
	{6, fn6},
	{7, fn7},
	{8, fn8},
	{9, fn9},
	{10, fn10},
};

server_map_info_t server_map_info = {
	.server_map = server_map,
	.server_map_size = sizeof(server_map) / sizeof(server_item_t),
};
