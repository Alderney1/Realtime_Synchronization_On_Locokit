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
// statemachine for RBS
#define BEACONSENDER 0
#define	RECIEVER_TO_RECIEVER 1

#define IDLE	0
#define SYNC	1

// sender and receiver
#define ALL_NODES 10		// How many nodes in the network
#define BEACON_NODE 0		// beacon node

#define TRIALS 500			// How many packets stored

//Inserting and getting
#define REMOTETIME 	1
#define LOCALTIME 	2
#define OFFSET 		3
#define REACHMAX	4
#define DATASIZE	5
#define SCALETIME   6
#define OFFSETMEAN	7
#define LSLR		8
#define NEWTIMELSLR	9
#define NEWTIMEMEAN	10

#define ZERO 0
#define ONE 1


struct NODE_INFO	// store data for each TRIAL
{
	struct NODE_INFO *next;		// next element in the list
	double remote_time;			// time for the remote node
	double local_time;			// time for the current node
	double offset;				// a node offset to the current node
	double scale_time;			// time for the current noce scaled
	double offset_mean;			// result from mean offset
	double lslr;				// result from LSLR
};
struct NODES		// store data for each NODE
{
		int number_of_node;		// number of the node
		int8_t n;				// current sample
		int8_t reach_max;		// When TRIALS is reached
		struct NODE_INFO info[TRIALS];	// stored data for each TRIALS
		struct NODE_INFO *end;			// GET the oldest TRIAL	
		struct NODE_INFO *front;			// GET the newest TRIAL	
};

struct RBS_INFO
{
	double realtime;		// time for current node
	double realtime_0;		// first get time for current node used by scale
	struct NODE_INFO *end;	// get oldest TRIAL
	struct NODES nodes[ALL_NODES];	// all nodes
};
/*****************************   Constants   *******************************/
static const char* LCM_IP = "udpm://239.255.76.67:7667?ttl=1";
static const char* SYNC_CHANNEL = "CHANNEL FOR SYNCHRONIZATION";

/*****************************   Functions   *******************************/



//extern void test2(void);

double getting(const exlcm_sync_t *, int, int);
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : Test function
*****************************************************************************/

double mean_time(const exlcm_sync_t *);
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : Test function
******************************************************************************/
double mean_offset(struct NODE_INFO * );
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : Test function
******************************************************************************/

exlcm_sync_t construct_msg(int8_t, int8_t, int8_t, double);
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : Test function
******************************************************************************/

void print_info(const exlcm_sync_t *);
int RBS(const exlcm_sync_t *);
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : Test function
******************************************************************************/
int setup_threads(void);
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : Test function
******************************************************************************/
void * thread1Run();
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : Test function
******************************************************************************/
void * thread2Run();
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : Test function
******************************************************************************/


/****************************** End Of Module *******************************/
#endif
