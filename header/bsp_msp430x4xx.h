#ifndef _bsp_H_
#define _bsp_H_

#include  <msp430FG4619.h>  // MSP430x4xx


#define   debounceVal      1000
#define   LEDs_SHOW_RATE   0xFFFF  // 62_5ms

// LCDs abstraction
#define LCD_DATA_DIR       P1DIR
#define LCD_DATA_SEL       P1SEL
#define LCD_DATA_READ      P1IN
#define LCD_DATA_WRITE     P1OUT
#define LCD_CONTROL_SEL    P2SEL
#define LCD_CONTROL_DIR    P2DIR

//PWN abstraction
#define PWMPortSel        P1SEL
#define PWMPortDir        P1DIR
#define PWMPortOut        P1OUT
#define PWMPortleg        0x04

// ADC12 pin
#define ADC12portSel       P6SEL

//Ultrasonic define
#define Echoleg            0x01         // P2.0
#define Triggerleg         0x02         // P2.1
#define UltrasonicPortSEL  P2SEL
#define UltrasonicPortDIR  P2DIR
#define UltrasonicPortOut  P2OUT
#define Distance           17322

// PushButtons 3 abstraction
#define PB3ArrPort         P1IN
#define PB3ArrIntPend      P1IFG
#define PB3ArrIntEn        P1IE
#define PB3ArrIntEdgeSel   P1IES
#define PB3ArrPortSel      P1SEL
#define PB3ArrPortDir      P1DIR
#define PB3ArrPortOut      P1OUT
#define PB3                0x01     // P1.0
// LDR abstraction

// UART defines
#define TXLED BIT0
#define RXLED BIT6
#define TXD BIT2
#define RXD BIT1

// Stepper motor defines
#define Stepper_freq        20

extern void GPIOconfig(void);
extern void TIMER0_B0_config(void);
extern void ADCconfig(void);
extern void TIMER1_A1_config(void);
extern void UART_init(void);
extern void TIMER_A2_config(void);
#endif



