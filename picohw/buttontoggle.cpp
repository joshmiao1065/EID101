#include "rcc_stdlib.h"
using namespace std;

int main () {
 
    stdio_init_all(); 
    sleep_ms(100);
    cyw43_arch_init();
    cyw43_arch_gpio_put(0, 1);
    rcc_init_pushbutton();


    while (true) {

        if(!gpio_get(RCC_PUSHBUTTON)) //if NOT gpio (if gpio is low)
            {
                //do stuff here when button pressed
                cout << "yo\n"; 
            }
        else {
            cout << "hey\n";
        }

    }
}