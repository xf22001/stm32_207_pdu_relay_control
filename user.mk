#
#
#================================================================
#   
#   
#   文件名称：user.mk
#   创 建 者：肖飞
#   创建日期：2019年10月25日 星期五 13时04分38秒
#   修改日期：2020年12月30日 星期三 17时09分57秒
#   描    述：
#
#================================================================

USER_C_INCLUDES += -Iapps
USER_C_INCLUDES += -Iapps/modules
USER_C_INCLUDES += -Iapps/modules/os
USER_C_INCLUDES += -Iapps/modules/drivers
USER_C_INCLUDES += -Iapps/modules/drivers/fw_1.9.0
USER_C_INCLUDES += -Iapps/modules/hardware
USER_C_INCLUDES += -Iapps/modules/app
USER_C_INCLUDES += -Iapps/modules/tests

C_INCLUDES += $(USER_C_INCLUDES)

USER_C_SOURCES += apps/app.c
USER_C_SOURCES += apps/uart_debug_handler.c
USER_C_SOURCES += apps/channel_config.c
USER_C_SOURCES += apps/can_config.c
USER_C_SOURCES += apps/relay_board.c
USER_C_SOURCES += apps/relay_board_communication.c
USER_C_SOURCES += apps/os_utils.c

USER_C_SOURCES += apps/modules/app/uart_debug.c
USER_C_SOURCES += apps/modules/app/can_command.c
USER_C_SOURCES += apps/modules/drivers/fw_1.9.0/can_txrx.c
USER_C_SOURCES += apps/modules/drivers/usart_txrx.c
USER_C_SOURCES += apps/modules/os/event_helper.c
USER_C_SOURCES += apps/modules/os/callback_chain.c
USER_C_SOURCES += apps/modules/os/bitmap_ops.c
USER_C_SOURCES += apps/modules/os/iap.c
USER_C_SOURCES += apps/modules/os/memory.c
USER_C_SOURCES += apps/modules/os/map_utils.c
USER_C_SOURCES += apps/modules/tests/test_serial.c

C_SOURCES += $(USER_C_SOURCES)

#BMS_VERSION_YEAR := $(shell date '+%-Y')
#BMS_VERSION_MONTH := $(shell date '+%-m')
#BMS_VERSION_DAY := $(shell date '+%-d')
#BMS_VERSION_SERIAL := $(shell date '+%-H%M')

#USER_CFLAGS += -DBMS_VERSION_YEAR=$(BMS_VERSION_YEAR)
#USER_CFLAGS += -DBMS_VERSION_MONTH=$(BMS_VERSION_MONTH)
#USER_CFLAGS += -DBMS_VERSION_DAY=$(BMS_VERSION_DAY)
#USER_CFLAGS += -DBMS_VERSION_SERIAL=$(BMS_VERSION_SERIAL)

CFLAGS += $(USER_CFLAGS)

default: all

cscope: all
	rm cscope e_cs -rf
	mkdir -p cscope
	#$(silent)tags.sh prepare;
	$(silent)touch dep_files;
	$(silent)for f in $$(find . -type f -name "*.d" 2>/dev/null); do \
		for i in $$(cat "$$f" | sed 's/^.*://g' | sed 's/[\\ ]/\n/g' | sort -h | uniq); do \
			if test "${i:0:1}" = "/";then \
				echo "$i" >> dep_files; \
			else \
				readlink -f "$$i" >> dep_files; \
			fi; \
		done; \
	done;
	$(silent)cat dep_files | sort | uniq | sed 's/^\(.*\)$$/\"\1\"/g' >> cscope/cscope.files;
	$(silent)cat dep_files | sort | uniq >> cscope/ctags.files;
	$(silent)rm dep_files
	$(silent)tags.sh cscope;
	$(silent)tags.sh tags;
	$(silent)tags.sh env;

clean: clean-cscope
clean-cscope:
	rm cscope e_cs -rf
