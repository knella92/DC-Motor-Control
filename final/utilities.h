#ifndef UTILITIES__H__
#define UTILITIES__H__

#include <xc.h> // processor SFR definitions
#include <sys/attribs.h> // __ISR macro
#include "NU32.h"

#define BUF_SIZE 200
#define IDLE 0
#define PWM 1
#define ITEST 2
#define HOLD 3
#define TRACK 4

void set_operating_mode();
int get_operating_mode();


#endif // UTILITIES__H__