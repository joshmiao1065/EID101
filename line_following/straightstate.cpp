#include "rcc_stdlib.h"
using namespace std;
 
typedef enum {
    DEFAULT,
    LEFT_ADJUST,
    RIGHT_ADJUST,
    END
} straightfollowing_t;
 
int main() {
    stdio_init_all();
    sleep_ms(5000); //sleep to give us time to place the robot
    cyw43_arch_init();
    cyw43_arch_gpio_put(0, 1);
 
    gpio_init(0);                        
    gpio_set_dir(0,false);                
 
    gpio_init(1);                        
    gpio_set_dir(1,false);                
 
    gpio_init(2);                          
    gpio_set_dir(2,false);
 
    gpio_init(3);                          
    gpio_set_dir(3,false);
 
    gpio_init(4);                          
    gpio_set_dir(4,false);
 
    gpio_init(5);                        
    gpio_set_dir(5,false);                    
 
    Motor motors;
    MotorInit(&motors, RCC_ENA, RCC_ENB, 1000);
    MotorsOn(&motors);
    cyw43_arch_deinit();
 
    straightfollowing_t straightstate = DEFAULT;
 
    while(true) {
 
       
        switch(straightstate) {
        case DEFAULT:
            MotorPower(&motors, 60, 65);
            cout << "default \n";
            if(gpio_get(1) && !gpio_get(3)) {
                straightstate = LEFT_ADJUST;
            }
            if(!gpio_get(1) && gpio_get(3)) {
                straightstate = RIGHT_ADJUST;
            }           
            if(gpio_get(4)) {
                straightstate = END;
            }
        break;
 
        case LEFT_ADJUST: //if it drifts to the left and MR detects the line
            MotorPower(&motors, 55, 49); //increase leftside by 20
            cout << "left-adjust \n";
            if(gpio_get(1) && gpio_get(3)) {
                straightstate = DEFAULT;
            }
            if(!gpio_get(1) && gpio_get(3)) {
                straightstate = RIGHT_ADJUST;
            }
           if(gpio_get(4)) {
                straightstate = END;
            }
        break;
 
        case RIGHT_ADJUST: //if it drifts to the right and ML detects the line
            MotorPower(&motors, 48, 63); //increase rightside by 20
            cout << "right-adjust \n";
            if (gpio_get(1) && gpio_get(3)){
                straightstate = DEFAULT;
            }
            if(gpio_get(1) && !gpio_get(3)) {
                straightstate = LEFT_ADJUST;
            }  
            if(gpio_get(4)) {
                straightstate = END;
            }
        break;
 
        case END:
            cout << "end \n";
            MotorPower(&motors, 0, 0);
        break;
        }
    }
}