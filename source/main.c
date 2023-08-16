#include  "../header/api.h"    		// private library - API layer
#include  "../header/app.h"    		// private library - APP layer

enum FSMstate state;
enum SYSmode lpm_mode;
void main(void){
  
  state = state0;  // start in idle state on RESET
  lpm_mode = mode0;     // start in idle state on RESET
  sysConfig();
  lcd_init();
  lcd_clear();
  
  while(1){
	switch(state){
	  case state0:                  // Sleep state
	      enterLPM(lpm_mode);
		  break;
		 
	  case state1:                  // Object detector system
	      enterLPM(lpm_mode);
	      if (state == state1){
	          servo_scan(0, 180);
	      }
	   	  break;
		 
	  case state2:                  // Telemeter
	      enterLPM(lpm_mode);
	      if(state == state2){
	          Telemeter(Tele_degree);
	      }
		  break;
                
      case state3:                  // Light source detector system
          enterLPM(lpm_mode);
          if ( state == state3){
              light_scan();
          }
          break;

      case state4:                  // Light source and object detector system
          enterLPM(lpm_mode);
          if(state == state4){
              scan_lights_objects();
          }

          break;

      case state5:                  // Script mode
          if(state == state5){
              ScriptFunc();
          }

          break;

      case state6:                  // Script mode
          Calibrate_LDR();
          state = state0;
          break;
	}
  }
}
  
  
  
  
  
  
