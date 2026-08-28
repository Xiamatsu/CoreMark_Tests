/****************************************Copyright (c)****************************************************
**                                      
**--------------File Info---------------------------------------------------------------------------------
** File name:               main.c
** Descriptions:            The GPIO LED application function
**
** Descriptions:            
**
*********************************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx.h"
#include "coremark.h"


#define SYSTICK_1MS ((uint32_t)1000)
#define SYSTICK_COUNTER_DIASBLE ((uint32_t)0xFFFFFFFE)
#define SYSTICK_COUNTER_ENABLE ((uint32_t)0x00000001)
#define SYSTICK_COUNTER_CLEAR ((uint32_t)0x00000000)
#define COREMARK_USART_BAUDRATE ((uint32_t)115200)

void SysTick_Init(uint32_t);
void SysTick_Stop_time(void);

/* Private functions ---------------------------------------------------------*/
void APP_Clock_Init(void);
void APP_USART_Init(void);
void UART_SendByte(USART_TypeDef* USARTx, uint8_t data);
    
void Delay (uint32_t nCount);

extern int CoreMark(void);

/*******************************************************************************
* Function Name  : main
*******************************************************************************/
int main(void)
{
    APP_Clock_Init();
    APP_USART_Init();
    ee_printf("** CoreMark Test Start\r\n");

    CoreMark();

    ee_printf("** CoreMark Test Stop\r\n");

    while (1);
}

void SystemClock_HSIPLL(uint32_t mul, uint32_t ws) {
    // 1. Включить HSE
    //RCC->CR |= RCC_CR_HSEON;
    //while (!(RCC->CR & RCC_CR_HSERDY));

    // 2. Настроить PLL: HSE * 9 = 72 МГц
    //RCC->CFGR |= RCC_CFGR_PLLSRC;
    
    // 2. Настроить PLL: HSI * mul = xx МГц
    RCC->CFGR &= ~RCC_CFGR_PLLSRC_Msk;
    RCC->CFGR &= ~RCC_CFGR_PLLMULL_Msk;
    //RCC->CFGR |= RCC_CFGR_PLLMULL12;
    RCC->CFGR |= mul;

    // 3. Включить PLL
    RCC->CR |= RCC_CR_PLLON;
    while (!(RCC->CR & RCC_CR_PLLRDY));

    // 4. Настроить делители шин
    RCC->CFGR &= ~RCC_CFGR_HPRE;   // AHB = 1
    RCC->CFGR &= ~RCC_CFGR_PPRE2;  // APB2 = 1
    RCC->CFGR &= ~RCC_CFGR_PPRE1;  // APB1 = 2
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;

    // 5. Настроить Flash (2 wait states + prefetch)
    //FLASH->ACR = FLASH_ACR_PRFTBE | FLASH_ACR_LATENCY_1;
    FLASH->ACR &= ~FLASH_ACR_LATENCY;
    FLASH->ACR = FLASH_ACR_PRFTBE | ws;

    // 6. Переключить SYSCLK на PLL
    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);
}

void SystemClock_HSEPLL(uint32_t mul, uint32_t ws) {
    // 1. Включить HSE
    RCC->CR |= RCC_CR_HSEON;
    while (!(RCC->CR & RCC_CR_HSERDY));

    // 2. Настроить PLL: HSE * 6 = 48 МГц
    RCC->CFGR |= RCC_CFGR_PLLSRC;
    RCC->CFGR &= ~RCC_CFGR_PLLMULL_Msk;
    RCC->CFGR |= mul;

    // 3. Включить PLL
    RCC->CR |= RCC_CR_PLLON;
    while (!(RCC->CR & RCC_CR_PLLRDY));

    // 4. Настроить делители шин
    RCC->CFGR &= ~RCC_CFGR_HPRE;   // AHB = 1
    RCC->CFGR &= ~RCC_CFGR_PPRE2;  // APB2 = 1
    RCC->CFGR &= ~RCC_CFGR_PPRE1;  // APB1 = 2
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;

    // 5. Настроить Flash (1 wait states + prefetch)
    FLASH->ACR &= ~FLASH_ACR_LATENCY;
    FLASH->ACR = FLASH_ACR_PRFTBE | ws;

    // 6. Переключить SYSCLK на PLL
    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);
}


void APP_Clock_Init(void) {
    // Latency = 0 for 8 MHz
    FLASH->ACR &= ~FLASH_ACR_LATENCY_Msk;
    //FLASH->ACR |= FLASH_ACR_HLFCYA;

    SystemClock_HSIPLL(RCC_CFGR_PLLMULL6,0); // FLASH_ACR_LATENCY_0); 
    //SystemClock_HSEPLL(RCC_CFGR_PLLMULL12,FLASH_ACR_LATENCY_1); 
        
    SystemCoreClockUpdate();

    SysTick_Init(SYSTICK_1MS);
}


void APP_USART_Init(void) {
    // 1. Тактирование
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
    //RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
    __NOP();

    // 2. Настройка пинов
    GPIOA->CRL &= ~(GPIO_CRL_MODE2 | GPIO_CRL_CNF2 | GPIO_CRL_MODE3 | GPIO_CRL_CNF3);
    GPIOA->CRL |= (GPIO_CRL_MODE2_1 | GPIO_CRL_MODE2_0) | GPIO_CRL_CNF2_1;
    GPIOA->CRL |= GPIO_CRL_CNF3_0;

    GPIOA->CRH &= ~(GPIO_CRH_MODE9 | GPIO_CRH_CNF9);
    GPIOA->CRH |= (GPIO_CRH_MODE9_1 | GPIO_CRH_MODE9_0) | GPIO_CRH_CNF9_1;
    
    
    // 3. Скорость 115200 
    if  ( SystemCoreClock > 8000000 ) 
        USART2->BRR = SystemCoreClock/(115200*2);
    else
        USART2->BRR = SystemCoreClock/(115200);
    //USART1->BRR = SystemCoreClock/(115200*2);

    // 4. Включение
    USART2->CR1 = USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;
    //USART1->CR1 = USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;
}

// Отправка одного байта (блокирующий режим)
void UART_SendByte(USART_TypeDef* USARTx, uint8_t data) {
    while (!(USARTx->SR & USART_SR_TXE)); // Ждем, пока буфер TX пуст
    USARTx->DR = data;    
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


/*******************************************************************************
* Function Name  : Delay
*******************************************************************************/
void  Delay (uint32_t nCount)
{
  for(; nCount != 0; nCount--);
}


/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
