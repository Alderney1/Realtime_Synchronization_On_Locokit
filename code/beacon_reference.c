// file: beacon_reference.c
//
// LCM example program.
//
// compile with:
//  $ gcc -o send_message beacon_reference.c -llcm
//
// On a system with pkg-config, you can also use:
//  $ gcc -o send_message send_message.c `pkg-config --cflags --libs lcm`
/*****************************************************************************
__author__ = "Mats Larsen"
__copyright__ = "Mats Larsen 2014"
__credits__ = ["Mats Larsen"]
__license__ = "GPLv3"
__maintainer__ = "Mats Larsen"
__email__ = "larsen.mats.87@gmail.com"
__status__ = "Development"
******************************************************************************
* University of Southern Denmark
* Software Aspects of Embedded Systems (EMB4)
*
* MODULENAME.: beacon_reference.c
*
* PROJECT....: Synchronization of a locokit
*
* DESCRIPTION: See module specification file (.h-file).
*
* Change Log:
******************************************************************************
* Date    Id    Change
* YYMMDD
* --------------------
* 121201  MATS  Module created.
*
*****************************************************************************/

/***************************** Include files *******************************/
#include <stdio.h>
#include <lcm/lcm.h>
#include <sys/time.h>
#include "exlcm_sync_t.h"
#include "beacon_reference.h"
/*****************************    Defines    *******************************/

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/
static lcm_t * lcm;
/*****************************   Functions   *******************************/

int setup_lcm(const char* IP_adress)
/*****************************************************************************
*   Input    :
*   Output   :
*   Function :
******************************************************************************/ 
{
	lcm = lcm_create(IP_adress);		// allocates and initializes an instance of lcm_t, specific IP
	//lcm = lcm_create(NULL);				// use just on local network
	if(!lcm)
	{
		return 1;						// creating fail
	}
	return 0;							// succes
}
void setup(void)
/*****************************************************************************
*   Input    :
*   Output   :
*   Function :
******************************************************************************/ 
{
	if(setup_lcm(LCM_IP))
	{
		printf("	Failure in setup_lcm with this IP %s\n",  LCM_IP);
	}
	else
	{
		printf("	LCM created with this IP %s\n",  LCM_IP);
	}
}

exlcm_sync_t construct_msg(int8_t sender, int8_t receiver, 
				int8_t operation, double timestamp)
/*****************************************************************************
*   Input    :
*   Output   :
*   Function :
******************************************************************************/ 
{
	static int64_t number_of_packets;
	exlcm_sync_t my_data = {
		.sender = sender,
		.receiver = receiver,
		.operation = operation,
		.number_of_packets = number_of_packets,
		.timestamp = timestamp,
		};
	number_of_packets++;
	return my_data;
	}
struct timeval get_localrealtime(void)
/*****************************************************************************
*   Input    :
*   Output   :
*   Function :
******************************************************************************/ 
{
	struct timeval tv;
	gettimeofday(&tv, NULL); 
	return tv;
}

int main(int argc, char ** argv)
/*****************************************************************************
*   Input    :
*   Output   :
*   Function :
******************************************************************************/ 
{
	exlcm_sync_t my_data;
	
	setup();				// setup LCM
	while(1)
	{
	my_data = construct_msg(BEACON_NODE,ALL_NODES,BEACONSENDER,(double)get_localrealtime().tv_sec + (double)(get_localrealtime().tv_usec / (double)1000000) ); // define send struct
	exlcm_sync_t_publish(lcm, SYNC_CHANNEL, &my_data);		// publish the struct
	printf("--------MESSAGE PUBLISH:-------\n NUMBER OF PACKETS 	=	%lld\n TIMESTAMP IN SEC	=	%f\n"
	, my_data.number_of_packets, my_data.timestamp);
	sleep(5);
	}
	lcm_destroy(lcm);									// terminate the allocated memory for lcm
	return 0;
    };
