/*******************************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    bootloader.c
    
  Summary:
    Interface for the Bootloader library.

  Description:
    This file contains the interface definition for the Bootloader library.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2014 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*******************************************************************************/
//DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Macro Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
// *****************************************************************************
// Section: Include Files
// *****************************************************************************
// *****************************************************************************
#include <sys/attribs.h>

#include "bootloader/src/bootloader.h"
#include "peripheral/nvm/plib_nvm.h"
#include "peripheral/int/plib_int.h"
#include "system/devcon/sys_devcon.h"
#include "system/reset/sys_reset.h"

#if(SYS_FS_MAX_FILES > 0)
uint8_t fileBuffer[512] __attribute__((coherent, aligned(16)));
#endif

BOOTLOADER_DATA bootloaderData __attribute__((coherent, aligned(16)));
#if defined(BOOTLOADER_LEGACY)
BOOTLOADER_STATES (*BootloaderTriggerCheck)(void) = NULL; // Start as NULL
#endif

void Bootloader_BufferEventHandler(DATASTREAM_BUFFER_EVENT buffEvent,
                            DATASTREAM_BUFFER_HANDLE hBufferEvent,
                            uint16_t context );

#if(BOOTLOADER_STATE_SAVE == 1)
/********************************************************************
* Function:     SwitchFlashPanels()
*
* Precondition:
*
* Input:        None.
*
* Output:
*
* Side Effects: No return from here.
*
* Overview:     The function will swap the flash panels in a safe
 *              place and jump to the application.
*
*
* Note:         This is placed in the .cache_init section in order
 *              to have it in boot flash rather than program flash.
********************************************************************/
void __longcall__ __attribute__ ((section (".cache_init"))) SwapFlashPanels(void)
{
    void (*fptr)(void);

    PLIB_NVM_MemoryModifyInhibit(NVM_ID_0);
    PLIB_NVM_FlashWriteKeySequence(NVM_ID_0, NVM_PROGRAM_UNLOCK_KEY1);
    PLIB_NVM_FlashWriteKeySequence(NVM_ID_0, NVM_PROGRAM_UNLOCK_KEY2);
    PLIB_NVM_ProgramFlashBank2LowerRegion(NVM_ID_0);

    /* Disable Global Interrupts */
    PLIB_INT_Disable(INT_ID_0);
    fptr = (void (*)(void))BOOTLOADER_RESET_ADDRESS;
    fptr();
}
#endif

/********************************************************************
* Function:     ConvertAsciiToHex()
*
* Precondition:
*
* Input:        ASCII buffer and hex buffer.
*
* Output:
*
* Side Effects: No return from here.
*
* Overview:     Converts ASCII to Hex.
*
*
* Note:         None.
********************************************************************/
void ConvertAsciiToHex(uint8_t* asciiRec, uint8_t* hexRec)
{
    uint8_t i = 0;
    uint8_t k = 0;
    uint8_t hex;


    while((asciiRec[i] >= 0x30) && (asciiRec[i] <= 0x66))
    {
            // Check if the ASCII values are in alpha numeric range.

            if(asciiRec[i] < 0x3A)
            {
                    // Numerical representation in ASCII found.
                    hex = asciiRec[i] & 0x0F;
            }
            else
            {
                    // Alphabetical value.
                    hex = 0x09 + (asciiRec[i] & 0x0F);
            }

            // Following logic converts 2 bytes of ASCII to 1 byte of hex.
            k = i%2;

            if(k)
            {
                    hexRec[i>>1] |= hex;

            }
            else
            {
                    hexRec[i>>1] = (hex << 4) & 0xF0;
            }
            i++;
    }

}

/**
 * Static table used for the table_driven implementation.
 *****************************************************************************/
static const uint16_t crc_table[16] =
{
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
    0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef
};

/********************************************************************
* Function:     CalculateCrc()
*
* Precondition:
*
* Input:        Data pointer and data length
*
* Output:       CRC.
*
* Side Effects: None.
*
* Overview:     Calculates CRC for the given data and len
*
*
* Note:         None.
********************************************************************/
uint32_t APP_CalculateCrc(uint8_t *data, uint32_t len)
{
    uint32_t i;
    uint16_t crc = 0;
    
    while(len--)
    {
        i = (crc >> 12) ^ (*data >> 4);
        crc = crc_table[i & 0x0F] ^ (crc << 4);
        i = (crc >> 12) ^ (*data >> 0);
        crc = crc_table[i & 0x0F] ^ (crc << 4);
        data++;
    }

    return (crc & 0xFFFF);
}
/******************************************************************************
  Function:
    SYS_MODULE_OBJ Bootloader_Initialize(const SYS_MODULE_INDEX   moduleIndex,
                              const SYS_MODULE_INIT    * const moduleInit)

  Summary:
    Initializes primitive data structures for the general features
    of the primitive layer.

  Description:
    Initializes external and internal data structure for the general
    features of the primitive layer.

    This function must be called at system initialization.

  Remarks:
    None.
*/
void Bootloader_Initialize ( const BOOTLOADER_INIT *drvBootloaderInit )
{
    /* Place the App state machine in it's initial state. */
    bootloaderData.currentState = BOOTLOADER_STATE_INIT;
    bootloaderData.cmdBufferLength = 0;
    bootloaderData.streamHandle = DRV_HANDLE_INVALID;
    bootloaderData.datastreamStatus = DRV_CLIENT_STATUS_ERROR;
    bootloaderData.usrBufferEventComplete = false;

    bootloaderData.type = drvBootloaderInit->drvType;
#ifdef BOOTLOADER_LEGACY
    BootloaderTriggerCheck = drvBootloaderInit->drvTrigger;
#endif
    
    bootloaderData.FlashEraseFunc = (BOOTLOADER_CALLBACK)NULL;
    bootloaderData.StartAppFunc = (BOOTLOADER_CALLBACK)NULL;
    bootloaderData.BlankCheckFunc = (BOOTLOADER_CALLBACK)NULL;
    bootloaderData.ProgramCompleteFunc = (BOOTLOADER_CALLBACK)NULL;
    bootloaderData.ForceBootloadFunc = (BOOTLOADER_CALLBACK)NULL;
}

void BOOTLOADER_FlashEraseRegister(BOOTLOADER_CALLBACK newFunc)
{
    bootloaderData.FlashEraseFunc = newFunc;
}

void BOOTLOADER_StartAppRegister(BOOTLOADER_CALLBACK newFunc)
{
    bootloaderData.StartAppFunc = newFunc;
}

void BOOTLOADER_BlankCheckRegister(BOOTLOADER_CALLBACK newFunc)
{
    bootloaderData.BlankCheckFunc = newFunc;
}

void BOOTLOADER_ProgramCompleteRegister(BOOTLOADER_CALLBACK newFunc)
{
    bootloaderData.ProgramCompleteFunc = newFunc;
}

void BOOTLOADER_ForceBootloadRegister(BOOTLOADER_CALLBACK newFunc)
{
    bootloaderData.ForceBootloadFunc = newFunc;
}

// *****************************************************************************
/* Function:
    void Bootloader_Tasks (SYS_MODULE_INDEX index);

  Summary:
    Maintains the Bootloader module state machine. It manages the Bootloader Module object list
    items and responds to Bootloader Module primitive events.

*/
void Bootloader_Tasks ()
{
    size_t BuffLen=0;
    uint16_t crc;
    unsigned int i;
#if !defined(BOOTLOADER_STATE_SAVE)
    void (*fptr)(void);
#endif

    /* Check the application state*/
    switch ( bootloaderData.currentState )
    {
        case BOOTLOADER_STATE_INIT:
        {
            bootloaderData.currentState = BOOTLOADER_CHECK_FOR_TRIGGER;
            break;
        }

        case BOOTLOADER_CHECK_FOR_TRIGGER:
        {
#if defined(BOOTLOADER_LEGACY)
            if(BootloaderTriggerCheck != NULL)
            {
                 bootloaderData.currentState = BootloaderTriggerCheck(); //Perform Trig Check
            }
            else
#endif
            if ((bootloaderData.ForceBootloadFunc != NULL) &&
                    (1 == bootloaderData.ForceBootloadFunc()))
                bootloaderData.currentState = BOOTLOADER_OPEN_DATASTREAM;
            else
                bootloaderData.currentState = BOOTLOADER_CHECK_FOR_PROGRAM;
            break;
        }

        case BOOTLOADER_CHECK_FOR_PROGRAM:
        {
            /* Check if the User reset address is erased. */
            if (0xFFFFFFFF == *(unsigned int *)BOOTLOADER_RESET_ADDRESS)
            {
                bootloaderData.currentState = BOOTLOADER_OPEN_DATASTREAM;
            }
            else
            {
                /* User reset address is not erased. Start program. */
                bootloaderData.currentState = BOOTLOADER_ENTER_APPLICATION;
            }
            break;
        }

        case BOOTLOADER_OPEN_DATASTREAM:
        {
            
            bootloaderData.streamHandle = DATASTREAM_Open(
                    DRV_IO_INTENT_READWRITE | DRV_IO_INTENT_NONBLOCKING);

            if (bootloaderData.streamHandle != DRV_HANDLE_INVALID )
            {
                
                if(bootloaderData.type != TYPE_USB_HOST)
                {
                DATASTREAM_BufferEventHandlerSet(bootloaderData.streamHandle,
                        Bootloader_BufferEventHandler, APP_USR_CONTEXT);
                bootloaderData.currentState = BOOTLOADER_GET_COMMAND;
                }
                else
                {
                /* Host layer was opened successfully. Enable operation
                * and then wait for operation to be enabled  */
                bootloaderData.currentState = BOOTLOADER_WAIT_FOR_HOST_ENABLE;
                }
            }
            break;
        }

        case BOOTLOADER_PROCESS_COMMAND:
        {
            Bootloader_ProcessBuffer(&bootloaderData);
            break;
        }

        case BOOTLOADER_GET_COMMAND:
        {
            /* Get the datastream driver status */
            bootloaderData.datastreamStatus = DATASTREAM_ClientStatus( bootloaderData.streamHandle );
            /* Check if client is ready or not */
            if ( bootloaderData.datastreamStatus == DRV_CLIENT_STATUS_READY )
            {
                bootloaderData.bufferSize = 512;
                
                 DATASTREAM_Data_Read( &(bootloaderData.datastreamBufferHandle),
                        bootloaderData.data.buffers.buff1, bootloaderData.bufferSize);

                if ( bootloaderData.datastreamBufferHandle == DRV_HANDLE_INVALID )
                {
                    /* Set the app state to invalid */
                    bootloaderData.currentState = BOOTLOADER_ERROR;
                }
                else
                {
                    /* Set the App. state to wait for done */
                    bootloaderData.prevState    = BOOTLOADER_GET_COMMAND;
                    bootloaderData.currentState = BOOTLOADER_WAIT_FOR_DONE;
                }
            }
            break;
        }

        case BOOTLOADER_WAIT_FOR_DONE:
        {
            /* check if the datastream buffer event is complete or not */
            if (bootloaderData.usrBufferEventComplete)
            {
                bootloaderData.usrBufferEventComplete = false;
                
                /* Get the next App. State */
                switch (bootloaderData.prevState)
                {
                    case BOOTLOADER_GET_COMMAND:
                        bootloaderData.currentState = BOOTLOADER_PROCESS_COMMAND;
                        break;
                    case BOOTLOADER_SEND_RESPONSE:
                    default:
                        bootloaderData.currentState = BOOTLOADER_GET_COMMAND;
                        break;
                }
            }
            break;
        }

    case BOOTLOADER_WAIT_FOR_NVM:
       if (PLIB_NVM_FlashWriteCycleHasCompleted(NVM_ID_0))
       {
         
         bootloaderData.currentState = BOOTLOADER_SEND_RESPONSE;
         PLIB_NVM_MemoryModifyInhibit(NVM_ID_0);
       }
       break;

    case BOOTLOADER_SEND_RESPONSE:
        {
            if(bootloaderData.bufferSize)
            {
                /* Calculate the CRC of the response*/
                crc = APP_CalculateCrc(bootloaderData.data.buffers.buff1, bootloaderData.bufferSize);
                bootloaderData.data.buffers.buff1[bootloaderData.bufferSize++] = (uint8_t)crc;
                bootloaderData.data.buffers.buff1[bootloaderData.bufferSize++] = (crc>>8);

                bootloaderData.data.buffers.buff2[BuffLen++] = SOH;

                for (i = 0; i < bootloaderData.bufferSize; i++)
                {
                    if ((bootloaderData.data.buffers.buff1[i] == EOT) || (bootloaderData.data.buffers.buff1[i] == SOH)
                        || (bootloaderData.data.buffers.buff1[i] == DLE))
                    {
                        bootloaderData.data.buffers.buff2[BuffLen++] = DLE;
                    }
                    bootloaderData.data.buffers.buff2[BuffLen++] = bootloaderData.data.buffers.buff1[i];
                }

                bootloaderData.data.buffers.buff2[BuffLen++] = EOT;
                bootloaderData.bufferSize = 0;

                DATASTREAM_Data_Write( &(bootloaderData.datastreamBufferHandle),
                        bootloaderData.data.buffers.buff2, BuffLen);

                if ( bootloaderData.datastreamBufferHandle == DRV_HANDLE_INVALID )
                {
                    bootloaderData.currentState = BOOTLOADER_ERROR;
                }
                else
                {
                    bootloaderData.prevState = BOOTLOADER_SEND_RESPONSE;
                    bootloaderData.currentState = BOOTLOADER_WAIT_FOR_DONE;
                }
            }
            break;
        }

        case BOOTLOADER_WAIT_FOR_HOST_ENABLE:

            /* Check if the host operation has been enabled */
            if(DATASTREAM_ClientStatus(bootloaderData.streamHandle) == DRV_CLIENT_STATUS_READY)
            {
                /* This means host operation is enabled. We can
                 * move on to the next state */
                 DATASTREAM_BufferEventHandlerSet((DRV_HANDLE)bootloaderData.streamHandle, NULL, 0);
                 bootloaderData.currentState = BOOTLOADER_WAIT_FOR_DEVICE_ATTACH;
            }
            break;

        case BOOTLOADER_WAIT_FOR_DEVICE_ATTACH:
            /* Wait for device attach. The state machine will move
             * to the next state when the attach event
             * is received.  */
            break;

#if(SYS_FS_MAX_FILES > 0)
        case BOOTLOADER_DEVICE_CONNECTED:

            /* Turn on LED to indicate connection. */
            //BSP_LEDOn(BSP_LED_2);
            /* Device was connected. We can try opening the file */
            bootloaderData.currentState = BOOTLOADER_OPEN_FILE;
            break;

        case BOOTLOADER_OPEN_FILE:

            /* Try opening the file for reading */
            bootloaderData.fileHandle = SYS_FS_FileOpen("/mnt/myDrive1/" BOOTLOADER_IMAGE_FILE_NAME, (SYS_FS_FILE_OPEN_READ));
            if(bootloaderData.fileHandle == SYS_FS_HANDLE_INVALID)
            {
                /* Could not open the file. Error out*/
                bootloaderData.currentState = BOOTLOADER_ERROR;
            }
            else
            {
                /* File opened successfully. Read file */
                APP_FlashErase();
                while (!PLIB_NVM_FlashWriteCycleHasCompleted(NVM_ID_0));
                APP_NVMClearError();
                PLIB_NVM_MemoryModifyInhibit(NVM_ID_0);
                bootloaderData.currentState = BOOTLOADER_READ_FILE;

            }
            break;

        case BOOTLOADER_READ_FILE:

            /* Try reading the file */
            BuffLen = DATASTREAM_Data_Read(NULL, (void*)fileBuffer, 512);
            if (BuffLen <= 0)
            {
                SYS_FS_FileClose(bootloaderData.fileHandle);
                bootloaderData.currentState = BOOTLOADER_CLOSE_DATASTREAM;
            }
            else
            {
                memcpy(&bootloaderData.data.buffer[bootloaderData.cmdBufferLength], fileBuffer, BuffLen);
                
                /* Process the buffer that we read. */
                bootloaderData.bufferSize = BuffLen;
                Bootloader_ProcessBuffer(&bootloaderData);
            }

            break;
#endif
        case BOOTLOADER_CLOSE_DATASTREAM:
            DATASTREAM_Close();
        case BOOTLOADER_ENTER_APPLICATION:
#if(BOOTLOADER_STATE_SAVE == 1)
            SYS_DEVCON_InstructionCacheFlush();
            SwapFlashPanels();
#else
            
            /* Disable Global Interrupts */
            PLIB_INT_Disable(INT_ID_0);
            if (bootloaderData.StartAppFunc != NULL)
                bootloaderData.StartAppFunc();
            fptr = (void (*)(void))BOOTLOADER_RESET_ADDRESS;
            fptr();
#endif
            break;

        case BOOTLOADER_ERROR:
            /* The application comes here when the demo
             * has failed. Switch on the LED 9.*/
            //BSP_LEDOn(BSP_LED_3);
            //BSP_LEDOn(BSP_LED_2);
            //BSP_LEDOn(BSP_LED_1);
            break;

        default:
            bootloaderData.currentState = BOOTLOADER_ERROR;
            break;
    }

    #if((DRV_USBFS_HOST_SUPPORT == false) && (DRV_USBHS_HOST_SUPPORT == false))
    /* Maintain Device Drivers */
    DATASTREAM_Tasks();
    #endif

}

#if defined(BOOTLOADER_LEGACY)
BOOTLOADER_STATES BootloaderFlashTriggerCheck(void)
{
#if defined(BOOTLOADER_FLASH_TRIGGER_ADDRESS)
     //Check that the last program flash location is all 0's
     if (*(unsigned int *)BOOTLOADER_FLASH_TRIGGER_ADDRESS != 0xFFFFFFFF)
     {
         return(BOOTLOADER_ENTER_APPLICATION);
          
     }
     else
#endif
     {
         return(BOOTLOADER_OPEN_DATASTREAM);
     }
}

BOOTLOADER_STATES BootloaderButtonTriggerCheck(void)
{
#if defined(BTL_SWITCH)
     if (BSP_SWITCH_STATE_PRESSED == BSP_SwitchStateGet(BTL_SWITCH))
     {
          return(BOOTLOADER_OPEN_DATASTREAM);
     }
     else
#endif    
     {
          return(BOOTLOADER_CHECK_FOR_PROGRAM);
     }
}

bool __attribute__((weak)) BootloaderProgramExistsCheck(void)
{
    return (0xFFFFFFFF == *(unsigned int *)BOOTLOADER_RESET_ADDRESS);
}

#endif
