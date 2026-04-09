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
#include "py32f0xx.h"
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
static void APP_USART_Config(void);
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
  /* Initialize USART(for printf use) */
  APP_USART_Config();

  ee_printf("** CoreMark Test Start\r\n");

  CoreMark();

  ee_printf("** CoreMark Test Stop\r\n");
}

/**
  * @brief  Configure Systemclock
  * @param  None
  * @retval None
  */

//#define PLL 

static void APP_SystemClockConfig(void)
{
  FLASH->ACR &= FLASH_ACR_LATENCY_Msk;

  /* Set AHB divider: HCLK = SYSCLK/2 */
  RCC->CR = (RCC->CR & ~RCC_CR_HSIDIV_Msk) | (1 << RCC_CR_HSIDIV_Pos);  

  /* Set AHB divider: HCLK = SYSCLK */
  //RCC->CR = (RCC->CR & ~RCC_CR_HSIDIV_Msk) | (0 << RCC_CR_HSIDIV_Pos);  
    
#ifdef PLL  
  /* Start PLL */
  RCC->PLLCFGR = RCC_PLLCFGR_PLLSRC_HSI;
  RCC->CR |= RCC_CR_PLLON;
  while (!(RCC->CR & RCC_CR_PLLRDY));
  
  /* SYSCLK = PLL  and  wait is ready */
  FLASH->ACR = FLASH_ACR_LATENCY;
  RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_SW_Msk) | RCC_CFGR_SWS_PLL >> 3;
  while ((RCC->CFGR & RCC_CFGR_SWS_Msk) != RCC_CFGR_SWS_PLL);
    
  /* Update CMSIS variable (which can be updated also through SystemCoreClockUpdate function) */
  SetSystemCoreClock(48000000);
#else  
  SetSystemCoreClock(12000000);
#endif

  //SystemCoreClockUpdate();
  //SysTick_Init(SYSTICK_1MS);
}

static void APP_USART_Config(void) {
    // 1. Включение тактирования периферии

    // GPIOA Clock enable
    RCC->IOPENR |= RCC_IOPENR_GPIOAEN;

    RCC->APBENR1 |= RCC_APBENR1_USART2EN; // Включаем тактирование USART2

    // 2. Настройка GPIO выводов
    // Сбрасываем настройки режима для PA2 и PA3
    GPIOA->MODER &= ~(GPIO_MODER_MODE2_Msk | GPIO_MODER_MODE3_Msk);
    // Устанавливаем альтернативную функцию для PA2 и PA3
    GPIOA->MODER |= (0x02 << GPIO_MODER_MODE2_Pos) | (0x02 << GPIO_MODER_MODE3_Pos);
    // Настраиваем тип альтернативной функции (AF4 для USART2 на PY32F030)
    GPIOA->AFR[0] &= ~(GPIO_AFRL_AFSEL2_Msk | GPIO_AFRL_AFSEL3_Msk);
    GPIOA->AFR[0] |= (0x04 << GPIO_AFRL_AFSEL2_Pos) | (0x04 << GPIO_AFRL_AFSEL3_Pos);

    // 3. Настройка параметров UART
    // Сбрасываем регистры USART2 в исходное состояние
    USART2->CR1 = 0;
    USART2->CR2 = 0;
    USART2->CR3 = 0;

    // Устанавливаем скорость передачи (бод)
    // Формула: BRR = PCLK1 / baudrate
    // 115200 бод
    USART2->BRR = SystemCoreClock/115200; 

    // Включаем передатчик, 8 бит данных, 1 стоп-бит, без контроля четности
    USART2->CR1 |= USART_CR1_RE | USART_CR1_TE;

    // 4. Включение UART
    USART2->CR1 |= USART_CR1_UE;
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
