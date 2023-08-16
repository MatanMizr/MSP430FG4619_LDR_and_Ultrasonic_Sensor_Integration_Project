#include  "../header/halGPIO.h"             // private library - HAL layer
#include  "../header/bsp_msp430x4xx.h"
#include  "../header/flash.h"     // private library - FLASH layer
#include "stdio.h"
#include "stdint.h"
#include "string.h"

// Global Variables
int interruptflag = 0;
int In_state_flag = 0;
char file_content[80];
char stringFromPC[80];
int ExecuteFlag;
int FlashBurnIFG;
char byte;
int SendFlag = 0;
char *ptr1, *ptr2, *ptr3;
//--------------------------------------------------------------------
//             System Configuration  
//--------------------------------------------------------------------
void sysConfig(void){ 
	    GPIOconfig();
	    TIMER0_B0_config();
	    TIMER_A2_config();
        ADCconfig();
        TIMER1_A1_config();
        UART_init();
}
//--------------------------------------------------------------------
//                            LCD 
//--------------------------------------------------------------------

//--------------------------------------------------------------------
//                   Shift right several times
//--------------------------------------------------------------------
void Go_right_xtimes(int x){
    int t = 0;
    for (t = 0; t < x; t ++){
        lcd_cursor_right();
    }
}
//--------------------------------------------------------------------
//                   Send data to the LCD
//--------------------------------------------------------------------
void lcd_data(char c){

    LCD_WAIT; // may check LCD busy flag, or just delay a little, depending on lcd.h

    LCD_DATA_WRITE &= ~OUTPUT_DATA;
    LCD_RS(1);
    if (LCD_MODE == FOURBIT_MODE)
    {
            LCD_DATA_WRITE &= ~OUTPUT_DATA;
            LCD_DATA_WRITE |= ((c >> 4) & 0x0F) << LCD_DATA_OFFSET;
            lcd_strobe();
            LCD_DATA_WRITE &= (0xF0 << LCD_DATA_OFFSET) | (0xF0 >> 8 - LCD_DATA_OFFSET);
            LCD_DATA_WRITE &= ~OUTPUT_DATA;
            LCD_DATA_WRITE |= (c & 0x0F) << LCD_DATA_OFFSET;
            lcd_strobe();
    }
    else
    {
            LCD_DATA_WRITE = c;
            lcd_strobe();
    }

    LCD_RS(0);
}
//--------------------------------------------------------------------
//                     Write a string to LCD 
//--------------------------------------------------------------------
void lcd_print(const char *s){
    while(*s)
        lcd_data(*s++);
}
//--------------------------------------------------------------------
//                        LCD COMMAND
//--------------------------------------------------------------------
void lcd_cmd(char c){

    LCD_WAIT; // may check LCD busy flag, or just delay a little, depending on lcd.h

    if (LCD_MODE == FOURBIT_MODE)
    {
        LCD_DATA_WRITE &= ~OUTPUT_DATA;// clear bits before new write
        LCD_DATA_WRITE |= ((c >> 4) & 0x0F) << LCD_DATA_OFFSET;
        lcd_strobe();
        LCD_DATA_WRITE &= ~OUTPUT_DATA;
        LCD_DATA_WRITE |= (c & (0x0F)) << LCD_DATA_OFFSET;
        lcd_strobe();
    }
    else
    {
        LCD_DATA_WRITE = c;
        lcd_strobe();
    }
}
//--------------------------------------------------------------------
//                            Delay usec  
//--------------------------------------------------------------------
void DelayUs(unsigned int cnt){

    unsigned char i;
    for(i=cnt ; i>0 ; i--) asm("NOP"); // tha command asm("nop") takes raphly 1usec

}
//--------------------------------------------------------------------
//                            Delay msec
//--------------------------------------------------------------------
void DelayMs(unsigned int cnt){

    unsigned char i;
    for(i=cnt ; i>0 ; i--) DelayUs(1000); // tha command asm("nop") takes raphly 1usec

}
//--------------------------------------------------------------------
//                           LCD Strobe
//--------------------------------------------------------------------
void lcd_strobe(){
  LCD_EN(1);
  asm("Nop");
  asm("nop");
  LCD_EN(0);
}
//--------------------------------------------------------------------
//                         LCD initialize  
//--------------------------------------------------------------------
void lcd_init(){

    char init_value;

    if (LCD_MODE == FOURBIT_MODE) init_value = 0x3 << LCD_DATA_OFFSET;
    else init_value = 0x3F;

    LCD_RS_DIR(OUTPUT_PIN);
    LCD_EN_DIR(OUTPUT_PIN);
    LCD_RW_DIR(OUTPUT_PIN);
    LCD_DATA_DIR |= OUTPUT_DATA;
    LCD_RS(0);
    LCD_EN(0);
    LCD_RW(0);

    DelayMs(15);
    LCD_DATA_WRITE &= ~OUTPUT_DATA;
    LCD_DATA_WRITE |= init_value;
    lcd_strobe();
    DelayMs(5);
    LCD_DATA_WRITE &= ~OUTPUT_DATA;
    LCD_DATA_WRITE |= init_value;
    lcd_strobe();
    DelayUs(200);
    LCD_DATA_WRITE &= ~OUTPUT_DATA;
    LCD_DATA_WRITE |= init_value;
    lcd_strobe();

    if (LCD_MODE == FOURBIT_MODE){
        LCD_WAIT; // may check LCD busy flag, or just delay a little, depending on lcd.h
        LCD_DATA_WRITE &= ~OUTPUT_DATA;
        LCD_DATA_WRITE |= 0x2 << LCD_DATA_OFFSET; // Set 4-bit mode
        lcd_strobe();
        lcd_cmd(0x28); // Function Set
    }
    else lcd_cmd(0x3C); // 8bit,two lines,5x10 dots

    lcd_cmd(0xF); //Display On, Cursor On, Cursor Blink
    lcd_cmd(0x1); //Display Clear
    lcd_cmd(0x6); //Entry Mode
    lcd_cmd(0x80); //Initialize DDRAM address to zero
}


//---------------------------------------------------------------------
//            Polling based Delay function
//---------------------------------------------------------------------
void delay(unsigned int t){  // t[msec]
	volatile unsigned int i;
	
	for(i=t; i>0; i--);
}
//---------------------------------------------------------------------
//            Enter from LPM0 mode
//---------------------------------------------------------------------
void enterLPM(unsigned char LPM_level){
	if (LPM_level == 0x00) 
	  _BIS_SR(LPM0_bits);     /* Enter Low Power Mode 0 */
        else if(LPM_level == 0x01) 
	  _BIS_SR(LPM1_bits);     /* Enter Low Power Mode 1 */
        else if(LPM_level == 0x02) 
	  _BIS_SR(LPM2_bits);     /* Enter Low Power Mode 2 */
	else if(LPM_level == 0x03) 
	  _BIS_SR(LPM3_bits);     /* Enter Low Power Mode 3 */
        else if(LPM_level == 0x04) 
	  _BIS_SR(LPM4_bits);     /* Enter Low Power Mode 4 */
}
//---------------------------------------------------------------------
//                      Enable interrupts
//---------------------------------------------------------------------
void enable_interrupts(){
  _BIS_SR(GIE);
}
//---------------------------------------------------------------------
//                         Disable interrupts
//---------------------------------------------------------------------
void disable_interrupts(){
  _BIC_SR(GIE);
}

//---------------------------------------------------------------------
//          Convert Hex to integer ( Hex to Decimal )
//---------------------------------------------------------------------
char hexToChar(int var1, int var2){
    int hexValue = (var1 << 4) | var2; // combine var1 and var2 to form the hex value
    if ((hexValue >= 20 && hexValue <= 0x7E) || (hexValue >= 0x09 && hexValue <= 0xD0)) return (char)hexValue; // printable ASCII characters
    else return '\0';
}
//---------------------------------------------------------------------
//          Convert Hex to integer ( Hex to Decimal )
//---------------------------------------------------------------------
unsigned long hex2int(char *hex) {
    unsigned long val = 0;
    for(j=0; j<2; j++) {
        // get current character then increment
        byte = *hex++;
        // transform hex character to the 4bit equivalent number, using the ASCII table indexes
        if (byte >= '0' && byte <= '9') byte = byte - '0';
        else if (byte >= 'a' && byte <='f') byte = byte - 'a' + 10;
        else if (byte >= 'A' && byte <='F') byte = byte - 'A' + 10;

        val = (val << 4) | (byte & 0xF);        // shift 4 to make space for new digit, and add the 4 bits of the new digit
    }
    return val;
}
//---------------------------------------------------------------------
//            TimerB0 Interrupt Service Routine
//---------------------------------------------------------------------
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = TIMERB0_VECTOR
__interrupt void Timer_B(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMERB0_VECTOR))) Timer_B (void)
#else
#error Compiler not supported!
#endif
{

    LPM0_EXIT;
    TBCTL = MC_0 + TBCLR;
}

//---------------------------------------------------------------------
//           ADC12 Vector Interrupt Service Routine
//---------------------------------------------------------------------
#pragma vector = ADC12_VECTOR
__interrupt void ADC12_ISR (void)
{

        N_ADC12_A3 = ADC12MEM3;
        N_ADC12_A4 = ADC12MEM4;
    __bic_SR_register_on_exit(CPUOFF);
}

//---------------------------------------------------------------------
//                        StartTimer B0 ( state 2 )
//---------------------------------------------------------------------
void startTimerB0( int up_to){
    TBCCR0 = up_to;
    TBCTL = TBSSEL_2 + MC_1 + ID_3;
    TBCCTL0 |= CCIE;               //  select: 2 - SMCLK ; control: 3 - Up/Down  ; divider: 3 - /8
    // ACLK doesn't work on our msp, so we have to use smclk and divide the freq to get to 1 sec.
    __bis_SR_register(LPM0_bits + GIE);       // Enter LPM0 w/ interrupt
}
//---------------------------------------------------------------------
//                        StartTimer A layer 2 ( Ultrasonic )
//---------------------------------------------------------------------
void start_Timer_A2(void){
    //TACCR0 = 0xFFFF;
    TACTL |= MC_1;
    TACCTL2 |= CCIE;


}
//---------------------------------------------------------------------
//                        StOPTimer B0 ( state 2 )
//---------------------------------------------------------------------
void stopTimerB0(){
    TBCTL = MC_0;
    TBCCTL0 &= ~CCIE;
}

//---------------------------------------------------------------------
//                        ADCon_A3 ( state 3 )
//---------------------------------------------------------------------
void ADC_on_A3(){
    ADC12IE = 0x10;
    ADC12CTL0 |= ENC + ADC12SC;             // Start sampling
}

//---------------------------------------------------------------------
//                        ADC stop interrupts ( state 3 )
//---------------------------------------------------------------------
void ADC_SI(){
    ADC12IE = 0x00;
    ADC12CTL0 &= ~ADC12ON;
}

//---------------------------------------------------------------------
//                        SET_PWM ( state 3 )
//---------------------------------------------------------------------
void set_pwm(int period, float T_on){
    TACCR0 = period;
    TACCR1 = TACCR0 * (T_on/Stepper_freq);
    TACTL |= MC_1;
}

//---------------------------------------------------------------------
//                        delay for X_ms
//---------------------------------------------------------------------
void delay_via_timer(unsigned int time){
    time *= 10;
    while (time >= 500){
        time -= 500;
        startTimerB0(0xFFFF);
    }
    if (time != 0){
        startTimerB0(131 * time);
    }
}


//---------------------------------------------------------------------
//                    Convert int to string
//---------------------------------------------------------------------
void int2str(char *str, unsigned int num){
    int strSize = 0;
    long tmp = num, len = 0;
    int j;
    // Find the size of the intPart by repeatedly dividing by 10
    while(tmp){
        len++;
        tmp /= 10;
    }

    // Print out the numbers in reverse
    for(j = len - 1; j >= 0; j--){
        str[j] = (num % 10) + '0';
        num /= 10;
    }
    strSize += len;
    str[strSize] = '\0';
}
//---------------------------------------------------------------------
//                    Stop timer A ( state 3 )
//---------------------------------------------------------------------
void StopTimerA(void){
    TACTL &= ~MC_1;
}

//-------------------------------------------------------------
//        Set trigger to ultrasonic trigger leg
//-------------------------------------------------------------
void Set_trigger(void){
    UltrasonicPortOut |= Triggerleg;
    startTimerB0(0x015);
    start_Timer_A2();
    UltrasonicPortOut &= ~Triggerleg;
}

//-------------------------------------------------------------
//        Timer_A3 Interrupt Vector (TAIV) handler
//-------------------------------------------------------------
#pragma vector=TIMERA1_VECTOR
__interrupt void Timer_A1(void)
{
  switch( TAIV )
  {
  case  2: break;
  case  4:
      if(!interruptflag){
          REdge = TACCR2;
          interruptflag++;
      }
      else{
          FEdge = TACCR2;
          interruptflag = 0x00;
          TACCTL2 &= ~CCIE;
          LPM0_EXIT;
      }
           break;
  case 10: break;
 }
}

//-------------------------------------------------------------
//                send message to PC side
//-------------------------------------------------------------
void send_message(int dist){
    U1TXBUF = dist / 2;
    //IFG2 &= ~UTXIFG1;
    IE2 |= UTXIE1;
    _BIS_SR(LPM0_bits);     /* Enter Low Power Mode 0 */
}

void send_degree(int deg){
    U1TXBUF = deg;
    IE2 |= UTXIE1;
    _BIS_SR(LPM0_bits);     /* Enter Low Power Mode 0 */
}

//*********************************************************************
//                           TX ISR
//*********************************************************************
#pragma vector=USART1TX_VECTOR
__interrupt void USART1_TX (void)
{

    if (state == state1) LPM0_EXIT;
    else if (state == state2) LPM0_EXIT;
    else if (state == state3) LPM0_EXIT;
    else if (state == state4) LPM0_EXIT;
    else if (state == state5) LPM0_EXIT;
    IE2 &= ~UTXIE1;                       // Disable USCI_A0 TX interrupt
}

//*********************************************************************
//                         RX ISR
//*********************************************************************
#pragma vector=USART1RX_VECTOR
__interrupt void USART1_RX (void)
{
    if ((In_state_flag) && (U1RXBUF != 255)){
        if (state == state1){
            if (U1RXBUF == 226) LPM0_EXIT;          // 226 - start scan
            else Max_dist = U1RXBUF * 2;            // update Max distance
        }

        if(state == state2){
            Tele_degree = U1RXBUF;                  // Set degree to Telemeter
            LPM0_EXIT;
        }

        if(state == state3){
            if (U1RXBUF == 226) LPM0_EXIT;          // 226 - start scan
        }

        if(state == state4){
            if (U1RXBUF == 226) LPM0_EXIT;          // 226 - start scan
            else if (U1RXBUF == 227) LPM0_EXIT;
            else Max_dist = U1RXBUF * 2;            // update Max distance
        }

        if(state == state5){
            stringFromPC[k] = U1RXBUF;
            k++;
            if (stringFromPC[k-1] == 'Z'){          // This if to get the file data. Added 'Z' to the end of the data in the PC file, acts like ACK
                k = 0;
                SendFlag = 0;
                for (j = 0; j < 80; j++) file_content[j] = stringFromPC[j];
            }
            // This if to get the file name
            if (!SendFlag && stringFromPC[k-1] == '\x0a'){
                for (j=0; j < k; j++) file.file_name[j] = stringFromPC[j];
                SendFlag = 1;
                k = 0;
            }
            if (stringFromPC[k-1] == 'a'){          //pointer for 1st selected file
                FlashBurnIFG = 1;
                ptr1 = (char*) 0x1000;
                file.file_ptr[0]=ptr1;
                file.num_of_files = 1;
                k = 0;
            }
            if (stringFromPC[k-1] == 'b'){          //pointer for 2nd selected file
                FlashBurnIFG = 1;
                ptr2 = (char*) 0x1040;
                file.file_ptr[1]=ptr2;
                file.num_of_files = 2;
                k = 0;
            }
            if (stringFromPC[k-1] == 'd'){          //pointer for 3rd selected file
                FlashBurnIFG = 1;
                ptr3 = (char*) 0x1080;
                file.file_ptr[2]=ptr3;
                file.num_of_files = 3;
                k = 0;
            }

            if (stringFromPC[k-1] == 'L'){        //index of executed list
                ExecuteFlag = 1;
                k = 0;
                file.num_of_files = 1;
            }
            if (stringFromPC[k-1] == 'U'){        //index of executed list
                ExecuteFlag = 1;
                k = 0;
                file.num_of_files = 2;
            }
            if (stringFromPC[k-1] == 'V'){        //index of executed list
                ExecuteFlag = 1;
                k = 0;
                file.num_of_files = 3;
            }
            LPM0_EXIT;
        }

    }

    if (!In_state_flag){

       if(U1RXBUF == 'O'){             // Objects Detector
            state = state1;
            In_state_flag ++;
            LPM0_EXIT;
        }

        else if(U1RXBUF == 'T'){        // Telemeter
            state = state2;
            In_state_flag ++;
            LPM0_EXIT;
        }

        else if(U1RXBUF == 'L'){        // Light Detector
            state = state3;
            In_state_flag ++;
            LPM0_EXIT;
        }

        else if(U1RXBUF == 'D'){        // Light source and Objects Detector
            state = state4;
            In_state_flag ++;
            LPM0_EXIT;
            }

        else if(U1RXBUF == 'S'){        // Script mode
            state = state5;
            In_state_flag ++;
            LPM0_EXIT;
        }

        else if(U1RXBUF == 'C'){        // Calibration
            state = state6;
            LPM0_EXIT;
        }
    }

    if(U1RXBUF == 255){
        In_state_flag = 0x00;
        state = state0;
        LPM0_EXIT;
    }

    /*
    switch(lpm_mode){
    case mode0:
        LPM0_EXIT; // must be called from ISR only
        break;
    case mode1:
        LPM1_EXIT; // must be called from ISR only
        break;
    case mode2:
        LPM2_EXIT; // must be called from ISR only
        break;
    case mode3:
        LPM3_EXIT; // must be called from ISR only
        break;
    case mode4:
        LPM4_EXIT; // must be called from ISR only
        break;
    }
    */
}

//*********************************************************************
//            Port1 Interrupt Service Rotine
//*********************************************************************
#pragma vector=PORT1_VECTOR
  __interrupt void PBs_handler(void){

    delay(10000);
    if(PB3ArrIntPend & PB3){
        PB3ArrIntPend &= 0x00;
        LPM0_EXIT;
    }
}

