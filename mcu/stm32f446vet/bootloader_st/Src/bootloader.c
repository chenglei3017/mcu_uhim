#include <string.h>
#include "stm32f4xx_hal.h"
#include "main.h"
#include "uhmi_common.h"
#include "bootloader.h"
#include "flash_if.h"
#include "bootloader_uart.h"

//2 MCU major and minor define
static const uint8_t BootInfo[2] =
{
    MCU_MINOR_EXC,
    MCU_MAJOR_ST446
};

/***************************** global var************************************/
BOOL cplt_flag;

uint32_t JumpAddress = 0x08000000;
pFunction JumpToApplication;

BOOTLOADER_DATA bootloaderData;


void Bootloader_Initialize (void)
{
    /* Place the App state machine in it's initial state. */
    bootloaderData.currentState = BOOTLOADER_STATE_INIT;
    bootloaderData.bufferSize = 0;
    bootloaderData.cmdBufferLength = 0;
    bootloaderData.usrBufferEventComplete = FALSE;

    memset(&bootloaderData.data, 0, sizeof(bootloaderData.data));    
}


/***************************
    Check boot trigger
    return 1, frace update
****************************/
int bootloader_check_trigger(void)
{
    // For most of the basic bootloaders, the check of the gpio and
    // the memory location will decide the question.
    if (1 ==  HAL_GPIO_ReadPin(BOOT_TRIGGER_GPIO_Port, BOOT_TRIGGER_Pin))
    {
        HAL_Delay(100);
        if (1 ==  HAL_GPIO_ReadPin(BOOT_TRIGGER_GPIO_Port, BOOT_TRIGGER_Pin))
        {
            //add code;
            return (1);
        }
    }

    return (0);    
}

/* Jump to user application */
void Bootloader_Load_APP(u32 appxaddr)
{
/* Test if user code is programmed starting from address "APPLICATION_ADDRESS" */
    if (((*(__IO uint32_t*)appxaddr) & 0x2FFE0000 ) == 0x20000000)
    {
        //CL++
        HAL_DeInit();

        /* Jump to user application */
        JumpAddress = *(__IO uint32_t*) (appxaddr + 4);
        JumpToApplication = (pFunction) JumpAddress;
        /* Initialize user application's Stack Pointer */
        __set_MSP(*(__IO uint32_t*) appxaddr);
        JumpToApplication();
    }
}

void Bootloader_ProcessBuffer( BOOTLOADER_DATA *handle )
{
    uint8_t Cmd = 0;
    uint32_t Address = 0;
    uint32_t Length = 0;
    uint16_t crc = 0;

    /* First, check that we have a valid command. */
    Cmd = handle->data.buffers.buff2[0];

    /* Build the response frame from the command. */
    handle->data.buffers.buff1[0] = handle->data.buffers.buff2[0];
    handle->bufferSize = 0;
    
    switch (Cmd)
    {
        case READ_BOOT_INFO:
        {
            memcpy(&handle->data.buffers.buff1[1], BootInfo, 2);
            handle->bufferSize = 2 + 1;
            handle->currentState = BOOTLOADER_SEND_RESPONSE;
            break;
        }
        case ERASE_FLASH:
        {
            if(0 == FLASH_If_Erase())
            {
                handle->currentState = BOOTLOADER_SEND_RESPONSE;
                handle->bufferSize = 1;
            }
            else
            {
                handle->currentState = BOOTLOADER_ERROR;
            }            
            break;
        }
        case PROGRAM_FLASH:
        {
            if (0==FLASH_If_Write((uint32_t*)(&handle->data.buffers.buff2[1]) , (handle->cmdBufferLength-3)/4))
            {
                handle->bufferSize = 1;
                handle->currentState = BOOTLOADER_SEND_RESPONSE;
            }
            else
            {
                handle->currentState = BOOTLOADER_GET_COMMAND;
            }
        }
        case READ_CRC:
        {
            memcpy(&Address, &handle->data.buffers.buff2[1], sizeof(Address));
            memcpy(&Length, &handle->data.buffers.buff2[5], sizeof(Length));
//#if defined(BOOTLOADER_STATE_SAVE)
//            crc = APP_CalculateCrc((uint8_t *)KVA0_TO_KVA1(Address + 
//                    ((BOOTLOADER_FLASH_END_ADDRESS - BOOTLOADER_FLASH_BASE_ADDRESS) / 2 + 1)), Length);
//#else            
//            crc = APP_CalculateCrc((uint8_t *)KVA0_TO_KVA1(Address), Length);
//#endif
            memcpy(&handle->data.buffers.buff1[1], &crc, 2);

            handle->bufferSize = 1 + 2;
            handle->currentState = BOOTLOADER_SEND_RESPONSE;
            break;
        }
        case JMP_TO_APP:
        {
            handle->currentState = BOOTLOADER_ENTER_APPLICATION;
            break;
        }
        default:
        {
            handle->currentState = BOOTLOADER_GET_COMMAND;
            break;
        }
    }
}

void Bootloader_Send_Response()
{
    size_t BuffLen=0;
    uint16_t crc = 0;
    int i = 0;
    if(bootloaderData.bufferSize)
    {
        /* Calculate the CRC of the response*/
        crc = CalculateCrc((char *)bootloaderData.data.buffers.buff1, bootloaderData.bufferSize);
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

        Send_TxBuffer(bootloaderData.data.buffers.buff2, BuffLen);

    }    
}

void Bootloader_Tasks(void)
{
    switch(bootloaderData.currentState)
    {
        case BOOTLOADER_STATE_INIT:
        {
            //printf("BOOTLOADER_STATE_INIT\n");
            bootloaderData.currentState = BOOTLOADER_CHECK_FOR_TRIGGER;
            break;
        }
        case BOOTLOADER_CHECK_FOR_TRIGGER:
        {
            //printf("BOOTLOADER_CHECK_FOR_TRIGGER\n");
            if (1 == bootloader_check_trigger())
            {
                //HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);
                //FLASH_If_Erase(APPLICATION_ADDRESS);
                //HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
              
                bootloaderData.currentState = BOOTLOADER_INIT_GUI;
            }
            else
            {
                //HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
                bootloaderData.currentState = BOOTLOADER_CHECK_FOR_PROGRAM;
            }
            break;
        }
        case BOOTLOADER_CHECK_FOR_PROGRAM:
        {
            //printf("BOOTLOADER_CHECK_FOR_PROGRAM\n");
            //2 Check if the User reset address is erased. 
            if (0xFFFFFFFF == *(uint32_t *)APPLICATION_ADDRESS)
            {
                bootloaderData.currentState = BOOTLOADER_INIT_GUI;
            }
            else
            {
                //2 User reset address is not erased. Start program. */
                bootloaderData.currentState = BOOTLOADER_ENTER_APPLICATION;
            }            
            break;
        }
        case BOOTLOADER_INIT_GUI:
        {
            ili9488_Init();
            ili9488_Clear(0x0000);
           
            DrawIcon(110,140);
            DrawText(99,283);
            HAL_GPIO_WritePin(BL_EN_GPIO_Port, BL_EN_Pin, GPIO_PIN_SET);
            bootloaderData.currentState = BOOTLOADER_GET_COMMAND;
            break;
        }      
        case BOOTLOADER_GET_COMMAND:
        {
            //printf("BOOTLOADER_GET_COMMAND\n");
            if (bootloaderData.usrBufferEventComplete)
            {
                bootloaderData.currentState = BOOTLOADER_PROCESS_COMMAND;
            }

            break;
        }
        case BOOTLOADER_PROCESS_COMMAND:
        {
            //printf("BOOTLOADER_PROCESS_COMMAND\n");
            Bootloader_ProcessBuffer(&bootloaderData);
            break;
        }
        case BOOTLOADER_SEND_RESPONSE:
        {
            //printf("BOOTLOADER_SEND_RESPONSE\n");
            Bootloader_Send_Response();
            bootloaderData.currentState = BOOTLOADER_GET_COMMAND;
            break;
        }
        case BOOTLOADER_ENTER_APPLICATION:
        {
            //printf("BOOTLOADER_ENTER_APPLICATION\n");            
            Bootloader_Load_APP(APPLICATION_ADDRESS);

            //2 program will not run here
            bootloaderData.currentState = BOOTLOADER_GET_COMMAND;
            break;
        }

        default:
        {
            //printf("default\n");
            //HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
            break;
        }
            


    }

    UART_Tasks();

}