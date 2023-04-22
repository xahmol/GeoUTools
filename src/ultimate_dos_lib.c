/*****************************************************************
Ultimate 64/II+ Command Library - DOS functions
Scott Hutter, Francesco Sblendorio

Based on ultimate_dos-1.2.docx and command interface.docx
https://github.com/markusC64/1541ultimate2/tree/master/doc

Disclaimer:  Because of the nature of DOS commands, use this code
soley at your own risk.

Patches and pull requests are welcome
******************************************************************/
#include <string.h>
#include "ultimate_common_lib.h"
#include "ultimate_dos_lib.h"

void uii_get_path(void)
{
	unsigned char cmd[] = {0x00,DOS_CMD_GET_PATH};	
	uii_settarget(TARGET_DOS1);
	uii_sendcommand(cmd, 2);
	uii_readdata();
	uii_readstatus();
	uii_accept();
}

void uii_open_dir(void)
{
	unsigned char cmd[] = {0x00,DOS_CMD_OPEN_DIR};
	uii_settarget(TARGET_DOS1);
	uii_sendcommand(cmd, 2);
	uii_readstatus();
	uii_accept();
}

void uii_get_dir(void)
{
	unsigned char cmd[] = {0x00,DOS_CMD_READ_DIR};
	int count = 0;
	uii_settarget(TARGET_DOS1);
	uii_sendcommand(cmd, 2);
}

void uii_change_dir(char* directory)
{
	int x = 0;
	unsigned char* fullcmd = (unsigned char *)malloc(strlen(directory)+2);
	fullcmd[0] = 0x00;
	fullcmd[1] = DOS_CMD_CHANGE_DIR;
	
	for(x=0;x<strlen(directory);x++)
		fullcmd[x+2] = directory[x];

	uii_settarget(TARGET_DOS1);
	uii_sendcommand(fullcmd, strlen(directory)+2);
	
	free(fullcmd);
	
	uii_readstatus();
	uii_accept();
}

void uii_change_dir_home(void)
{
	unsigned char cmd[] = {0x00,DOS_CMD_COPY_HOME_PATH};
	int count = 0;
	
	uii_settarget(TARGET_DOS1);
	uii_sendcommand(cmd, 2);
	uii_readstatus();
	uii_accept();
}

void uii_mount_disk(unsigned char id, char *filename)
{
	int x = 0;
	unsigned char* fullcmd = (unsigned char *)malloc(strlen(filename)+3);
	fullcmd[0] = 0x00;
	fullcmd[1] = DOS_CMD_MOUNT_DISK;
	fullcmd[2] = id;
	
	for(x=0;x<strlen(filename);x++)
		fullcmd[x+3] = filename[x];
	
	uii_settarget(TARGET_DOS1);
	uii_sendcommand(fullcmd, strlen(filename)+3);
	
	free(fullcmd);
	
	uii_readdata();
	uii_readstatus();
	uii_accept();
}

void uii_open_file(unsigned char attrib, char *filename)
{
	// Attrib will be:
	// 0x01 = Read
	// 0x02 = Write
	// 0x06 = Create new file
	// 0x0E = Create (overwriting an existing file)
	
	int x = 0;
	unsigned char* fullcmd = (unsigned char *)malloc(strlen(filename)+3);
	fullcmd[0] = 0x00;
	fullcmd[1] = DOS_CMD_OPEN_FILE;
	fullcmd[2] = attrib;
	
	for(x=0;x<strlen(filename);x++)
		fullcmd[x+3] = filename[x];
	
	uii_settarget(TARGET_DOS1);
	uii_sendcommand(fullcmd, strlen(filename)+3);
	
	free(fullcmd);
	
	uii_readdata();
	uii_readstatus();
	uii_accept();
}

void uii_close_file(void)
{
	unsigned char cmd[] = {0x00,DOS_CMD_CLOSE_FILE};
	
	uii_settarget(TARGET_DOS1);
	uii_sendcommand(cmd, 2);
	
	uii_readdata();
	uii_readstatus();
	uii_accept();
}

void uii_delete_file(char* filename)
{
	int x = 0;
	unsigned char* fullcmd = (unsigned char *)malloc(strlen(filename)+2);
	fullcmd[0] = 0x00;
	fullcmd[1] = DOS_CMD_DELETE_FILE;
	
	for(x=0;x<strlen(filename);x++)
		fullcmd[x+2] = filename[x];
	
	uii_settarget(TARGET_DOS1);
	uii_sendcommand(fullcmd, strlen(filename)+2);
	
	free(fullcmd);
	
	uii_readstatus();
	uii_accept();
}

void uii_get_deviceinfo(void)
{
	unsigned char cmd[] = {0x00,CTRL_CMD_DEVICE_INFO};
	
	uii_settarget(TARGET_CONTROL);
	uii_sendcommand(cmd, 2);

	uii_readdata();
	uii_readstatus();
	uii_accept();
}

void uii_get_ramdisk_info(void)
{
	unsigned char cmd[] = {0x00,CTRL_CMD_GET_RAMDISK_INFO};
	
	uii_settarget(TARGET_CONTROL);
	uii_sendcommand(cmd, 2);

	uii_readdata();
	uii_readstatus();
	uii_accept();
}

void uii_loadIntoRamDisk(unsigned char id, char *filename, unsigned char whatif)
{
	int x = 0;
	unsigned char* fullcmd = (unsigned char *)malloc(strlen(filename)+3);
	fullcmd[0] = 0x00;
	fullcmd[1] = DOS_CMD_LOAD_INTO_RAMDISK;
	fullcmd[2] = id + (128*whatif);
	
	for(x=0;x<strlen(filename);x++)
		fullcmd[x+3] = filename[x];
	
	uii_settarget(TARGET_DOS1);
	uii_sendcommand(fullcmd, strlen(filename)+3);
	
	free(fullcmd);
	
	uii_readdata();
	uii_readstatus();
	uii_accept();
}

void uii_saveRamDisk(unsigned char id, char *filename)
{
	int x = 0;
	unsigned char* fullcmd = (unsigned char *)malloc(strlen(filename)+3);
	fullcmd[0] = 0x00;
	fullcmd[1] = DOS_CMD_SAVE_RAMDISK;
	fullcmd[2] = id;
	
	for(x=0;x<strlen(filename);x++)
		fullcmd[x+3] = filename[x];
	
	uii_settarget(TARGET_DOS1);
	uii_sendcommand(fullcmd, strlen(filename)+3);
	
	free(fullcmd);
	
	uii_readdata();
	uii_readstatus();
	uii_accept();
}

void uii_save_reu(unsigned char size)
{
	// Function to save REU memory to REU file
	// Size is memory length to save in 64k blocks - 1

	unsigned char cmd[] = {0x00,DOS_CMD_SAVE_REU,0x00,0x00,0x00,0x00,0xff,0xff,0x00,0x00};

	cmd[8] = size;
	uii_settarget(TARGET_DOS1);
	uii_sendcommand(cmd,10);
	uii_readdata();
	uii_readstatus();
	uii_accept();
}
