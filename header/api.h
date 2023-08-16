#ifndef _api_H_
#define _api_H_

#include  "../header/halGPIO.h"     // private library - HAL layer

extern void Count_up();
extern void change_Xms_delay();
extern void rra_lcd(char ch);
extern void dec_lcd(int x);
extern void inc_lcd(int x);
extern void Get_motor_to(unsigned int degree);
extern void Get_Sample_Ultrasonic();
extern void sample_LDR(void);
extern void Telemeter(int deg);
extern void Calibrate_LDR(void);
extern void set_delay(unsigned int d);
extern void servo_scan(int l, int r);
extern void send_dist(void);
extern void light_scan(void);
extern void scan_lights_objects(void);
extern void ScriptFunc();
extern void ExecuteScript();
extern void process_data(int* arr, int size, int* final_arr);
#endif







