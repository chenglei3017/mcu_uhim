#include "uhmi_bootloader.h"
#include "uhmi_debug.h"
#include "uhmi_def.h"
#include <errno.h>

BOOL read_version_ack = FALSE;
BOOL erase_flash_ack = FALSE;
BOOL program_flash_ack = FALSE;
BOOL read_crc_ack = FALSE;

const int tx_timeout = 5000;
const int tx_retry = 3;

BOOT_PARAMS_T g_boot_params;

char hexfile[64] = {0};
//int boot_major = 0;
//int boot_minor = 0;

void boot_rx_handler(u8 *packet, u32 plen)
{
    T_COMMANDS cmd = 0;

    cmd = packet[0];

    switch(cmd)
    {
        //add code ...
        
        case READ_BOOT_INFO:
            read_version_ack = TRUE;
//            boot_major = packet[2];
//            boot_minor = packet[1];
            g_uhmi_params.mcu.version.major = packet[2];
            g_uhmi_params.mcu.version.minor = packet[1];
            DPRINT(DBG_LEVEL_DEBUG, "major: %d, minor: %d\n", 
                g_uhmi_params.mcu.version.major, 
                g_uhmi_params.mcu.version.minor);
            break;

        case ERASE_FLASH:
            erase_flash_ack = TRUE;
            DPRINT(DBG_LEVEL_DEBUG, "ERASE_FLASH ok!\n");
            break;

        case PROGRAM_FLASH:
            program_flash_ack = TRUE;
            break;

        case READ_CRC:
            read_crc_ack = TRUE;
            break;

        default:
            break;
    }
    
}


static int boot_init(void)
{
#if 0    
    int ret = -1;
    char hexfile[32] = {0};
    sprintf(hexfile, "/mcu/app.%d.%d.%d.hex", 
        g_uhmi_params.hexver.major, g_uhmi_params.hexver.minor, g_uhmi_params.hexver.swver);
    if (0 == access(hexfile, F_OK))
    {
        ret = 0;
    }
    else
    {
        DPRINT(DBG_LEVEL_ERROR, "hex file not exist: %s\n", strerror(errno));
    }
    return ret;
#else
    return 0;
#endif
}

static int boot_read_info(void)
{
    int ret = -1;
    int retry = 0;
    int time_count = 0;
    int i = 0;
    
    
    u8 data[PACKET_SIZE] = {0};
    u32 dlen = 0;    
    data[dlen++] = READ_BOOT_INFO;

    read_version_ack = FALSE;
    uhmi_send_data(data, dlen);
    do
    {
        usleep(1000);
        if (TRUE == read_version_ack)
        {
            DPRINT(DBG_LEVEL_DEBUG, "read_version_ack ok\n");
            ret = 0;
        }
        else if (time_count++ >= tx_timeout)
        {
            if (++retry == tx_retry)
            {
                DPRINT(DBG_LEVEL_DEBUG, "read_version_ack timeout\n");
                break;
            }
            uhmi_send_data(data, dlen);
            time_count = 0;
        }
    }while(ret == -1);


    // if get major minor
    if (0 == ret)
    {
        ret = -1;
        for(i = 0; i < g_uhmi_params.mcu_hex_count; i++)
        {
            if ((g_uhmi_params.hexver[i].major == g_uhmi_params.mcu.version.major) &&
                (g_uhmi_params.hexver[i].minor == g_uhmi_params.mcu.version.minor))
            {
                if (MCU_MAJOR_ST446 == g_uhmi_params.hexver[i].major)
                {
                    sprintf(hexfile, "/mcu/app.%d.%d.%d.bin", 
                        g_uhmi_params.hexver[i].major, 
                        g_uhmi_params.hexver[i].minor, 
                        g_uhmi_params.hexver[i].swver);
                }
                else
                {
                    sprintf(hexfile, "/mcu/app.%d.%d.%d.hex", 
                        g_uhmi_params.hexver[i].major, 
                        g_uhmi_params.hexver[i].minor, 
                        g_uhmi_params.hexver[i].swver);
                }

                DPRINT(DBG_LEVEL_DEBUG, "hexfile: %s\n", hexfile);

                if (0 == access(hexfile, F_OK))
                {
                    ret = 0;
                }
                else
                {
                    DPRINT(DBG_LEVEL_ERROR, "hex file not exist: %s\n", strerror(errno));
                }
                
                break;
            }
        }
    }   

    return ret;    
}

static int boot_erase_flash(void)
{
    int ret = -1, retry = 0, time_count = 0;
    
    
    u8 data[PACKET_SIZE] = {0};
    u32 dlen = 0;    
    data[dlen++] = ERASE_FLASH;

    erase_flash_ack = FALSE;
    uhmi_send_data(data, dlen);
    do
    {
        usleep(1000);
        if (TRUE == erase_flash_ack)
        {
            DPRINT(DBG_LEVEL_DEBUG, "boot_erase_flash ok\n");
            ret = 0;
        }
        else if (time_count++ >= tx_timeout)
        {
            if (++retry == tx_retry)
            {
                DPRINT(DBG_LEVEL_DEBUG, "erase_flash_ack timeout\n");
                break;
            }
            uhmi_send_data(data, dlen);
            time_count = 0;
        }
    }while(ret == -1);

    return ret;    
}

/****************************************************************************
 * Converts ASCII to hex.
 *
 * \param  VdAscii: Hex Record in ASCII format.
 * \param  VdHexRec: Hex record in Hex format.
 * \param 
 * \return  Number of bytes in Hex record(Hex format)    
 *****************************************************************************/
u32 ConvertAsciiToHex(void *VdAscii, void *VdHexRec)
{
	char temp[5] = {'0','x', 0, 0, 0};
	u32 i = 0;
	char *Ascii;
	char *HexRec;

	Ascii = (char *)VdAscii;
	HexRec = (char *)VdHexRec;

	while(1)
	{
		temp[2] = Ascii[i++];
		temp[3] = Ascii[i++];
		if((temp[2] == 0) || (temp[3] == 0) || temp[2] == 0x0d)
		{
			// Not a valid ASCII. Stop conversion and break.
			i -= 2;
			break;			
		}
		else
		{
			// Convert ASCII to hex.
			sscanf(temp, "%x", HexRec);
			HexRec++;			
		}
	}

	return (i/2); // i/2: Because, an representing Hex in ASCII takes 2 bytes.
}


/****************************************************************************
 * Gets next hex record from the hex file
 *
 * \param  HexRec: Pointer to HexRec.
 * \param  BuffLen: Buffer Length 
 * \param 
 * \return Length of the hex record in bytes.  
 *****************************************************************************/
u32 GetNextHexRecord(FILE *fp, char *HexRec, unsigned int BuffLen)
{
	unsigned short len = 0;
    char Ascii[PACKET_SIZE] = {0};
	
	if(!feof(fp))
	{
		fgets(Ascii, BuffLen, fp);

		if(Ascii[0] != ':')
		{
			// Not a valid hex record.
			return 0;
		}
		// Convert rest to hex.
		len = ConvertAsciiToHex((void *)&Ascii[1], (void *)HexRec);

		//HexCurrLineNo++;		
	}	
	return len;
}

/**********************************
FAIL: return -1
SUCCEED: return 0
**********************************/
static int boot_program_flash_MCHIP(void)
{
    int ret = -1, retry = 0, time_count = 0;
    FILE *hexfp = NULL;
    u32 totalRecords = 10;
    u32 HexRecLen = 0;
    u32 write_count = 0;
    
    u8 data[PACKET_SIZE] = {0};
    u32 dlen = 0;


    // open mcu hex file
    printf("load mcu hex file: %s\n", hexfile);
    hexfp = fopen(hexfile, "r");
    if (NULL == hexfp)
    {
        DPRINT(DBG_LEVEL_ERROR, "cannot open hex file: %s\n", strerror(errno));
        goto pgm_exit;
    }

    while(1)
    {
        memset(data, 0, sizeof(data));
        totalRecords = 10;        
        dlen = 0;
        data[dlen++] = PROGRAM_FLASH;
        
        HexRecLen = GetNextHexRecord(hexfp, &data[dlen], (sizeof(data) - 5));
        if(HexRecLen == 0)
        {
            //Not a valid hex file.
            printf("\n");
            ret = 0;
            break;
        }
        dlen += HexRecLen;
        write_count += HexRecLen;
        
        while(totalRecords)
        {
            HexRecLen = GetNextHexRecord(hexfp, &data[dlen], (sizeof(data) - 5));
            dlen += HexRecLen;
            write_count += HexRecLen;
            totalRecords--;
        }

        if (debug_level&DBG_LEVEL_INFO)
        {
            printf("\rflash write_count: %d", write_count);
            fflush(stdout);
        }

        ret = -1;
        time_count = 0;
        retry = 0;
        program_flash_ack  = FALSE;
        uhmi_send_data(data, dlen);
        do
        {
            usleep(1000);
            if (TRUE == program_flash_ack)
            {
                ret = 0;
            }
            else if (time_count++ >= tx_timeout)
            {
                DPRINT(DBG_LEVEL_ERROR, "program_flash_ack timeout\n");
                if (++retry == tx_retry)
                {
                    DPRINT(DBG_LEVEL_ERROR, "boot_program_flash fail\n");
                    goto pgm_exit;
                }
                uhmi_send_data(data, dlen);
                time_count = 0;
            }
        }while(ret == -1);
        
    }    

pgm_exit:

    if (hexfp)
        fclose(hexfp);
    return ret;    
}


/**********************************
FAIL: return -1
SUCCEED: return 0
**********************************/
static int boot_program_flash_ST446(void)
{
    int ret = -1, retry = 0, time_count = 0;
    FILE *hexfp = NULL;

    u32 write_count = 0;
    u8 data[260] = {0};
    u32 dlen=0;

    u32 count = 0;
    u32 remainder = 0;

    // open mcu bin file
    printf("load mcu bin file: %s\n", hexfile);
    hexfp = fopen(hexfile, "r");
    if (NULL == hexfp)
    {
        DPRINT(DBG_LEVEL_ERROR, "cannot open bin file: %s\n", strerror(errno));
        goto pgm_exit;
    }

    while(1)
    {
        memset(data, 0xff, sizeof(data));
        dlen = 0;
        data[dlen++] = PROGRAM_FLASH;

        count = fread(&data[dlen], 1, 256, hexfp);

        // check whether fread success.
        if (count == 0)
        {
            // if fread 0 char, image loading complete.
            ret = 0;
            break;
        }
        else if (count != 256)
        {
            if (feof(hexfp) == 0)
                break;
        }

        // filling four bytes
        remainder = count%4;
        if (remainder)
        {
            count += (4-remainder);
        }

        dlen += count;
        write_count += count;

        if (debug_level&DBG_LEVEL_INFO)
        {
            printf("\rflash write_count: %d", write_count);
            fflush(stdout);
        }

        ret = -1;
        time_count = 0;
        retry = 0;
        program_flash_ack  = FALSE;
        uhmi_send_data(data, dlen);
        do
        {
            usleep(1000);
            if (TRUE == program_flash_ack)
            {
                ret = 0;
            }
            else if (time_count++ >= tx_timeout)
            {
                DPRINT(DBG_LEVEL_ERROR, "program_flash_ack timeout\n");
                if (++retry == tx_retry)
                {
                    DPRINT(DBG_LEVEL_ERROR, "boot_program_flash fail\n");
                    goto pgm_exit;
                }
                uhmi_send_data(data, count);
                time_count = 0;
            }
        }while(ret == -1);
    } 
pgm_exit:

    if (hexfp)
        fclose(hexfp);
    return ret;
}


/**********************************
FAIL: return -1
SUCCEED: return 0
**********************************/
static int boot_program_flash(const MCU_MAJOR_E vMajor)
{
    int ret = -1;
    if (MCU_MAJOR_PIC32EFE == vMajor)
    {
        ret = boot_program_flash_MCHIP();
    }
    else if(MCU_MAJOR_ST446 == vMajor)
    {
        ret = boot_program_flash_ST446();
    }
    else
    {
        DPRINT(DBG_LEVEL_ERROR, "Invalid major:%d\n", vMajor);
    }

    return ret;
}

void boot_task(void)
{
    DPRINT(DBG_LEVEL_DEBUG, "bootloader_task\n");

    //2 add code ...
    switch(g_boot_params.step)
    {
        case BOOT_INIT:
            DPRINT(DBG_LEVEL_DEBUG, "BOOT_INIT\n");
            if (boot_init() == 0)
            {
                g_boot_params.step = BOOT_READ_VERSION;
            }
            else
            {
                g_boot_params.step = BOOT_FAIL;
            }
            break;

        case BOOT_READ_VERSION:
            DPRINT(DBG_LEVEL_DEBUG, "BOOT_READ_VERSION\n");
            if (boot_read_info() == 0)
            {
                g_boot_params.step = BOOT_ERASE_FLASH;
            }
            else
            {
                g_boot_params.step = BOOT_FAIL;                
            }
            break;

        case BOOT_ERASE_FLASH:
            DPRINT(DBG_LEVEL_DEBUG, "BOOT_ERASE_FLASH\n");
            if (boot_erase_flash() == 0)
            {
                g_boot_params.step = BOOT_PROGRAM_FLASH;
            }
            else
            {
                g_boot_params.step = BOOT_FAIL;                
            }
            break;

        case BOOT_PROGRAM_FLASH:
            DPRINT(DBG_LEVEL_DEBUG, "BOOT_PROGRAM_FLASH\n");
            if (boot_program_flash(g_uhmi_params.mcu.version.major) == 0)
            {
                DPRINT(DBG_LEVEL_DEBUG, "boot_program_flash succeed\n");
                g_boot_params.step = BOOT_DONE;
            }
            else
            {
                g_boot_params.step = BOOT_FAIL;                
            }
            break;

        case BOOT_READ_CRC:
            DPRINT(DBG_LEVEL_DEBUG, "BOOT_READ_CRC\n");
            break;

        case BOOT_DONE:
            DPRINT(DBG_LEVEL_DEBUG, "BOOT_DONE\n");
            uhmi_jump_to_app();
            break;
        default:
            DPRINT(DBG_LEVEL_DEBUG, "BOOT Invaild Stauts\n");
            break;
    }
}

