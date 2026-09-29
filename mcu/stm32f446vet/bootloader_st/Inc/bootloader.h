#ifndef __BOOTLOADER__
#define __BOOTLOADER__

#include "main.h"
#include "stm32f4xx_hal.h"
#include "uhmi_common.h"

/* ******************** common definition ***************************/
typedef enum
{
  FALSE = 0,
  TRUE  = 1
}BOOL;

typedef enum
{
    READ_BOOT_INFO = 1,
    ERASE_FLASH,
    PROGRAM_FLASH,
    READ_CRC,
    JMP_TO_APP
}T_COMMANDS;

typedef union
{
    uint8_t buffer[1024];
    struct
    {
        uint8_t buff1[512];
        uint8_t buff2[512];
    }buffers;

} BOOTLOADER_BUFFER;

// *****************************************************************************
/* Application states

Summary:
BOOTLOADER states enumeration.

Description:
This enumeration defines the valid application states.  These states
determine the behavior of the application at various times.
*/

typedef enum
{
  /* Application's state machine's initial state. */
  BOOTLOADER_STATE_INIT=0,

  /* Check memory location to know if we need to force the bootloader */
  BOOTLOADER_CHECK_FOR_TRIGGER,

  /* The application checks to see if an application has already
  * been programmed. */
  BOOTLOADER_CHECK_FOR_PROGRAM,
  
  /* Init GUI*/
  BOOTLOADER_INIT_GUI,

  /* The application gets a command from the host application. */
  BOOTLOADER_GET_COMMAND,

  /* The application processes the command from the host application. */
  BOOTLOADER_PROCESS_COMMAND,

  /* The application sends data back to the user. */
  BOOTLOADER_SEND_RESPONSE,

  /* The application enters the user application. */
  BOOTLOADER_ENTER_APPLICATION,

  /* This state indicates an error has occurred. */
  BOOTLOADER_ERROR,

} BOOTLOADER_STATES;


/* Application Data

  Summary:
    Holds application data.

  Description:
    This structure holds the application's data.

  Remarks:
    Application strings and buffers are be defined outside this structure.
 */

typedef struct
{
    /* Application current state */
    BOOTLOADER_STATES currentState;

    /* Application data buffer */
    BOOTLOADER_BUFFER data;

    /* Datastream buffer size */
    uint16_t bufferSize;

    /* Command buffer length */
    uint16_t cmdBufferLength;

    /* Flag to indicate the user message is been processed */
    BOOL usrBufferEventComplete;

} BOOTLOADER_DATA;

extern BOOTLOADER_DATA bootloaderData;
void Bootloader_Initialize(void);
void Bootloader_Tasks(void);

#endif