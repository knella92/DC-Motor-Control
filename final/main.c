#include "NU32.h"          // config bits, constants, funcs for startup and UART
#include "encoder.h"
#include "utilities.h"
#include "ina219.h"
#include "currentcontrol.h"
#include "positioncontrol.h"
#include <stdio.h>


int main() 
{
  char buffer[BUF_SIZE];

  NU32_Startup(); // cache on, min flash wait, interrupts on, LED/button init, UART init
  INA219_Startup();
  UART2_Startup();
  CurrentControl_Startup();
  PositionControl_Startup();

  NU32_LED1 = 1;  // turn off the LEDs
  NU32_LED2 = 1;        
  __builtin_disable_interrupts();

  __builtin_enable_interrupts();

  while(1)
  {
    NU32_ReadUART3(buffer,BUF_SIZE); // we expect the next character to be a menu command
    NU32_LED2 = 1;                   // clear the error LED
    switch (buffer[0]) {
      case 'b':                      // read current in mA
      {
        float current = INA219_read_current();
        sprintf(buffer, "%f\r\n", current);
        NU32_WriteUART3(buffer);
        break;
      }
      case 'd':                      // read encoder in degrees
      {
        WriteUART2("a"); // asks Pico for position
        while(!get_encoder_flag()){} // wait for new position while flag is 0
        set_encoder_flag(0);
        int pos = get_encoder_count();
        float pos_d = pos/(4.0*334.0) * 360.0;
        sprintf(buffer, "%f\r\n", pos_d);
        NU32_WriteUART3(buffer);
        break;
      }
      case 'c':             // ask for encoder position in counts                
      {
        WriteUART2("a"); // asks Pico for position
        while(!get_encoder_flag()){} // wait for new position while flag is 0
        set_encoder_flag(0);
        int pos = get_encoder_count();
        sprintf(buffer, "%d\r\n", pos);
        NU32_WriteUART3(buffer);
        break;
      }
      case 'e':             // set encoder to position zero              
      {
        WriteUART2("b"); // set encoder to position zero
        break;
      }
      case 'f':             // set PWM             
      {
        char msg[10];
        signed int pwm_user = 0;
        NU32_ReadUART3(msg, sizeof(msg));
        sscanf(msg,"%d", &pwm_user);
        set_PWM_dutycycle(pwm_user);
        set_operating_mode(PWM);
        break;
      }
      case 'g':             // set current gains         
      {
        char gns[100];
        float kp_u, ki_u;
        NU32_ReadUART3(gns, sizeof(gns));
        sscanf(gns, "%f %f", &kp_u, &ki_u);
        set_current_gains(kp_u, ki_u);
        break;
      }
      case 'h':             // get current gains         
      {
        float kp_u, ki_u;
        char kp_gain[100];
        char ki_gain[100];
        kp_u = get_current_kp();
        ki_u = get_current_ki();
        sprintf(kp_gain, "%f\r\n", kp_u);
        sprintf(ki_gain, "%f\r\n", ki_u);
        NU32_WriteUART3(kp_gain);
        NU32_WriteUART3(ki_gain);
        break;
      }
      case 'i':             // set position gains         
      {
        char gns[100];
        float kp_p, ki_p, kd_p;
        NU32_ReadUART3(gns, sizeof(gns));
        sscanf(gns, "%f %f %f", &kp_p, &ki_p, &kd_p);
        set_position_gains(kp_p, ki_p, kd_p);
        break;
      }
      case 'j':             // get position gains         
      {
        float kp_p, ki_p, kd_p;
        char kpp_gain[100];
        char kip_gain[100];
        char kdp_gain[100];
        kp_p = get_position_kp();
        ki_p = get_position_ki();
        kd_p = get_position_kd();
        sprintf(kpp_gain, "%f\r\n", kp_p);
        sprintf(kip_gain, "%f\r\n", ki_p);
        sprintf(kdp_gain, "%f\r\n", kd_p);
        NU32_WriteUART3(kpp_gain);
        NU32_WriteUART3(kip_gain);
        NU32_WriteUART3(kdp_gain);
        break;
      }
      case 'k':             // Test current gains         
      {
        set_operating_mode(ITEST);
        while(get_operating_mode() == ITEST){}
        ITEST_print();
        break;
      }
      case 'l':             // Go to angle (deg)        
      {
        char deg[50];
        float pdeg;
        NU32_ReadUART3(deg, sizeof(deg));
        sscanf(deg, "%f", &pdeg);
        set_angle(pdeg);
        set_operating_mode(HOLD);
        break;
      }
      case 'm':
      {
        char ref_len[20]; int j;
        char the_big_kahuna[1000]; float temp;
        NU32_ReadUART3(ref_len, sizeof(ref_len));
        sscanf(ref_len, "%d", &j);

        for (int i = 0; i<j; i++){
          NU32_ReadUART3(the_big_kahuna, sizeof(the_big_kahuna));
          sscanf(the_big_kahuna, "%f", &temp);
          TRACKArray[i] = temp;
        }
        array_length = j;
        break;
      }
      case 'n':
      {
        char ref_len[20]; int p;
        char the_big_kalua[1000]; float temp;
        NU32_ReadUART3(ref_len, sizeof(ref_len));
        sscanf(ref_len, "%d", &p);
        for (int i = 0; i<p; i++){
          NU32_ReadUART3(the_big_kalua, sizeof(the_big_kalua));
          sscanf(the_big_kalua, "%f", &temp);
          TRACKArray[i] = temp;
        }
        array_length = p;
        break;
      }
      case 'o': // execute trajectory
      {
        set_operating_mode(TRACK);
        while(get_operating_mode() == TRACK){}
        TRACK_print();
        break;
      }
      case 'p':             // unpower motor            
      {
        set_operating_mode(IDLE);
        break;
      }
      case 'r':                      
      {
        int mode = get_operating_mode();
        if(mode == IDLE){
          sprintf(buffer, "IDLE\r\n");}
        else if(mode == PWM){
          sprintf(buffer, "PWM\r\n");}
        else if(mode == ITEST){
          sprintf(buffer, "ITEST\r\n");}
        else if(mode == HOLD){
          sprintf(buffer, "HOLD\r\n");}
        else if(mode == TRACK){
          sprintf(buffer, "TRACK\r\n");}
        NU32_WriteUART3(buffer);
        break;
      }
      case 'q':
      {
        set_operating_mode(IDLE);
      }
      default:
      {
        NU32_LED2 = 0;  // turn on LED2 to indicate an error
        break;
      }
    }
  }
  return 0;
}


