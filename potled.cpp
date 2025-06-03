#include "rcc_stdlib.h"
using namespace std;

int main()
{
    stdio_init_all();
    sleep_ms(1500);
    cyw43_arch_init();

    rcc_init_potentiometer(); //setup ADC
    while(true) {
    int32_t pot_val = adc_read();
    
        if(pot_val >= 2253)
        {
            cyw43_arch_gpio_put(0, 1);
        }
    else
        {
            cyw43_arch_gpio_put(0, 0);
        }
        }
}     
    