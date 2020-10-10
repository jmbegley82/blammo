/* Timing.c
 *
 */

#include <unistd.h>
#include <time.h>

#if !defined CLOCK_MONOTONIC_RAW
#include <sys/time.h>
#endif // ndef CLOCK_MONOTONIC_RAW

#include "Timing.h"

double GetTimeInMsec() {
	return GetTimeInUsec()/1000.0;
}

double GetTimeInUsec() {
	double retval = 0;
#if defined CLOCK_MONOTONIC_RAW
	//linux-specific code follows
	struct timespec currentTime;
	clock_gettime(CLOCK_MONOTONIC_RAW, &currentTime);
	retval = currentTime.tv_sec * 1000000.0;
	retval += currentTime.tv_nsec / 1000.0;
#else
	//more generic code here:
	struct timeval currentTime;
	gettimeofday(&currentTime, 0);
	retval = currentTime.tv_sec * 1000000.0;
	retval += currentTime.tv_usec;
#endif
	return retval;
}

void SleepMsec(double ms) {
	if(ms > 0) usleep((int)(ms*1000.0));
}

void SleepUsec(int us) {
	if(us > 0) usleep(us);
}

