#include "rcc_stdlib.h"
using namespace std;

typedef enum {
    DWELL_STRAIGHT,
    STRAIGHT,
    DWELL_LEFT90,
    LEFT90,
    DWELL_TURN180,
    TURN180,
    END
}linefollowing_t;

typedef enum {
    DEFAULT,
    LEFT_ADJUST,
    RIGHT_ADJUST,
    DWELL_SSM
} straightfollowing_t;
 
typedef enum {
    DWELL,
    INTEGRATE
} integrate_t;

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
 
    rcc_init_i2c();
    MPU6050 imu;
    imu.begin(i2c1);
    imu.calibrate();
 
    int ssm = 1; //straight state machine not engaged
 
    double theta = 0.0;
    double ninety = 52.5;//this is a 90 degree turn, idk why the imu is off by so much
    double oneeighty = 100.0;//180 degree turn MotorPowers(&motors, -55 70)
 
    uint32_t cur, prev; // curent and previous time
    uint32_t cur_integrate, prev_integrate;//current and previous times for the integration state machine
    uint32_t dx = 10000; //10ms riemann sum interval
    uint32_t pause = 200000;//200ms rest period between states
 
    linefollowing_t movestate = STRAIGHT;
    //straightfollowing_t straightstate = DEFAULT;
    integrate_t integrationstate = DWELL;
 
    while(true){
        cur = time_us_32();
        cur_integrate = time_us_32();
        imu.update_pico();
        cout <<"theta is" <<theta <<"\n";

        switch(movestate) {
        case STRAIGHT:  
            MotorPower(&motors, 50, 50);  
            cout << "STRAIGHT\n";
            
            ssm = 1; 
 
            if ((gpio_get(0) && !gpio_get(2) && !gpio_get(4)) || (gpio_get(0) && gpio_get(2) && !gpio_get(4)) || (gpio_get(0) && !gpio_get(2) && gpio_get(4))){
                cur = time_us_32();
                prev = cur;
                ssm = 0;                
                movestate = DWELL_LEFT90;
            }

            if((!gpio_get(0) && !gpio_get(2) && !gpio_get(4))) {
                cur = time_us_32();
                prev = cur;
                movestate = DWELL_TURN180;
            }

            if((gpio_get(0) && gpio_get(2) && gpio_get(4))) {
                movestate = END;
            }
        break;

        case LEFT90:
        cout << "LEFT90\n";
            MotorPower(&motors, 0 ,75);
            if(theta >= ninety){
                cur = time_us_32();
                prev = cur;              
                ssm = 0;  
                movestate = DWELL_STRAIGHT;
            }
        break;
 
        case TURN180:
        cout << "TURN180\n";
            MotorPower(&motors, -55, 70);
            if(theta >= oneeighty){
                cur = time_us_32();
                prev = cur;             
                ssm = 0;   
                movestate = DWELL_STRAIGHT;
                }
            break;

         case DWELL_STRAIGHT:
        cout << "DWELL_STRAIGHT\n";
        cout << "this is cur "<< cur  <<"\n";
        cur = time_us_32();
            MotorPower(&motors, 0,0);
            if (cur - prev >= pause) {
                ssm = 0;
                movestate = STRAIGHT;
                prev = cur;
            }
        break;
 
        case DWELL_LEFT90:
        cout << "DWELL_LEFT90\n";
        cout << "this is cur "<< cur  <<"\n";
        cur = time_us_32();
 
            MotorPower(&motors, 0,0);
            if (cur - prev >= pause) {
                theta = 0.0;
                movestate = LEFT90;
                prev = cur;
            }
        break;
 
        case DWELL_TURN180:
        cout << "DWELL_TURN180\n";
        cout << "this is cur "<< cur  <<"\n";
        cur = time_us_32();
            MotorPower(&motors, 0,0);
            if (cur - prev >= pause) {
                theta = 0.0;
                movestate = TURN180;
                prev = cur;
            }
        break;

        case END:
            MotorPower(&motors, 0, 0);
    }
 
//----------------straight correcting state machine-------------------------------------------------
 
    /* switch(straightstate) { 

        case DEFAULT:
            MotorPower(&motors, 50, 50);
            cout << "default \n";
            
            if(!gpio_get(1) && gpio_get(3) && ssm == 1) {
                straightstate = LEFT_ADJUST;
            }

            if(gpio_get(1) && !gpio_get(3) && ssm == 1) {
                straightstate = RIGHT_ADJUST;
            }
            if(ssm == 0) {
                straightstate = DWELL_SSM;
            }           
        break;
 
        case LEFT_ADJUST: //if it drifts to the left and MR detects the line
            MotorPower(&motors, 53, 48); //increase leftside by 20
            cout << "left-adjust \n";
            if(!gpio_get(1) && !gpio_get(3) && ssm == 1) {
                straightstate = DEFAULT;
            }
            if(gpio_get(1) && !gpio_get(3) && ssm == 1) {
                straightstate = RIGHT_ADJUST;
            }
            if(ssm == 0) {
                straightstate = DWELL_SSM;
            } 
        break;
 
        case RIGHT_ADJUST: //if it drifts to the right and ML detects the line
            MotorPower(&motors, 48, 53); //increase rightside by 20
            cout << "right-adjust \n";
            if (!gpio_get(1) && !gpio_get(3) && ssm == 1){
                straightstate = DEFAULT;
            }
            if(!gpio_get(1) && gpio_get(3) && ssm == 1) {
                straightstate = LEFT_ADJUST;
            }
            if(ssm == 0) {
                straightstate = DWELL_SSM;
            } 
        break;

        case DWELL_SSM:
            cout << "Dwell_SSM\n";
            if(!gpio_get(1) && !gpio_get(3) && ssm == 1) {
                straightstate = DEFAULT;
            }
            if(!gpio_get(1) && gpio_get(3) && ssm == 1) {
                straightstate = LEFT_ADJUST;
            }

            if(gpio_get(1) && !gpio_get(3) && ssm == 1) {
                straightstate = RIGHT_ADJUST;
            }           
        break;


    }   
    */ 
 
//--------------integration state machine------------------------------------------------
 
    switch(integrationstate) {
 
    case DWELL:
            if(cur_integrate - prev_integrate >= dx) {
                integrationstate = INTEGRATE;
            }
    break;

    case INTEGRATE:
        theta = theta + imu.getAngVelZ()* dx / 1000000.0;
        cur_integrate = time_us_32(); 
        prev_integrate = cur_integrate;
        integrationstate = DWELL;                
    break;
    }
 
    }

return 0; 
}