#ifndef ENCODER__H__
#define ENCODER__H__

#include <xc.h> // processor SFR definitions
#include <sys/attribs.h> // __ISR macro

#include "NU32.h"

void UART2_Startup(); 
void WriteUART2(const char * string); //to talk to Pico (initialize pins, same thing as UART3_Startup). turn on ISR on PIC - interrupts on each number (i.e. angle = 100 \n)
int get_encoder_flag();
void set_encoder_flag();
int get_encoder_count();


#endif // ENCODER__H__
