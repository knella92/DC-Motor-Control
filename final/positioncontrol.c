#include "positioncontrol.h"

static volatile float kpp = 10;
static volatile float kip = 0;
static volatile float kdp = 500;
static volatile float pos_des;
static volatile float eint = 0;
static volatile float eprev = 0;
static volatile float REALArray[10000];


float TRACKArray[10000];
int array_length;

void __ISR(_TIMER_4_VECTOR, IPL5SOFT) PController(void){
    static int mode = 0, counter = 0;
    static float e = 0, refp = 0;
    static int cp, curr_p;
    static float cpdeg = 0, curr_pdeg = 0; 
    static float edot; static float des_current = 0;

    mode = get_operating_mode();
    switch (mode){
        case HOLD:
        {
            WriteUART2("a"); // asks Pico for position
            while(!get_encoder_flag()){} // wait for new position while flag is 0
            set_encoder_flag(0);
            
            cp = get_encoder_count();
            cpdeg = cp/(4.0*334.0) * 360.0;
            float eint_max = 49999/kip;
            
            // PID controller

            e = pos_des - cpdeg;
            eint = eint + e;
            if (eint > eint_max){
                eint = eint_max;
            } else if (eint < -1*eint_max){
                eint = -1*eint_max;
            }

            edot = e - eprev;

            des_current = kpp*e + kip*eint + kdp*edot;
            set_des_current(des_current);

            eprev = e;
            break;
        }
        case TRACK:
        {
            refp = TRACKArray[counter];
           

            WriteUART2("a"); // asks Pico for position
            while(!get_encoder_flag()){} // wait for new position while flag is 0
            set_encoder_flag(0);
            
            curr_p = get_encoder_count();
            curr_pdeg = curr_p/(4.0*334.0) * 360.0;
            REALArray[counter] = curr_pdeg;
            // float eint_max = 49999/kip;
            
            // PID controller

            e = refp - curr_pdeg;

            eint = eint + e;
            // if (eint > eint_max){
            //     eint = eint_max;
            // } else if (eint < -1*eint_max){
            //     eint = -1*eint_max;
            // }

            edot = e - eprev;

            des_current = kpp*e + kip*eint + kdp*edot;
            set_des_current(des_current);

            eprev = e;
            if (counter < (array_length-1)){
                counter++;
            }
            else if (counter == array_length-1){
                pos_des = TRACKArray[counter];
                set_operating_mode(HOLD);
                counter = 0;
            }
            break;
        }
    }
    IFS0bits.T4IF = 0;
}



void PositionControl_Startup(){
    __builtin_disable_interrupts();

    //Timer4 for 200Hz ISR
    T4CONbits.TCKPS = 3;            // Prescalar of 8
    PR4 = 49999;
    TMR4 = 0;
    T4CONbits.TGATE = 0;            //             not gated input (the default)
    IPC4bits.T4IP = 5;              // INT step 4: priority
    IPC4bits.T4IS = 0;              //             subpriority
    IFS0bits.T4IF = 0;              // INT step 5: clear interrupt flag
    IEC0bits.T4IE = 1;              // INT step 6: enable interrupt


    T4CONbits.ON = 1;               // Turn on Timer4 (200 Hz ISR)
    __builtin_enable_interrupts();

}


void set_position_gains(float kp_p, float ki_p, float kd_p){
    kpp = kp_p;
    kip = ki_p;
    kdp = kd_p;
}

float get_position_kp(){
    return kpp;
}

float get_position_ki(){
    return kip;
}

float get_position_kd(){
    return kdp;
}

void set_angle(float pdes){
    pos_des = pdes; // IN DEGREES
}

void TRACK_print(){
    char m[50];
    char track_length[20];
    sprintf(track_length,"%d\r\n", array_length);
    NU32_WriteUART3(track_length);
    int i;
    for(i = 0; i<array_length; i++){
        sprintf(m,"%f %f\r\n", TRACKArray[i], REALArray[i]);
        NU32_WriteUART3(m);
    }
}