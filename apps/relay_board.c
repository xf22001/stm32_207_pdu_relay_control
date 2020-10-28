

/*================================================================
 *
 *
 *   文件名称：relay_board.c
 *   创 建 者：肖飞
 *   创建日期：2020年07月06日 星期一 17时29分03秒
 *   修改日期：2020年10月28日 星期三 14时09分30秒
 *   描    述：
 *
 *================================================================*/
#include "relay_board.h"

#include "main.h"

#define LOG_UART
#include "log.h"

extern ADC_HandleTypeDef hadc1;

static uint8_t relay_board_id = 0xff;
static adc1_values_t adc1_values_data[ADC_VALUES_GROUPS];

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
static uint32_t relay_board_config_stamp = 0;

int relay_board_set_config(uint8_t config)
{
	int ret = 0;
	u_uint8_bits_t *u_uint8_bits = (u_uint8_bits_t *)&config;
	uint32_t ticks = osKernelSysTick();

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
	relay_board_config_stamp = ticks;

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
	uint32_t ticks = osKernelSysTick();

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

	if(status != relay_board_config) {
		debug("status %02x, relay_board_config:%02x\n", status, relay_board_config);

		if(ticks - relay_board_config_stamp >= 1 * 1000) {
			fault = 1;
		}
	}

	if(fault == 0) {
		HAL_GPIO_WritePin(led_fault_GPIO_Port, led_fault_Pin, GPIO_PIN_SET);
	} else {
		HAL_GPIO_WritePin(led_fault_GPIO_Port, led_fault_Pin, GPIO_PIN_RESET);
	}

	return fault;
}


void start_adc1(void)
{
	uint32_t length = ADC_VALUES_GROUPS * (sizeof(adc1_values_t) / sizeof(uint16_t));
	HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc1_values_data, length);
}

static uint16_t get_adc_channel_value(uint16_t value)
{
	u_adc1_value_t u_adc1_value;

	u_adc1_value.v = value;

	return u_adc1_value.s.value;
}

adc1_values_t *get_adc1_value(void)
{
	static adc1_values_t ret;
	int i;
	uint32_t value1 = 0;
	uint32_t value2 = 0;

	for(i = 0; i < ADC_VALUES_GROUPS; i++) {
		adc1_values_t *adc1_values = adc1_values_data + i;

		value1 += get_adc_channel_value(adc1_values->value1);
		value2 += get_adc_channel_value(adc1_values->value2);
	}

	if(ADC_VALUES_GROUPS != 0) {
		value1 /= ADC_VALUES_GROUPS;
		value2 /= ADC_VALUES_GROUPS;
	} else {
		value1 = 0;
		value2 = 0;
	}

	ret.value1 = value1;
	ret.value2 = value2;

	return &ret;
}

typedef struct {
	int temperature;
	uint32_t resistor;
} temperature_resistor_item_t;

const temperature_resistor_item_t temperature_resistor_items[] = {
	{-40, 335745},
	{-39, 314213},
	{-38, 294198},
	{-37, 275584},
	{-36, 258265},
	{-35, 242142},
	{-34, 227126},
	{-33, 213135},
	{-32, 200093},
	{-31, 187929},
	{-30, 176580},
	{-29, 165987},
	{-28, 156094},
	{-27, 146852},
	{-26, 138213},
	{-25, 130136},
	{-24, 122579},
	{-23, 115508},
	{-22, 108887},
	{-21, 102687},
	{-20, 96876},
	{-19, 91430},
	{-18, 86323},
	{-17, 81531},
	{-16, 77035},
	{-15, 72813},
	{-14, 68848},
	{-13, 65123},
	{-12, 61621},
	{-11, 58328},
	{-10, 55231},
	{-9, 52317},
	{-8, 49574},
	{-7, 46990},
	{-6, 44557},
	{-5, 42264},
	{-4, 40102},
	{-3, 38064},
	{-2, 36141},
	{-1, 34326},
	{0, 32613},
	{1, 30996},
	{2, 29468},
	{3, 28024},
	{4, 26660},
	{5, 25370},
	{6, 24149},
	{7, 22994},
	{8, 21901},
	{9, 20867},
	{10, 19888},
	{11, 18959},
	{12, 18079},
	{13, 17245},
	{14, 16454},
	{15, 15704},
	{16, 14993},
	{17, 14317},
	{18, 13676},
	{19, 13067},
	{20, 12489},
	{21, 11940},
	{22, 11417},
	{23, 10921},
	{24, 10449},
	{25, 10000},
	{26, 9572},
	{27, 9165},
	{28, 8778},
	{29, 8409},
	{30, 8058},
	{31, 7724},
	{32, 7405},
	{33, 7101},
	{34, 6811},
	{35, 6534},
	{36, 6271},
	{37, 6019},
	{38, 5779},
	{39, 5549},
	{40, 5330},
	{41, 5121},
	{42, 4921},
	{43, 4730},
	{44, 4547},
	{45, 4373},
	{46, 4206},
	{47, 4046},
	{48, 3893},
	{49, 3747},
	{50, 3607},
	{51, 3473},
	{52, 3345},
	{53, 3222},
	{54, 3104},
	{55, 2991},
	{56, 2883},
	{57, 2780},
	{58, 2680},
	{59, 2585},
	{60, 2493},
	{61, 2405},
	{62, 2321},
	{63, 2240},
	{64, 2163},
	{65, 2088},
	{66, 2017},
	{67, 1948},
	{68, 1882},
	{69, 1818},
	{70, 1757},
	{71, 1698},
	{72, 1642},
	{73, 1588},
	{74, 1535},
	{75, 1485},
	{76, 1437},
	{77, 1390},
	{78, 1345},
	{79, 1302},
	{80, 1261},
	{81, 1221},
	{82, 1182},
	{83, 1145},
	{84, 1109},
	{85, 1075},
	{86, 1041},
	{87, 1008},
	{88, 978},
	{89, 948},
	{90, 920},
	{91, 892},
	{92, 865},
	{93, 839},
	{94, 814},
	{95, 790},
	{96, 767},
	{97, 744},
	{98, 722},
	{99, 701},
	{100, 681},
	{101, 662},
	{102, 643},
	{103, 624},
	{104, 607},
	{105, 589},
	{106, 573},
	{107, 557},
	{108, 541},
	{109, 526},
	{110, 512},
	{111, 498},
	{112, 484},
	{113, 471},
	{114, 458},
	{115, 446},
	{116, 434},
	{117, 422},
	{118, 411},
	{119, 400},
	{120, 390},
	{121, 379},
	{122, 369},
	{123, 360},
	{124, 351},
	{125, 342},
};

int get_ntc_temperature(uint32_t ref_resistor, uint16_t adc_voltage, uint16_t adc_max)
{
	uint32_t ntc_resistor;
	int ntc_temperature = -40;
	int i;

	if(adc_voltage == 0) {
		return ntc_temperature;
	}

	ntc_resistor = ref_resistor * (adc_max  - adc_voltage) / (adc_voltage);

	for(i = 0; i < ARRAY_SIZE(temperature_resistor_items); i++) {
		const temperature_resistor_item_t *item = temperature_resistor_items + i;

		if(ntc_resistor >= item->resistor) {
			ntc_temperature = item->temperature;
			break;
		}
	}

	return ntc_temperature;
}
