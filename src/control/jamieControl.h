/*
	Header file for Jamie 

*/

#ifndef _JAMIECONTROL__H_
#define _JAMIECONTROL__H_

#include "../leap/leap_c.h"
#include <cflie/CCrazyflie.h>

// Defines FrameNode Structure.
typedef struct _FrameNode {
	struct _FrameNode *next;        // pointer to the next member of the list.    
	struct _FrameNode *prev;        // pointer to the previous member of the list.   
	int pos_x;                   // document identifier            
	int pos_y;
	int pos_z;
} __FrameNode;

typedef struct _FrameNode FrameNode;

FrameNode* record_tool_movement(leap_vector hand_position);

FrameNode* new_frame_node(int pos_x, int pos_y, int pos_z);

void jamieOnFrame(leap_hand_ref hand, leap_frame_ref frame, CCrazyflie* cflie);
//void fly_pattern(CCrazyflie* cflie);

//frees all frame nodes starting from passed node
void freeFrameNodes(FrameNode *current);

#endif