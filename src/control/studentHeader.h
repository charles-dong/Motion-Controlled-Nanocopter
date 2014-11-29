/*
	Header file so Jamie and Charlie can access global variables

*/

#ifndef _STUDENTHEADER__H_
#define _STUDENTHEADER__H_

#include <pthread.h>
#include "jamieControl.h"

//CS50_TODO:  define your own states
//These are all the states the copter will have 
//#define *other states*
//#define the states you will use here
#define LAND_STATE 1
#define NORMAL_STATE 2
#define PREHOVER_STATE 3
#define HOVER_STATE 4
#define PRENORMAL_STATE 5


//CS50_TODO:  define your own signals
//Define All the signals
#define NO_SIG 11//Each time when our state machine processes the current signal
//A good practice is that when the current signal is processed, set the current signal variable back to "no signal"
#define CHANGE_HOVER_SIG 12//Use to transit state between Normal and Hover
#define TIME_OUT_SIG 13 //The "time out signal" should be created every several seconds
#define FLY_SIG 14
#define LAND_SIG 15


//Define the trim value

//POSITIVE ROLL GOES TO M2
#define TRIM_ROLL 0

//POSITIVE PITCH GOES TOWARD M1
#define TRIM_PITCH 0 
#define TRIM_YAW 0

//Other Defines
#define PITCH_CONSTANT 9
#define ROLL_CONSTANT 9

#define THRUST_MULTIPLIER 17
#define THRUST_DEFAULT_NUMBER 36000

#define HOVERTHRUST 32767

#define LIFTOFF_THRUST 100 
#define LIFTOFF_TIME 100

#define LANDING_TIME 1000

#define TIMEOUT_TIME 500
#define BATTERYCONSTANT 5

//CS50_TODO:  define other variables here
//Such as: current states, current thrust
//variable for state and signals

extern unsigned short DEFAULTTHRUST;
extern float DEFAULTPITCH;
extern float DEFAULTROLL;
extern float DEFAULTYAW;

extern unsigned short current_thrust;
extern float current_pitch;
extern float current_roll;
extern float current_yaw;
extern int current_state;
extern int current_signal;

extern int8_t flightModeHover;

extern pthread_mutex_t lockz;



//extension

extern FrameNode* DLL_HEAD;
extern FrameNode* current_node;
extern int frame_count;
extern int FLY_PATTERN;
extern int FLY_PATTERN_COUNT; 
extern int totalFrames;
extern unsigned short THRUST_YAXIS_EQUALS_ZERO;
#define FLY_PATTERN_TIME 20 // time spent flying each frame
#define MIN_FRAME_COUNT 20
#define NUMFRAMES_TO_DELETE 10

#endif