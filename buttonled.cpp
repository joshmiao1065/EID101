#include "rcc_stdlib.h"
using namespace std;

int main()
{
    stdio_init_all();
    sleep_ms(1500);
    cyw43_arch_init();

    rcc_init_pushbutton(); //setup ADC

    while(true)
    {
        if(!gpio_get(RCC_PUSHBUTTON))
          {
             cyw43_arch_gpio_put(0,1);
          }
        else{
            cyw43_arch_gpio_put(0,0);
        }
    }
}