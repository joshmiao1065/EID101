#include "rcc_stdlib.h"
using namespace std;
//TEST WHAT DURATION IS NEEDED FOR THE DWELL CHECK/LOGIC STATES
//CHANGE POSITION OF ML AND MR SENSORS
//EDIT LOG
//WORK ON REVERSES AND 180s
typedef enum {
    DWELL_STRAIGHT,
    STRAIGHT,
    DWELL_STRAIGHT000,
    STRAIGHT000,
    DWELL_LEFT90,
    LEFT90,
    DWELL_RIGHT90,
    RIGHT90,
    DWELL_TURN180,
    TURN180,
    DWELL_CHECK000,
    CHECK000,
    DWELL_TURN180_000,
    TURN180_000,
    DWELL_CHECK_111,
    CHECK111,
    LOGIC111,
    DWELL_REVERSE,
    REVERSE,
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
 
    int ppb = 0; //ping pong ball not launched
    int ssm = 0; //straight state machine not engaged
 
    double theta = 0.0;
    double ninety = 52.0;//this is a 90 degree turn, idk why the imu is off by so much
    double oneeighty = 105.5;//180 degree turn MotorPowers(&motors, -55 70)
 
    uint32_t cur, prev; // curent and previous time
    uint32_t cur_integrate, prev_integrate;//current and previous times for the integration state machine
    uint32_t dx = 10000; //10ms riemann sum interval
    uint32_t pause = 200000;//200ms rest period between states
 
    linefollowing_t movestate = STRAIGHT;
    straightfollowing_t straightstate = DEFAULT;
    integrate_t integrationstate = DWELL;
 
    while(true){
        cur = time_us_32();
        cur_integrate = time_us_32();
        imu.update_pico();
        cout <<"theta is" <<theta <<"\n";
//--------------line following state machine---------------------------------------------------------------
 
    switch(movestate) {

        case STRAIGHT:  
            //MotorPower(&motors, 50, 51);  
            cout << "STRAIGHT\n";
            
            ssm = 1; 
             switch(straightstate) { 

        case DEFAULT:
            MotorPower(&motors, 50, 51);
            cout << "default \n";
            
            if(!gpio_get(1) && gpio_get(3) ) {
                straightstate = LEFT_ADJUST;
            }

            if(gpio_get(1) && !gpio_get(3) ) {
                straightstate = RIGHT_ADJUST;
            }
            if(ssm == 0) {
                straightstate = DWELL_SSM;
            }           
        break;
 
        case LEFT_ADJUST: //if it drifts to the left and MR detects the line
            MotorPower(&motors, 60, 40); //increase leftside
            cout << "left-adjust \n";
            if(!gpio_get(1) && !gpio_get(3) ) {
                straightstate = DEFAULT;
            }
            if(gpio_get(1) && !gpio_get(3) ) {
                straightstate = RIGHT_ADJUST;
            }
            if(ssm == 0) {
                straightstate = DWELL_SSM;
            } 
        break;
 
        case RIGHT_ADJUST: //if it drifts to the right and ML detects the line
            MotorPower(&motors, 45, 60); //increase rightside
            cout << "right-adjust \n";
            if (!gpio_get(1) && !gpio_get(3) ){
                straightstate = DEFAULT;
            }
            if(!gpio_get(1) && gpio_get(3) ) {
                straightstate = LEFT_ADJUST;
            }
            if(ssm == 0) {
                straightstate = DWELL_SSM;
            } 
        break;

        case DWELL_SSM:
            cout << "Dwell_SSM\n";
            if(!gpio_get(1) && !gpio_get(3) ) {
                straightstate = DEFAULT;
            }
            if(!gpio_get(1) && !gpio_get(3) ) {
                straightstate = LEFT_ADJUST;
            }

            if(gpio_get(1) && !gpio_get(3) ) {
                straightstate = RIGHT_ADJUST;
            }           
        break;


    }    
 
 
            if ((gpio_get(0) && !gpio_get(2) && !gpio_get(4)) || (gpio_get(0) && gpio_get(2) && !gpio_get(4)) || (gpio_get(0) && !gpio_get(2) && gpio_get(4) && ppb == 0)){
                cur = time_us_32();
                prev = cur;
                ssm = 0;                
                movestate = DWELL_LEFT90;
            }
            if ((!gpio_get(0) && gpio_get(4) || (gpio_get(0) && !gpio_get(2) && gpio_get(4) && ppb == 1))){
                cur = time_us_32();
                prev = cur;   
                ssm = 0;             
                movestate = DWELL_RIGHT90;
            }
            if (!gpio_get(0) && !gpio_get(2) && !gpio_get(4)){
                cur = time_us_32();
                prev = cur;                
                ssm = 0;
                movestate = DWELL_CHECK000;
            }
            if(gpio_get(0) && gpio_get(2) && gpio_get(4)) {
                cur = time_us_32();
                prev = cur;             
                ssm = 0;   
                movestate = DWELL_CHECK_111;
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
 
        case RIGHT90:
        cout << "RIGHT90\n";
            MotorPower(&motors, 75 ,0);
            if(theta <= ( (-1) * ninety)){
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
 
        case CHECK111:
            cout << "CHECK111\n";
            ssm = 1;
            MotorPower(&motors, 50, 50);
            if(cur - prev >= pause) {
                ssm = 0;
                movestate = LOGIC111;
            }
        break;
 
        case LOGIC111:
        cout << "LOGIC111\n";
            if(!gpio_get(0) && gpio_get(2) && !gpio_get(4)) {
                MotorPower(&motors, -50, -51); //reverse for a little while and then turn right
                if(cur - prev >= pause) {
                    cur = time_us_32();
                    prev = cur;     
                    ssm = 0;           
                    movestate = DWELL_RIGHT90;
                 }
            }
            if(gpio_get(0) && gpio_get(2) && gpio_get(4) && ppb == 0) {
                cur = time_us_32();
                prev = cur;         
                ssm = 0;       
                movestate = DWELL_TURN180;
            }
            if(gpio_get(0) && gpio_get(2) && gpio_get(4) && ppb == 1) {
                movestate = END;
            }
        break;
 
        case CHECK000:
            cout << "CHECK000\n";
            MotorPower(&motors, 0, 0);
        //ASSUMING IR SENSOR PIN 5 FOR BUCKET DETECTION
            if(gpio_get(5)) {
                //ADD CODE TO ROTATE SERVO TO SHOOT BALL AND SET PPB = 1
            }
            if(!gpio_get(5)) {
                cur = time_us_32();
                prev = cur;
                ssm = 0;
                theta = 0.0;              
                movestate = DWELL_TURN180_000;
            }
        break;
 
        case TURN180_000:
//        cout << "TURN180_000\n";
        cout << "theta: " << theta <<"\n";
            MotorPower(&motors, -55, 70);
            if(theta >= oneeighty){
                cur = time_us_32();
                prev = cur;           
                movestate = DWELL_STRAIGHT000;
                }
        break;
       
        case STRAIGHT000:
            ssm = 1;
            cout << "STRAIGHT000\n";
            MotorPower(&motors, 50, 50);
 
            if(gpio_get(0) && gpio_get(2) && !gpio_get(4) || !gpio_get(0) && gpio_get(2) && gpio_get(4)) {
                movestate = STRAIGHT;
            }
            if(gpio_get(0) && !gpio_get(2) && !gpio_get(4)) {
                cur = time_us_32();
                prev = cur;       
                ssm = 0;         
                movestate = DWELL_LEFT90;
            }
            if(gpio_get(0) && gpio_get(2) && gpio_get(4)) {
                cur = time_us_32();
                prev = cur;             
                ssm = 0;   
                movestate = DWELL_CHECK_111;
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
       
        case DWELL_RIGHT90:
        cout << "DWELL_RIGHT90\n";
        cout << "this is cur "<< cur  <<"\n";
        cur = time_us_32();
 
            MotorPower(&motors, 0,0);
            if (cur - prev >= pause) {
                theta = 0.0;
                movestate = RIGHT90;
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
 
        case DWELL_CHECK000:
            cout << "DWELL_CHECK000\n";
            cout << "this is cur "<< cur  <<"\n";
            ssm = 1;
            MotorPower(&motors, 31,30); //find
            cur = time_us_32();
 
            cout << "this is prev "<< prev  <<"\n";
            if (cur - prev >= pause) {
                cur = time_us_32();
                prev = cur;        
                ssm = 0;        
                movestate = CHECK000;
            }
        break;
 
        case DWELL_CHECK_111:
            cout << "DWELL_CHECK_111\n";
            cout << "this is cur "<< cur  <<"\n";
            cur = time_us_32();
            prev = cur;
            ssm = 1;
        
            MotorPower(&motors, 31,30); //find
            if (cur - prev >= pause) {
                cur = time_us_32();
                prev = cur; 
                ssm = 0;               
                movestate = CHECK111;
        }
        break;
 
        case DWELL_TURN180_000:
        cout << "DWELL_TURN180_000\n";
            MotorPower(&motors, 0,0);
            if (cur - prev >= pause) {
                theta = 0.0;
                movestate = TURN180_000;
                prev = cur;
            }
        break;
 
        case DWELL_STRAIGHT000:
        cout << "DWELL_STRAIGHT000\n";
            MotorPower(&motors, 0,0);
            if (cur - prev >= pause) {
                movestate = STRAIGHT000;
                prev = cur;
            }
        break;
 
        case END:
            MotorPower(&motors, 0, 0);
    }
 
//----------------straight correcting state machine-------------------------------------------------
 
    switch(straightstate) { 

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
            MotorPower(&motors, 60, 40); //increase leftside
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
            MotorPower(&motors, 45, 60); //increase rightside
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
            if(!gpio_get(1) && !gpio_get(3) && ssm == 1) {
                straightstate = LEFT_ADJUST;
            }

            if(gpio_get(1) && !gpio_get(3) && ssm == 1) {
                straightstate = RIGHT_ADJUST;
            }           
        break;


    }    
 
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
