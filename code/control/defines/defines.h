#ifndef DEFINES_H
#define DEFINES_H

//#####################################################################################################################
// ID OUT
//#####################################################################################################################
#define ID_OUT_LIGHTS               0
#define ID_OUT_MOTOR                1
#define ID_OUT_SERVO                2

#define ID_OUT_LIGHTS_EFFECT        1
#define ID_OUT_INDICATOR_LF         2
#define ID_OUT_INDICATOR_RF         3
#define ID_OUT_INDICATOR_LB         4
#define ID_OUT_INDICATOR_RB         5
#define ID_OUT_BRAKE                6
//#####################################################################################################################
// ID IN
//#####################################################################################################################
#define ID_IN_ULTRASONIC_CENTER     1
#define ID_IN_ULTRASONIC_CENTER_R   2
#define ID_IN_ULTRASONIC_SIDE_FRONT 3
#define ID_IN_ULTRASONIC_SIDE_BACK  4
#define ID_IN_ULTRASONIC_BACK       5
#define ID_IN_ENCODER_L             6
#define ID_IN_ENCODER_R             7
#define ID_IN_BUTTON_LANE           8
#define ID_IN_BUTTON_PARK           9
#define ID_IN_BUTTON_OVERTAKE       10
#define ID_IN_YAW                   11
#define ID_IN_ROLL                  12
#define ID_IN_PITCH                 13
#define ID_IN_KM                    0
//#####################################################################################################################
// SERIAL
//#####################################################################################################################
#define _PI 3.1415926535897
#define KM_IN_CM  100000
#define BAUD_RATE 115200
#define MOTOR_IDLE 90
#define MOTOR_MIN  0
#define MOTOR_MAX  180
#define STRAIGHT_DEGREES 90
#define MIN_DEGREES 0
#define MAX_DEGREES 180
#define US_MIN_RANGE 0
#define US_MAX_RANGE 100
#define G_MIN_RANGE 0
#define G_MAX_RANGE 361
#define IDS_MIN_RANGE 0
#define IDS_MAX_RANGE 13
//#####################################################################################################################
// GLOBAL
//#####################################################################################################################
#define ONE_SECOND 1000000
#define NO_DATA 0
#define NO_OBSTACLE -1
//#####################################################################################################################
// LANE FOLLOWER
//#####################################################################################################################
#define _SPEED 102
//#####################################################################################################################
// PARK
//#####################################################################################################################
#define MIN_SPACE_SIZE 7
#define US 1
#define GAP 11
#define RIGHT_TURN 1
#define LEFT_TURN 2
#define INGAP_RIGHT_TURN 3
#define END 4
//#####################################################################################################################
// OVERTAKE
//#####################################################################################################################


#endif
