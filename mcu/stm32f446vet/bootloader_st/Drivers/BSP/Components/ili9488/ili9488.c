/**
  ******************************************************************************
   * @file    ili9488.h
  * @author  Lingfeng.fu
  * @version V1.0.0
  * @date    14-Spr-2017
  * @brief   This file includes the LCD driver for ILI9488 LCD.
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "ili9488/ili9488.h"

/** @addtogroup BSP
  * @{
  */ 

#define SET_RESET_LOW   HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2, GPIO_PIN_RESET)
#define SET_RESET_HIGH  HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2, GPIO_PIN_SET)


/** @addtogroup Components
  * @{
  */ 
  
/** @addtogroup ili9488
  * @brief     This file provides a set of functions needed to drive the 
  *            ILI9488 LCD.
  * @{
  */

/** @defgroup ILI9488_Private_TypesDefinitions
  * @{
  */ 

/**
  * @}
  */ 

/** @defgroup ILI9488_Private_Defines
  * @{
  */

/**
  * @}
  */ 
  
/** @defgroup ILI9488_Private_Macros
  * @{
  */
     
/**
  * @}
  */  

/** @defgroup ILI9488_Private_Variables
  * @{
  */ 
LCD_DrvTypeDef   ili9488_drv = 
{
  ili9488_Init,
  ili9488_ReadID,
  ili9488_DisplayOn,
  ili9488_DisplayOff,
  ili9488_SetCursor,
  ili9488_WritePixel,
  ili9488_ReadPixel,
  ili9488_SetDisplayWindow,
  ili9488_DrawHLine,
  ili9488_DrawVLine,
  ili9488_GetLcdPixelWidth,
  ili9488_GetLcdPixelHeight,
  ili9488_DrawBitmap,
  ili9488_DrawRGBImage,  
};

static uint16_t ArrayRGB[320] = {0};
/**
  * @}
  */ 
  
/** @defgroup ILI9488_Private_FunctionPrototypes
  * @{
  */
 static void TMR_DelayMS( uint32_t TMR_DelayMS_in_ms )
 {
   HAL_Delay(TMR_DelayMS_in_ms);
 }

/**
  * @}
  */ 
  
/** @defgroup ILI9488_Private_Functions
  * @{
  */   
static void ILI9488PanelRowArea (uint16_t start, uint16_t end)
{
    LCD_IO_WriteReg(0x2B);
    LCD_IO_WriteData((start>>8) & 0xff);
    LCD_IO_WriteData(start & 0xff);
    LCD_IO_WriteData((end>>8) & 0xff);
    LCD_IO_WriteData(end & 0xff);
}

void ILI9488PanelColumnArea (uint16_t start, uint16_t end)
{
    LCD_IO_WriteReg(0x2A);
    LCD_IO_WriteData((start>>8) & 0xff);
    LCD_IO_WriteData(start & 0xff);
    LCD_IO_WriteData((end>>8) & 0xff);
    LCD_IO_WriteData(end & 0xff);
}
/**
  * @brief  Initialize the ILI9488 LCD Component.
  * @param  None
  * @retval None
  */
void ili9488_Init(void)
{  
  /* Initialize ILI9488 low level bus layer ----------------------------------*/
  //had initialize in LCD_LL_Init
  //LCD_IO_Init();
  
  /* Reset ili9488 */
  SET_RESET_HIGH;
  TMR_DelayMS(1);
  SET_RESET_LOW;
  TMR_DelayMS(10);
  SET_RESET_HIGH;
  TMR_DelayMS(120);
  
  /* Start Initial Sequence --------------------------------------------------*/
  // 5.3.33. PGAMCTRL (Positive Gamma Control) (E0h)
  LCD_IO_WriteReg(LCD_REG_PGC);
  LCD_IO_WriteData(0x00);		
  LCD_IO_WriteData(0x04);
  LCD_IO_WriteData(0x0E);
  LCD_IO_WriteData(0x08);
  LCD_IO_WriteData(0x17);
  LCD_IO_WriteData(0x0A);
  LCD_IO_WriteData(0x40);
  LCD_IO_WriteData(0x79);
  LCD_IO_WriteData(0x4D);
  LCD_IO_WriteData(0x07);
  LCD_IO_WriteData(0x0E);
  LCD_IO_WriteData(0x0A);
  LCD_IO_WriteData(0x1A);
  LCD_IO_WriteData(0x1D);
  LCD_IO_WriteData(0x0F);  
  
  //5.3.34. NGAMCTRL (Negative Gamma Control) (E1h)     
  LCD_IO_WriteReg(LCD_REG_NGC);
  LCD_IO_WriteData(0x00);
  LCD_IO_WriteData(0x1B);
  LCD_IO_WriteData(0x1F);
  LCD_IO_WriteData(0x02);
  LCD_IO_WriteData(0x10);
  LCD_IO_WriteData(0x05);
  LCD_IO_WriteData(0x32);
  LCD_IO_WriteData(0x34);
  LCD_IO_WriteData(0x43);
  LCD_IO_WriteData(0x02);
  LCD_IO_WriteData(0x0A);
  LCD_IO_WriteData(0x09);
  LCD_IO_WriteData(0x33);
  LCD_IO_WriteData(0x37);
  LCD_IO_WriteData(0x0F);

  //power control 1, 0xC0
  LCD_IO_WriteReg(LCD_REG_POWCTRL1);
  LCD_IO_WriteData(0x18);
  LCD_IO_WriteData(0x16);
  //power control 2, 0xC1
  LCD_IO_WriteReg(LCD_REG_POWCTRL2);
  LCD_IO_WriteData(0x41);
  //VCOM control, 0xC5
  LCD_IO_WriteReg(LCD_REG_VCOMCTRL);
  LCD_IO_WriteData(0x00);
  LCD_IO_WriteData(0x22);	//set VCOM = -1.46875
  LCD_IO_WriteData(0x80);	//set VCM_REG_EN = 1
  //memory access control, 0x36 
  LCD_IO_WriteReg(LCD_REG_MEMCTRL);
  LCD_IO_WriteData(0x08);	//set RGB-BGR order
  //Interface Pixel Format, 0x3A, 16 bits/pixel
  LCD_IO_WriteReg(LCD_REG_IPF);
  LCD_IO_WriteData(0x55);

  //Interface Mode Control, 0xB0
  LCD_IO_WriteReg(LCD_REG_IMC);  
  LCD_IO_WriteData(0x00);
  //Frame rate 70HZ, 0xB1
  LCD_IO_WriteReg(LCD_REG_SFR);   
  LCD_IO_WriteData(0xB0);
  //Display Inversion Control, 0xB4, 2 dot inversion
  LCD_IO_WriteReg(LCD_REG_DIC);
  LCD_IO_WriteData(0x02);
  //Display Function Control, 0xB6
  LCD_IO_WriteReg(LCD_REG_DFC); //RGB/MCU Interface Control
  LCD_IO_WriteData(0x02);
  LCD_IO_WriteData(0x22);
  LCD_IO_WriteData(0x3B);
  //Set Image Function, 0xE9, set DB_EN disabled default
  LCD_IO_WriteReg(LCD_REG_SIF);
  LCD_IO_WriteData(0x00);
  //Adjust Control 3, 0xF7
  LCD_IO_WriteReg(LCD_REG_ADJCTRL3);
  LCD_IO_WriteData(0xA9);
  LCD_IO_WriteData(0x51);
  LCD_IO_WriteData(0x2C);
  LCD_IO_WriteData(0x82);    

  //Sleep OUT, 0x11
  LCD_IO_WriteReg(LCD_REG_SLEEPOUT);
  
  TMR_DelayMS(120); 
  ili9488_DisplayOn();  
  
  //set dispaly window
  //ili9488_SetDisplayWindow(0,0, ILI9488_LCD_PIXEL_WIDTH - 1, ILI9488_LCD_PIXEL_HEIGHT - 1);
  
  /* Prepare to write GRAM */
  LCD_IO_WriteReg(LCD_REG_MEMWR);
}

/**
  * @brief  Enables the Display.
  * @param  None
  * @retval None
  */
void ili9488_DisplayOn(void)
{
  /* Display On,0x29 */
   LCD_IO_WriteReg(LCD_REG_DISPLAYON);
}

/**
  * @brief  Disables the Display.
  * @param  None
  * @retval None
  */
void ili9488_DisplayOff(void)
{
  
  /* Display Off */
  LCD_IO_WriteReg(LCD_REG_DISPLAYOFF); 
}

/**
  * @brief  Get the LCD pixel Width.
  * @param  None
  * @retval The Lcd Pixel Width
  */
uint16_t ili9488_GetLcdPixelWidth(void)
{
 return (uint16_t)ILI9488_LCD_PIXEL_WIDTH;
}

/**
  * @brief  Get the LCD pixel Height.
  * @param  None
  * @retval The Lcd Pixel Height
  */
uint16_t ili9488_GetLcdPixelHeight(void)
{
 return (uint16_t)ILI9488_LCD_PIXEL_HEIGHT;
}

/**
  * @brief  Get the ILI9488 ID.
  * @param  None
  * @retval The ILI9488 ID 
  */
uint16_t ili9488_ReadID(void)
{
  LCD_IO_Init(); 
  return (ili9488_ReadReg(LCD_REG_RDID));
}

/**
  * @brief  Set Cursor position.
  * @param  Xpos: specifies the X position.
  * @param  Ypos: specifies the Y position.
  * @retval None
  */
void ili9488_SetCursor(uint16_t Xpos, uint16_t Ypos)
{
  ILI9488PanelColumnArea(Xpos, Xpos + 1);
  ILI9488PanelRowArea(Ypos, Ypos + 1);
  LCD_IO_WriteReg(LCD_REG_MEMWR);     
}

/**
  * @brief  Write pixel.   
  * @param  Xpos: specifies the X position.
  * @param  Ypos: specifies the Y position.
  * @param  RGBCode: the RGB pixel color
  * @retval None
  */
void ili9488_WritePixel(uint16_t Xpos, uint16_t Ypos, uint16_t RGBCode)
{
  /* Set Cursor */
  ILI9488PanelRowArea(Ypos,Ypos+1);
  ILI9488PanelColumnArea(Xpos,Xpos+1);
  LCD_IO_WriteReg(0x2a);
  LCD_IO_WriteData((Xpos >> 8) & 0xff);
  LCD_IO_WriteData(Xpos & 0xff);
  
   LCD_IO_WriteReg(0x2b);
  LCD_IO_WriteData((Ypos >> 8) & 0xff);
  LCD_IO_WriteData(Ypos & 0xff);
  /* Prepare to write GRAM */
  LCD_IO_WriteReg(LCD_REG_MEMWR);

  /* Write 16-bit GRAM Reg */
  LCD_IO_WriteMultipleData((uint8_t*)&RGBCode, 2);
  
}

/**
  * @brief  Read pixel.
  * @param  None
  * @retval The RGB pixel color
  */
uint16_t ili9488_ReadPixel(uint16_t Xpos, uint16_t Ypos)
{
  /* Set Cursor */
  ILI9488PanelRowArea(Ypos,Ypos+1);
  ILI9488PanelColumnArea(Xpos,Xpos+1);
  
  /* Read 16-bit Reg */
  return (LCD_IO_ReadData(LCD_REG_MEMRD));
}

/**
  * @brief  Writes to the selected LCD register.
  * @param  LCDReg: Address of the selected register.
  * @param  LCDRegValue: Value to write to the selected register.
  * @retval None
  */
void ili9488_WriteReg(uint8_t LCDReg, uint16_t LCDRegValue)
{
  LCD_IO_WriteReg(LCDReg);
  
  /* Write 16-bit GRAM Reg */
  LCD_IO_WriteMultipleData((uint8_t*)&LCDRegValue, 2);
}

/**
  * @brief  Reads the selected LCD Register.
  * @param  LCDReg: address of the selected register.
  * @retval LCD Register Value.
  */
uint16_t ili9488_ReadReg(uint8_t LCDReg)
{ 
  /* Read 16-bit Reg */
  return (LCD_IO_ReadData(LCDReg));
}

/**
  * @brief  Sets a display window
  * @param  Xpos:   specifies the X bottom left position.
  * @param  Ypos:   specifies the Y bottom left position.
  * @param  Height: display window height.
  * @param  Width:  display window width.
  * @retval None
  */
void ili9488_SetDisplayWindow(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height)
{
  /* Horizontal GRAM Start and End Address */
  ILI9488PanelColumnArea(Xpos, Xpos + Width - 1); 
  
  /* Vertical GRAM Start and End Address */
  ILI9488PanelRowArea(Ypos, Ypos + Height - 1);
}

/**
  * @brief  Draw vertical line.
  * @param  RGBCode: Specifies the RGB color   
  * @param  Xpos:     specifies the X position.
  * @param  Ypos:     specifies the Y position.
  * @param  Length:   specifies the Line length.  
  * @retval None
  */
void ili9488_DrawHLine(uint16_t RGBCode, uint16_t Xpos, uint16_t Ypos, uint16_t Length)
{
  uint16_t counter = 0;
  
  /* Set Cursor */
  ili9488_SetDisplayWindow(Xpos, Ypos, Length, 1); 
  
  /* Prepare to write GRAM */
  LCD_IO_WriteReg(LCD_REG_MEMWR);

  /* Sent a complete line */
  for(counter = 0; counter < Length; counter++)
  {
    ArrayRGB[counter] = RGBCode;
  }  

  LCD_IO_WriteMultipleData((uint8_t*)&ArrayRGB[0], Length * 2);
}

/**
  * @brief  Draw vertical line.
  * @param  RGBCode: Specifies the RGB color    
  * @param  Xpos:     specifies the X position.
  * @param  Ypos:     specifies the Y position.
  * @param  Length:   specifies the Line length.  
  * @retval None
  */
void ili9488_DrawVLine(uint16_t RGBCode, uint16_t Xpos, uint16_t Ypos, uint16_t Length)
{
  uint16_t counter = 0;

  /* Set DisplayWindow */
  ili9488_SetDisplayWindow(Xpos, Ypos, 1, Length);
  
  /* Prepare to write GRAM */
  LCD_IO_WriteReg(LCD_REG_MEMWR);

  /* Fill a complete vertical line */
  for(counter = 0; counter < Length; counter++)
  {
    ArrayRGB[counter] = RGBCode;
  }
  
  /* Write 16-bit GRAM Reg */
  LCD_IO_WriteMultipleData((uint8_t*)&ArrayRGB[0], Length * 2);
}

/**
  * @brief  Displays a bitmap picture.
  * @param  BmpAddress: Bmp picture address.
  * @param  Xpos: Bmp X position in the LCD
  * @param  Ypos: Bmp Y position in the LCD    
  * @retval None
  */
void ili9488_DrawBitmap(uint16_t Xpos, uint16_t Ypos, uint8_t *pbmp)
{
  uint32_t index = 0, size = 0;
  uint16_t width = 0, height = 0;
  /* Read bitmap size */
  size = *(volatile uint16_t *) (pbmp + 2);
  size |= (*(volatile uint16_t *) (pbmp + 4)) << 16;
  /* Get bitmap data address offset */
  index = *(volatile uint16_t *) (pbmp + 10);
  index |= (*(volatile uint16_t *) (pbmp + 12)) << 16;
  size = (size - index)/2;
  pbmp += index;
  
  /* read bitmap height and width */
  width =  *(volatile uint16_t *) (pbmp + 18);
  width |= (*(volatile uint16_t *) (pbmp + 20)) << 16;
  height =  *(volatile uint16_t *) (pbmp + 22);
  height |= (*(volatile uint16_t *) (pbmp + 24)) << 16;
  
  
  /* Set DisplayWindow */
  ili9488_SetDisplayWindow(Xpos, Ypos, width, height);  
  
  /* Prepare to write GRAM */
  LCD_IO_WriteReg(LCD_REG_MEMWR);
 
  LCD_IO_WriteMultipleData((uint8_t*)pbmp, size*2);
}

/**
  * @brief  Displays picture.
  * @param  pdata: picture address.
  * @param  Xpos: Image X position in the LCD
  * @param  Ypos: Image Y position in the LCD
  * @param  Xsize: Image X size in the LCD
  * @param  Ysize: Image Y size in the LCD
  * @retval None
  */
void ili9488_DrawRGBImage(uint16_t Xpos, uint16_t Ypos, uint16_t Xsize, uint16_t Ysize, uint8_t *pdata)
{
  uint32_t size = 0;

  size = (Xsize * Ysize);

  /* Set DisplayWindow */
  ili9488_SetDisplayWindow(Xpos, Ypos, Xsize, Ysize);   
  
  /* Prepare to write GRAM */
  LCD_IO_WriteReg(LCD_REG_MEMWR);
 
  LCD_IO_WriteMultipleData((uint8_t*)pdata, size*2);
}

void ili9488_Clear(uint16_t color)
{
  int i=0;
  ili9488_SetDisplayWindow(0,0,ILI9488_LCD_PIXEL_WIDTH,ILI9488_LCD_PIXEL_HEIGHT);
  LCD_IO_WriteReg(LCD_REG_MEMWR);
  for(i=0;i<ILI9488_LCD_PIXEL_WIDTH*ILI9488_LCD_PIXEL_HEIGHT;i++)
  {
       LCD_IO_WriteData(color);
  }
}

uint16_t getPixelColor(uint8_t  c)
{
    
  uint16_t r = 0xF800,g = 0x07E0,b = 0x001F;
  uint16_t point_color = 0xFFFF;
  uint16_t color;
  
  r = (point_color&r)>>11;
  g = (point_color&g)>>5;
  b = point_color&b;
  
  r = ((r+1)/16)*c;
  g = ((g+1)/16)*c;
  b = ((b+1)/16)*c;
  
  r = r>0?r-1:r;
  g = g>0?g-1:g;
  b = b>0?b-1:b;
  
  color = (r<<11)|(g<<5)|b;
  return color;
}



/**
  * @}
  */ 

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
