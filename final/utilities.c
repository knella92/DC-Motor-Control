#include "utilities.h"

static volatile int mode;

void set_operating_mode(int i){
    mode = i;
}

int get_operating_mode(){
    return mode;
}