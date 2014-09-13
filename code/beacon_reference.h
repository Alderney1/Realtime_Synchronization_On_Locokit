/*****************************************************************************
* University of Southern Denmark
* Embedded Programming (EMP)
*
* MODULENAME.: emp.h
*
* PROJECT....: EMP
*
* DESCRIPTION: Test.
*
* Change Log:
******************************************************************************
* Date    Id    Change
* YYMMDD
* --------------------
* 050128  KA    Module created.
*
*****************************************************************************/


#ifndef _SYNCHEADER_
	#define _SYNCHEADER_ 

/***************************** Include files *******************************/

/*****************************    Defines    *******************************/
#define BEACONSENDER 0
#define	RECIEVER_TO_RECIEVER 1

#define IDLE	0
#define SYNC	1

// sender and receiver
#define ALL_NODES 10
#define BEACON_NODE 0

struct node
{
		int number_of_node;
		int m;
		int offset[100];
		int sum_offset[100];
		double current_offset;
		int local_clock[100];
		double lslr[100];
};

/*****************************   Constants   *******************************/
static const char* LCM_IP = "udpm://239.255.76.67:7667?ttl=1";
static const char* SYNC_CHANNEL = "CHANNEL FOR SYNCHRONIZATION";
/*****************************   Functions   *******************************/



//extern void test2(void);
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : Test function
******************************************************************************/


/****************************** End Of Module *******************************/
#endif
