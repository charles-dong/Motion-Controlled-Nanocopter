/*
	Header file for Charlie

*/

#ifndef _CHARLIECONTROL__H_
#define _CHARLIECONTROL__H_


//set current_pitch and current_roll of copter according to global variables
  //if flightModeHover == 1, do not add current_thrust
  //else flightModeHover == 0
void fly(CCrazyflie *cflieCopter);

//enables/disables hover mode
bool charlieSendParam(CCrazyflie* crFile, int8_t althold);

//increase thrust gradually, and then hover
void liftOff(CCrazyflie *cflieCopter);

//decrease thrust to zero
void land(CCrazyflie *cflieCopter);

void countTimeOut(CCrazyflie *cflieCopter);

#endif