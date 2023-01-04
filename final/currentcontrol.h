#ifndef CURRENTCONTROL__H__
#define CURRENTCONTROL__H__

#include <xc.h> // processor SFR definitions
#include <sys/attribs.h> // __ISR macro
#include "utilities.h"      // to use get mode commands
#include "NU32.h"
#include <stdio.h>
#include "ina219.h"

void CurrentControl_Startup();
void set_PWM_dutycycle();
void set_current_gains(float, float);
float get_current_kp();
float get_current_ki();
float set_des_current(float);
void ITEST_print();

volatile signed int pwm_user;


#endif // CURRENTCONTROL__H__