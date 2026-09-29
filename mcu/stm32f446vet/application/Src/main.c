/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2017 STMicroelectronics
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_hal.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
CRC_HandleTypeDef hcrc;

QSPI_HandleTypeDef hqspi;

TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;

UART_HandleTypeDef huart1;

SRAM_HandleTypeDef hsram1;

IWDG_HandleTypeDef IwdgHandle;
TIM_HandleTypeDef  TimInputCaptureHandle;
RCC_ClkInitTypeDef RCC_ClockFreq;
/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_FMC_Init(void);
static void MX_TIM3_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_QUADSPI_Init(void);
static void MX_CRC_Init(void);
static void MX_TIM4_Init(void);
static void IWDG_Init(void);

/* parameters for LSIfreq capture */
static __IO uint32_t uwLsiFreq = 0;
__IO uint32_t uwCaptureNumber = 0;
__IO uint32_t uwPeriodValue = 0;
__IO uint32_t uwMeasurementDone = 0;

uint16_t tmpCC4[2] = {0, 0};

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */
void Start_Tim_IT(void)
{
  if (HAL_TIM_Base_Start_IT(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_Base_Start_IT(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
}
/* USER CODE END 0 */

int main(void)
{

  /* USER CODE BEGIN 1 */
  SCB->VTOR = FLASH_BASE | 0x4000;
  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_FMC_Init();

  MX_USART1_UART_Init();
  MX_QUADSPI_Init();
  MX_CRC_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();

  /* USER CODE BEGIN 2 */
  //read SR to clear TC flag before send the first byte data
  uint8_t clear;
  clear = huart1.Instance->SR;

  GUI_Init();
  HAL_GPIO_WritePin(GPIOD, EBI_RESET_Pin|BL_EN_Pin, GPIO_PIN_SET);
  HAL_Delay(100);
  //if Button_IC_Init fail(return 1), the button_IC is MHS
  //else it is Cypress
  if ( Button_IC_Init() == 1)
  {
    Button_IC_Type = BUTTON_MSH;
  }
  else
  {
	  Button_IC_Type = BUTTON_CYPRESS;
  }
  CY8CMBR3108_DeInit();
  //init IWDG
//  IWDG_count = IWDG_MAX_COUNT;
//  IWDG_Init();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  Start_Tim_IT();
  APP_Initialize();

  while (1)
  {
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
  APP_Tasks();
  }
  /* USER CODE END 3 */

}


/**
  * @brief  Configures TIM5 to measure the LSI oscillator frequency.
  * @param  None
  * @retval LSI Frequency
  */
//static uint32_t GetLSIFrequency(void)
//{
//  uint32_t pclk1 = 0, latency = 0;
//  TIM_IC_InitTypeDef timinputconfig = {0};
//  RCC_OscInitTypeDef oscinit = {0};
//  RCC_ClkInitTypeDef  clkinit =  {0};
//
//  /* Enable LSI Oscillator */
//  oscinit.OscillatorType = RCC_OSCILLATORTYPE_LSI;
//  oscinit.LSIState = RCC_LSI_ON;
//  oscinit.PLL.PLLState = RCC_PLL_NONE;
//  if (HAL_RCC_OscConfig(&oscinit)!= HAL_OK)
//  {
//    Error_Handler();
//  }
//
//  /* Configure the TIM peripheral */
//  /* Set TIMx instance */
//  TimInputCaptureHandle.Instance = TIM5;
//
//  /* TIMx configuration: Input Capture mode ---------------------
//  The LSI clock is connected to TIM5 CH4.
//  The Rising edge is used as active edge.
//  The TIM5 CCR4 is used to compute the frequency value.
//  ------------------------------------------------------------ */
//  TimInputCaptureHandle.Init.Prescaler         = 0;
//  TimInputCaptureHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;
//  TimInputCaptureHandle.Init.Period            = 0xFFFF;
//  TimInputCaptureHandle.Init.ClockDivision     = 0;
//  TimInputCaptureHandle.Init.RepetitionCounter = 0;
//
//  if (HAL_TIM_IC_Init(&TimInputCaptureHandle) != HAL_OK)
//  {
//    /* Initialization Error */
//    Error_Handler();
//  }
//  /* Connect internally the  TIM5 CH4 Input Capture to the LSI clock output */
//  HAL_TIMEx_RemapConfig(&TimInputCaptureHandle, TIM_TIM5_LSI);
//
//  /* Configure the Input Capture of channel 4 */
//  timinputconfig.ICPolarity  = TIM_ICPOLARITY_RISING;
//  timinputconfig.ICSelection = TIM_ICSELECTION_DIRECTTI;
//  timinputconfig.ICPrescaler = TIM_ICPSC_DIV8;
//  timinputconfig.ICFilter    = 0;
//
//  if (HAL_TIM_IC_ConfigChannel(&TimInputCaptureHandle, &timinputconfig, TIM_CHANNEL_4) != HAL_OK)
//  {
//    /* Initialization Error */
//    Error_Handler();
//  }
//
//  /* Reset the flags */
//  TimInputCaptureHandle.Instance->SR = 0;
//
//  /* Start the TIM Input Capture measurement in interrupt mode */
//  if (HAL_TIM_IC_Start_IT(&TimInputCaptureHandle, TIM_CHANNEL_4) != HAL_OK)
//  {
//    /* Starting Error */
//    Error_Handler();
//  }
//
//  /* Wait until the TIM5 get 2 LSI edges (refer to TIM5_IRQHandler() in
//  stm32f4xx_it.c file) */
//  while (uwMeasurementDone == 0)
//  {
//  }
//  uwCaptureNumber = 0;
//
//  /* Deinitialize the TIM5 peripheral registers to their default reset values */
//  HAL_TIM_IC_DeInit(&TimInputCaptureHandle);
//
//  /* Compute the LSI frequency, depending on TIM5 input clock frequency (PCLK1)*/
//  /* Get PCLK1 frequency */
//  pclk1 = HAL_RCC_GetPCLK1Freq();
//  HAL_RCC_GetClockConfig(&clkinit, &latency);
//
//  /* Get PCLK1 prescaler */
//  if ((clkinit.APB1CLKDivider) == RCC_HCLK_DIV1)
//  {
//    /* PCLK1 prescaler equal to 1 => TIMCLK = PCLK1 */
//    return ((pclk1 / uwPeriodValue) * 8);
//  }
//  else
//  {
//    /* PCLK1 prescaler different from 1 => TIMCLK = 2 * PCLK1 */
//    return (((2 * pclk1) / uwPeriodValue) * 8) ;
//  }
//}

/**
  * @brief  Input Capture callback in non blocking mode
  * @param  htim : TIM IC handle
  * @retval None
  */
//void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
//{
//  /* Get the Input Capture value */
//  tmpCC4[uwCaptureNumber++] = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_4);
//
//  if (uwCaptureNumber >= 2)
//  {
//    /* Compute the period length */
//    uwPeriodValue = (uint16_t)(0xFFFF - tmpCC4[0] + tmpCC4[1] + 1);
//    uwMeasurementDone = 1;
//    uwCaptureNumber = 0;
//  }
//}


/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

    /**Configure the main internal regulator output voltage
    */
  __HAL_RCC_PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /**Initializes the CPU, AHB and APB busses clocks
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 15;
  RCC_OscInitStruct.PLL.PLLN = 216;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Activate the Over-Drive mode
    */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* CRC init function */
static void MX_CRC_Init(void)
{

  hcrc.Instance = CRC;
  if (HAL_CRC_Init(&hcrc) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* QUADSPI init function */
static void MX_QUADSPI_Init(void)
{

  hqspi.Instance = QUADSPI;
  hqspi.Init.ClockPrescaler = 255;
  hqspi.Init.FifoThreshold = 1;
  hqspi.Init.SampleShifting = QSPI_SAMPLE_SHIFTING_NONE;
  hqspi.Init.FlashSize = 1;
  hqspi.Init.ChipSelectHighTime = QSPI_CS_HIGH_TIME_1_CYCLE;
  hqspi.Init.ClockMode = QSPI_CLOCK_MODE_0;
  hqspi.Init.FlashID = QSPI_FLASH_ID_1;
  hqspi.Init.DualFlash = QSPI_DUALFLASH_DISABLE;
  if (HAL_QSPI_Init(&hqspi) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* TIM3 init function */
static void MX_TIM3_Init(void)
{

  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;

  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 9000;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 200;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* TIM4 init function */
static void MX_TIM4_Init(void)
{

  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;

  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 9000;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 400;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* USART1 init function */
static void MX_USART1_UART_Init(void)
{

  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}
/* FMC initialization function */
static void MX_FMC_Init(void)
{
  FMC_NORSRAM_TimingTypeDef Timing;

  /** Perform the SRAM1 memory initialization sequence
  */
  hsram1.Instance = FMC_NORSRAM_DEVICE;
  hsram1.Extended = FMC_NORSRAM_EXTENDED_DEVICE;
  /* hsram1.Init */
  hsram1.Init.NSBank = FMC_NORSRAM_BANK1;
  hsram1.Init.DataAddressMux = FMC_DATA_ADDRESS_MUX_DISABLE;
  hsram1.Init.MemoryType = FMC_MEMORY_TYPE_SRAM;
  hsram1.Init.MemoryDataWidth = FMC_NORSRAM_MEM_BUS_WIDTH_16;
  hsram1.Init.BurstAccessMode = FMC_BURST_ACCESS_MODE_DISABLE;
  hsram1.Init.WaitSignalPolarity = FMC_WAIT_SIGNAL_POLARITY_LOW;
  hsram1.Init.WaitSignalActive = FMC_WAIT_TIMING_BEFORE_WS;
  hsram1.Init.WriteOperation = FMC_WRITE_OPERATION_ENABLE;
  hsram1.Init.WaitSignal = FMC_WAIT_SIGNAL_DISABLE;
  hsram1.Init.ExtendedMode = FMC_EXTENDED_MODE_DISABLE;
  hsram1.Init.AsynchronousWait = FMC_ASYNCHRONOUS_WAIT_DISABLE;
  hsram1.Init.WriteBurst = FMC_WRITE_BURST_DISABLE;
  hsram1.Init.ContinuousClock = FMC_CONTINUOUS_CLOCK_SYNC_ONLY;
  hsram1.Init.WriteFifo = FMC_WRITE_FIFO_ENABLE;
  hsram1.Init.PageSize = FMC_PAGE_SIZE_NONE;
  /* Timing */
  Timing.AddressSetupTime = 15;
  Timing.AddressHoldTime = 15;
  Timing.DataSetupTime = 30;
  Timing.BusTurnAroundDuration = 15;
  Timing.CLKDivision = 16;
  Timing.DataLatency = 17;
  Timing.AccessMode = FMC_ACCESS_MODE_A;
  /* ExtTiming */

  if (HAL_SRAM_Init(&hsram1, &Timing, NULL) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/
static void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, EBI_RESET_Pin|BL_EN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, IIC_SDA_Pin|IIC_SCL_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : BOOT_APP_Pin */
  GPIO_InitStruct.Pin = BOOT_APP_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BOOT_APP_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : SPI2_CS_Pin */
  GPIO_InitStruct.Pin = SPI2_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(SPI2_CS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : SW_HOME_Pin SW_LEFT_Pin SW_RIGHT_Pin */
  GPIO_InitStruct.Pin = SW_HOME_Pin|SW_LEFT_Pin|SW_RIGHT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : EBI_RESET_Pin */
  GPIO_InitStruct.Pin = EBI_RESET_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(EBI_RESET_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : BL_EN_Pin */
  GPIO_InitStruct.Pin = BL_EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(BL_EN_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : IIC_SDA_Pin IIC_SCL_Pin */
  GPIO_InitStruct.Pin = IIC_SDA_Pin|IIC_SCL_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
void UART_SendData(uint8_t data)
{
  huart1.Instance->DR = data;
  while (((huart1.Instance->SR) & USART_SR_TC) == RESET);
}

void UART_ITConfig(uint32_t IT_Flag, uint8_t action)
{
//  uint8_t Clear = 0;
  if (action == 1)
  {
    SET_BIT(huart1.Instance->CR1, IT_Flag);
    //clear IDLE flag
    //while (READ_BIT(huart1.Instance->CR1, IT_Flag) == RESET);
    //SET_BIT(huart1.Instance->CR3, USART_CR3_EIE);
  }
  else if (action == 0)
  {
    CLEAR_BIT(huart1.Instance->CR1, IT_Flag);
    //clear IDLE flag
     //while (READ_BIT(huart1.Instance->CR1, IT_Flag) != RESET);
  }
}
//
//void IWDG_Init(void)
//{
//  /*##-1- Check if the system has resumed from IWDG reset ####################*/
//  if (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST) != RESET)
//  {
//    /* if resumed from IWDG, set flag to tell app start from home page */
//    IWDG_resumed_state = 1;
//
//    /* Clear reset flags */
//    __HAL_RCC_CLEAR_RESET_FLAGS();
//  }
//  else
//  {
//    IWDG_resumed_state = 0;
//  }
//
//  /*##-2- Get the LSI frequency: TIM5 is used to measure the LSI frequency ###*/
//  uwLsiFreq = GetLSIFrequency();
//
//  /*##-3- Configure the IWDG peripheral ######################################*/
//  /* Set counter reload value to obtain 4000ms IWDG TimeOut.
//     IWDG counter clock Frequency = LsiFreq / 32
//     Counter Reload Value = 2000ms / IWDG counter clock period
//                          = 2s / (32/LsiFreq)
//                          = LsiFreq / (32 / 2)
//                          = LsiFreq / 16 */
//  IwdgHandle.Instance = IWDG;
//
//  IwdgHandle.Init.Prescaler = IWDG_PRESCALER_32;
//  IwdgHandle.Init.Reload    = uwLsiFreq / 16;
//
//  if (HAL_IWDG_Init(&IwdgHandle) != HAL_OK)
//  {
//    /* Initialization Error */
//    Error_Handler();
//  }
//
//  /*##-4- Start the IWDG #####################################################*/
//  if (HAL_IWDG_Start(&IwdgHandle) != HAL_OK)
//  {
//    Error_Handler();
//  }
//}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void _Error_Handler(char * file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */

/**
  * @}
*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
