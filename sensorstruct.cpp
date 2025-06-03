#include "rcc_stdlib.h"
using namespace std;

struct sensorstruct
{
    int32_t pot_val;
    bool buttonstate;
};

struct sensorstruct rcc;

void change_led(struct sensorstruct *pico) {
    // fill in
    if ((pico->buttonstate) && (pico->pot_val > 2253)) {
        cyw43_arch_gpio_put(0, 1);
    } else {
        cyw43_arch_gpio_put(0, 0);
        return;
    }
           
}

void update_data_sensor(struct sensorstruct *pico) {
   // fill this in 
   pico->buttonstate = !gpio_get(RCC_PUSHBUTTON);
   pico->pot_val = adc_read();
   change_led(pico); //false;//some value
   // call change_led(&s);
}

int main()
{
    
    stdio_init_all();
    sleep_ms(1500);
    cyw43_arch_init();
    cyw43_arch_gpio_put(0,1); //led on

    rcc_init_potentiometer(); //setup ADC
    rcc_init_pushbutton();
    while(true)
    {
        update_data_sensor(&rcc);
    }
}