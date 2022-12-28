#
#
#================================================================
#   
#   
#   文件名称：config.mk
#   创 建 者：肖飞
#   创建日期：2021年08月26日 星期四 11时10分19秒
#   修改日期：2022年12月28日 星期三 13时23分29秒
#   描    述：
#
#================================================================

CONFIG_LIST := 

$(foreach config_item,$(CONFIG_LIST),$(eval $(addprefix CONFIG_,$(config_item)) := $(config_item)))

CONFIG_CFLAGS := $(addprefix -D,$(CONFIG_LIST))
