/**
  ******************************************************************************
  * @file    main.c
  * @author  MCU Application Team
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2023 Puya Semiconductor Co.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by Puya under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2016 STMicroelectronics.
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
//#include "main.h"
#include "main.h"
#include "coremark.h"


/* Private define ------------------------------------------------------------*/
#define SYSTICK_1MS ((uint32_t)1000)
#define SYSTICK_COUNTER_DIASBLE ((uint32_t)0xFFFFFFFE)
#define SYSTICK_COUNTER_ENABLE ((uint32_t)0x00000001)
#define SYSTICK_COUNTER_CLEAR ((uint32_t)0x00000000)
#define COREMARK_USART_BAUDRATE ((uint32_t)115200)

/* Private variables ---------------------------------------------------------*/


/* Private user code ---------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
extern void SetSystemCoreClock(uint32_t);
static void APP_SystemClockConfig(void);
static void APP_GPIO_Config(void);
static void APP_UART_Config(void);
void SysTick_Init(uint32_t);
void SysTick_Stop_time(void);

/**
 *  iCache_CoreMark
 */


extern int CoreMark(void);

/**
  * @brief  Main program.
  * @param  None
  * @retval int
  */
int main(void)
{
  /* Configure Systemclock */
  APP_SystemClockConfig();

  /* Initialize GPIO for UART */
  APP_GPIO_Config();

  /* Initialize USART(for printf use) */
  APP_UART_Config();

  ee_printf("** CoreMark Test Start\r\n");

  CoreMark();

  ee_printf("** CoreMark Test Stop\r\n");
}

/**
  * @brief  Configure Systemclock
  * @param  None
  * @retval None
  */


static void APP_SystemClockConfig(void)
{
    SysTick_Init(SYSTICK_1MS);
}

static void APP_GPIO_Config(void) {
    
    // enable clock GPIOB  
    CW_SYSCTRL->AHBEN |= 0x5A5A0000 | (1 << 5);
    
    //  Config  GPIO PB05 - output no PU
    CW_GPIOB->ANALOG_f.PIN5 = 0;
    CW_GPIOB->DIR_f.PIN5 = 0;
    CW_GPIOB->OPENDRAIN_f.PIN5 = 0;
    CW_GPIOB->PUR_f.PIN5 = 0;
    CW_GPIOB->AFRL_f.AFR5 = 1;

    //  Config  GPIO PB06 - input PU 
    CW_GPIOB->ANALOG_f.PIN6 = 0;
    CW_GPIOB->DIR_f.PIN6 = 1;
    CW_GPIOB->OPENDRAIN_f.PIN6 = 0;
    CW_GPIOB->PUR_f.PIN6 = 1;
    CW_GPIOB->AFRL_f.AFR6 = 1;

}


static void APP_UART_Config(void) {
    // enable clock UART2
    CW_SYSCTRL->APBEN1 |= 0x5A5A0000 | (1 << 4);
    //CW_SYSCTRL->APBEN1_f.UART2 = ENABLE;

    CW_UART2->CR1 = 0;
    CW_UART2->CR2 = 0;
    CW_UART2->CR3 = 0;
    
    uint32_t k = (SystemCoreClock + 115200/2)/115200;
    CW_UART2->BRRI = k / 16;
    CW_UART2->BRRF = k % 16;
    CW_UART2->CR1_f.TXEN = 1;
    CW_UART2->CR1_f.RXEN = 1;
}


/**
 * @brief  SysTick Init.
 * @param  NUM   Interrupt Time(us)
 */
void SysTick_Init(uint32_t NUM)
{
    /* SystemCoreClock / NUM */
    if (SysTick_Config(SystemCoreClock / NUM))
    {
        while (1)
            ;
    }
}

/**
 * @brief  SysTick_Stop_time.
 */
void SysTick_Stop_time(void)
{
    SysTick->CTRL &= SYSTICK_COUNTER_DIASBLE;
    /* Clear the SysTick Counter */
    SysTick->VAL = SYSTICK_COUNTER_CLEAR;
}


/**
  * @brief  Error handling function
  * @param  None
  * @retval None
  */
void APP_ErrorHandler(void)
{
  /* Infinite loop */
  while (1);
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file：Pointer to the source file name
  * @param  line：assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* User can add His own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* Infinite loop */
  while (1)
  {
  }
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT Puya *****END OF FILE******************/
