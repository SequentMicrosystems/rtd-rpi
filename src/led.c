#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "led.h"
#include "comm.h"


#define LED_THRESHOLD_MIN -200
#define LED_THRESHOLD_MAX 300

const CliCmdType CMD_READ_LED_MODE =
	{
		"ledmrd",
		2,
		&doLedModeRead,
		"\tledmrd:     Read rtd led's mode: off<0>; low<1> (turn on if temperature above threshold); high<2> (turn on if temperature below threshold)\n",
		"\tUsage:      rtd <id> ledmrd <channel>\n",
		"",
		"\tExample:    rtd 0 ledmrd 2; Read the led mode on channel #2 on Board #0\n"};

const CliCmdType CMD_WRITE_LED_MODE =
	{
		"ledmwr",
		2,
		&doLedModeWrite,
		"\tledmwr:     Write rtd led's mode: off<0>; low<1> (turn on if temperature above threshold); high<2> (turn on if temperature below threshold)\n",
		"\tUsage:      rtd <id> ledmwr <channel> <value>\n",
		"",
		"\tExample:    rtd 0 ledmwr 2 1; Write the led mode on channel #2 on Board #0 to high\n"};

const CliCmdType CMD_READ_LED_TH =
	{
		"ledthrd",
		2,
		&doLedThresholdRead,
		"\tledthrd:    Read rtd led's threshold value in deg C [-200, 300]\n",
		"\tUsage:      rtd <id> ledthrd <channel>\n",
		"",
		"\tExample:    rtd 0 ledthrd 2; Read the led threshold on channel #2 on Board #0\n"};

const CliCmdType CMD_WRITE_LED_TH =
	{
		"ledthwr",
		2,
		&doLedThresholdWrite,
		"\tledthwr:    Write rtd led's threshold value in deg C [-200, 300]\n",
		"\tUsage:      rtd <id> ledthwr <channel> <value>\n",
		"",
		"\tExample:    rtd 0 ledthwr 2 10; Write the led threshold on channel #2 on Board #0 to 10 deg C\n"};

int ledGetMode(int dev, int ch, int* val)
{
	u8 buff[2];
	u16 readVal = 0;

	if (NULL == val)
	{
		return ERROR;
	}

	if (FAIL == i2cMem8Read(dev, RTD_LEDS_FUNC, buff, 2))
	{
		return ERROR;
	}
	memcpy(&readVal, buff, 2);
	*val = 0x03 & (readVal >> (2 * (ch - 1)));
	return OK;
}

int ledSetMode(int dev, int ch, int val)
{
	u8 buff[2];
	u16 readVal = 0;

	if ( (val > 2) || (val < 0))
	{
		return ERROR;
	}
	if (FAIL == i2cMem8Read(dev, RTD_LEDS_FUNC, buff, 2))
	{
		return ERROR;
	}
	memcpy(&readVal, buff, 2);

	readVal &= ~ ((u16)0x03 << (2 * (ch - 1)));
	readVal += val << (2 * (ch - 1));
	memcpy(buff, &readVal, 2);
	if (FAIL == i2cMem8Write(dev, RTD_LEDS_FUNC, buff, 2))
	{
		return ERROR;
	}
	return OK;
}

int ledGetThreshold(int dev, int ch, int* val)
{
	u8 buff[2];
	s16 readVal = 0;

	if (NULL == val)
	{
		return ERROR;
	}

	if (FAIL == i2cMem8Read(dev, RTD_LED_THRESHOLD1 + 2 * (ch - 1), buff, 2))
	{
		return ERROR;
	}
	memcpy(&readVal, buff, 2);
	*val = readVal;
	return OK;
}

int ledSetThreshold(int dev, int ch, int val)
{
	u8 buff[2];
	s16 readVal = 0;

	if ( (val < LED_THRESHOLD_MIN) || (val > LED_THRESHOLD_MAX))
	{
		printf("Threshold out of range!");
		return ERROR;
	}
	readVal = (s16)val;
	memcpy(buff, &readVal, 2);
	if (FAIL == i2cMem8Write(dev, RTD_LED_THRESHOLD1 + 2 * (ch - 1), buff, 2))
	{
		return ERROR;
	}
	return OK;
}

//******************************************

int doLedModeRead(int argc, char *argv[])
{
	int ch = 0;
	int val = 0;
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

		if (OK != ledGetMode(dev, ch, &val))
		{
			printf("Fail to read!\n");
			exit(1);
		}
		printf("%d\n", val);
	}
	else
	{
		printf("Invalid arguments number for %s cmd\n", argv[0]);
		exit(1);
	}
	return OK;
}

int doLedModeWrite(int argc, char *argv[])
{
	int ch = 0;
	int val = 0;
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

		val = atoi(argv[4]);

		if (OK != ledSetMode(dev, ch, val))
		{
			printf("Fail to write!\n");
			exit(1);
		}
	}
	else
	{
		printf("Invalid arguments number for %s cmd\n", argv[0]);
		exit(1);
	}
	return OK;
}

int doLedThresholdRead(int argc, char *argv[])
{
	int ch = 0;
	int val = 0;
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

		if (OK != ledGetThreshold(dev, ch, &val))
		{
			printf("Fail to read!\n");
			exit(1);
		}
		printf("%d\n", val);
	}
	else
	{
		printf("Invalid arguments number for %s cmd\n", argv[0]);
		exit(1);
	}
	return OK;
}

int doLedThresholdWrite(int argc, char *argv[])
{
	int ch = 0;
		int val = 0;
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

			val = atoi(argv[4]);

			if (OK != ledSetThreshold(dev, ch, val))
			{
				printf("Fail to write!\n");
				exit(1);
			}
		}
		else
		{
			printf("Invalid arguments number for %s cmd\n", argv[0]);
			exit(1);
		}
		return OK;
}
