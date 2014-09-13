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
* University of Southern Denmark
* Software Aspects of Embedded Systems (EMB4)
*
* MODULENAME.: node_sync.c
*
* PROJECT....: Synchronization of a locokit
*
* DESCRIPTION: See module specification file (sync.h-file).
*
* Change Log:
******************************************************************************
* Date    Id    Change
* YYMMDD
* --------------------
* 121201  MATS  Module created.
* 121212  MATS  Least square linar regression implemented
* 121213  MATS  repeatable storing of packets implemented
* 121226  MATS	Modify packets as pointers
*****************************************************************************/

/***************************** Include files *******************************/
#include <stdio.h>

#include <lcm/lcm.h>
#include <pthread.h>
#include <sys/time.h>
#include <math.h>
#include "exlcm_sync_t.h"
#include "sync_node.h"
#include "functions.h"
/*****************************    Defines    *******************************/
#define NODE 1		// Should be appiled for this node
/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/
static lcm_t * lcm;					// lcm handler
static struct RBS_INFO rbs;			// top node for all infomation
	
/*****************************   Functions   *******************************/
static void my_handler(const lcm_recv_buf_t *rbuf, const char * channel, 
        const exlcm_sync_t * msg, void * user)
/*****************************************************************************
*   Input    :
*   Output   :
*   Function :
******************************************************************************/ 
{
	RBS(msg);						// handle lcm inputs
	//printf("Received message on channel \"%s\":\n", channel);

}

int leastsquarelinarregression(struct NODE_INFO * xyCollection)
/*****************************************************************************
*   Input    :
*   Output   :
*   Function :
******************************************************************************/
{


   double SUMx = 0;     //sum of x values
   double SUMy = 0;     //sum of y values
   double SUMxy = 0;    //sum of x * y
   double SUMxx = 0;    //sum of x^2
   double SUMres = 0;   //sum of squared residue
   double res = 0;      //residue squared
   double slope = 0;    //slope of regression line
   double y_intercept = 0; //y intercept of regression line
   double SUM_Yres = 0; //sum of squared of the discrepancies
   double AVGy = 0;     //mean of y
   double AVGx = 0;     //mean of x
   double Yres = 0;     //squared of the discrepancies
   double Rsqr = 0;     //coefficient of determination

	struct NODE_INFO * i = xyCollection;

   //calculate various sums 
  
   int dataSIZE = 0;
   while(i->next != NULL)
   {
		//sum of x
		SUMx = SUMx + i->scale_time;
		//printf("Localtime  = %f     nr = %d\n",i->local_time, dataSIZE );
		//printf("SCALELocaltime  = %f\n",i->scale_time );
		//sum of y
		SUMy = SUMy + i->offset;
		//sum of squared x*y
		SUMxy = SUMxy + i->scale_time * i->offset;
		//sum of squared x
		SUMxx = SUMxx + i->scale_time * i->scale_time;
		i = i->next;
		dataSIZE++;
	}
	
	//sum of x
	SUMx = SUMx + i->scale_time;
    //printf("Localtime  = %f     nr = %d\n",i->local_time, dataSIZE );
	//printf("SCALELocaltime  = %f\n",i->scale_time );
    //sum of y
    SUMy = SUMy + i->offset;
    //sum of squared x*y
    SUMxy = SUMxy + i->scale_time * i->offset;
    //sum of squared x
    SUMxx = SUMxx + i->scale_time * i->scale_time;

	dataSIZE++;
	//calculate the means of x and y
	AVGy = SUMy / (double)dataSIZE;
	AVGx = SUMx / (double)dataSIZE;

	//slope or a1
	slope = (dataSIZE * SUMxy - SUMx * SUMy) / (dataSIZE * SUMxx - SUMx*SUMx);

	//y itercept or a0
	y_intercept = AVGy - slope * AVGx;
   
	// printf("x mean(AVGx) = %f\n", AVGx);
	//printf("y mean(AVGy) = %f\n", AVGy);	

	i->lslr = slope * i->scale_time + y_intercept;	// determine Y for regression

	printf ("\n");
	printf ("The linear equation that best fits the given data:\n");
	printf ("       y = %2.8lfx + %2.8f\n", slope, y_intercept);
	printf ("------------------------------------------------------------\n");
	//printf ("   Original (x,y)   (y_i - y_avg)^2     (y_i - a_o - a_1*x_i)^2\n");
	printf (" Y = %f \n", i->lslr);
	printf ("------------------------------------------------------------\n");	
	
	//calculate squared residues, their sum etc.
	dataSIZE = 0;
	i = xyCollection;
	while(i->next != NULL)
	{
		//current (y_i - a0 - a1 * x_i)^2
		Yres = pow(i->offset - y_intercept - (slope * i->local_time), 2);

		//sum of (y_i - a0 - a1 * x_i)^2
		SUM_Yres += Yres;

		//current residue squared (y_i - AVGy)^2
		res = pow(i->offset - AVGy, 2);

		//sum of squared residues
		SUMres += res;
      
		//printf ("   (%0.2f 	%0.2f)      %0.5E         %0.5E\n", i->local_time, i->offset, res, Yres);
		i = i->next;
		dataSIZE++;
	}

	//current (y_i - a0 - a1 * x_i)^2
     Yres = pow(i->offset - y_intercept - (slope * i->local_time), 2);

	//sum of (y_i - a0 - a1 * x_i)^2
    SUM_Yres += Yres;

    //current residue squared (y_i - AVGy)^2
    res = pow(i->offset - AVGy, 2);

    //sum of squared residues
    SUMres += res;
      
   // printf ("   (%0.2f 	%0.2f)      %0.5E         %0.5E\n", i->local_time, i->offset, res, Yres);
       
    dataSIZE++;

	//calculate r^2 coefficient of determination
	Rsqr = (SUMres - SUM_Yres) / SUMres;
   
	printf("--------------------------------------------------\n");
	printf("Sum of (y_i - y_avg)^2 = %f\t\n", SUMres);
	printf("Sum of (y_i - a_o - a_1*x_i)^2 = %f\t\n", SUM_Yres);
	printf("Standard deviation(St)^2= %f\n", (SUMres / ((double)dataSIZE - (double)1)));
	printf("Standard error of the estimate(Sr)^2 = %f\t\n", (SUM_Yres / (dataSIZE-2)));
	printf("Coefficent of determination(r^2) = %f\t\n", (SUMres - SUM_Yres)/SUMres);
	printf("Correlation coefficient(r)^2 = %f\t\n", (Rsqr));

	return 0;
}
	
double mean_time(const exlcm_sync_t * msg)
/*****************************************************************************
*   Input    :
*   Output   :
*   Function :
******************************************************************************/
{
	struct NODE_INFO *i;
	i = rbs.nodes[msg->sender].end;
	double time;
	int count = 0;
	while(i->next->next != NULL)
	{
		count++;
		time = time + i->next->local_time - i->local_time;	// sum of all offset
		//printf("   Timetemp 	=  %f\n" , time);
		i = i->next;
	}
	count++;
	time = time + i->next->local_time - i->local_time;
	//printf("   Timetemp 	=  %f\n" , time);
	
	return (double)time / ((double)count); // mean of offset				
}

double mean_offset(struct NODE_INFO * xyCollection)
/*****************************************************************************
*   Input    :
*   Output   :
*   Function :
******************************************************************************/
{
	struct NODE_INFO *i;
	i = xyCollection;
	double offset;
	int count = 0;
	while(i->next != NULL)
	{
		count++;
		offset = offset + i->remote_time - i->local_time;	// sum of all offset
		//printf("   Offsettemp 	=  %f\n" , offset);
		i = i->next;
	}
	count++;
	offset = offset + i->remote_time - i->local_time;
	//printf("   Offsettemp 	=  %f\n" , offset);
	i->offset_mean = (double)offset / ((double)count); // mean of offset		
	return 0; 								
}


int handle_linkedlist(const exlcm_sync_t * msg)
/*****************************************************************************
*   Input    :
*   Output   :
*   Function :
******************************************************************************/
{
	if(rbs.nodes[msg->sender].n == ZERO)
	{
		rbs.nodes[msg->sender].info[rbs.nodes[msg->sender].n].next = NULL;
		rbs.nodes[msg->sender].end = &rbs.nodes[msg->sender].info[rbs.nodes[msg->sender].n];
		rbs.nodes[msg->sender].front = &rbs.nodes[msg->sender].info[rbs.nodes[msg->sender].n];
		rbs.nodes[msg->sender].reach_max = 0;
	}
	else if(rbs.nodes[msg->sender].n == TRIALS)
	{
		
		rbs.nodes[msg->sender].info[rbs.nodes[msg->sender].n - ONE].next = &rbs.nodes[msg->sender].info[ZERO];
		rbs.nodes[msg->sender].end = rbs.nodes[msg->sender].end->next;
		rbs.nodes[msg->sender].info[ZERO].next = NULL;
		rbs.nodes[msg->sender].front = &rbs.nodes[msg->sender].info[ZERO]; 
		rbs.nodes[msg->sender].n = ZERO;
		rbs.nodes[msg->sender].reach_max = ONE;
	}
	else if(rbs.nodes[msg->sender].reach_max == ZERO)
	{

		rbs.nodes[msg->sender].info[rbs.nodes[msg->sender].n - ONE].next = &rbs.nodes[msg->sender].info[rbs.nodes[msg->sender].n];
		rbs.nodes[msg->sender].info[rbs.nodes[msg->sender].n].next = NULL;
		rbs.nodes[msg->sender].front = &rbs.nodes[msg->sender].info[rbs.nodes[msg->sender].n];
	}
	if(rbs.nodes[msg->sender].reach_max == ONE & rbs.nodes[msg->sender].n != ZERO)
	{
		rbs.nodes[msg->sender].info[rbs.nodes[msg->sender].n -ONE].next = &rbs.nodes[msg->sender].info[rbs.nodes[msg->sender].n];
		rbs.nodes[msg->sender].end = rbs.nodes[msg->sender].end->next;
		rbs.nodes[msg->sender].info[rbs.nodes[msg->sender].n].next = NULL;
		rbs.nodes[msg->sender].front = &rbs.nodes[msg->sender].info[rbs.nodes[msg->sender].n];
	}
	return 0;
	}	
	
double sample_remoterealtime(const exlcm_sync_t * msg)
/*****************************************************************************
*   Input    :
*   Output   :
*   Function :
******************************************************************************/
{
	static double dd;
	dd++;
	return msg->timestamp ;// + (double)0.01;// * dd;	// store the recived timestamp
}

double getting(const exlcm_sync_t * msg, int operation, int element)
/*****************************************************************************
*   Input    :
*   Output   :
*   Function :
******************************************************************************/ 	
{
	switch(operation)
	{
		case REMOTETIME:
			return rbs.nodes[msg->sender].info[element].remote_time;
			break;
		case LOCALTIME:
			return rbs.nodes[msg->sender].info[element].local_time;
			break;
		case OFFSET:
			return rbs.nodes[msg->sender].info[element].offset;
			break;
		case REACHMAX:
			return rbs.nodes[msg->sender].reach_max;
			break;
		case DATASIZE:
			return (double)rbs.nodes[msg->sender].n;
			break;
		case OFFSETMEAN:
			return rbs.nodes[msg->sender].info[element].offset_mean;
			break;
		case LSLR:
			return rbs.nodes[msg->sender].info[element].lslr;
			break;
		case NEWTIMELSLR:
			return rbs.nodes[msg->sender].info[element].local_time + rbs.nodes[msg->sender].info[element].lslr;
			break;
		case NEWTIMEMEAN:
			return rbs.nodes[msg->sender].info[element].local_time + rbs.nodes[msg->sender].info[element].offset_mean;
			break;
		case SCALETIME:
			return rbs.nodes[msg->sender].info[element].scale_time;
			break;
		default:
			return 1;
			break;
	}
	return 0;
}

int inserting(const exlcm_sync_t * msg, int operation, double insert)
/*****************************************************************************
*   Input    :
*   Output   :
*   Function :
******************************************************************************/ 	
{
	switch(operation)
	{
		case REMOTETIME:
			rbs.nodes[msg->sender].info[rbs.nodes[msg->sender].n].remote_time = insert;
			break;
		case LOCALTIME:
			rbs.nodes[msg->sender].info[rbs.nodes[msg->sender].n].local_time = insert;
			break;
		case OFFSET:
			rbs.nodes[msg->sender].info[rbs.nodes[msg->sender].n].offset = insert;
			break;
		case SCALETIME:
			rbs.nodes[msg->sender].info[rbs.nodes[msg->sender].n].scale_time = insert;
			break;
		default:
			return 1;
			break;
	}
	return 0;
}
int RBS(const exlcm_sync_t * msg)
/*****************************************************************************
*   Input    :
*   Output   :
*   Function :
******************************************************************************/ 	
{
	struct timeval tv;			// create timestruct
	exlcm_sync_t my_data;		// create a struct to LCM
	
	switch(msg->operation)		// statemachine of handling LCM packets
		{
			case BEACONSENDER:	// case when beacon message is recieved	
				
				tv = get_localrealtime();	// get timestamp for received packets
				rbs.realtime = (double)tv.tv_sec + (double)tv.tv_usec / (double)1000000;	// calculate time in sec
				my_data = construct_msg(NODE,ALL_NODES,RECIEVER_TO_RECIEVER,rbs.realtime); // define send struct
				exlcm_sync_t_publish(lcm, SYNC_CHANNEL, &my_data);		// publish the struct
				print_info(msg);				// print all approprivately informations
				break;	
			case RECIEVER_TO_RECIEVER:	// case when nodes inform each other
				if(msg->sender != NODE)   		// ONly run RBS for others nodes  
				{
					//printf("---------------- RECEIVER TO REVEVER SENDER ------------------------\n");
					handle_linkedlist(msg);		// handle list with packets
					
					if(inserting(msg, REMOTETIME, sample_remoterealtime(msg))) // inserting time for remote nodes
						printf("FAILURE IN INSERTING REMOTETIME \n");
					
					if(getting(msg, DATASIZE, ZERO) == ZERO & rbs.nodes[msg->sender].reach_max == ZERO)	// this is also run the first time
					{
						rbs.realtime_0 = rbs.realtime;		// first time for current stored
						if(inserting(msg, SCALETIME, 0))	// store realtime in nodes
							printf("FAILURE IN INSERTING REMOTETIME first run \n");	
					}
					else
					{
						if(inserting(msg, SCALETIME, rbs.realtime - rbs.realtime_0))	// store scalerealtime in nodes
							printf("FAILURE IN INSERTING SCALETIME \n");				
					}
					
					if(inserting(msg, LOCALTIME, rbs.realtime))	// store realtime in nodes
							printf("FAILURE IN INSERTING REALTIME \n");	
					
					if(inserting(msg, OFFSET, sample_remoterealtime(msg) - rbs.realtime))		// store offset (remote - current)
						printf("FAILURE IN INSERTING OFFSET \n");
					
					
					
					mean_offset(rbs.nodes[msg->sender].end);		// calculate meanoffset
					if(rbs.nodes[msg->sender].end->next != NULL)	// skip first sample
						leastsquarelinarregression(rbs.nodes[msg->sender].end);	// calculate leastsquareregression
						
					//printf("----------------END RECEIVER TO REVEVER SENDER ------------------------\n");
					
					print_info(msg);				// print all approprivately informations
					rbs.nodes[msg->sender].n++;		// count x increment
					
				}
				break;
			default:
				break;
			}
	return 0;
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

void print_info(const exlcm_sync_t * msg)
/*****************************************************************************
*   Input    :
*   Output   :
*   Function :
******************************************************************************/ 	
{
	printf("----------------- PRINT INFO ----------------\n");
	printf("  Sender   		= %u\n", msg->sender);
	printf("  Reciver   		= %u\n", msg->receiver);
	printf("  Operation   		= %u\n", msg->operation);
	printf("  Number_of_packets	= %lld\n", msg->number_of_packets);
	printf("  timestamp   	= %f\n", msg->timestamp);
	
	//printf("------------------------------------------------\n");
	printf("Remote realtime  =  %f\n" , getting(msg, REMOTETIME, getting(msg, DATASIZE, ZERO)));
	printf("Local realtime  =  %f\n" , getting(msg, LOCALTIME, getting(msg, DATASIZE, ZERO)));
	printf("OFFSET  =  %f\n" ,getting(msg, OFFSET, getting(msg, DATASIZE, ZERO)));
	//printf("------------------------------------------------\n");
	printf("SCALETIME		=	%f\n", getting(msg, SCALETIME, getting(msg, DATASIZE, ZERO)));
	printf("MEAN OFFSET		=	%f\n", getting(msg, OFFSETMEAN, getting(msg, DATASIZE, ZERO)));
	printf("LSLR OFFSET		=	%f\n", getting(msg, LSLR, getting(msg, DATASIZE, ZERO)));
	printf("NEW TIME MEANOFFSET	=	%f\n", getting(msg, NEWTIMEMEAN, getting(msg, DATASIZE, ZERO)));
	printf("NEW TIME LSLR		=	%f\n", getting(msg, NEWTIMELSLR, getting(msg, DATASIZE, ZERO)));
	printf("----------------- END PRINT INFO ----------------\n");
	
	struct NODE_INFO * i = &rbs.nodes[msg->sender].end;
	printf("DATASIZE = %f\n", getting(msg, DATASIZE,ZERO));
   int dataSIZE = 0;
   while(i->next != NULL)
   {
	   printf("LSLR = %f\n",  i->lslr);
	   	i = i->next;
		dataSIZE++;
	}
	printf("LSLR = %f\n", i->lslr);
	
}	
int setup_lcm(const char* IP_adress)
/*****************************************************************************
*   Input    :
*   Output   :
*   Function :
******************************************************************************/ 
{
	lcm = lcm_create(IP_adress);		// allocates and initializes an instance of lcm_t, specific IP
	//lcm = lcm_create(NULL);				// just use for local network
	if(!lcm)
	{
		return 1;						// creating fail
	}
	return 0;							// succes
}	
int init_variables()
/*****************************************************************************
*   Input    :
*   Output   :
*   Function :
******************************************************************************/ 
{
	int i;
	for(i=0; i < ALL_NODES;i++)		// resetting datafields
	{
		rbs.nodes[i].n = 0;
		rbs.nodes[i].reach_max = 0;
	}
	return 0;
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
	if(init_variables())
	{
	}
		
	if(setup_threads())			// run threads
	{
		printf("	Threads UNSUCCESSFUL created\n");
	}
	else
	{
		printf("	Threads succesful created\n");
	}
}

int setup_threads(void)
/*****************************************************************************
*   Input    :
*   Output   :
*   Function :
******************************************************************************/ 
{
	
	pthread_t thread1, thread2;

	if(pthread_create(&thread1,NULL,thread1Run,NULL))
	{
		return 1;
	}
    if(pthread_create(&thread2,NULL,thread2Run,NULL))
    {
		return 1;
	}
	
    if(pthread_join(thread1,NULL))
    {
		
		return 1;
	} 
    if(pthread_join(thread2,NULL))
    {
		return 1;
	}

	//printf("	----------------i'M HERE -------------------\n");
	return 0; 				//succesS
}

void * thread1Run()
/*****************************************************************************
*   Input    :
*   Output   :
*   Function :
******************************************************************************/ 
{

    exlcm_sync_t_subscribe(lcm, SYNC_CHANNEL, &my_handler, NULL);
	while(1)
    lcm_handle(lcm);

    lcm_destroy(lcm);

    
}

void * thread2Run()
/*****************************************************************************
*   Input    :
*   Output   :
*   Function :
******************************************************************************/ 
{
	//while(1);
	
}

int main(int argc, char ** argv)
/*****************************************************************************
*   Input    :
*   Output   :
*   Function :
******************************************************************************/ 
{
	setup();				// setup LCM
	
	return 0;
	};
