
#include "main.h"
#include "coremark.h"


/* Private define ------------------------------------------------------------*/
#define SYSTICK_1MS ((uint32_t)1000)
#define SYSTICK_COUNTER_DIASBLE ((uint32_t)0xFFFFFFFE)
#define SYSTICK_COUNTER_ENABLE ((uint32_t)0x00000001)
#define SYSTICK_COUNTER_CLEAR ((uint32_t)0x00000000)
#define COREMARK_USART_BAUDRATE ((uint32_t)115200)


void APP_Clock_Init12(void);
void APP_Clock_Init24(void);
void APP_Clock_Init48(void);
void APP_GPIO_Init(void);
void APP_GPIO_UART_Init(void);
void APP_UART_Init(void);

void SysTick_Init(uint32_t);
void SysTick_Stop_time(void);

extern int CoreMark(void);

int main(void) {
    APP_Clock_Init48();
    SysTick_Init(SYSTICK_1MS);
    //APP_GPIO_Init();
    APP_GPIO_UART_Init();
    APP_UART_Init();

    ee_printf("** CoreMark Test Start\r\n");

    CoreMark();

    ee_printf("** CoreMark Test Stop\r\n");

    
    while (1);
}    

void APP_Clock_Init12(void) {
    stc_sysctrl_clk_cfg_t stcCfg;
    
    // FLASH Clock
    Sysctrl_SetPeripheralGate(SysctrlPeripheralFlash, TRUE);
    
    // HCLK  24 MHz：FLASH - 0 cycle 
    Flash_WaitCycle(FlashWaitCycle0);
    
    // 24 MHz  RCH
    Sysctrl_SetRCHTrim(SysctrlRchFreq24MHz);             
    
    stcCfg.enClkSrc  = SysctrlClkRCH;

    // HCLK SYSCLK/2
    stcCfg.enHClkDiv = SysctrlHclkDiv2;
    // PCLK HCLK/1
    stcCfg.enPClkDiv = SysctrlPclkDiv1;

    Sysctrl_ClkInit(&stcCfg);     
    
    SystemCoreClockUpdate();
}


void APP_Clock_Init24(void) {
    stc_sysctrl_clk_cfg_t stcCfg;
    
    // FLASH Clock
    Sysctrl_SetPeripheralGate(SysctrlPeripheralFlash, TRUE);
    
    // HCLK  24 MHz：FLASH - 0 cycle 
    Flash_WaitCycle(FlashWaitCycle0);
    
    // 24 MHz  RCH
    Sysctrl_SetRCHTrim(SysctrlRchFreq24MHz);             
    
    stcCfg.enClkSrc  = SysctrlClkRCH;

    // HCLK SYSCLK/1
    stcCfg.enHClkDiv = SysctrlHclkDiv1;
    // PCLK HCLK/1
    stcCfg.enPClkDiv = SysctrlPclkDiv1;

    Sysctrl_ClkInit(&stcCfg);     
    
    SystemCoreClockUpdate();
}


void APP_Clock_Init48(void) {
    stc_sysctrl_clk_cfg_t stcCfg;
    stc_sysctrl_pll_cfg_t stcPLLCfg;    
    
    // FLASH Clock
    Sysctrl_SetPeripheralGate(SysctrlPeripheralFlash, TRUE);
    
    // PLL  HCLK  48MHz：FLASH - 1 cycle 
    Flash_WaitCycle(FlashWaitCycle1);
    
    // PLL input 4 MHz  RCH
    Sysctrl_SetRCHTrim(SysctrlRchFreq4MHz);             
    
    stcPLLCfg.enInFreq    = SysctrlPllInFreq4_6MHz;     // RCH 4MHz
    stcPLLCfg.enOutFreq   = SysctrlPllOutFreq36_48MHz;  // PLL 48MHz
    //stcPLLCfg.enOutFreq   = SysctrlPllOutFreq24_36MHz;  // PLL 48MHz
    stcPLLCfg.enPllClkSrc = SysctrlPllRch;              // sourceRCH
    stcPLLCfg.enPllMul    = SysctrlPllMul12;            // 4MHz x 12 = 48MHz
    Sysctrl_SetPLLFreq(&stcPLLCfg);       
    
    // PLL is HCLK
    stcCfg.enClkSrc  = SysctrlClkPLL;

    // HCLK SYSCLK/1
    stcCfg.enHClkDiv = SysctrlHclkDiv1;
    // PCLK HCLK/1
    stcCfg.enPClkDiv = SysctrlPclkDiv1;

    Sysctrl_ClkInit(&stcCfg);     
    
    SystemCoreClockUpdate();
}


void APP_GPIO_Init(void) {
    stc_gpio_cfg_t        stcGpioCfg;
    
    // Enable GPIO 
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);
    
    stcGpioCfg.enDir = GpioDirOut;
    stcGpioCfg.enDrv = GpioDrvH;
    stcGpioCfg.enPu = GpioPuDisable;
    stcGpioCfg.enPd = GpioPdDisable;
    stcGpioCfg.enOD = GpioOdDisable;
    Gpio_Init(GpioPortB, GpioPin0, &stcGpioCfg);
    Gpio_Init(GpioPortB, GpioPin1, &stcGpioCfg);
    Gpio_Init(GpioPortA, GpioPin1, &stcGpioCfg);
    
    //   PLL to PB00
    Gpio_SetAfMode(GpioPortB, GpioPin0, GpioAf7);
    
    //   HCLK to PA01
    Gpio_SfHClkOutputCfg(GpioSfHclkOutEnable, GpioSfHclkOutDiv1);
    Gpio_SetAfMode(GpioPortA, GpioPin1, GpioAf6);
    
}


void APP_GPIO_UART_Init(void) {
    stc_gpio_cfg_t stcGpioCfg;
    
    DDL_ZERO_STRUCT(stcGpioCfg);
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio,TRUE); 
    
    stcGpioCfg.enDir = GpioDirOut;
    Gpio_Init(GpioPortA,GpioPin9,&stcGpioCfg);
    Gpio_SetAfMode(GpioPortA,GpioPin9,GpioAf1); // PA09 UART0 TX
    stcGpioCfg.enDir = GpioDirIn;
    Gpio_Init(GpioPortA,GpioPin10,&stcGpioCfg);
    Gpio_SetAfMode(GpioPortA,GpioPin10,GpioAf1);// PA10 UART0 RX
}
void APP_UART_Init0(void) {
    Sysctrl_SetPeripheralGate(SysctrlPeripheralUart0,TRUE); // UART0 clock enable
    M0P_UART0->SCON = UartMskMode1 | UartMsk1bit | UartMsk8Or16Div;
    M0P_UART0->SCNT =52; 
    M0P_UART0->SCON |= 1 << UartRenFunc;
    Uart_ClrStatus(M0P_UART0,UartRC);    
    Uart_ClrStatus(M0P_UART0,UartTC);    
    Uart_EnableIrq(M0P_UART0,UartTxIrq); 
}

void APP_UART_Init(void) {
    stc_uart_cfg_t  stcCfg;
    stc_uart_multimode_t stcMulti;
    stc_uart_baud_t stcBaud;

    DDL_ZERO_STRUCT(stcCfg);
    DDL_ZERO_STRUCT(stcMulti);
    DDL_ZERO_STRUCT(stcBaud);
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralUart0,TRUE); // UART0 clock enable
    
    stcCfg.enRunMode = UartMskMode1;     // mode 1
    stcCfg.enStopBit = UartMsk1bit;      //
    stcCfg.enMmdorCk = UartMskDataOrAddr;  //
    stcCfg.stcBaud.u32Baud = 115200;       //  115200
    stcCfg.stcBaud.enClkDiv = UartMsk8Or16Div;         
    stcCfg.stcBaud.u32Pclk = Sysctrl_GetPClkFreq();    
    Uart_Init(M0P_UART0, &stcCfg);       

    Uart_ClrStatus(M0P_UART0,UartRC);    
    Uart_ClrStatus(M0P_UART0,UartTC);    
    //Uart_EnableIrq(M0P_UART0,UartRxIrq); 
    //Uart_EnableIrq(M0P_UART0,UartTxIrq); 
}


void SysTick_Init(uint32_t NUM) {
    /* SystemCoreClock / NUM */
    if (SysTick_Config(SystemCoreClock / NUM))
    {
        while (1)
            ;
    }
}    
    
void SysTick_Stop_time(void) {
    SysTick->CTRL &= SYSTICK_COUNTER_DIASBLE;
    /* Clear the SysTick Counter */
    SysTick->VAL = SYSTICK_COUNTER_CLEAR;
}


