/**
  ******************************************************************************
  * @file    stm32446e_eval.c
  * @author  MCD Application Team
  * @version V2.0.0
  * @date    27-January-2017
  * @brief   This file provides a set of firmware functions to manage LEDs, 
  *          push-buttons and COM ports available on STM32446E-EVAL evaluation
  *          board(MB1045) RevB from STMicroelectronics.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* File Info: ------------------------------------------------------------------
                                   User NOTE

   This driver requires the stm32446e_eval_io to manage the joystick

------------------------------------------------------------------------------*/

/* Includes ------------------------------------------------------------------*/
#include "stm32446e_eval.h"


/** @defgroup BSP BSP
  * @{
  */ 

/**
  * @brief STM32446E EVAL BSP Driver version number V2.0.0
  */
#define __STM32446E_EVAL_BSP_VERSION_MAIN   (0x02) /*!< [31:24] main version */
#define __STM32446E_EVAL_BSP_VERSION_SUB1   (0x00) /*!< [23:16] sub1 version */
#define __STM32446E_EVAL_BSP_VERSION_SUB2   (0x00) /*!< [15:8]  sub2 version */
#define __STM32446E_EVAL_BSP_VERSION_RC     (0x00) /*!< [7:0]  release candidate */
#define __STM32446E_EVAL_BSP_VERSION         ((__STM32446E_EVAL_BSP_VERSION_MAIN << 24)\
                                             |(__STM32446E_EVAL_BSP_VERSION_SUB1 << 16)\
                                             |(__STM32446E_EVAL_BSP_VERSION_SUB2 << 8 )\
                                             |(__STM32446E_EVAL_BSP_VERSION_RC))

uint32_t BSP_GetVersion(void)
{
  return __STM32446E_EVAL_BSP_VERSION;
}
/** @defgroup STM32446E_EVAL STM32446E EVAL
  * @{
  */

/** @defgroup STM32446E_EVAL_LOW_LEVEL STM32446E EVAL LOW LEVEL
  * @{
  */

/** @defgroup STM32446E_EVAL_LOW_LEVEL_Private_TypesDefinitions STM32446E EVAL LOW LEVEL Private TypesDefinitions
  * @{
  */
typedef struct
{
  __IO uint16_t REG;
  __IO uint16_t RAM;
}LCD_CONTROLLER_TypeDef;
/**
  * @}
  */

/* Compared to F4xG we use BANK1 FMC_NE1 signal,choose A16(PD11)  */
#define FMC_BANK1_BASE          ((uint32_t)(0x60000000 | 0x0001FFFE))
#define FMC_BANK1_BASE_A0       ((uint32_t)(0x60000000 | 0x00000000))
#define FMC_BANK1               ((LCD_CONTROLLER_TypeDef *) FMC_BANK1_BASE)

static void     FMC_BANK1_WriteData(uint16_t Data);
static void     FMC_BANK1_WriteReg(uint8_t Reg);
static uint16_t FMC_BANK1_ReadData(void);
static void     FMC_BANK1_Init(void);
//static void     FMC_BANK1_MspInit(void);

/* LCD IO functions */
void            LCD_IO_Init(void);
void            LCD_IO_WriteData(uint16_t Data); 
void            LCD_IO_WriteMultipleData(uint8_t *pData, uint32_t Size);
void            LCD_IO_WriteReg(uint8_t Reg);
uint16_t        LCD_IO_ReadData(uint16_t Reg);

/**
  * @}
  */
    




/*************************** FMC Routines ************************************/
/**
  * @brief  Initializes FMC_BANK1 MSP.
  */
//static void FMC_BANK1_MspInit(void)
//{
//  /* has defined and used in main.c and stm32f4xx_hal_msp.c */
//}


/**
  * @brief  Initializes LCD IO.
  */
static void FMC_BANK1_Init(void) 
{  
  /* has initialized in main.c */
  
}


/**
  * @brief  Writes register value.
  * @param  Data: Data to be written 
  */
static void FMC_BANK1_WriteData(uint16_t Data) 
{
  /* Write 16-bit Reg */
  FMC_BANK1->RAM = Data;
}

/**
  * @brief  Writes register address.
  * @param  Reg: Register to be written
  */
static void FMC_BANK1_WriteReg(uint8_t Reg) 
{
  /* Write 16-bit Index, then write register */
  FMC_BANK1->REG = Reg;
}

/**
  * @brief  Reads register value.
  * @retval Read value
  */
static uint16_t FMC_BANK1_ReadData(void) 
{
  return FMC_BANK1->RAM;
}

/********************************* LINK LCD ***********************************/

/**
  * @brief  Initializes LCD low level.
  */
void LCD_IO_Init(void) 
{
    FMC_BANK1_Init();
}

/**
  * @brief  Writes data on LCD data register.
  * @param  Data: Data to be written
  */
void LCD_IO_WriteData(uint16_t Data) 
{
  /* Write 16-bit Reg */
  FMC_BANK1_WriteData(Data);
}

/**
  * @brief  Writes multiple data on LCD data register.
  * @param  pData Pointer on the register value
  * @param  Size Size of byte to transmit to the register
  * @retval None
  */
void LCD_IO_WriteMultipleData(uint8_t *pData, uint32_t Size)
{
  uint32_t counter;
  uint16_t *ptr = (uint16_t *) pData;
  
  for (counter = 0; counter < Size; counter+=2)
  {  
    /* Write 16-bit Reg */
    FMC_BANK1_WriteData(*ptr);
    ptr++;
  }
}

/**
  * @brief  Writes register on LCD register.
  * @param  Reg: Register to be written
  */
void LCD_IO_WriteReg(uint8_t Reg) 
{
  /* Write 16-bit Index, then Write Reg */
  FMC_BANK1_WriteReg(Reg);
}

/**
  * @brief  Reads data from LCD data register.
  * @param  Reg: Register to be read
  * @retval Read data.
  */
uint16_t LCD_IO_ReadData(uint16_t Reg)
{
  FMC_BANK1_WriteReg(Reg);
  
  /* Read 16-bit Reg */  
  return FMC_BANK1_ReadData();
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
