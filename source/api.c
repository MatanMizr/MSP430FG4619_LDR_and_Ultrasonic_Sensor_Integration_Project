#include  "../header/api.h"    		// private library - API layer
#include  "../header/halGPIO.h"     // private library - HAL layer
#include  "../header/flash.h"     // private library - FLASH layer
#include "stdio.h"

// Global Variables
float LDR1[50] = {1.51179,1.51179,1.51179,1.51179,1.51179,1.6541,1.79642,1.93873,2.08105,2.22336,2.31217,2.40097,2.48978
                  ,2.57859,2.66739,2.76426,2.86112,2.95798,3.05485,3.15171,3.18137,3.21102,3.24068,3.27034,3.2999, 3.2999, 3.2999
                  , 3.2999, 3.2999, 3.2999, 3.2999, 3.2999, 3.2999, 3.2999, 3.2999, 3.2999, 3.2999, 3.2999, 3.2999, 3.2999, 3.2999
                  , 3.2999, 3.2999, 3.2999, 3.2999, 3.2999, 3.2999, 3.2999, 3.2999, 3.2999};

float LDR2[50] = {1.44651, 1.44651, 1.44651, 1.44651, 1.44651,1.62187 ,1.79722 ,1.97258,2.14793,2.32329,2.41355,2.50380,2.59406
                  ,2.68432,2.77457,2.87579,2.977,3.07822,3.17944,3.28065,3.28452,3.28839,3.29226,3.29613, 3.2999, 3.2999, 3.2999
                  , 3.2999, 3.2999, 3.2999, 3.2999, 3.2999, 3.2999, 3.2999, 3.2999, 3.2999, 3.2999, 3.2999, 3.2999, 3.2999, 3.2999
                  , 3.2999, 3.2999, 3.2999, 3.2999, 3.2999, 3.2999, 3.2999, 3.2999, 3.2999};
float max_dif[10] = {0.07836,0.12,0.128916,0.154728,0, 0, 0, 0, 0, 0};
int right_angle, left_angle;
char character;
unsigned int X_ms = 50;
int count_up = 0;
int j = 0;
int* count_up_address = &count_up;
char count_up_str[6];
unsigned int N_ADC12_A3, N_ADC12_A4;
float T = 0;
unsigned int REdge = 0;
unsigned int FEdge = 0;
unsigned long int tempo = 0;
float Range = 0;
float one_step = 0.00080586;
float voltage1, voltage2;
int q;
int max;
int Tele_degree = 0;
int LDR1_sample;
int LDR2_sample;
int Max_dist = 450;
int flag_script = 1;
char *Flash_ptrscript;                         // Segment pointer
char OPCstr, Operand1Flash[2], Operand2Flash[2];
unsigned int Oper2ToInt, X, start, stop, y;
float avg_Range = 0;
int avg_counter = 0;
float tmp_sample = 0;
int i;
//-------------------------------------------------------------
//                Count up onto LCD (state 2)
//-------------------------------------------------------------
void inc_lcd(int x){
    while ((*count_up_address <= x) && (state == state5)){
        lcd_clear();
        lcd_home();
        int2str(count_up_str, *count_up_address);
        lcd_print(count_up_str);
        cursor_off;
        delay_via_timer(X_ms);
        *count_up_address = (*count_up_address + 1) % 65536;
    }
    *count_up_address = 0;

}

//-------------------------------------------------------------
//                Count down onto LCD (state 2)
//-------------------------------------------------------------
void dec_lcd(int x){
    *count_up_address = x;
    while ((*count_up_address >= 0) && (state == state5)){
        lcd_clear();
        lcd_home();
        int2str(count_up_str, *count_up_address);
        lcd_print(count_up_str);
        cursor_off;
        delay_via_timer(X_ms);
        *count_up_address = (*count_up_address - 1);
    }
    *count_up_address = 0;
}

//-------------------------------------------------------------
//                Rotate right onto LCD (state 2)
//-------------------------------------------------------------
void rra_lcd(char ch){
    i = 0;
    for(i = 0; i < 16; i++){
        lcd_clear();
        Go_right_xtimes(i);
        lcd_data(ch);
        cursor_off;
        delay_via_timer(X_ms);
    }
    for(i = 0; i < 16; i++){
        lcd_clear();
        lcd_new_line;
        Go_right_xtimes(i);
        lcd_data(ch);
        cursor_off;
        delay_via_timer(X_ms);
    }
    lcd_clear();
}

//-------------------------------------------------------------
//               Set delay to d value
//-------------------------------------------------------------
void set_delay(unsigned int d){
    X_ms = d;
}

//-------------------------------------------------------------
//                          Servo scan
//-------------------------------------------------------------
void servo_scan(int l, int r){
    for(j = l; j <= r; j += 3){
        Get_motor_to(j);
        Get_Sample_Ultrasonic();
        send_message((int) Range);
    }
    if(state != state5) send_message(500);
}

//-------------------------------------------------------------
//        Move stepper motor to location by degree
//-------------------------------------------------------------
void Get_motor_to(unsigned int degree){
    // T = (0.0085*degree) + 0.6; // - for 40 Hz
    T = (0.0089*degree) + 0.48; // - for 50 Hz
    set_pwm(0x0A3D, T); // 0x0A3D - 50 Hz , 0x0CC - for 40Hz
    delay_via_timer(50);           // we are using TimerA to get sample from the Ultrasonic sensor so we set delay to let the servo enough time to get to his place
}

//-------------------------------------------------------------
//        Get sample from Ultrasonic sensor
//-------------------------------------------------------------
void Get_Sample_Ultrasonic(void){
    for(i = 0; i < 3; i++){
        Set_trigger();
        __bis_SR_register(LPM0_bits + GIE);
        if (FEdge > REdge) tempo = (FEdge - REdge);
        else tempo = (0xFFFF - REdge) + FEdge;
        tempo *= Distance;
        tmp_sample = tempo / 131072.0;
        if (( tmp_sample > Max_dist) || (tmp_sample == 0)){
            continue;
        }
        avg_Range += tmp_sample;    // divide by 2^17 - clock count rate
        avg_counter ++;
        delay_via_timer(6);
    }
    if (avg_counter == 0) Range = 0;
    else{
        Range = avg_Range / avg_counter;
        avg_Range = 0;
        avg_counter = 0;
    }
}

//-------------------------------------------------------------
//                  Get sample from LDRs
//-------------------------------------------------------------
void sample_LDR(void){
    ADC_on_A3();            // Start sampling
    __bis_SR_register(LPM0_bits + GIE);       // Enter LPM0 w/ interrupt
    ADC_SI(); // Don't get into interrupt
    voltage1 = N_ADC12_A3 * one_step;
    voltage2 = N_ADC12_A4 * one_step;
}

//-------------------------------------------------------------
//                          Telemeter
//-------------------------------------------------------------
void Telemeter(int deg){
    Get_motor_to(deg);
    Get_Sample_Ultrasonic();
    send_dist();
}

//-------------------------------------------------------------
//               Calibrate LDRs voltage level
//-------------------------------------------------------------
void Calibrate_LDR(void){
    Get_motor_to(90);
    for(j = 4; j<50; j += 5){
        lcd_clear();
        lcd_print("Push button to");
        lcd_new_line;
        lcd_print("Sample LDRs");
        __bis_SR_register(LPM0_bits + GIE);
        lcd_clear();
        lcd_print("Wait");
        sample_LDR();
        LDR1[j] = voltage1;
        LDR2[j] = voltage2;
        delay_via_timer(100);
        if(j != 4){
            for(q = 1; q < 5; q++){
                LDR1[j-5+q] = ((LDR1[j] - LDR1[j-5])/5)*(q) + LDR1[j-5];
                LDR2[j-5+q] = ((LDR2[j] - LDR2[j-5])/5)*(q) + LDR2[j-5];
            }
        }
        else{
            for(q = 0; q<4; q++){
                LDR1[q] = LDR1[j];
                LDR2[q] = LDR2[j];
            }
        }
        max_dif[(j - 4) / 5] = 1.2 * fabs(LDR1[j] - LDR2[j]);
    }
    lcd_clear();
    lcd_print("Calibration");
    lcd_new_line;
    lcd_print("Finished");
    delay_via_timer(300);
    lcd_clear();
}
//-------------------------------------------------------------
//              Send Telemeter distance
//-------------------------------------------------------------
void send_dist(void){
    send_degree(Tele_degree);
    send_message((int) Range);
    send_message(500);
}

//-------------------------------------------------------------
//             Light source detector scan
//-------------------------------------------------------------
void light_scan(void){
    for(j = 0; j <= 180; j += 3){
        Get_motor_to(j);
        sample_LDR();

        for(q = 0; q < 50; q++){
            if (voltage1 >= 3.29){
                LDR1_sample = 0;
                break;
            }
            if (voltage1 < LDR1[q]){
                LDR1_sample = q;       // q represent cm
                break;
            }
        }
        for(q = 0; q < 50; q++){
            if (voltage2 >= 3.29){
                LDR2_sample = 0;
                break;
            }
            if (voltage2 < LDR2[q]){
                LDR2_sample = q;       // q represent cm
                break;
            }
        }
        if(max_dif[LDR1_sample / 5] < max_dif[LDR2_sample / 5]) max = LDR1_sample;
        else max = LDR2_sample;
        if(fabs(voltage1 - voltage2) < max_dif[max / 5]) send_message((LDR1_sample + LDR2_sample)/2);
        else send_message(0);
    }
    send_message(500);
}


//-------------------------------------------------------------
//          Light source and Objects detector scan
//-------------------------------------------------------------
void scan_lights_objects(void){

    for(j = 0; j <= 180; j += 3){
        Get_motor_to(j);
        Get_Sample_Ultrasonic();
        send_message((int)Range);

        sample_LDR();
        for(q = 0; q < 50; q++){
            if (voltage1 >= 3.29){
                LDR1_sample = 0;
                break;
            }
            if (voltage1 < LDR1[q]){
                LDR1_sample = q;       // q represent cm
                break;
            }
        }
        for(q = 0; q < 50; q++){
            if (voltage2 >= 3.29){
                LDR2_sample = 0;
                break;
            }
            if (voltage2 < LDR2[q]){
                LDR2_sample = q;       // q represent cm
                break;
            }
        }
        if(max_dif[LDR1_sample / 5] < max_dif[LDR2_sample / 5]) max = LDR1_sample;
        else max = LDR2_sample;
        if(fabs(voltage1 - voltage2) < max_dif[max / 5]) send_message((LDR1_sample + LDR2_sample)/2);
        else send_message(0);
    }
    send_message(500);
}

// ----------------------------------------------------------------- FLASH ----------------------------------------------------------------------- //

//-------------------------------------------------------------
//          Write to flash or execute script
//-------------------------------------------------------------
void ScriptFunc() {

    if(FlashBurnIFG){
        FlashBurnIFG=0;
        FCTL2 = FWKEY + FSSEL0 + FN1;             // MCLK/3 for Flash Timing Generator

        file.file_size[file.num_of_files - 1] = strlen(file_content) - 1;
        write_Seg();
        send_message(0x08);
    }
    if(ExecuteFlag){
        ExecuteFlag=0;
        flag_script = 1;
        ExecuteScript();
    }
    __bis_SR_register(LPM0_bits + GIE);
}

//-------------------------------------------------------------
//                  Execute script operations
//-------------------------------------------------------------
void ExecuteScript(void)
{
    if (flag_script)
        Flash_ptrscript = file.file_ptr[file.num_of_files - 1];
    flag_script = 0;
  //  IE2 != ~UCA0RXIE; // Now added by mg
   // for (y = 0; y <= file.file_size[file.num_of_files - 1];)
    for (y = 0; y < 64;)
    {
        OPCstr = *Flash_ptrscript++;
        OPCstr = *Flash_ptrscript++;
        y = y + 2;
        switch (OPCstr)
        {
        case '1':       // Count up from zero to x with delay d onto LCD
            Operand1Flash[0] = *Flash_ptrscript++;
            Operand1Flash[1] = *Flash_ptrscript++;
            y = y + 2;
            Oper2ToInt = hex2int(Operand1Flash);
            inc_lcd(Oper2ToInt);
            delay_via_timer(10);
            break;

        case '2':       // Count down from x to zero with delay d onto LCD
            Operand1Flash[0] = *Flash_ptrscript++;
            Operand1Flash[1] = *Flash_ptrscript++;
            y = y + 2;
            Oper2ToInt = hex2int(Operand1Flash);
            dec_lcd(Oper2ToInt);
            delay_via_timer(10);
            break;

        case '3':       // Rotate right onto LCD from pixel index 0 to pixel index 31 a single char x (ASCII value) with delay d
            Operand1Flash[0] = *Flash_ptrscript++;
            Operand1Flash[1] = *Flash_ptrscript++;
            y = y + 2;
            character = hexToChar(Operand1Flash[0],Operand1Flash[1]);
            rra_lcd(character);
            delay_via_timer(10);
            break;

        case '4':       // Set the delay d value ( units of 10ms)
            Operand1Flash[0] = *Flash_ptrscript++;
            Operand1Flash[1] = *Flash_ptrscript++;
            y = y + 2;
            X_ms = hex2int(Operand1Flash);
            delay_via_timer(10);
            break;

        case '5':       // Clear LCD screen
            lcd_clear();
            delay_via_timer(10);
            break;

        case '6':       // Point the Ultrasonic sensor to degree p and show the degree and distance (dynamically) onto PC screen
            Operand1Flash[0] = *Flash_ptrscript++;
            Operand1Flash[1] = *Flash_ptrscript++;
            y = y + 2;
            Tele_degree = hex2int(Operand1Flash);
            Telemeter(Tele_degree);
            send_dist();
            delay_via_timer(100);
            break;

        case '7':       // Scan area between left l angle to right r angle (once) and show the degree and distance (dynamically) onto PC screen
            Operand1Flash[0] = *Flash_ptrscript++;
            Operand1Flash[1] = *Flash_ptrscript++;
            y = y + 2;
            Operand2Flash[0] = *Flash_ptrscript++;
            Operand2Flash[1] = *Flash_ptrscript++;
            y = y + 2;
            Oper2ToInt = hex2int(Operand1Flash);
            left_angle = Oper2ToInt;
            Oper2ToInt = hex2int(Operand2Flash);
            right_angle = Oper2ToInt;
            send_degree(left_angle);
            servo_scan(left_angle, right_angle);
            send_degree(right_angle);
            send_message(500);
            delay_via_timer(100);
            break;

        case '8':       // Set the MCU into sleep mode
            //enterLPM(lpm_mode);
            break;

        }
    }
}













