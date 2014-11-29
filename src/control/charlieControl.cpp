/*
	Author: Charles Dong

	Functions: fly, liftoff, land, countTimeOut
*/

#include <cflie/CCrazyflie.h>
#include <stdio.h>
#include <unistd.h>
#include "../leap/leap_c.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <pthread.h>
#include "studentHeader.h"
#include "charlieControl.h"


//In normal state, the flyNormal function will be called, set different parameter
//In hover state, different function should be called, because at that time, we should set the thrust as a const value(32767), see reference 
//(http://forum.bitcraze.se/viewtopic.php?f=6&t=523&start=20)

/*

*fly*
------------

Description: set current_pitch and current_roll of copter according to global variables
  if flightModeHover == 1, do not add current_thrust
  else flightModeHover == 0

Input: pointer to copter

Return: n/a

** Pseudo Code **

(1) 

*****

*/
void fly(CCrazyflie *cflieCopter){

    //set cflieCopter's pitch, roll, and yaw
    cflieCopter->m_fRoll = DEFAULTROLL + current_roll;
    cflieCopter->m_fPitch = DEFAULTPITCH + current_pitch;
    cflieCopter->m_fYaw = DEFAULTYAW + current_yaw;

    if (flightModeHover == 0) {
    	setThrust(cflieCopter, DEFAULTTHRUST + current_thrust*THRUST_MULTIPLIER*(BATTERYCONSTANT - batteryLevel(cflieCopter)));
      //printf("current_thrust = %hu\n", (unsigned short)cflieCopter->m_nThrust);
    }
    else {
    	//hover mode - use default hover thrust
    	cflieCopter->m_nThrust = HOVERTHRUST;
    }

}



/*

*land*
------------

Description: decrease thrust to zero

Input: pointer to copter

Return: n/a

** Pseudo Code **

(1) 

*****

*/
void land(CCrazyflie *cflieCopter){

  for (int i = 0; i < 15; i++) {
    printf("LANDING. KILLING THRUST!\n");
  }
  
  current_pitch = 0;
  current_roll = 0;
  current_yaw = 0;
  int new_current_thrust = 0;

  //set roll, pitch, yaw, thrust to 0
  cflieCopter->m_fRoll = current_roll;
  cflieCopter->m_fPitch = current_pitch;
  cflieCopter->m_fYaw = current_yaw;

  //stay at thrust 35000 for a bit
  double dTimeNow = currentTime();
  while ((currentTime() - dTimeNow) < LANDING_TIME){
    setThrust(cflieCopter, DEFAULTTHRUST + new_current_thrust);
    cycle(cflieCopter);
  }

  //decrease thrust to 0
  while ((DEFAULTTHRUST + new_current_thrust) >= 0){
    setThrust(cflieCopter, DEFAULTTHRUST + new_current_thrust);
    cycle(cflieCopter);
    new_current_thrust = new_current_thrust - 500;
  }
  setThrust(cflieCopter, 0);
  cycle(cflieCopter);
}


/*

*liftoff*
------------

Description: 

Input: pointer to copter

Return: n/a

** Pseudo Code **

(1) 

*****

*/
void liftOff(CCrazyflie *cflieCopter){

  current_pitch = 0;
  current_roll = 0;
  current_yaw = 0;

  //set roll, pitch, yaw
  setPitch(cflieCopter,DEFAULTPITCH + current_pitch);
  setRoll(cflieCopter,DEFAULTROLL + current_roll);
  cflieCopter->m_fYaw = DEFAULTYAW + current_yaw;

  //set liftoff thrust
  current_thrust = LIFTOFF_THRUST;
  setThrust(cflieCopter, DEFAULTTHRUST + current_thrust);
  printf("cflieCopter->m_nThrust == %d", cflieCopter->m_nThrust);

  //liftoff
  /*
  double dTimeNow = currentTime();
  while ((currentTime() - dTimeNow) < LIFTOFF_TIME){
    printf("STILL LIFTING OFF\n");
  }
  */

}

/*

*charlieSendParam*
------------

Description: enables/disables hover mode;

Input: pointer to copter, althold == 1 to enable, althold == 0 to disable

Return: not sure what boolean return value means 

*****

*/
bool charlieSendParam(CCrazyflie* crFile, int8_t althold) {

	//*pseudocode*
        /*
            Checkout sendSetpoint function, it is similiar to the sendSetpoint function
            But the char cBuffer[nSize]; should have the an "unsigned char" type
            varid (as the id of the flightmode.althold) and followed by a "int8_t"
            variable (1 means enable the hover mode and 0 meas disable the hover mode)
            Then initilaize it with Port 2, Channel 2 packets send this packet to 
            the copter
        */
    //*pseudocode* 

    unsigned char varid = idForName(crFile->m_tocParameters,"flightmode.althold");
    int nSize = sizeof(unsigned char) + sizeof(int8_t);
    char cBuffer[nSize];
    memcpy(&cBuffer[0], &varid, sizeof(unsigned char));
    memcpy(&cBuffer[1 * sizeof(unsigned char)], &althold, sizeof(int8_t));

    CCRTPPacket *crtpPacket = new CCRTPPacket;
    CCRTPPacketInit3(crtpPacket,cBuffer, nSize, 2); //sets port to 2
    setChannel(crtpPacket, 2); //set channel to 2
    CCRTPPacket *crtpReceived = sendPacket(crFile->m_crRadio,crtpPacket);
  
    delete crtpPacket;
    if(crtpReceived != NULL) {
      delete crtpReceived;
      return true;
    } else {
      return false;
    }

}





/*

*countTimeOut*
------------

Description: count until MAX_COUNT, then set current_signal = TIME_OUT_SIG
  set pitch/roll to 0 and sendSetpoint

Input: pointer to copter, current roll/pitch/yaw/thrust, pointer to current_signal

Return: n/a

** Pseudo Code **

(1) count until MAX_COUNT, then issue current_signal = TIME_OUT_SIG

*****

*/
void countTimeOut(CCrazyflie *cflieCopter){
  
    current_pitch = 0;
    current_roll = 0;
    current_yaw = 0;
    current_pitch = 0;

    //set roll, pitch, yaw
    setPitch(cflieCopter, DEFAULTPITCH + current_pitch);
    setRoll(cflieCopter, DEFAULTROLL + current_roll);
    cflieCopter->m_fYaw = DEFAULTYAW + current_yaw;
    cflieCopter->m_nThrust = HOVERTHRUST;

    //Time Out
    double dTimeNow = currentTime();
    while ((currentTime() - dTimeNow) < TIMEOUT_TIME){
      printf("STILL TIMED OUT\n");
      cycle(cflieCopter);
    }
  current_signal = TIME_OUT_SIG;

}


