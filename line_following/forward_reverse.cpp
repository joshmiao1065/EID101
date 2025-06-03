#include "rcc_stdlib.h"
using namespace std;
//TEST WHAT DURATION IS NEEDED FOR THE DWELL CHECK/LOGIC STATES
//CHANGE POSITION OF ML AND MR SENSORS
//EDIT LOG
//WORK ON REVERSES AND 180s
typedef enum {
    DWELL_STRAIGHT,
    STRAIGHT,
    DWELL_REVERSE,
    REVERSE,
    END
}linefollowing_t;

int main() {
    stdio_init_all();
    sleep_ms(4000); //sleep to give us time to place the robot
    cyw43_arch_init();
    cyw43_arch_gpio_put(0, 1);

    Motor motors;
    MotorInit(&motors, RCC_ENA, RCC_ENB, 1000);
    MotorsOn(&motors);
    cyw43_arch_deinit();

    int counter = 0;

    uint32_t cur, prev; // curent and previous time
    uint32_t move_time = 1000000; //move for 1 second
    uint32_t pause = 200000;//200ms rest period between states
 
     linefollowing_t movestate = STRAIGHT;

    cur = time_us_32();
    prev = cur;

     while(true){
        cur = time_us_32();

        switch(movestate) {

            case STRAIGHT:  
                MotorPower(&motors, 50, 51);  
                cout << "STRAIGHT\n";

                if(cur - prev >= move_time) {
                    movestate = DWELL_REVERSE;
                    prev = cur;
                }

                if(counter >= 3) {
                    movestate = END;
                }
            break;

            case REVERSE:
                MotorPower(&motors, -50, -51);
                cout << "reverse\n";

                if(cur - prev >= move_time) {
                    movestate = DWELL_STRAIGHT;
                    prev = cur;
                }
                
                if(counter >= 3) {
                    movestate = END;
                }
            break;

            case DWELL_STRAIGHT:
                MotorPower(&motors, 0, 0);
                cout <<"DWELL_STRAIGHT\n";

                if(cur - prev >= pause) {
                    movestate = STRAIGHT;
                    counter++;
                    prev = cur;
                } 

                if(counter >= 3) {
                    movestate = END;
                }
            break;

            case DWELL_REVERSE:
                MotorPower(&motors, 0, 0);
                cout << "DWELL_REVERSE\n";

                if(cur - prev >= pause) {
                    movestate = REVERSE;
                    counter++;
                    prev = cur;
                }

                if(counter >= 3) {
                    movestate = END;
                }
            break;

            case END:
                MotorPower(&motors, 0, 0);
        }
    }
return 0;
}


            

            
                

