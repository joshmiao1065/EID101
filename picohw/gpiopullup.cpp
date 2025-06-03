#include <stdio.h>
#include "rcc_stdlib.h"
#include <iostream>

#define GPIO_IN 1

using namespace std;

int main (void) {
	stdio_init_all();

if(cyw43_arch_init())
       	{
               	cout << "CYW43 init failed !" << "\n";
        }
	cyw43_arch_gpio_put(0, 1);

gpio_init(GPIO_IN);
gpio_pull_up(GPIO_IN);
gpio_set_dir(GPIO_IN, false);

while(true) {

	if(gpio_get(GPIO_IN)) {
		cout << "gpio on\n";
	}
else {
	cout << "gpio off\n";
}
}
}
