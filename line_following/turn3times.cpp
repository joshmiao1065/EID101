#include "rcc_stdlib.h"
using namespace std;
//TEST WHAT DURATION IS NEEDED FOR THE DWELL CHECK/LOGIC STATES
//CHANGE POSITION OF ML AND MR SENSORS
//EDIT LOG
//WORK ON REVERSES AND 180s
typedef enum {
    DWELL_LEFT90,
    LEFT90,
    END
}linefollowing_t;

typedef enum {
    DWELL,
    INTEGRATE
} integrate_t;

int main() {
    stdio_init_all();
    sleep_ms(4000); //sleep to give us time to place the robot
    cyw43_arch_init();
    cyw43_arch_gpio_put(0, 1);

    Motor motors;
    MotorInit(&motors, RCC_ENA, RCC_ENB, 1000);
    MotorsOn(&motors);
    cyw43_arch_deinit();
 
    rcc_init_i2c();
    MPU6050 imu;
    imu.begin(i2c1);
    imu.calibrate();
    
    double theta = 0.0;
    double ninety = 51.5;

    int counter = 0;

    uint32_t cur, prev; // curent and previous time
    uint32_t cur_integrate, prev_integrate;//current and previous times for the integration state machine
    uint32_t dx = 10000; //10ms riemann sum interval
    uint32_t pause = 200000;

    linefollowing_t movestate = LEFT90;
    integrate_t integrationstate = DWELL;

    while(true){
        cur = time_us_32();
        cur_integrate = time_us_32();
        imu.update_pico();
        cout <<"theta is" <<theta <<"\n";

        switch(movestate) {

            case LEFT90:
                cout << "LEFT90\n";
                MotorPower(&motors, -70 ,70);
                if(theta >= ninety){
                    cur = time_us_32();
                    prev = cur;              
                    movestate = DWELL_LEFT90;
                }

                if(counter >= 3) {
                    movestate = END;
                }
            break;

        case DWELL_LEFT90:
            cout << "DWELL_LEFT90\n";
            cout << "this is cur "<< cur  <<"\n";
            cur = time_us_32();
 
            MotorPower(&motors, 0,0);
            if (cur - prev >= pause) {
                theta = 0.0;
                counter++;
                movestate = LEFT90;
                prev = cur;
            }

            if(counter >= 3) {
                    movestate = END;
                }
        break;
        
        case END:
            cout <<"END\n";
            MotorPower(&motors, 0, 0);
    }
//------------------integrator------------------------------------------
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