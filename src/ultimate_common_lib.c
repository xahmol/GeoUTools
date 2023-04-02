/*****************************************************************
Ultimate 64/II+ Command Library - Core functions
Scott Hutter, Francesco Sblendorio

Based on ultimate_dos-1.2.docx and command interface.docx
https://github.com/markusC64/1541ultimate2/tree/master/doc

Disclaimer:  Because of the nature of DOS commands, use this code
soley at your own risk.

Patches and pull requests are welcome
******************************************************************/

#include <string.h>
#include "ultimate_common_lib.h"

unsigned char *id_reg = (unsigned char *)ID_REG;
unsigned char *cmddatareg = (unsigned char *)CMD_DATA_REG;
unsigned char *controlreg = (unsigned char *)CONTROL_REG;
unsigned char *statusreg = (unsigned char *)STATUS_REG;
unsigned char *respdatareg = (unsigned char *)RESP_DATA_REG;
unsigned char *statusdatareg = (unsigned char *)STATUS_DATA_REG;

char uii_status[STATUS_QUEUE_SZ+1];
char uii_data[(DATA_QUEUE_SZ*2)+1];
char temp_string_onechar[2];
int uii_data_index;
int uii_data_len;

unsigned char uii_target = TARGET_DOS1;

// Core functions
unsigned char uii_detect(void)
{
	// Detect present of UCI via ID_REG. Value should be $C9
	if(*id_reg == 0xc9) {
		// Reset UCI
		uii_abort();
		
		// Return 1 for detected = true
		return 1;
	} else {
		// Return 0 for detected = false
		return 0;
	}
}

void uii_logtext(char *text)
{
#ifdef DEBUG
	printf("%s", text);
#else
	text = 0;  // to eliminate the warning in cc65
#endif
}

void uii_logstatusreg(void)
{
#ifdef DEBUG
	printf("\nstatus reg %p = %d",statusreg, *statusreg);
#endif
}

void uii_settarget(unsigned char id)
{
	uii_target = id;
}

void uii_freeze(void)
{
	unsigned char cmd[] = {0x00,0x05};
	
	uii_settarget(TARGET_CONTROL);
	
	uii_sendcommand(cmd, 2);
	uii_readdata();
	uii_readstatus();
	uii_accept();
	
}

void uii_identify(void)
{
	unsigned char cmd[] = {0x00,DOS_CMD_IDENTIFY};
	uii_settarget(TARGET_DOS1);
	uii_sendcommand(cmd, 2);
	uii_readdata();
	uii_readstatus();
	uii_accept();
}

void uii_echo(void)
{
	unsigned char cmd[] = {0x00,DOS_CMD_ECHO};
	uii_settarget(TARGET_DOS1);
	uii_sendcommand(cmd, 2);

	uii_readdata();
	uii_readstatus();
	uii_accept();
}

void uii_getinterfacecount(void)
{
	unsigned char tempTarget = uii_target;
	unsigned char cmd[] = {0x00,NET_CMD_GET_INTERFACE_COUNT};
	
	uii_settarget(TARGET_NETWORK);
	uii_sendcommand(cmd, 0x02);

	uii_readdata();
	uii_readstatus();
	uii_accept();
	
	uii_target = tempTarget;
}

void uii_sendcommand(unsigned char *bytes, int count)
{
	int x =0;
	int success = 0;
	
	bytes[0] = uii_target;
	
	while(success == 0)
	{
		// Wait for idle state
		uii_logtext("\nwaiting for cmd_busy to clear...");
		uii_logstatusreg();
		
		while (*statusreg & 0x35) {
			uii_logtext("\nwaiting...");
			uii_logstatusreg();
		};
		
		// Write byte by byte to data register
		uii_logtext("\nwriting command...");
		while(x<count)
			*cmddatareg = bytes[x++];
		
		// Send PUSH_CMD
		uii_logtext("\npushing command...");
		*controlreg |= 0x01;
		
		uii_logstatusreg();
		
		// check ERROR bit.  If set, clear it via ctrl reg, and try again
		if ((*statusreg & 4) == 4)
		{
			uii_logtext("\nerror was set. trying again");
			*controlreg |= 0x08;
		}
		else
		{
			uii_logstatusreg();
			
			// check for cmd busy
			while ( ((*statusreg & 32) == 0) && ((*statusreg & 16) == 16) )
			{
				uii_logtext("\nstate is busy");
			}
			success = 1;
		}
	}
	
	uii_logstatusreg();
	uii_logtext("\ncommand sent");
	
}

void uii_accept(void)
{
	// Acknowledge the data
	uii_logstatusreg();
	uii_logtext("\nsending ack");
	*controlreg |= 0x02;
	while ( !(*statusreg & 2) == 0 )  {
		uii_logtext("\nwaiting for ack...");
		uii_logstatusreg();
	};
}

int uii_isdataavailable(void)
{
	if ( ((*statusreg & 128) == 128 ) )
		return 1;
	else
		return 0;
}

int uii_isstatusdataavailable(void)
{
	if ( ((*statusreg & 64) == 64 ) )
		return 1;
	else
		return 0;
}

void uii_abort(void)
{
	// abort the command
	uii_logstatusreg();
	uii_logtext("\nsending abort");
	*controlreg |= 0x04;
}

int uii_readdata(void) 
{
	int count = 0;
	uii_data[0] = 0;
	uii_logtext("\n\nreading data...");
	uii_logstatusreg();

	// If there is data to read
	while (uii_isdataavailable() && count<DATA_QUEUE_SZ*2)
	{
		uii_data[count++] = *respdatareg;
	}
	uii_data[count] = 0;
	return count;
}

int uii_readstatus(void) 
{
	int count = 0;
	uii_status[0] = 0;
	
	uii_logtext("\n\nreading status...");
	uii_logstatusreg();

	while(uii_isstatusdataavailable() && count<STATUS_QUEUE_SZ)
	{
		uii_status[count++] = *statusdatareg;
	}
	
	uii_status[count] = 0;
	return count;
}