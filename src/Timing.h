/* Timing.h
 *
 */

#if !defined TIMING_H
#define TIMING_H

#if defined __cplusplus
extern "C" {
#endif //__cplusplus

/**
 * @brief Get a timestamp representing the current time with millisecond granularity
 * @details The starting point of the timestamp will depend upon library providing the function.
 * @return double The timestamp in milliseconds
 */
double GetTimeInMsec();

/**
 * @brief Get a timestamp representing the current time with microsecond granularity
 * @details The starting point of the timestamp will depend upon library providing the function.
 * @return double The timestamp in microseconds
 */
double GetTimeInUsec();

/**
 * @brief Attempt to sleep for at least ms milliseconds
 * @details Or more literally, call SleepUsec(ms*1000)
 * @param ms (double) The amount of time to sleep, in milliseconds
 */
void SleepMsec(double ms);

/**
 * @brief Attempt to sleep for at least us microseconds
 * @details Currently our sleeping is done by using the usleep function.
 * @param us (int) The amount of time to sleep, in microseconds
 */
void SleepUsec(int us);

#if defined DEBUG
int Timing_Test();	//!< Run internal tests
#endif //DEBUG

#if defined __cplusplus
}
#endif //__cplusplus

#endif //TIMING_H
