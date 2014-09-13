
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



