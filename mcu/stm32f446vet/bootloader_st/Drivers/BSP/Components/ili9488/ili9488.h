/**
  ******************************************************************************
  * @file    ili9488.h
  * @author  Lingfeng.fu
  * @version V1.0.0
  * @date    14-Spr-2017
  * @brief   This file contains all the functions prototypes for the ili9488.c
  *          driver.
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ILI9488_H
#define __ILI9488_H

#ifdef __cplusplus
 extern "C" {
#endif 

/* Includes ------------------------------------------------------------------*/
#include "Common/lcd.h"
#include "stm32446e_eval.h"

/** @addtogroup BSP
  * @{
  */ 

/** @addtogroup Components
  * @{
  */ 
  
/** @addtogroup ili9488
  * @{
  */

/** @defgroup ILI9488_Exported_Types
  * @{
  */
   
/**
  * @}
  */ 
/** @defgroup ILI9488_Exported_Constants
  * @{
  */
/** 
  * @brief  ILI9488 ID  
  */  
#define  ILI9488_ID    0x9488
#define  ILI9488_ID    0x9488
/** 
  * @brief  ILI9488 Size  
  */  
#define  ILI9488_LCD_PIXEL_WIDTH    ((uint16_t)320)
#define  ILI9488_LCD_PIXEL_HEIGHT   ((uint16_t)480)
   
/** 
  * @brief  ILI9488 Registers  
  */ 
#define LCD_REG_RDID          0x04              //read IC ID
#define LCD_REG_NORMALON      0x13              //Normal Display Mode ON (13h)
#define LCD_REG_PGC           0xE0              //PGAMCTRL (Positive Gamma Control) (E0h)
#define LCD_REG_NGC           0xE1              //NGAMCTRL (Negative Gamma Control) (E1h)
#define LCD_REG_POWCTRL1      0xC0              //power control 1
#define LCD_REG_POWCTRL2      0xC1              //power control 2
#define LCD_REG_VCOMCTRL      0xC5              //VCOM control
#define LCD_REG_MEMCTRL       0x36              //memory access control,
#define LCD_REG_IPF           0x3A              //Interface Pixel Format
#define LCD_REG_IMC           0xB0              //Interface Mode Control
#define LCD_REG_SFR           0xB1              //select Frame rate 
#define LCD_REG_DIC           0xB4              //Display Inversion Control
#define LCD_REG_DFC           0xB6              //Display Function Control
#define LCD_REG_SIF           0xE9              //Set Image Function
#define LCD_REG_ADJCTRL3      0xF7              //Adjust Control 3
#define LCD_REG_SLEEPOUT      0x11              //Sleep OUT     
#define LCD_REG_DISPLAYON     0x29              //display on
#define LCD_REG_DISPLAYOFF    0x28              //display off
#define LCD_REG_CAS           0x2A              //Column Address Set
#define LCD_REG_PAS           0x2B              //Page Address Set
#define LCD_REG_MEMWR         0x2C              //Memory Write (2Ch)
#define LCD_REG_MEMRD         0x2E              //Memory Read (2Eh)
/**
  * @}
  */
  
/** @defgroup ILI9488_Exported_Functions
  * @{
  */ 
void     ili9488_Init(void);
uint16_t ili9488_ReadID(void);
void     ili9488_WriteReg(uint8_t LCDReg, uint16_t LCDRegValue);
uint16_t ili9488_ReadReg(uint8_t LCDReg);

void     ili9488_DisplayOn(void);
void     ili9488_DisplayOff(void);
void     ili9488_SetCursor(uint16_t Xpos, uint16_t Ypos);
void     ili9488_WritePixel(uint16_t Xpos, uint16_t Ypos, uint16_t RGBCode);
uint16_t ili9488_ReadPixel(uint16_t Xpos, uint16_t Ypos);

void     ili9488_DrawHLine(uint16_t RGBCode, uint16_t Xpos, uint16_t Ypos, uint16_t Length);
void     ili9488_DrawVLine(uint16_t RGBCode, uint16_t Xpos, uint16_t Ypos, uint16_t Length);
void     ili9488_DrawBitmap(uint16_t Xpos, uint16_t Ypos, uint8_t *pbmp);
void     ili9488_DrawRGBImage(uint16_t Xpos, uint16_t Ypos, uint16_t Xsize, uint16_t Ysize, uint8_t *pdata);

void     ili9488_SetDisplayWindow(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height);

void ili9488_Clear(uint16_t color);

uint16_t ili9488_GetLcdPixelWidth(void);
uint16_t ili9488_GetLcdPixelHeight(void);

/* LCD driver structure */
extern LCD_DrvTypeDef   ili9488_drv;

/* LCD IO functions */
void     LCD_IO_Init(void);
void     LCD_IO_WriteMultipleData(uint8_t *pData, uint32_t Size);
void     LCD_IO_WriteReg(uint8_t Reg);
uint16_t LCD_IO_ReadData(uint16_t Reg);

void ili9488_Clear(uint16_t color);
uint16_t getPixelColor(uint8_t  c);
void DrawIcon(uint16_t x,uint16_t y);
void DrawText(uint16_t x,uint16_t y);
/**
  * @}
  */ 
      
#ifdef __cplusplus
}
#endif

#endif /* __ILI9488_H */

/**
  * @}
  */ 

/**
  * @}
  */ 

/**
  * @}
  */
  
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
