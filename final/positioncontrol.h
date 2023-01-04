#ifndef POSITIONCONTROL__H__
#define POSITIONCONTROL__H__

#include <xc.h> // processor SFR definitions
#include <sys/attribs.h> // __ISR macro
#include "NU32.h"
#include "utilities.h"
#include "encoder.h"
#include "currentcontrol.h"

extern float TRACKArray[10000];
extern int array_length;

void PositionControl_Startup();
void set_position_gains(float, float, float);
float get_position_kp();
float get_position_ki();
float get_position_kd();
void set_angle(float);
void TRACK_print();


#endif // POSITIONCONTROL__H__