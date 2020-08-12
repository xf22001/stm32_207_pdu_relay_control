

/*================================================================
 *
 *
 *   文件名称：os_utils.c
 *   创 建 者：肖飞
 *   创建日期：2019年11月13日 星期三 11时13分17秒
 *   修改日期：2020年08月12日 星期三 17时36分22秒
 *   描    述：
 *
 *================================================================*/
#include "os_utils.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cmsis_os.h"
#include "list_utils.h"
#include "main.h"

#include "log.h"

typedef struct {
	size_t size;
	struct list_head list;
} mem_node_info_t;

typedef struct {
	size_t size;
	struct list_head mem_info_list;
} mem_info_t;

#define LOG_BUFFER_SIZE (1024)

static osMutexId os_utils_mutex = NULL;
static mem_info_t mem_info;

static int init_os_utils_mutex(void)
{
	int ret = -1;
	osMutexDef(os_utils_mutex);

	INIT_LIST_HEAD(&mem_info.mem_info_list);
	mem_info.size = 0;

	os_utils_mutex = osMutexCreate(osMutex(os_utils_mutex));

	if(os_utils_mutex != NULL) {
		ret = 0;
	}

	return ret;
}

static void *xmalloc(size_t size)
{
	osStatus os_status;
	mem_node_info_t *mem_node_info;

	if(os_utils_mutex == NULL) {
		if(init_os_utils_mutex() != 0) {
			return NULL;
		}
	}

	os_status = osMutexWait(os_utils_mutex, osWaitForever);

	if(os_status != osOK) {
	}

	mem_node_info = (mem_node_info_t *)malloc(sizeof(mem_node_info_t) + size);

	if(mem_node_info != NULL) {
		mem_node_info->size = size;
		list_add_tail(&mem_node_info->list, &mem_info.mem_info_list);
		mem_info.size += mem_node_info->size;
	}

	os_status = osMutexRelease(os_utils_mutex);

	if(os_status != osOK) {
	}

	return (mem_node_info != NULL) ? (mem_node_info + 1) : NULL;
}

static void xfree(void *p)
{
	osStatus os_status;

	if(os_utils_mutex == NULL) {
		if(init_os_utils_mutex() != 0) {
			return;
		}
	}

	os_status = osMutexWait(os_utils_mutex, osWaitForever);

	if(os_status != osOK) {
	}

	if(p != NULL) {
		mem_node_info_t *mem_node_info = (mem_node_info_t *)p;

		mem_node_info--;

		list_del(&mem_node_info->list);
		mem_info.size -= mem_node_info->size;

		free(mem_node_info);
	}

	os_status = osMutexRelease(os_utils_mutex);

	if(os_status != osOK) {
	}
}

void get_mem_info(size_t *total_size, size_t *max_size, size_t *count)
{
	osStatus os_status;
	mem_node_info_t *mem_node_info;
	struct list_head *head;

	*total_size = 0;
	*max_size = 0;
	*count = 0;

	if(os_utils_mutex == NULL) {
		if(init_os_utils_mutex() != 0) {
			return;
		}
	}

	os_status = osMutexWait(os_utils_mutex, osWaitForever);

	if(os_status != osOK) {
	}

	*total_size = mem_info.size;

	head = &mem_info.mem_info_list;

	list_for_each_entry(mem_node_info, head, mem_node_info_t, list) {
		*count += 1;

		if(mem_node_info->size > *max_size) {
			*max_size = mem_node_info->size;
		}
	}

	os_status = osMutexRelease(os_utils_mutex);

	if(os_status != osOK) {
	}
}

void *os_alloc(size_t size)
{
	void *p;

	p = xmalloc(size);

	return p;
}

void os_free(void *p)
{
	xfree(p);
}

int log_printf(log_fn_t log_fn, const char *fmt, ...)
{
	va_list ap;
	int ret = -1;
	char *log_buffer = (char *)os_alloc(LOG_BUFFER_SIZE);

	if(log_buffer == NULL) {
		return ret;
	}

	va_start(ap, fmt);
	ret = vsnprintf(log_buffer, LOG_BUFFER_SIZE, fmt, ap);
	va_end(ap);

	if(ret > LOG_BUFFER_SIZE) {
		ret = LOG_BUFFER_SIZE;
	}

	if(log_fn != NULL) {
		HAL_GPIO_WritePin(usart_con_GPIO_Port, usart_con_Pin, GPIO_PIN_RESET);
		ret = log_fn(log_buffer, ret);
		HAL_GPIO_WritePin(usart_con_GPIO_Port, usart_con_Pin, GPIO_PIN_SET);
	}

	os_free(log_buffer);

	return ret;
}

static int32_t my_isprint(int32_t c)
{
	if(((uint8_t)c >= 0x20) && ((uint8_t)c <= 0x7e)) {
		return 0x4000;
	} else {
		return 0;
	}
}

#define BUFFER_LEN 80
void log_hexdump(log_fn_t log_fn, const char *label, const char *data, int len)
{
	int ret = 0;
	char *buffer = (char *)os_alloc(BUFFER_LEN);
	const char *start = data;
	const char *end = start + len;
	int c;
	int puts(const char *s);
	char *buffer_start = buffer;
	int i;
	long offset = 0;
	int bytes_per_line = 16;

	if(buffer == NULL) {
		return;
	}

	if(label != NULL) {
		ret = snprintf(buffer, BUFFER_LEN, "%s:\n", label);

		if(ret > BUFFER_LEN) {
			ret = BUFFER_LEN;
		}

		if(log_fn != NULL) {
			HAL_GPIO_WritePin(usart_con_GPIO_Port, usart_con_Pin, GPIO_PIN_RESET);
			ret = log_fn(buffer, ret);
			HAL_GPIO_WritePin(usart_con_GPIO_Port, usart_con_Pin, GPIO_PIN_SET);
		}
	}

	while(start < end) {
		int left = BUFFER_LEN;
		long address = start - data;

		buffer_start = buffer;

		c = end - start;

		if(c > bytes_per_line) {
			c = bytes_per_line;
		}

		ret = snprintf(buffer_start, left, "%08lx", offset + address);
		buffer_start += ret;

		if(ret >= left) {
			left = 0;
			goto out;
		} else {
			left -= ret;
		}

		ret = snprintf(buffer_start, left, " ");
		buffer_start += ret;

		if(ret >= left) {
			left = 0;
			goto out;
		} else {
			left -= ret;
		}

		for(i = 0; i < c; i++) {
			if(i % 8 == 0) {
				ret = snprintf(buffer_start, left, " ");
				buffer_start += ret;

				if(ret >= left) {
					left = 0;
					goto out;
				} else {
					left -= ret;
				}
			}

			ret = snprintf(buffer_start, left, "%02x ", (unsigned char)start[i]);
			buffer_start += ret;

			if(ret >= left) {
				left = 0;
				goto out;
			} else {
				left -= ret;
			}
		}

		for(i = c; i < bytes_per_line; i++) {
			if(i % 8 == 0) {
				ret = snprintf(buffer_start, left, " ");
				buffer_start += ret;

				if(ret >= left) {
					left = 0;
					goto out;
				} else {
					left -= ret;
				}
			}

			ret = snprintf(buffer_start, left, "%2s ", " ");
			buffer_start += ret;

			if(ret >= left) {
				left = 0;
				goto out;
			} else {
				left -= ret;
			}
		}

		ret = snprintf(buffer_start, left, "|");
		buffer_start += ret;

		if(ret >= left) {
			left = 0;
			goto out;
		} else {
			left -= ret;
		}

		for(i = 0; i < c; i++) {
			ret = snprintf(buffer_start, left, "%c", my_isprint(start[i]) ? start[i] : '.');
			buffer_start += ret;

			if(ret >= left) {
				left = 0;
				goto out;
			} else {
				left -= ret;
			}
		}

		ret = snprintf(buffer_start, left, "|");
		buffer_start += ret;

		if(ret >= left) {
			left = 0;
			goto out;
		} else {
			left -= ret;
		}

		ret = snprintf(buffer_start, left, "\n");
		buffer_start += ret;

		if(ret >= left) {
			left = 0;
			goto out;
		} else {
			left -= ret;
		}

	out:

		if(log_fn != NULL) {
			HAL_GPIO_WritePin(usart_con_GPIO_Port, usart_con_Pin, GPIO_PIN_RESET);
			ret = log_fn(buffer, ret);
			HAL_GPIO_WritePin(usart_con_GPIO_Port, usart_con_Pin, GPIO_PIN_SET);
		}

		start += c;
	}

	os_free(buffer);
}

int log_puts(log_fn_t log_fn, const char *s)
{
	int ret = 0;
	ret = strlen(s);

	if(log_fn != NULL) {
		if(ret > (1024 - 1)) {
			log_hexdump(log_fn, NULL, s, ret);
		} else {
			HAL_GPIO_WritePin(usart_con_GPIO_Port, usart_con_Pin, GPIO_PIN_RESET);
			ret = log_fn((void *)s, ret);
			HAL_GPIO_WritePin(usart_con_GPIO_Port, usart_con_Pin, GPIO_PIN_SET);
		}
	}

	return ret;
}

void app_panic(void)
{
	while(1);
}

unsigned char mem_is_set(char *values, size_t size, char value)
{
	unsigned char ret = 1;
	int i;

	for(i = 0; i < size; i++) {
		if(values[i] != value) {
			ret = 0;
			break;
		}
	}

	return ret;
}
