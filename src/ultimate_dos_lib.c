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

void uii_create_dir(char *directory)
{
	int x = 0;
	unsigned char* fullcmd = (unsigned char *)malloc(strlen(directory)+2);
	fullcmd[0] = 0x00;
	fullcmd[1] = DOS_CMD_CREATE_DIR;
	
	for(x=0;x<strlen(directory);x++)
		fullcmd[x+2] = directory[x];
	
	uii_settarget(TARGET_DOS1);
	uii_sendcommand(fullcmd, strlen(directory)+2);
	
	free(fullcmd);
	
	uii_readdata();
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

void uii_unmount_disk(unsigned char id)
{
	unsigned char cmd[] = {0x00, DOS_CMD_UMOUNT_DISK, 0x00};

	cmd[2] = id;

	uii_settarget(TARGET_DOS1);
	uii_sendcommand(cmd, 3);

	uii_readdata();
	uii_readstatus();
	uii_accept();
}

void uii_swap_disk(unsigned char id)
{
	unsigned char cmd[] = {0x00,DOS_CMD_SWAP_DISK, 0x00};	

	cmd[2] = id;
	
	uii_settarget(TARGET_DOS1);
	uii_sendcommand(cmd, 3);

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

void uii_read_file(unsigned char length)
{
	unsigned char cmd[] = {0x00,DOS_CMD_READ_DATA, 0x00, 0x00};
	
	cmd[2] = length & 0xFF;
	cmd[3] = length >> 8;
	
	uii_settarget(TARGET_DOS1);
	uii_sendcommand(cmd, 2);
	
	// As with _get_dir(), read this in a loop, and _accept() the data
	// in order to get the next packet
	//
	// each data packet is 512 bytes each
}

void uii_write_file(unsigned char* data, int length)
{
	int x = 0;
	unsigned char *fullcmd = (unsigned char *)malloc(length+4);
	fullcmd[0] = 0x00;
	fullcmd[1] = DOS_CMD_WRITE_DATA;
	fullcmd[2] = 0x00;
	fullcmd[3] = 0x00;
	
	for(x=0;x<length;x++)
		fullcmd[x+4] = data[x];
	
	uii_settarget(TARGET_DOS1);
	uii_sendcommand(fullcmd, length+4);

	free(fullcmd);

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

void uii_rename_file(char* filename, char* newname)
{
	int x = 0;
	int y = 0;
	unsigned char* fullcmd = (unsigned char *)malloc(strlen(filename)+ 3 + strlen(newname));
	fullcmd[0] = 0x00;
	fullcmd[1] = DOS_CMD_RENAME_FILE;
	
	for(x=0;x<strlen(filename);x++)
		fullcmd[x+2] = filename[x];
	
	fullcmd[x++] = 0x00;
	
	for(;x<strlen(newname);x++)
		fullcmd[x] = newname[y++];
	
	uii_settarget(TARGET_DOS1);
	uii_sendcommand(fullcmd, strlen(filename)+ 3 + strlen(newname));
	
	free(fullcmd);
	
	uii_readstatus();
	uii_accept();
}

void uii_copy_file(char* sourcefile, char* destfile)
{
	int x = 0;
	int y = 0;
	unsigned char* fullcmd = (unsigned char *)malloc(strlen(sourcefile)+ 3 + strlen(destfile));
	fullcmd[0] = 0x00;
	fullcmd[1] = DOS_CMD_COPY_FILE;
	
	for(x=0;x<strlen(sourcefile);x++)
		fullcmd[x+2] = sourcefile[x];
	
	fullcmd[x++] = 0x00;
	
	for(;x<strlen(destfile);x++)
		fullcmd[x] = destfile[y++];
	
	uii_settarget(TARGET_DOS1);
	uii_sendcommand(fullcmd, strlen(sourcefile)+ 3 + strlen(destfile));
	
	free(fullcmd);
	
	uii_readstatus();
	uii_accept();
}

void uii_enable_drive_a(void)
{
	unsigned char cmd[] = {0x00,CTRL_CMD_ENABLE_DISK_A};

	uii_settarget(TARGET_CONTROL);
	uii_sendcommand(cmd, 2);

	uii_readdata();
	uii_readstatus();
	uii_accept();
}

void uii_disable_drive_a(void)
{
#define CTRL_CMD_DISABLE_DISK_A	0x31
	unsigned char cmd[] = {0x00,CTRL_CMD_DISABLE_DISK_A};

	uii_settarget(TARGET_CONTROL);
	uii_sendcommand(cmd, 2);

	uii_readdata();
	uii_readstatus();
	uii_accept();
}

void uii_enable_drive_b(void)
{
	unsigned char cmd[] = {0x00,CTRL_CMD_ENABLE_DISK_B};

	uii_settarget(TARGET_CONTROL);
	uii_sendcommand(cmd, 2);

	uii_readdata();
	uii_readstatus();
	uii_accept();
}

void uii_disable_drive_b(void)
{
	unsigned char cmd[] = {0x00,CTRL_CMD_DISABLE_DISK_B};

	uii_settarget(TARGET_CONTROL);
	uii_sendcommand(cmd, 2);

	uii_readdata();
	uii_readstatus();
	uii_accept();
}

void uii_get_drive_a_power(void) 
{
	unsigned char cmd[] = {0x00,CTRL_CMD_DRIVE_A_POWER};

	uii_settarget(TARGET_CONTROL);
	uii_sendcommand(cmd, 2);

	uii_readdata();
	uii_readstatus();
	uii_accept();
}

void uii_get_drive_b_power(void) 
{
	unsigned char cmd[] = {0x00,CTRL_CMD_DRIVE_B_POWER};
	
	uii_settarget(TARGET_CONTROL);
	uii_sendcommand(cmd, 2);

	uii_readdata();
	uii_readstatus();
	uii_accept();
}

void uii_load_reu(unsigned char size)
{
	// REU sizes on UII+:
	// 0 = 128 KB
	// 1 = 256 KB
	// 2 = 512 KB
	// 3 = 1 MB
	// 4 = 2 MB
	// 5 = 4 MB
	// 6 = 8 MB
	// 7 = 16 MB

	unsigned char cmd[] = {0x00,DOS_CMD_LOAD_REU,0x00,0x00,0x00,0x00,0xff,0xff,0xff,0x00};
	unsigned char sizes[8] = {0x01,0x03,0x07,0x0f,0x1f,0x3f,0x7f,0xff};

	cmd[8] = sizes[size];

	uii_settarget(TARGET_DOS1);
	uii_sendcommand(cmd,10);
	uii_readdata();
	uii_readstatus();
	uii_accept();
}