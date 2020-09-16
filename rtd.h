#ifndef RTD_H_
#define RTD_H_

#include <stdint.h>

#define RETRY_TIMES	10
enum
{
	RTD_VAL1_ADD = 0,
	RTD_VAL2_ADD = RTD_VAL1_ADD + 4,
	RTD_VAL3_ADD = RTD_VAL2_ADD + 4,
	RTD_VAL4_ADD = RTD_VAL3_ADD + 4,
	RTD_VAL5_ADD = RTD_VAL4_ADD + 4,
	RTD_VAL6_ADD = RTD_VAL5_ADD + 4,
	RTD_VAL7_ADD = RTD_VAL6_ADD + 4,
	RTD_VAL8_ADD = RTD_VAL7_ADD + 4,
	DIAG_TEMPERATURE_MEM_ADD = RTD_VAL8_ADD + 4,
	DIAG_5V_MEM_ADD,
	DIAG_5V_MEM_ADD1,
	WDT_RESET_ADD,
	WDT_INTERVAL_SET_ADD,
	SPARE9,
	WDT_INTERVAL_GET_ADD,
	SPARE10,
	WDT_INIT_INTERVAL_SET_ADD,
	SPARE11,
	I2C_WDT_INIT_INTERVAL_GET_ADD,
	SPARE12,
	WDT_RESET_COUNT_ADD,
	SPARE13,
	WDT_CLEAR_RESET_COUNT_ADD,
	I2C_POWER_OFF_INTERVAL_SET_ADD,
	SPARE14,
	SPARE15,
	SPARE16,
	I2C_POWER_OFF_INTERVAL_GET_ADD,
	SPARE17,
	SPARE18,
	SPARE19,
	REVISION_HW_MAJOR_MEM_ADD,
	REVISION_HW_MINOR_MEM_ADD,
	REVISION_MAJOR_MEM_ADD,
	REVISION_MINOR_MEM_ADD,

	SLAVE_BUFF_SIZE,
};


#define CHANNEL_NR_MIN		1
#define RTD_CH_NR_MAX		8

#define ERROR	-1
#define OK		0
#define FAIL	-1

#define SLAVE_OWN_ADDRESS_BASE 0x40

typedef uint8_t u8;
typedef uint16_t u16;

typedef enum
{
	OFF = 0,
	ON,
	STATE_COUNT
} OutStateEnumType;

typedef struct
{
 const char* name;
 const int namePos;
 void(*pFunc)(int, char**);
 const char* help;
 const char* usage1;
 const char* usage2;
 const char* example;
}CliCmdType;

#endif //RELAY8_H_
