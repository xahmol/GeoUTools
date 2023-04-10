/*****************************************************************
Ultimate 64/II+ Command Library - DOS functions
Scott Hutter, Francesco Sblendorio

Based on ultimate_dos-1.2.docx and command interface.docx
https://github.com/markusC64/1541ultimate2/tree/master/doc

Disclaimer:  Because of the nature of DOS commands, use this code
soley at your own risk.

Patches and pull requests are welcome
******************************************************************/

#ifndef _ULTIMATE_DOS_LIB_H_
#define _ULTIMATE_DOS_LIB_H_

// prototypes

void uii_get_path(void);
void uii_open_dir(void);
void uii_get_dir(void);
void uii_change_dir(char* directory);
void uii_create_dir(char *directory);
void uii_change_dir_home(void);
void uii_mount_disk(unsigned char id, char *filename);
void uii_unmount_disk(unsigned char id);
void uii_swap_disk(unsigned char id);
void uii_open_file(unsigned char attrib, char *filename);
void uii_close_file(void);
void uii_read_file(unsigned char length);
void uii_write_file(unsigned char* data, int length);
void uii_delete_file(char* filename);
void uii_rename_file(char* filename, char* newname);
void uii_copy_file(char* sourcefile, char* destfile);
void uii_enable_drive_a(void);
void uii_disable_drive_a(void);
void uii_enable_drive_b(void);
void uii_disable_drive_b(void);
void uii_get_drive_a_power(void);
void uii_get_drive_b_power(void);
void uii_get_deviceinfo(void);
void uii_get_ramdisk_info(void);
void uii_loadIntoRamDisk(unsigned char id, char *filename);
void uii_load_reu(unsigned char size);

#endif
