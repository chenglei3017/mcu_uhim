/**
  ******************************************************************************
  * @file    stm32446e_eval.h
  * @author  MCD Application Team
  * @version V2.0.0
  * @date    27-January-2017
  * @brief   This file contains definitions for STM32446E_EVAL's LEDs,
  *          push-buttons and COM ports hardware resources.
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

/* IMPORTANT: in order to compile with RevA following flag shall be defined  */
/* in the preprocessor options:  USE_STM32446E_EVAL_REVA !!!!!!!!!! */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32446E_EVAL_H
#define __STM32446E_EVAL_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
  

/* Private define ------------------------------------------------------------*/
#define BOOT_APP_Pin GPIO_PIN_3
#define BOOT_APP_GPIO_Port GPIOE
#define SPI2_CS_Pin GPIO_PIN_3
#define SPI2_CS_GPIO_Port GPIOC
#define MH204_RESET_Pin GPIO_PIN_4
#define MH204_RESET_GPIO_Port GPIOA
#define SW1_SET_Pin GPIO_PIN_5
#define SW1_SET_GPIO_Port GPIOA
#define SW2_UP_Pin GPIO_PIN_6
#define SW2_UP_GPIO_Port GPIOA
#define SW3_DOWN_Pin GPIO_PIN_7
#define SW3_DOWN_GPIO_Port GPIOA
#define LED1_Pin GPIO_PIN_7
#define LED1_GPIO_Port GPIOB
#define EBI_RESET_Pin GPIO_PIN_2
#define EBI_RESET_GPIO_Port GPIOD
#define EBI_Dimming_Pin GPIO_PIN_3
#define EBI_Dimming_GPIO_Port GPIOD
#define LED3_Pin GPIO_PIN_14
#define LED3_GPIO_Port GPIOB

   
uint32_t         BSP_GetVersion(void); 

/* LCD IO functions */
extern void            LCD_IO_Init(void);
extern void            LCD_IO_WriteData(uint16_t Data); 
extern void            LCD_IO_WriteMultipleData(uint8_t *pData, uint32_t Size);
extern void            LCD_IO_WriteReg(uint8_t Reg);
extern uint16_t        LCD_IO_ReadData(uint16_t Reg);


#ifdef __cplusplus
}
#endif

#endif /* __STM32446E_EVAL_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
