/*
 * rtd.c:
 *	Command-line interface to the Raspberry
 *	Pi's MEGAS-RTD board.
 *	Copyright (c) 2016-2021 Sequent Microsystem
 *	<http://www.sequentmicrosystem.com>
 ***********************************************************************
 *	Author: Alexandru Burcea
 ***********************************************************************
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "rtd.h"
#include "comm.h"
#include "thread.h"
#include "wdt.h"
#include "led.h"
#include "rs485.h"

#define VERSION_BASE	(int)1
#define VERSION_MAJOR	(int)2
#define VERSION_MINOR	(int)5
/* #define VERSION_DEV     "" */
#define VERSION_DEV     "-Dev3"

#define UNUSED(X) (void)X      /* To avoid gcc/g++ warnings */

void usage(void);

int doHelp(int argc, char *argv[]);
const CliCmdType CMD_HELP =
	{
		"-h",
		1,
		&doHelp,
		"\t-h          Display the list of command options or one command option details\n",
		"\tUsage:      rtd -h    Display command options list\n",
		"\tUsage:      rtd -h <param>   Display help for <param> command option\n",
		"\tExample:    rtd -h write    Display help for \"write\" command option\n"};

int doVersion(int argc, char *argv[]);
const CliCmdType CMD_VERSION =
{
	"-v",
	1,
	&doVersion,
	"\t-v          Display the version number\n",
	"\tUsage:      rtd -v\n",
	"",
	"\tExample:    rtd -v  Display the version number\n"};

int doWarranty(int argc, char *argv[]);
const CliCmdType CMD_WAR =
{
	"-warranty",
	1,
	&doWarranty,
	"\t-warranty   Display the warranty\n",
	"\tUsage:      rtd -warranty\n",
	"",
	"\tExample:    rtd -warranty  Display the warranty text\n"};

int doList(int argc, char *argv[]);
const CliCmdType CMD_LIST =
	{
		"-list",
		1,
		&doList,
		"\t-list:      List all rtd the connected cards, returnnr of boards and stack level for every board\n",
		"\tUsage:      rtd -list\n",
		"",
		"\tExample:    rtd -list display all the connected rtd cards \n"};

int doRtdRead(int argc, char *argv[]);
const CliCmdType CMD_READ =
	{
		"read",
		2,
		&doRtdRead,
		"\tread:       Read rtd channel temperature\n",
		"\tUsage:      rtd <id> read <channel>\n",
		"",
		"\tExample:    rtd 0 read 2; Read the temperature on channel #2 on Board #0\n"};

int doRtdReadR(int argc, char *argv[]);
const CliCmdType CMD_READ_R =
	{
		"readres",
		2,
		&doRtdReadR,
		"\treadres:    Read rtd channel resistance\n",
		"\tUsage:      rtd <id> readres <channel>\n",
		"",
		"\tExample:    rtd 0 readres 2; Read the resistance on channel #2 on Board #0\n"};

int doRtdReadPoly5(int argc, char *argv[]);
const CliCmdType CMD_READ_POLY5 =
	{
		"readpoly5",
		2,
		&doRtdReadPoly5,
		"\treadpoly5:  Read rtd channel temperature, using 5th order polynomial resistance-to-temperature fit\n",
		"\tUsage:      rtd <id> readpoly5 <channel>\n",
		"",
		"\tExample:    rtd 0 readpoly5 2; Read the temperature on channel #2 on Board #0\n"};

int doRtdCalib(int argc, char *argv[]);
const CliCmdType CMD_CALIB =
	{
		"cal",
		2,
		&doRtdCalib,
		"\tcal:	    Calibrate the resistance measurement, perform 2 points calibration for completion \n",
		"\tUsage:      rtd <id> cal <channel> <value in ohms>\n",
		"",
		"\tExample:    rtd 0 cal 2 100.34; Send one point of calibration at 100.34 ohms for channel #2 on card #0 \n"};

int doRtdCalibRst(int argc, char *argv[]);
const CliCmdType CMD_CALIB_RST =
	{
		"calrst",
		2,
		&doRtdCalibRst,
		"\tcalrst:	    Reset calibration data for one channel\n",
		"\tUsage:      rtd <id> calrst <channel>\n",
		"",
		"\tExample:    rtd 0 calrst 2; Reset calibration data at factory default for channel #2 on card #0 \n"};

int doBoard(int argc, char *argv[]);
const CliCmdType CMD_BOARD =
{
	"board",
	2,
	&doBoard,
	"\tboard:      Display board firmware version\n",
	"\tUsage:      rtd <id> board\n",
	"",
	"\tExample:    rtd 0 board\n"};

int doSnsTypeRead(int argc, char *argv[]);
const CliCmdType CMD_SNS_TYPE_READ =
	{
		"styperd",
		2,
		&doSnsTypeRead,
		"\tstyperd:    Display sensor type(0: PT100, 1:PT1000) per group of channels(1: ch[1..4], 2:ch[5..8] \n",
		"\tUsage:      rtd <id> styperd <group>\n",
		"",
		"\tExample:    rtd 0 styperd 1; Display the type of sensor for group 1 (channels 1..4) on the board #0\n"};

int doSnsTypeWrite(int argc, char *argv[]);
const CliCmdType CMD_SNS_TYPE_WRITE =
	{
		"stypewr",
		2,
		&doSnsTypeWrite,
		"\tstypewr:    Set sensor type(0: PT100, 1:PT1000) per group of channels(1: ch[1..4], 2:ch[5..8] \n",
		"\tUsage:      rtd <id> stypewr <group> <type> \n",
		"",
		"\tExample:    rtd 0 stypewr 1 1; Set the type of sensor for group 1(channels 1..4) on the board #0 to PT1000\n"};

int doSwitchSamplesRead(int argc, char *argv[]);
const CliCmdType CMD_SWITCH_SAMPLES_READ =
	{
		"swsrd",
		2,
		&doSwitchSamplesRead,
		"\tswsrd:    Display the number of sample measure on a single channel until switch to the next one\n",
		"\tUsage:      rtd <id> swsrd\n",
		"",
		"\tExample:    rtd 0 swsrd; Display the number of samples on the board #0\n"};

int doSwitchSamplesWrite(int argc, char *argv[]);
const CliCmdType CMD_SWITCH_SAMPLES_WRITE =
	{
		"swswr",
		2,
		&doSwitchSamplesWrite,
		"\tswswr:    Set the number of sample measure on a single channel until switch to the next one\n",
		"\tUsage:      rtd <id> swswr <samples> \n",
		"",
		"\tExample:    rtd 0 swswr 100; Set the number of samples on the board #0 to 100\n"};

char *warranty =
	"	       Copyright (c) 2016-2021 Sequent Microsystems\n"
		"                                                             \n"
		"		This program is free software; you can redistribute it and/or modify\n"
		"		it under the terms of the GNU Leser General Public License as published\n"
		"		by the Free Software Foundation, either version 3 of the License, or\n"
		"		(at your option) any later version.\n"
		"                                    \n"
		"		This program is distributed in the hope that it will be useful,\n"
		"		but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
		"		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
		"		GNU Lesser General Public License for more details.\n"
		"			\n"
		"		You should have received a copy of the GNU Lesser General Public License\n"
		"		along with this program. If not, see <http://www.gnu.org/licenses/>.";

const CliCmdType *gCmdArray[] =
{
	&CMD_HELP,
	&CMD_WAR,
	&CMD_LIST,
	&CMD_VERSION,
	&CMD_READ,
	&CMD_READ_R,
	&CMD_READ_POLY5,
	&CMD_BOARD,
	&CMD_WDT_RELOAD,
	&CMD_WDT_SET_PERIOD,
	&CMD_WDT_GET_PERIOD,
	&CMD_WDT_SET_INIT_PERIOD,
	&CMD_WDT_GET_INIT_PERIOD,
	&CMD_WDT_SET_OFF_PERIOD,
	&CMD_WDT_GET_OFF_PERIOD,
   &CMD_WDT_GET_RESETS_COUNT,
   &CMD_WDT_CLR_RESETS_COUNT,
	&CMD_READ_LED_MODE,
	&CMD_WRITE_LED_MODE,
	&CMD_READ_LED_TH,
	&CMD_WRITE_LED_TH,
	&CMD_CALIB,
	&CMD_CALIB_RST,
	&CMD_RS485_READ,
	&CMD_RS485_WRITE,
	&CMD_SNS_TYPE_READ,
	&CMD_SNS_TYPE_WRITE,
	&CMD_SWITCH_SAMPLES_READ,
	&CMD_SWITCH_SAMPLES_WRITE,
	NULL}; //null terminated array of cli structure pointers

int doBoardInit(int stack)
{
	int dev = 0;
	int add = 0;
	uint8_t buff;

	if ( (stack < 0) || (stack > 7))
	{
		printf("Invalid stack level [0..7]!");
		return ERROR;
	}
	add = stack + SLAVE_OWN_ADDRESS_BASE;
	dev = i2cSetup(add);
	if (dev == -1)
	{
		return ERROR;
	}
	if (ERROR == i2cMem8Read(dev, REVISION_MAJOR_MEM_ADD, &buff, 1))
	{
		printf("MEGA-RTD id %d not detected\n", stack);
		return ERROR;
	}

	return dev;
}

int boardCheck(u8 add)
{
	int dev;
	u8 buff;

	dev = i2cSetup(add);
	if (dev == -1)
	{
		return ERROR;
	}
	return (i2cMem8Read(dev, REVISION_MAJOR_MEM_ADD, &buff, 1));
}

int rtdHwTypeGet(int dev, int* hw)
{
	u8 buff;
	if (FAIL == i2cMem8Read(dev, RTD_CARD_TYPE, &buff, 1))
	{
		return ERROR;
	}
	*hw = buff;
	return OK;
}

int rtdChGet(int dev, u8 channel, float *temperature)
{
	u8 buff[sizeof(float)];

	if (NULL == temperature)
	{
		return ERROR;
	}

	if ( (channel < CHANNEL_NR_MIN) || (channel > RTD_CH_NR_MAX))
	{
		printf("Invalid rtd channel nr!\n");
		return ERROR;
	}

	if (FAIL
		== i2cMem8Read(dev, RTD_VAL1_ADD + sizeof(float) * (channel - 1), buff,
			sizeof(float)))
	{
		return ERROR;
	}

	memcpy(temperature, buff, sizeof(float));
	return OK;
}

int rtdChGetR(int dev, u8 channel, float *resistance)
{
	u8 buff[sizeof(float)];

	if (NULL == resistance)
	{
		return ERROR;
	}

	if ( (channel < CHANNEL_NR_MIN) || (channel > RTD_CH_NR_MAX))
	{
		printf("Invalid rtd channel nr!\n");
		return ERROR;
	}

	if (FAIL
		== i2cMem8Read(dev, RTD_RES1_ADD + sizeof(float) * (channel - 1), buff,
			sizeof(float)))
	{
		return ERROR;
	}

	memcpy(resistance, buff, sizeof(float));
	return OK;
}

/*
 * doRtdRead:
 *	Read temperature on one channel
 ******************************************************************************************
 */
int doRtdRead(int argc, char *argv[])
{
	int ch = 0;
	float val = 0;
	int dev = 0;

	dev = doBoardInit(atoi(argv[1]));
	if (dev <= 0)
	{
		exit(1);
	}

	if (argc == 4)
	{
		ch = atoi(argv[3]);
		if ( (ch < CHANNEL_NR_MIN) || (ch > RTD_CH_NR_MAX))
		{
			printf("RTD channel number value out of range!\n");
			exit(1);
		}

		if (OK != rtdChGet(dev, ch, &val))
		{
			printf("Fail to read!\n");
			exit(1);
		}
		printf("%06f\n", val);
	}
	else
	{
		printf("Usage: %s read temperature value\n", argv[0]);
		exit(1);
	}
	return OK;
}

int doRtdReadR(int argc, char *argv[])
{
	int ch = 0;
	float val = 0;
	int dev = 0;

	dev = doBoardInit(atoi(argv[1]));
	if (dev <= 0)
	{
		exit(1);
	}

	if (argc == 4)
	{
		ch = atoi(argv[3]);
		if ( (ch < CHANNEL_NR_MIN) || (ch > RTD_CH_NR_MAX))
		{
			printf("RTD channel number value out of range!\n");
			exit(1);
		}

		if (OK != rtdChGetR(dev, ch, &val))
		{
			printf("Fail to read!\n");
			exit(1);
		}
		printf("%06f\n", val);
	}
	else
	{
		printf("Usage: %s read resistance value\n", argv[0]);
		exit(1);
	}
	return OK;
}

/*
 * doRtdReadPoly5:
 *  Read temperature on one channel, using a 5th order polynomial fit
 *  of resistance to temperature
 ******************************************************************************************
 */
int doRtdReadPoly5(int argc, char *argv[])
{
	int ch = 0;
	float res = 0.0;
    float temp_C = 0.0;
	int dev = 0;

    /* coeffs for 5th order fit */
    float c5 = -2.10678E-11;
    float c4 = 2.27311E-08;
    float c3 = -8.20888E-06;
    float c2 = 2.38589E-03;
    float c1 = 2.24745E+00;
    float c0 = -2.42522E+02;

	dev = doBoardInit(atoi(argv[1]));
	if (dev <= 0)
	{
		exit(1);
	}

	if (argc == 4)
	{
		ch = atoi(argv[3]);
		if ( (ch < CHANNEL_NR_MIN) || (ch > RTD_CH_NR_MAX))
		{
			printf("RTD channel number value out of range!\n");
			exit(1);
		}

        /* get the resistance */
		if (OK != rtdChGetR(dev, ch, &res))
		{
			printf("Fail to read!\n");
			exit(1);
		}

        /* 
         * perform the resistance-to-temperature fit using 5th order polynomial
         *  
         * Rearrange a bit to make it friendlier (less expensive) to calculate
         *    temp_C = res ( res ( res ( res ( res * c5 + c4) + c3) + c2) + c1) + c0
         */
        temp_C = res * c5 + c4;

        temp_C *= res;
        temp_C += c3;

        temp_C *= res;
        temp_C += c2;

        temp_C *= res;
        temp_C += c1;

        temp_C *= res;
        temp_C += c0;

		printf("%06f\n", temp_C);
	}
	else
	{
		printf("Usage: %s read temperature value\n", argv[0]);
		exit(1);
	}
	return OK;
}

int doHelp(int argc, char *argv[])
{
	int i = 0;
	if (argc == 3)
	{
		while (NULL != gCmdArray[i])
		{
			if (gCmdArray[i]->name != NULL)
			{
				if (strcasecmp(argv[2], gCmdArray[i]->name) == 0)
				{
					printf("%s%s%s%s", gCmdArray[i]->help, gCmdArray[i]->usage1,
						gCmdArray[i]->usage2, gCmdArray[i]->example);
					break;
				}
			}
			i++;
		}
		if (NULL == gCmdArray[i])
		{
			printf("Option \"%s\" not found\n", argv[2]);
			i = 0;
			while (NULL != gCmdArray[i])
			{
				if (gCmdArray[i]->name != NULL)
				{
					printf("%s", gCmdArray[i]->help);
					break;
				}
				i++;
			}
		}
	}
	else
	{
		i = 0;
		while (NULL != gCmdArray[i])
		{
			if (gCmdArray[i]->name != NULL)
			{
				printf("%s", gCmdArray[i]->help);
			}
			i++;
		}
	}
	return OK;
}

//********************** Calibration *************************
int rtdSetCalib(int dev, int channel, float value)
{
	u8 buff[sizeof(float) + 1];

	if ( (value < 0) || (value > 4000))
	{
		return ERROR;
	}

	if ( (channel < CHANNEL_NR_MIN) || (channel > RTD_CH_NR_MAX))
	{
		printf("Invalid rtd channel nr!\n");
		return ERROR;
	}
	memcpy(buff, &value, sizeof(float));
	buff[sizeof(float)] = channel;
	if (FAIL == i2cMem8Write(dev, I2C_CALIB_RES, buff, sizeof(float) + 1))
	{
		return ERROR;
	}
	return OK;
}

int rtdResetCalib(int dev, int channel)
{
	u8 buff[sizeof(float) + 1];
	float value = -1;

	if ( (channel < CHANNEL_NR_MIN) || (channel > RTD_CH_NR_MAX))
	{
		printf("Invalid rtd channel nr!\n");
		return ERROR;
	}
	memcpy(buff, &value, sizeof(float));
	buff[sizeof(float)] = channel;
	if (FAIL == i2cMem8Write(dev, I2C_CALIB_RES, buff, sizeof(float) + 1))
	{
		return ERROR;
	}
	return OK;
}

int doRtdCalib(int argc, char *argv[])
{
	int ch = 0;
	float val = 0;
	int dev = 0;

	dev = doBoardInit(atoi(argv[1]));
	if (dev <= 0)
	{
		exit(1);
	}

	if (argc == 5)
	{
		ch = atoi(argv[3]);
		if ( (ch < CHANNEL_NR_MIN) || (ch > RTD_CH_NR_MAX))
		{
			printf("RTD channel number value out of range!\n");
			exit(1);
		}
		val = atof(argv[4]);

		if (OK != rtdSetCalib(dev, ch, val))
		{
			printf("Fail to calibrate!\n");
			exit(1);
		}
		printf("OK\n");
	}
	else
	{
		printf("%s", CMD_CALIB.usage1);
		exit(1);
	}
	return OK;
}

int doRtdCalibRst(int argc, char *argv[])
{
	int ch = 0;

	int dev = 0;

	dev = doBoardInit(atoi(argv[1]));
	if (dev <= 0)
	{
		exit(1);
	}

	if (argc == 4)
	{
		ch = atoi(argv[3]);
		if ( (ch < CHANNEL_NR_MIN) || (ch > RTD_CH_NR_MAX))
		{
			printf("RTD channel number value out of range!\n");
			exit(1);
		}

		if (OK != rtdResetCalib(dev, ch))
		{
			printf("Fail to calibrate!\n");
			exit(1);
		}
		printf("OK\n");
	}
	else
	{
		printf("%s", CMD_CALIB_RST.usage1);
		exit(1);
	}
	return OK;
}

int doVersion(int argc, char *argv[])
{
	UNUSED(argc);
	UNUSED(argv);
	printf("rtd v%d.%d.%d%s Copyright (c) 2016 - 2023 Sequent Microsystems\n",
	VERSION_BASE, VERSION_MAJOR, VERSION_MINOR, VERSION_DEV);
	printf("\nThis is free software with ABSOLUTELY NO WARRANTY.\n");
	printf("For details type: rtd -warranty\n");
	return OK;
}

int doList(int argc, char *argv[])
{
	int ids[8];
	int i;
	int cnt = 0;

	UNUSED(argc);
	UNUSED(argv);

	for (i = 0; i < 8; i++)
	{
		if (boardCheck(SLAVE_OWN_ADDRESS_BASE + i) == OK)
		{
			ids[cnt] = i;
			cnt++;
		}
	}
	printf("%d board(s) detected\n", cnt);
	if (cnt > 0)
	{
		printf("Id:");
	}
	while (cnt > 0)
	{
		cnt--;
		printf(" %d", ids[cnt]);
	}
	printf("\n");
	return OK;
}

//#define DEBUG_ADS
/* 
 * Self test for production
 */
int doBoard(int argc, char *argv[])
{
	int dev = 0;
#ifdef DEBUG_ADS
	int reinit = 0;
	u8 cardType = 0;
	u16 sps[2] =
	{
		0,
		0};
#endif	
	u16 aux16 = 0;
	s8 saux8 = 0;
	float vIn = 0;
	float vRasp = 0;
	u8 buff[5] =
	{
		0,
		0,
		0,
		0,
		0};

	dev = doBoardInit(atoi(argv[1]));
	if (dev <= 0)
	{
		exit(1);
	}

	if (argc == 3)
	{
#ifdef DEBUG_ADS
		if (FAIL == i2cMem8Read(dev, RTD_SPS1_ADD, buff, 5))
		{
			exit(1);
		}
		memcpy(sps, buff, 4);
		cardType = buff[4];
		if (FAIL == i2cMem8Read(dev, RTD_REINIT_COUNT, buff, 4))
		{
			exit(1);
		}
		memcpy(&reinit, buff, 4);
#endif		
		if (FAIL == i2cMem8Read(dev, DIAG_TEMPERATURE_MEM_ADD, buff, 3))
		{
			exit(1);
		}
		memcpy(&saux8, buff, 1);
		memcpy(&aux16, &buff[1], 2);
		vIn = (float)aux16 / 1000;

		if (FAIL == i2cMem8Read(dev, RTD_RASP_VOLT, buff, 2))
		{
			exit(1);
		}
		memcpy(&aux16, buff, 2);
		vRasp = (float)aux16 / 1000;

		if (FAIL == i2cMem8Read(dev, REVISION_MAJOR_MEM_ADD, buff, 2))
		{
			exit(1);
		}
		printf("Mega RTD firmware version %d.%02d\n", (int)buff[0], (int)buff[1]);
#ifdef DEBUG_ADS
		printf("ADC: ARC = %d, SPS1 = %d, SPS2 = %d, Card Type = %d\n", reinit,
		(int)sps[0], (int)sps[1], (int)cardType);
#endif		
		printf("Vin %0.3fV, Vrasp %0.3fV, CPU Temp %dC\n", vIn, vRasp,
			(int)saux8);

	}
#ifdef DEBUG_ADS	
	else if (argc == 4)
	{
		printf("Perform reset..");
		if (FAIL == i2cMem8Write(dev, 0xaa, buff, 1))
		{
			printf("fail!\n");
		}
		else
		{
			printf("done\n");
		}
	}
#endif	
	else
	{
		printf("Invalid arguments number! Usage: %s\n", CMD_BOARD.usage1);
	}
	return OK;
}

int doWarranty(int argc UNU, char *argv[] UNU)
{
	printf("%s\n", warranty);
	return OK;
}

int sensorRead(int dev, int group, int* val)
{
	u8 buff;

	if (NULL == val)
	{
		return ERROR;
	}

	if ( (group < CHANNEL_NR_MIN) || (group > 2))
	{
		printf("Invalid rtd group of channels!\n");
		return ERROR;
	}

	if (FAIL == i2cMem8Read(dev, I2C_SENSORS_TYPE, &buff, 1))
	{
		return ERROR;
	}

	*val = 0x0f & (buff >> (4 * (group - 1)));
	return OK;
}

int sensorWrite(int dev, int group, int val)
{
	u8 buff;

	if (val < 0 || val > 1)
	{
		printf("Invalid sensor type! Use 0/1 : PT100/PT1000\n");
		return ERROR;
	}

	if ( (group < CHANNEL_NR_MIN) || (group > 2))
	{
		printf("Invalid rtd group of channels!\n");
		return ERROR;
	}

	if (FAIL == i2cMem8Read(dev, I2C_SENSORS_TYPE, &buff, 1))
	{
		return ERROR;
	}
	if (val != 0)
	{
		buff |= 1 << (4 * (group - 1));
	}
	else
	{
		buff &= 0xff & (~ (1 << (4 * (group - 1))));
	}
	if (FAIL == i2cMem8Write(dev, I2C_SENSORS_TYPE, &buff, 1))
	{
		return ERROR;
	}

	return OK;
}

int doSnsTypeRead(int argc, char *argv[])
{
	int ch = 0;
	int val = 0;
	int dev = 0;
	int card = 0;

	dev = doBoardInit(atoi(argv[1]));
	if (dev <= 0)
	{
		exit(1);
	}
	if (OK != rtdHwTypeGet(dev, &card))
	{
		printf("Fail to read card type!\n");
		exit(1);
	}
	if (card < 1)
	{
		printf("Available only for hardware version >= 5.0!\n");
		exit(1);
	}

	if (argc == 4)
	{
		ch = atoi(argv[3]);
		if (OK != sensorRead(dev, ch, &val))
		{
			printf("Fail to read!\n");
			exit(1);
		}
		printf("%d\n", val);
	}
	else
	{
		printf("%s", CMD_SNS_TYPE_READ.usage1);
		exit(1);
	}
	return OK;
}

int doSnsTypeWrite(int argc, char *argv[])
{
	int ch = 0;
	int val = 0;
	int dev = 0;
	int card = 0;

	dev = doBoardInit(atoi(argv[1]));
	if (dev <= 0)
	{
		exit(1);
	}
	if (OK != rtdHwTypeGet(dev, &card))
	{
		printf("Fail to read card type!\n");
		exit(1);
	}
	if (card < 1)
	{
		printf("Available only for hardware version >= 5.0!\n");
		exit(1);
	}
	if (argc == 5)
	{
		ch = atoi(argv[3]);
		val = atoi(argv[4]);

		if (OK != sensorWrite(dev, ch, val))
		{
			printf("Fail to write!\n");
			exit(1);
		}
		printf("OK\n");
	}
	else
	{
		printf("%s", CMD_SNS_TYPE_WRITE.usage1);
		exit(1);
	}
	return OK;
}


int samplesRead(int dev, int* val)
{
	u8 buff[2];
	u16 uVal = 0;

	if (NULL == val)
	{
		return ERROR;
	}

	
	if (FAIL == i2cMem8Read(dev, I2C_MEM_ADS_SAMPLE_SWITCH, buff, 2))
	{
		return ERROR;
	}
	memcpy(&uVal, buff, 2);
	*val = uVal;
	return OK;
}

int samplesWrite(int dev, int val)
{
	u8 buff[2];
	u16 uVal = 1;

	if (val < 1 || val > 10000)
	{
		printf("Invalid switch samples number  [1..10000]\n");
		return ERROR;
	}
	uVal = (u16)val;
	memcpy(buff, &uVal, 2);
	
	if (FAIL == i2cMem8Write(dev, I2C_MEM_ADS_SAMPLE_SWITCH, buff, 2))
	{
		return ERROR;
	}

	return OK;
}


int doSwitchSamplesRead(int argc, char *argv[])
{
	int val = 0;
	int dev = 0;

	dev = doBoardInit(atoi(argv[1]));
	if (dev <= 0)
	{
		exit(1);
	}
	
	if (argc == 3)
	{
		if (OK != samplesRead(dev, &val))
		{
			printf("Fail to read!\n");
			exit(1);
		}
		printf("%d\n", val);
	}
	else
	{
		printf("%s", CMD_SWITCH_SAMPLES_READ.usage1);
		exit(1);
	}
	return OK;
}

int doSwitchSamplesWrite(int argc, char *argv[])
{
	int val = 0;
	int dev = 0;

	dev = doBoardInit(atoi(argv[1]));
	if (dev <= 0)
	{
		exit(1);
	}

	if (argc == 4)
	{
		val = atoi(argv[3]);

		if (OK != samplesWrite(dev, val))
		{
			printf("Fail to write!\n");
			exit(1);
		}
		printf("OK\n");
	}
	else
	{
		printf("%s", CMD_SWITCH_SAMPLES_WRITE.usage1);
		exit(1);
	}
	return OK;
}

void usage(void)
{
	int i = 0;
	while (gCmdArray[i] != NULL)
	{
		if (gCmdArray[i]->name != NULL)
		{
			if (strlen(gCmdArray[i]->usage1) > 2)
			{
				printf("%s", gCmdArray[i]->usage1);
			}
			if (strlen(gCmdArray[i]->usage2) > 2)
			{
				printf("%s", gCmdArray[i]->usage2);
			}
		}
		i++;
	}
	printf("Where: <id> = Board level id = 0..7\n");
	printf("Type rtd -h <command> for more help\n");
}

int main(int argc, char *argv[])
{
	int i = 0;
	int ret = OK;

	if (argc == 1)
	{
		usage();
		return -1;
	}
	while (NULL != gCmdArray[i])
	{
		if ( (gCmdArray[i]->name != NULL) && (gCmdArray[i]->namePos < argc))
		{
			if (strcasecmp(argv[gCmdArray[i]->namePos], gCmdArray[i]->name) == 0)
			{
				ret = gCmdArray[i]->pFunc(argc, argv);
				if (ret == ARG_CNT_ERR)
				{
					printf("Invalid parameters number!\n");
					printf("%s", gCmdArray[i]->usage1);
					if (strlen(gCmdArray[i]->usage2) > 2)
					{
						printf("%s", gCmdArray[i]->usage2);
					}
				}
				return ret;
			}
		}
		i++;
	}
	printf("Invalid command option\n");
	usage();

	return -1;
}
