#include  "../header/bsp_msp430x4xx.h"       // private library - BSP layer


//-----------------------------------------------------------------------------  
//           GPIO configuration
//-----------------------------------------------------------------------------
void GPIOconfig(void){
 // volatile unsigned int i; // in case of while loop usage
  
  WDTCTL = WDTHOLD | WDTPW;	      // Stop WDT
  // LCD configuration
  LCD_DATA_WRITE &= 0x00;             // clear LCD_DATA
  LCD_DATA_SEL &= 0x0F;               // Bit clear P1.4-P1.7
  LCD_DATA_DIR |= 0xF0;               // P1.4-P1.7 To Output('1')
  LCD_CONTROL_SEL  &= 0x1F;           // P2.5-P2.7 - GPIO
  LCD_CONTROL_DIR  &= 0x1F;           // P2.5 - P2.7 - INPUT

  // PWM Setup
  PWMPortSel |= 0x04;             // P1.2 Select = '1'
  PWMPortDir |= 0x04;             // P1.2 Output compare - '1'
  PWMPortOut &= ~0x04;            // P1.2 out = '0'

  // PushButton 3 Setup
    PB3ArrPortSel &= ~0x01;              // P1.0 GPIO capability
    PB3ArrPortDir &= ~0x01;              // P1.0 - Input '0'
    PB3ArrIntEdgeSel &= ~0x01;           // pull-down mode P1.0 = '0'
    PB3ArrIntEn |= 0x01;                 // Enable interrupts to PB3
    PB3ArrIntPend &= ~0x01;              // clear pending interrupts

  // Ultrasonic config
  UltrasonicPortSEL |= Echoleg;
  UltrasonicPortDIR &= ~Echoleg;
  UltrasonicPortSEL &= ~Triggerleg;
  UltrasonicPortDIR |= Triggerleg;


  // ADC12 Setup
  ADC12portSel |= 0x18;            // P6.3 ADC option select


  // enable interrupts
  _BIS_SR(GIE);                     // enable interrupts globally
}                             


//------------------------------------------------------------------------------------- 
//            Timer 0.5sec configuration - For state2 - layer 0
//-------------------------------------------------------------------------------------
void TIMER0_B0_config(void){
    WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
    TBCCTL0 = CCIE;
    TBCCR0 = 0xFFFF;
    TBCTL = TBSSEL_2 + MC_0 + ID_3;  //  select: 2 - SMCLK ; control: 3 - Up/Down  ; divider: 3 - /8
    //__bis_SR_register(LPM0_bits + GIE);       // Enter LPM0 w/ interrupt
} 

//------------------------------------------------------------------------------------- 
//                         TimerA Capture configuration
//-------------------------------------------------------------------------------------
void TIMER_A2_config(void){
    TACCTL2 = CM_3 + CCIS_0 + SCS + CAP; // Capture the signal at p3.7 - Echo leg
}
//-------------------------------------------------------------------------------------
//            ADC configuration
//-------------------------------------------------------------------------------------
void ADCconfig(void){
      ADC12CTL0 = SHT0_15 + ADC12ON + MSC;      // 16*ADC12CLK+ Turn on
      ADC12CTL1 = SHP + CSTARTADD_3 + CONSEQ_1;     // source select + ADC12MEM3 register
      ADC12MCTL3= INCH_3;                // A3 input channel
      ADC12MCTL4 = INCH_4 + EOS;                // A4 input channel
      ADC12IE = 0x10;                    // enable interrupts
      ADC12CTL0 |= ENC;                  // ADC12 enable conversion
}

//-------------------------------------------------------------------------------------
//            Timer1 A configuration - For state3 - layer 1
//-------------------------------------------------------------------------------------
void TIMER1_A1_config(void){
    TACTL = TASSEL_2 + MC_0 + ID_3 + TACLR;  //  select: 2 - SMCLK ; control: 3 - Up/Down  ; divider: 3 - /8
    TACCR0 = 0xFFFF;
    TACCTL1 =  OUTMOD_7; // TACCR1 reset/set;
}

//-------------------------------------------------------------------------------------
//                              UART init
//-------------------------------------------------------------------------------------
void UART_init(void){
    volatile unsigned int i;

    WDTCTL = WDTPW + WDTHOLD;                // Stop WDT
    FLL_CTL0 |= XCAP14PF;                    // Configure load caps

    do
    {
    IFG1 &= ~OFIFG;                          // Clear OSCFault flag
    for (i = 0x47FF; i > 0; i--);            // Time for flag to set
    }
    while ((IFG1 & OFIFG));                  // OSCFault flag still set?

    P4SEL |= 0x03;                           // P4.1,0 = USART1 TXD/RXD
    ME2 |= UTXE1 + URXE1;                    // Enable USART1 TXD/RXD
    U1CTL |= CHAR;                           // 8-bit character
    U1TCTL |= 0x20;                          // UCLK = SMCLK
    U1BR0 = 104;                             // according to data sheet -> 104
    U1BR1 = 0x00;                            //
    U1MCTL = 0x4A;                           // Modulation
    U1CTL &= ~SWRST;                         // Initialize USART state machine
    IE2 |= URXIE1;                           // Enable USART1 RX interrupt
}


 
             
             
            
  

