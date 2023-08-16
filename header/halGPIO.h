#ifndef _halGPIO_H_
#define _halGPIO_H_

#include  "../header/app.h"         // private library - APP layer
#include  "../header/bsp_msp430x4xx.h"



extern enum FSMstate state;   // global variable
extern enum SYSmode lpm_mode; // global variable
extern unsigned int FEdge;
extern unsigned int N_ADC12_A3, N_ADC12_A4;
extern unsigned int X_ms;
extern int count_up;
extern unsigned int REdge;
extern int j;
extern int Tele_degree;
extern int Max_dist;
extern int In_state_flag;
extern int LDR1_sample;
extern int LDR1_sample;
extern float LDR2[50];
extern float LDR1[50];

extern char file_content[];
extern char stringFromPC[];
extern int ExecuteFlag;
extern int FlashBurnIFG;
extern int file_to_execute;
extern int file_to_write;

extern unsigned long hex2int(char *hex);
extern char hexToChar(int var1, int var2);

extern void sysConfig(void);
extern void delay(unsigned int);
extern void enterLPM(unsigned char);
extern void enable_interrupts();
extern void disable_interrupts();

#endif

#define LCD_WAIT DelayMs(5)
#define LCD_EN(a)   (!a ? (P2OUT&=~0X20) : (P2OUT|=0X20)) // P2.5 is lcd enable pin
#define LCD_EN_DIR(a)   (!a ? (P2DIR&=~0X20) : (P2DIR|=0X20)) // P2.5 pin direction

#define LCD_RS(a)   (!a ? (P2OUT&=~0X40) : (P2OUT|=0X40)) // P2.6 is lcd RS pin
#define LCD_RS_DIR(a)   (!a ? (P2DIR&=~0X40) : (P2DIR|=0X40)) // P2.6 pin direction

#define LCD_RW(a)   (!a ? (P2OUT&=~0X80) : (P2OUT|=0X80)) // P2.7 is lcd RW pin
#define LCD_RW_DIR(a)   (!a ? (P2DIR&=~0X80) : (P2DIR|=0X80)) // P2.7 pin direction

#define LCD_DATA_OFFSET 0x04 //data pin selection offset for 4 bit mode, variable range is 0-4, default 0 - Px.0-3, no offset

#define FOURBIT_MODE    0x0
#define EIGHTBIT_MODE   0x1
#define LCD_MODE        FOURBIT_MODE

#define OUTPUT_PIN      1
#define INPUT_PIN       0
#define OUTPUT_DATA     (LCD_MODE ? 0xFF : (0x0F << LCD_DATA_OFFSET))
#define INPUT_DATA      0x00

#define LCD_STROBE_READ(value)  LCD_EN(1), \
                asm("nop"), asm("nop"), \
                value=LCD_DATA_READ, \
                LCD_EN(0)

#define lcd_cursor(x)       lcd_cmd(((x)&0x7F)|0x80)
#define lcd_clear()         lcd_cmd(0x01)
#define lcd_putchar(x)      lcd_data(x)
#define lcd_goto(x)         lcd_cmd(0x80+(x))
#define lcd_cursor_right()  lcd_cmd(0x14)
#define lcd_cursor_left()   lcd_cmd(0x10)
#define lcd_display_shift() lcd_cmd(0x1C)
#define lcd_home()          lcd_cmd(0x02)
#define cursor_off          lcd_cmd(0x0C)
#define cursor_on           lcd_cmd(0x0F)
#define lcd_function_set    lcd_cmd(0x3C) // 8bit,two lines,5x10 dots
#define lcd_new_line        lcd_cmd(0xC0)

extern void lcd_cmd(char);
extern void lcd_print(const char * s);
extern void lcd_data(char);
extern void lcd_init();
extern void lcd_strobe();
extern void DelayMs(unsigned int);
extern void DelayUs(unsigned int);
extern void Go_right_xtimes(int x);

extern void startTimerB0(int);
extern void stopTimerB0();
extern void ADC_on_A3();
extern void ADC_SI();
extern void set_pwm(int period, float T_on);
extern void StopTimerA(void);

extern void delay_via_timer(unsigned int);
extern void int2str(char*, unsigned int);

extern void Set_trigger(void);
extern void start_Timer_A2(void);

extern void send_message(int dist);
extern void send_degree(int deg);


