/**
  ******************************************************************************
  * File Name          : USART.c
  * Description        : This file provides code for the configuration
  *                      of the USART instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "usart.h"

/* USER CODE BEGIN 0 */
uint8_t USART1_RxBuff[USART1_RxBuff_Size];
// DEFINE_KFIFO(usart1_Rxkfifo, uint8_t, USART1_RXKFIFO_SIZE);
uint8_t USART1_Rx_temp = 0;
HAL_StatusTypeDef USART1_Rx_flag = HAL_OK;
/* USER CODE END 0 */

UART_HandleTypeDef huart1;

/* USART1 init function */

void MX_USART1_UART_Init(void)
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
    Error_Handler();
  }

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */
    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART1 interrupt Init */
    HAL_NVIC_SetPriority(USART1_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();

    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);

    /* USART1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

/**
  * @brief  Rx Transfer completed callbacks.
  * @param  huart  Pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if (huart->Instance == USART1)
  {
    static uint16_t USART1_Rx_Number = 0;

    if (USART1_Rx_flag != HAL_OK)
    {
      if (USART1_Rx_Number != 0) //Receive the following data
      {
        // kfifo_in(&usart1_Rxkfifo, &USART1_Rx_temp, 1);
        USART1_RxBuff[USART1_Rx_Number] = USART1_Rx_temp;
        USART1_Rx_Number++;

        if (USART1_Rx_Number == USART1_RX_DATA_SIZE) //Receive all data
        {
          USART1_Rx_Number = 0;
          USART1_Rx_flag = HAL_OK;
          return; // if the last data is 0xA5, the function should be return
        }
      }
      else if (USART1_Rx_temp == 0xA5 && USART1_Rx_Number == 0) //Receive the first data
      {
        // kfifo_in(&usart1_Rxkfifo, &USART1_Rx_temp, 1);
        USART1_RxBuff[0] = USART1_Rx_temp;
        USART1_Rx_Number = 1;
      }
      else
      {
        USART1_Rx_flag = HAL_ERROR;
      }
      HAL_UART_Receive_IT(&huart1, &USART1_Rx_temp, 1);
    }
  }
}

// uint32_t kfifo_out_usart1(uint8_t *buff, uint32_t size)
// {
//   uint32_t Data_Out_Length;

//   Data_Out_Length = kfifo_out(&usart1_Rxkfifo, buff, size);

//   return Data_Out_Length;
// }

//  /**
//    * @brief  UART IRQHandler for IDLE Interrupt.
//    * @param  huart  Pointer to a UART_HandleTypeDef structure that contains
//    *                the configuration information for the specified UART module.
//    * @retval None
//    */
//  void USER_UART_IRQHandler(UART_HandleTypeDef *huart)
//  {
//    if (__HAL_UART_GET_FLAG(huart, UART_FLAG_IDLE) != RESET)
//    {
//      __HAL_UART_CLEAR_IDLEFLAG(huart);
//      USAR_UART_IDLECallback(huart);
//    }
//  }

//  /**
//    * @brief  UART IDLE completed callbacks.
//    * @param  huart  Pointer to a UART_HandleTypeDef structure that contains
//    *                the configuration information for the specified UART module.
//    * @retval None
//    */
//  void USAR_UART_IDLECallback(UART_HandleTypeDef *huart)
//  {
//    if (huart->Instance == USART1)
//    {
//      HAL_UART_DMAStop(&huart1); // 停止本次DMA传输

//      // 测试函数：将接收到的数据打印出去
//      uint16_t UART_DMA_RxData_Length = USART1_RxBuff_Size - __HAL_DMA_GET_COUNTER(&hdma_usart1_rx); // 计算接收到的数据长度

//      // printf("Receive Data(length = %d): ", UART_DMA_RxData_Length);
//      kfifo_in(&usart1_Rxkfifo, USART1_RxBuff, UART_DMA_RxData_Length);
//      uint8_t UART_RxData[UART_DMA_RxData_Length];
//      kfifo_out(&usart1_Rxkfifo, UART_RxData, UART_DMA_RxData_Length);
//      HAL_UART_Transmit(&huart1, USART1_RxBuff, UART_DMA_RxData_Length, 1000);

//      memset(USART1_RxBuff, 0, USART1_RxBuff_Size); // 清零接收缓冲�????

//      HAL_UART_Receive_DMA(&huart1, (uint8_t *)USART1_RxBuff, USART1_RxBuff_Size); // 重启DMA传输 每次255字节数据
//    }
//  }

/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
