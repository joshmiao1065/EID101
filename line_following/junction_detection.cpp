#include "rcc_stdlib.h"
using namespace std;
//TEST WHAT DURATION IS NEEDED FOR THE DWELL CHECK/LOGIC STATES
//CHANGE POSITION OF ML AND MR SENSORS
//EDIT LOG
//WORK ON REVERSES AND 180s
typedef enum {
    STRAIGHT,
    JUNCTION,
    POST_JUNCTION
}linefollowing_t;

int main() {
    stdio_init_all();
    sleep_ms(4000); //sleep to give us time to place the robot
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
 
    gpio_init(5); //bucket detector                        
    gpio_set_dir(5,false);                    
 
    Motor motors;
    MotorInit(&motors, RCC_ENA, RCC_ENB, 1000);
    MotorsOn(&motors);
    cyw43_arch_deinit();

    linefollowing_t movestate = STRAIGHT;

    while(true){

        switch(movestate) {
            
            case STRAIGHT:
                MotorPower(&motors, 60, 61);

                if(gpio_get(0) && gpio_get(2) && gpio_get(4)) {
                movestate = JUNCTION;
            }
            break;

            case JUNCTION:
                MotorPower(&motors, 0, 0);
                sleep_ms(1500);
                movestate = POST_JUNCTION;
            break;

            case POST_JUNCTION:
                MotorPower(&motors, -75, 75);
            break;
        }
    }
return 0;
}
