#include "ssr40.h"
#include <M5EPD.h>

uint8_t activationPin;

SSR40::SSR40(uint8_t activationPin) {
    this->activationPin = activationPin;
}

void SSR40::switchOn(){
    //digitalWrite(activationPin, 0);
}

void SSR40::switchOff(){
    //digitalWrite(activationPin, 1);
}