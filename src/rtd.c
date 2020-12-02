/*
 * rtd.c:
 *	Command-line interface to the Raspberry
 *	Pi's MEGAS-RTD board.
 *	Copyright (c) 2016-2020 Sequent Microsystem
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

#define VERSION_BASE	(int)1
#define VERSION_MAJOR	(int)0
#define VERSION_MINOR	(int)0

#define UNUSED(X) (void)X      /* To avoid gcc/g++ warnings */
#define CMD_ARRAY_SIZE	5

void usage(void);

static void doHelp(int argc, char *argv[]);
const CliCmdType CMD_HELP =
	{
		"-h",
		1,
		&doHelp,
		"\t-h          Display the list of command options or one command option details\n",
		"\tUsage:      rtd -h    Display command options list\n",
		"\tUsage:      rtd -h <param>   Display help for <param> command option\n",
		"\tExample:    rtd -h write    Display help for \"write\" command option\n"};

static void doVersion(int argc, char *argv[]);
const CliCmdType CMD_VERSION =
{
	"-v",
	1,
	&doVersion,
	"\t-v          Display the version number\n",
	"\tUsage:      rtd -v\n",
	"",
	"\tExample:    rtd -v  Display the version number\n"};

static void doWarranty(int argc, char* argv[]);
const CliCmdType CMD_WAR =
{
	"-warranty",
	1,
	&doWarranty,
	"\t-warranty   Display the warranty\n",
	"\tUsage:      rtd -warranty\n",
	"",
	"\tExample:    rtd -warranty  Display the warranty text\n"};

//static void doList(int argc, char *argv[]);
//const CliCmdType CMD_LIST =
//{
//	"-list",
//	1,
//	&doList,
//	"\t-list:       List all rtd boards connected,\n\treturn       nr of boards and stack level for every board\n",
//	"\tUsage:       rtd -list\n",
//	"",
//	"\tExample:     rtd -list display: 1,0 \n"};

static void doRtdRead(int argc, char *argv[]);
const CliCmdType CMD_READ =
	{
		"read",
		2,
		&doRtdRead,
		"\tread:       Read rtd channel temperature\n",
		"\tUsage:      rtd <id> read <channel>\n",
		"",
		"\tExample:    rtd 0 read 2; Read the temperature on channel #2 on Board #0\n"};

static void doBoard(int argc, char* argv[]);
const CliCmdType CMD_BOARD =
{
	"board",
	2,
	&doBoard,
	"\tboard:      Display board firmware version\n",
	"\tUsage:      rtd <id> board\n",
	"",
	"\tExample:    rtd 0 board\n"};

CliCmdType gCmdArray[CMD_ARRAY_SIZE];


char *warranty =
	"	       Copyright (c) 2016-2020 Sequent Microsystems\n"
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

int doBoardInit(int stack)
{
	int dev = 0;
	int add = 0;
	uint8_t buff[8];

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
	if (ERROR == i2cMem8Read(dev, REVISION_MAJOR_MEM_ADD, buff, 1))
	{
		printf("MEGA-RTD id %d not detected\n", stack);
		return ERROR;
	}

	return dev;
}

int rtdChGet(int dev, u8 channel, float* temperature)
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

/*
 * doRelayRead:
 *	Read relay state
 ******************************************************************************************
 */
static void doRtdRead(int argc, char *argv[])
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
}

static void doHelp(int argc, char *argv[])
{
	int i = 0;
	if (argc == 3)
	{
		for (i = 0; i < CMD_ARRAY_SIZE; i++)
		{
			if ( (gCmdArray[i].name != NULL))
			{
				if (strcasecmp(argv[2], gCmdArray[i].name) == 0)
				{
					printf("%s%s%s%s", gCmdArray[i].help, gCmdArray[i].usage1,
						gCmdArray[i].usage2, gCmdArray[i].example);
					break;
				}
			}
		}
		if (CMD_ARRAY_SIZE == i)
		{
			printf("Option \"%s\" not found\n", argv[2]);
			usage();
		}
	}
	else
	{
		usage();
	}
}

static void doVersion(int argc, char *argv[])
{
	UNUSED(argc);
	UNUSED(argv);
	printf("rtd v%d.%d.%d Copyright (c) 2016 - 2020 Sequent Microsystems\n",
	VERSION_BASE, VERSION_MAJOR, VERSION_MINOR);
	printf("\nThis is free software with ABSOLUTELY NO WARRANTY.\n");
	printf("For details type: rtd -warranty\n");

}

//static void doList(int argc, char *argv[])
//{
//	int ids[8];
//	int i;
//	int cnt = 0;
//
//	UNUSED(argc);
//	UNUSED(argv);
//
//	for (i = 0; i < 8; i++)
//	{
//		if (boardCheck(RELAY8_HW_I2C_BASE_ADD + i) == OK)
//		{
//			ids[cnt] = i;
//			cnt++;
//		}
//	}
//	printf("%d board(s) detected\n", cnt);
//	if (cnt > 0)
//	{
//		printf("Id:");
//	}
//	while (cnt > 0)
//	{
//		cnt--;
//		printf(" %d", ids[cnt]);
//	}
//	printf("\n");
//}

/* 
 * Self test for production
 */
static void doBoard(int argc, char* argv[])
{
	int dev = 0;
	u8 buff[2] =
	{
		0,
		0};

	dev = doBoardInit(atoi(argv[1]));
	if (dev <= 0)
	{
		exit(1);
	}

	if (argc == 3)
	{
		if (FAIL == i2cMem8Read(dev, REVISION_MAJOR_MEM_ADD, buff, 2))
		{
			exit(1);
		}
		printf("Mega RTD firmware version %d.%02d\n", (int)buff[0], (int)buff[1]);
	}
	else
	{
		printf("Invalid arguments number! Usage: %s\n", CMD_BOARD.usage1);
	}

}

static void doWarranty(int argc UNU, char* argv[] UNU)
{
	printf("%s\n", warranty);
}

void usage(void)
{
	int i = 0;
	for (i = 0; i < CMD_ARRAY_SIZE; i++)
	{
		if (sizeof (gCmdArray[i].usage1) > 1)
		{
			printf("%s", gCmdArray[i].usage1);
		}
		if (sizeof (gCmdArray[i].usage2) > 1)
		{
			printf("%s", gCmdArray[i].usage2);
		}
	}
}

static void cliInit(void)
{
	int i = 0;

	memset(gCmdArray, 0, sizeof(CliCmdType) * CMD_ARRAY_SIZE);

	memcpy(&gCmdArray[i], &CMD_HELP, sizeof(CliCmdType));
	i++;
	memcpy(&gCmdArray[i], &CMD_WAR, sizeof(CliCmdType));
	i++;
//	memcpy(&gCmdArray[i], &CMD_LIST, sizeof(CliCmdType));
//	i++;
	memcpy(&gCmdArray[i], &CMD_READ, sizeof(CliCmdType));
	i++;
	memcpy(&gCmdArray[i], &CMD_BOARD, sizeof(CliCmdType));
	i++;
	memcpy(&gCmdArray[i], &CMD_VERSION, sizeof(CliCmdType));

}

int main(int argc, char *argv[])
{
	int i = 0;

	cliInit();

	if (argc == 1)
	{
		usage();
		return 1;
	}
	for (i = 0; i < CMD_ARRAY_SIZE; i++)
	{
		if ( (gCmdArray[i].name != NULL) && (gCmdArray[i].namePos < argc))
		{
			if (strcasecmp(argv[gCmdArray[i].namePos], gCmdArray[i].name) == 0)
			{
				gCmdArray[i].pFunc(argc, argv);
				return 0;
			}
		}
	}
	printf("Invalid command option\n");
	usage();

	return 0;
}
