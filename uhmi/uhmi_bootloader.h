#ifndef __UHMI_BOOTLOADER_H__
#define __UHMI_BOOTLOADER_H__

#include "uhmi_def.h"


typedef enum {
	READ_BOOT_INFO = 1,
	ERASE_FLASH, 
	PROGRAM_FLASH,
	READ_CRC,
	JMP_TO_APP	
}T_COMMANDS;

typedef enum 
{
    BOOT_INIT = 0,
    BOOT_READ_VERSION,
    BOOT_ERASE_FLASH,
    BOOT_PROGRAM_FLASH,
    BOOT_READ_CRC,
    BOOT_DONE,
    BOOT_FAIL
}BOOT_STEP_E;

typedef struct {
    BOOT_STEP_E step;
}BOOT_PARAMS_T;

extern void boot_rx_handler(u8 *packet, u32 plen);
extern void boot_task(void);

extern BOOT_PARAMS_T g_boot_params;

#endif


