#include <M5EPD.h>
class SSR40 {
    public:
    uint8_t activationPin;
    void switchOn();
    void switchOff();
    SSR40(uint8_t activationPin);
};