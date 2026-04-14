#include <stdint.h>
#include <stm32c0xx.h>
#include "coremark.h"

#define SYSTICK_1MS ((uint32_t)1000)
#define SYSTICK_COUNTER_DIASBLE ((uint32_t)0xFFFFFFFE)
#define SYSTICK_COUNTER_ENABLE ((uint32_t)0x00000001)
#define SYSTICK_COUNTER_CLEAR ((uint32_t)0x00000000)
#define COREMARK_USART_BAUDRATE ((uint32_t)115200)

void SysTick_Init(uint32_t);
void SysTick_Stop_time(void);

void APP_SetSystemClock(void);
void APP_MCO_Test(void);
void APP_Pipe_test(void);
void APP_USART_Init(void);
void UART_SendString(USART_TypeDef* USARTx, char* str);
extern int CoreMark(void);

int main(void) {
    APP_SetSystemClock();
    APP_USART_Init();
    ee_printf("** CoreMark Test Start\r\n");

    CoreMark();

    ee_printf("** CoreMark Test Stop\r\n");

    while (1);
}

void APP_SetSystemClock(void) {

    // SYSCLK = HSI48/128 = 375 kHz
    //RCC->CR = (RCC->CR & ~RCC_CR_HSIDIV_Msk) | (7 << RCC_CR_HSIDIV_Pos);  

    // Latency = 1  for 48 MHz
    FLASH->ACR |= FLASH_ACR_LATENCY;
    //FLASH->ACR &= ~FLASH_ACR_LATENCY_Msk;

    // SYSCLK = HSI48/4
    //RCC->CR = (RCC->CR & ~RCC_CR_HSIDIV_Msk) | (2 << RCC_CR_HSIDIV_Pos);  
    
    // SYSCLK = HSI48/2
    //RCC->CR = (RCC->CR & ~RCC_CR_HSIDIV_Msk) | (1 << RCC_CR_HSIDIV_Pos);  
    
    // SYSCLK = HSI48
    RCC->CR = (RCC->CR & ~RCC_CR_HSIDIV_Msk) | (0 << RCC_CR_HSIDIV_Pos);  
    
    SystemCoreClockUpdate();
}
    
void APP_MCO_Test(void) {
    // GPIOA Clock enable
    RCC->IOPENR |= RCC_IOPENR_GPIOAEN;
    
    // MCO enable  SYSCLK/1
    RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_MCOSEL_Msk) | RCC_CFGR_MCOSEL_0;
    RCC->CFGR &= ~RCC_CFGR_MCOPRE_Msk;
    
    // PA8 - output MCO  (Alternate mode AF0)
    GPIOA->MODER = (GPIOA->MODER & ~GPIO_MODER_MODE8_Msk) | (2 << GPIO_MODER_MODE8_Pos);
    GPIOA->OTYPER &= ~GPIO_OTYPER_OT8;
    GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED8;
    GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD8;
    GPIOA->AFR[1] &= ~GPIO_AFRH_AFSEL8; 
    
    // PA7 - output Pulse
    GPIOA->MODER = (GPIOA->MODER & ~GPIO_MODER_MODE7_Msk) | (1 << GPIO_MODER_MODE7_Pos);
    GPIOA->OTYPER &= ~GPIO_OTYPER_OT7;
    GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED7;
    GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD7;
    //GPIOA->AFR[1] &= ~GPIO_AFRH_AFSEL8; 
}


void APP_Pipe_test(void) {
    __NOP();
    __NOP();
    while (1) {
        GPIOA->BSRR = GPIO_BSRR_BS7;
        GPIOA->BRR = GPIO_BRR_BR7;
        GPIOA->BSRR = GPIO_BSRR_BS7;
        GPIOA->BRR = GPIO_BRR_BR7;
        GPIOA->BSRR = GPIO_BSRR_BS7;
        GPIOA->BRR = GPIO_BRR_BR7;
        GPIOA->BSRR = GPIO_BSRR_BS7;
        GPIOA->BRR = GPIO_BRR_BR7;
    }
}

void APP_USART_Init(void) {

    // GPIOA Clock enable
    RCC->IOPENR |= RCC_IOPENR_GPIOAEN;

    RCC->APBENR1 |= RCC_APBENR1_USART2EN; // Включаем тактирование USART2

    // 2. Настройка GPIO выводов
    // Сбрасываем настройки режима для PA2 и PA3
    GPIOA->MODER &= ~(GPIO_MODER_MODE2_Msk | GPIO_MODER_MODE3_Msk);
    // Устанавливаем альтернативную функцию для PA2 и PA3
    GPIOA->MODER |= (0x02 << GPIO_MODER_MODE2_Pos) | (0x02 << GPIO_MODER_MODE3_Pos);
    // Настраиваем тип альтернативной функции (AF7 для USART2 на STM32F4)
    GPIOA->AFR[0] &= ~(GPIO_AFRL_AFSEL2_Msk | GPIO_AFRL_AFSEL3_Msk);
    GPIOA->AFR[0] |= (0x01 << GPIO_AFRL_AFSEL2_Pos) | (0x01 << GPIO_AFRL_AFSEL3_Pos);

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
    while (!(USARTx->ISR & USART_ISR_TXE_TXFNF));
    // Записываем данные в регистр данных
    USARTx->TDR = data;

    // Ждём завершения передачи
    while (!(USARTx->ISR & USART_ISR_TC));
}

// Отправка строки (блокирующий режим)
void UART_SendString(USART_TypeDef* USARTx, char* str) {
    while (*str) {
        UART_SendByte(USARTx, *str++);
    }
}
/*
int fputc(int ch, FILE* f) {
    // Ждём, пока регистр передачи не освободится
    while (!(USART2->ISR & USART_ISR_TXE_TXFNF));
    // Записываем данные в регистр данных
    USART2->TDR = ch;

    // Ждём завершения передачи
    while (!(USART2->ISR & USART_ISR_TC));
    return ch;                  // indicate success to the caller
}
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
