#include <stdio.h>
#include "rcc_stdlib.h"
using namespace std;

int main(void) {
	stdio_init_all();
	if(cyw43_arch_init())
	{
		cout << "CYW43 init failed !" << "\n";
	}
while(true)
{
	// S= . . .
	cyw43_arch_gpio_put(0,1);
	sleep_ms(100);
	cyw43_arch_gpio_put(0,0);
	sleep_ms(100);

	cyw43_arch_gpio_put(0,1);
        sleep_ms(100);
        cyw43_arch_gpio_put(0,0);
        sleep_ms(100);

	cyw43_arch_gpio_put(0,1);
        sleep_ms(100);
        cyw43_arch_gpio_put(0,0);
        sleep_ms(300);  //rest here is longer to separate letters
	//O= - - -
	cyw43_arch_gpio_put(0,1);
        sleep_ms(300);
        cyw43_arch_gpio_put(0,0);
        sleep_ms(100);

	cyw43_arch_gpio_put(0,1);
        sleep_ms(300);
        cyw43_arch_gpio_put(0,0);
        sleep_ms(100);

	cyw43_arch_gpio_put(0,1);
        sleep_ms(300);
        cyw43_arch_gpio_put(0,0);
        sleep_ms(300);	//rest here is longer to separate letters
	//S = . . .
	cyw43_arch_gpio_put(0,1);
        sleep_ms(100);
        cyw43_arch_gpio_put(0,0);
        sleep_ms(100);

        cyw43_arch_gpio_put(0,1);
        sleep_ms(100);
        cyw43_arch_gpio_put(0,0);
        sleep_ms(100);

        cyw43_arch_gpio_put(0,1);
        sleep_ms(100);
        cyw43_arch_gpio_put(0,0);
	//rest between cycles of SOS
        sleep_ms(500);
}
}
