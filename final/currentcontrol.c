#include "currentcontrol.h"

static volatile float kp = 0;
static volatile float ki = 0;
static volatile int eint = 0;
static volatile float REFArray[100];
static volatile float CValArray[100];
static volatile float dcur;

void __ISR(_TIMER_2_VECTOR, IPL4SOFT) CController(void){
    // static int counter = 0;
    // static int plotind = 0;
    // static int decctr = 0;
    static float e = 0.0, eint_max = 0.0;
    // unsigned int elapsed = 0;
    static float u = 0.0; //unew = 0.0;
    static int mode = 0;
    static int itest_ctr = 0;
    static float ref = 0.0, cval = 0.0;

    mode = get_operating_mode();
    switch (mode){
        case IDLE:
        {
            OC1RS = 0;
            LATDbits.LATD1 = 0;
            break;
        }
        case PWM:
        {
            if (pwm_user<0){
                LATDbits.LATD1 = 0;
                float pwm_p = abs(pwm_user)/100.0;
                OC1RS = (unsigned int) (pwm_p*4000.0);
            }
            else if (pwm_user>0){
                LATDbits.LATD1 = 1;
                float pwm_p = abs(pwm_user)/100.0;
                OC1RS = (unsigned int) (pwm_p*4000.0);
            }
            break;
        }
        case ITEST:
        {
            if (itest_ctr<25){
                ref = 200.0;
            }
            else if (itest_ctr>=25 && itest_ctr<50){
                ref = -200.0;
            }
            else if (itest_ctr>=50 && itest_ctr<75){
                ref = 200.0;
            }
            else if (itest_ctr>=75 && itest_ctr<99){
                ref = -200.0;
            }
            else if (itest_ctr == 99){
                set_operating_mode(IDLE);
            }
            
            REFArray[itest_ctr] = ref;
            eint_max = 3999/ki;
            
            // PI controller
            cval = INA219_read_current();
            CValArray[itest_ctr] = cval;
            e = ref - cval;
            eint = eint + e;
            if (eint > eint_max){
                eint = eint_max;
            } else if (eint < -1*eint_max){
                eint = -1*eint_max;
            }
            u = kp*e + ki*eint;
            //unew = u + 50.0;
            if (u > 100.0){
                u = 100.0;
            } else if (u < -100.0){
                u = -100.0;
            }

            OC1RS = (unsigned int) ((abs(u)/100.0)*3999);
            if (u<0){
                LATDbits.LATD1 = 0;
            }
            else if (u>0){
                LATDbits.LATD1 = 1;
            }

            itest_ctr++;
            if (itest_ctr == 100){
                itest_ctr = 0;
                eint = 0;
            }
            break;
        }
        case HOLD:
        {
            eint_max = 3999/ki;
            kp = 0.1; ki=0.04;
            cval = INA219_read_current();
            // PI controller

            e = dcur - cval;
            eint = eint + e;
            if (eint > eint_max){
                eint = eint_max;
            } else if (eint < -1*eint_max){
                eint = -1*eint_max;
            }
            u = kp*e + ki*eint;

            if (u > 100.0){
                u = 100.0;
            } else if (u < -100.0){
                u = -100.0;
            }

            OC1RS = (unsigned int) ((abs(u)/100.0)*3999);
            if (u<0){
                LATDbits.LATD1 = 0;
            }
            else if (u>0){
                LATDbits.LATD1 = 1;
            }
            break;
        }
        case TRACK:
        {
            eint_max = 3999/ki;
            kp = 0.1; ki=0.04;
            cval = INA219_read_current();
            // PI controller

            e = dcur - cval;
            eint = eint + e;
            if (eint > eint_max){
                eint = eint_max;
            } else if (eint < -1*eint_max){
                eint = -1*eint_max;
            }
            u = kp*e + ki*eint;

            if (u > 100.0){
                u = 100.0;
            } else if (u < -100.0){
                u = -100.0;
            }

            OC1RS = (unsigned int) ((abs(u)/100.0)*3999);
            if (u<0){
                LATDbits.LATD1 = 0;
            }
            else if (u>0){
                LATDbits.LATD1 = 1;
            }
            break;
        }
    }


    IFS0bits.T2IF = 0;
}

void CurrentControl_Startup(){
    __builtin_disable_interrupts();

    //Timer2 for 5kHz ISR
    T2CONbits.TCKPS = 0;            // Prescalar of 1
    PR2 = 15999;
    TMR2 = 0;
    T2CONbits.TGATE = 0;            //             not gated input (the default)
    IPC2bits.T2IP = 4;              // INT step 4: priority
    IPC2bits.T2IS = 0;              //             subpriority
    IFS0bits.T2IF = 0;              // INT step 5: clear interrupt flag
    IEC0bits.T2IE = 1;              // INT step 6: enable interrupt

    //Output Compare for 20 kHz PWM signal
    T3CONbits.TCKPS = 0;     // Timer3 prescaler N=1
    PR3 = 3999;              // period = (PR3+1) * N * 12.5 ns = 100 us, 10 kHz
    TMR3 = 0;                // initial TMR3 count is 0
    OC1CONbits.OCTSEL = 1;   // Timer3
    OC1CONbits.OCM = 0b110;  // PWM mode without fault pin; other OC1CON bits are defaults
    OC1R = 1000;              // initialize before turning OC1 on; afterward it is read-only

    //Digital Output for Direction
    TRISDbits.TRISD1 = 0;           // Set RD0 to digital output


    T2CONbits.ON = 1;               // Turn on Timer2 (5 kHz ISR)
    T3CONbits.ON = 1;               // turn on Timer3
    OC1CONbits.ON = 1;              // turn on OC1
    __builtin_enable_interrupts();

}

void set_PWM_dutycycle(int i){
    pwm_user = i;
}

void set_current_gains(float kp_u, float ki_u){
    kp = kp_u;
    ki = ki_u;
}

float get_current_kp(){
    return kp;
}

float get_current_ki(){
    return ki;
}

float set_des_current(float des_cur){
    dcur = des_cur;
}

void ITEST_print(){
    char m[50];
    NU32_WriteUART3("100\r\n");
    int i;
    for(i = 0; i<100; i++){
        sprintf(m,"%f %f\r\n", REFArray[i], CValArray[i]);
        NU32_WriteUART3(m);
    }
}