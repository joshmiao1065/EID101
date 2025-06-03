#include "rcc_stdlib.h"
using namespace std;

int main()
{
    stdio_init_all();
    sleep_ms(1500);
    cyw43_arch_init();
    cyw43_arch_gpio_put(0,1); //led on

    rcc_init_potentiometer(); //setup ADC
    rcc_init_pushbutton();
    while(true){
        int32_t pot_val = adc_read();
        if(pot_val <= 2500 && pot_val >= 2000 && !gpio_get(RCC_PUSHBUTTON))
            {
                cout << "pumpkin\n";
            }
        else {
            cout << "no pumpkin for you :(\n";
        }
    }
}