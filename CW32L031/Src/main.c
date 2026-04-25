/******************************************************************************/
/** \file main.c
 **
 ******************************************************************************/

#include "main.h"
#include "coremark.h"


/* Private define ------------------------------------------------------------*/
#define SYSTICK_1MS ((uint32_t)1000)
#define SYSTICK_COUNTER_DIASBLE ((uint32_t)0xFFFFFFFE)
#define SYSTICK_COUNTER_ENABLE ((uint32_t)0x00000001)
#define SYSTICK_COUNTER_CLEAR ((uint32_t)0x00000000)
#define COREMARK_USART_BAUDRATE ((uint32_t)115200)

extern void SetSystemCoreClock(uint32_t);
static void APP_SystemClockConfig(void);
static void APP_UART_Config(void);
void SysTick_Init(uint32_t);
void SysTick_Stop_time(void);

extern int CoreMark(void);

void Delay(uint16_t nCount);
/**
 ******************************************************************************
 ** \brief  Main function of project
 **
 ** \return uint32_t return value, if needed
 **
 ** 
 **
 ******************************************************************************/
int main(void)
{
  /* Configure Systemclock */
  APP_SystemClockConfig();

  /* Initialize USART(for printf use) */
  APP_UART_Config();

  ee_printf("** CoreMark Test Start\r\n");

  CoreMark();

  ee_printf("** CoreMark Test Stop\r\n");
   
}



static void APP_SystemClockConfig(void)
{
    // HSI - 48MHz
    //SYSCTRL_HSI_Enable(SYSCTRL_HSIOSC_DIV1);
    //CW_FLASH->CR2 = 0x5A5A0000 + 0x0018;
    
    //RCC_AHBPeriphClk_Enable(RCC_AHB_PERIPH_GPIOA,ENABLE);
    //RCC_APBPeriphClk_Enable2(RCC_APB2_PERIPH_UART1,ENABLE);
    //SYSCTRL_AHBPeriphClk_Enable(SYSCTRL_AHB_PERIPH_GPIOB, ENABLE);
    //SYSCTRL_APBPeriphClk_Enable1(SYSCTRL_APB1_PERIPH_UART2, ENABLE);

    SysTick_Init(SYSTICK_1MS);
}

static void APP_UART_Config(void) {
    // enable clock GPIOA  
    CW_SYSCTRL->AHBEN |= 0x5A5A0000 | (1 << SYSCTRL_AHBEN_GPIOA_Pos);
        
    //  Config  GPIO PA02 - output no PU
    CW_GPIOA->ANALOG_f.PIN2 = 0;
    CW_GPIOA->DIR_f.PIN2 = 0;
    CW_GPIOA->OPENDRAIN_f.PIN2 = 0;
    CW_GPIOA->PUR_f.PIN2 = 0;
    CW_GPIOA->AFRL_f.AFR2= 1;

    //  Config  GPIO PA03 - input PU 
    CW_GPIOA->ANALOG_f.PIN3 = 0;
    CW_GPIOA->DIR_f.PIN3 = 1;
    CW_GPIOA->OPENDRAIN_f.PIN3 = 0;
    CW_GPIOA->PUR_f.PIN3 = 1;
    CW_GPIOA->AFRL_f.AFR3 = 1;

    // enable clock UART3
    CW_SYSCTRL->APBEN1 |= 0x5A5A0000 | (1 << SYSCTRL_APBEN1_UART3_Pos);
    //CW_SYSCTRL->APBEN1_f.UART2 = ENABLE;

    CW_UART3->CR1 = 0;
    CW_UART3->CR2 = 0;
//    CW_UART2->CR3 = 0;
    
    uint32_t k = (SystemCoreClock + 115200/2)/115200;
    CW_UART3->BRRI = k / 16;
    CW_UART3->BRRF = k % 16;
    CW_UART3->CR1_f.TXEN = 1;
    CW_UART3->CR1_f.RXEN = 1;
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
* @brief 
 *
 * @param nCount
 */
void Delay(__IO uint16_t nCount)
{
    /* Decrement nCount value */
    while (nCount != 0)
    {
        nCount--;
    }
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/

#ifdef  USE_FULL_ASSERT
 /**
   * @brief  Reports the name of the source file and the source line number
   *         where the assert_param error has occurred.
   * @param  file: pointer to the source file name
   * @param  line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
       /* USER CODE END 6 */
}
#endif
