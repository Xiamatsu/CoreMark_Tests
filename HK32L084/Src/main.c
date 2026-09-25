/*******************************************************************************
* @copyright: Shenzhen Hangshun Chip Technology R&D Co., Ltd
* @filename:  main.c
* @brief:     Main program body
* @author:    AE Team
* @version:   V1.0.0/2024-01-04
*             1.Initial version
* @log:
*******************************************************************************/


/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "coremark.h"

/** @addtogroup HK32L0xx_StdPeriph_Examples
  * @{
  */

#define SYSTICK_1MS ((uint32_t)1000)
#define SYSTICK_COUNTER_DIASBLE ((uint32_t)0xFFFFFFFE)
#define SYSTICK_COUNTER_ENABLE ((uint32_t)0x00000001)
#define SYSTICK_COUNTER_CLEAR ((uint32_t)0x00000000)
#define COREMARK_USART_BAUDRATE ((uint32_t)115200)

void SysTick_Init(uint32_t);
void SysTick_Stop_time(void);


/** @addtogroup USART_Printf
  * @{
  */
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void APP_USART_Init(void);
void UART_SendByte(USART_TypeDef* USARTx, uint8_t data);
extern int CoreMark(void);

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
    APP_USART_Init();

    ee_printf("** CoreMark Test Start\r\n");

    CoreMark();

    ee_printf("** CoreMark Test Stop\r\n");

    while (1);
}

void APP_USART_Init(void) {

    // GPIOA Clock enable
    RCC->AHBENR |= RCC_AHBENR_IOPAEN;
    //RCC->IOPENR |= RCC_IOPENR_GPIOAEN;

    RCC->APB1ENR |= RCC_APB1ENR_USART2EN; // Включаем тактирование USART2

    // 2. Настройка GPIO выводов
    // Сбрасываем настройки режима для PA2 и PA3
    GPIOA->MODER &= ~(GPIO_MODER_MODER2_Mask | GPIO_MODER_MODER3_Mask);
    // Устанавливаем альтернативную функцию для PA2 и PA3
    GPIOA->MODER |= (0x02 << GPIO_MODER_MODER2_Pos) | (0x02 << GPIO_MODER_MODER3_Pos);
    // Настраиваем тип альтернативной функции (AF7 для USART2 на STM32F4)
    GPIOA->AFRL &= ~(GPIO_AFRL_AFSEL2_Mask | GPIO_AFRL_AFSEL3_Mask);
    GPIOA->AFRL |= (0x04 << GPIO_AFRL_AFSEL2_Pos) | (0x04 << GPIO_AFRL_AFSEL3_Pos);

    // 3. Настройка параметров UART
    // Сбрасываем регистры USART2 в исходное состояние
    USART2->CR1 = 0;
    USART2->CR2 = 0;
    USART2->CR3 = 0;

    // Устанавливаем скорость передачи (бод)
    // Формула: BRR = PCLK1 / baudrate
    USART2->BRR = SystemCoreClock/115200; 

    // Включаем приёмник и передатчик, 8 бит данных, 1 стоп-бит, без контроля четности
    USART2->CR1 |= USART_CR1_RE | USART_CR1_TE;

    // 4. Включение UART
    USART2->CR1 |= USART_CR1_UE;
}

// Отправка одного байта (блокирующий режим)
void UART_SendByte(USART_TypeDef* USARTx, uint8_t data) {
    // Ждём, пока регистр передачи не освободится
    while (!(USARTx->ISR & USART_ISR_TXE));
    // Записываем данные в регистр данных
    USARTx->TDR = data;

    // Ждём завершения передачи
    while (!(USARTx->ISR & USART_ISR_TC));
}

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
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

    /* Infinite loop */
    while (1)
    {
    }
}
#endif

/**
  * @}
  */

/**
  * @}
  */

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

