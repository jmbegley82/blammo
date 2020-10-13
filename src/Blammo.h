/* Logger.h
 *
 */

#if !defined BLAMMO_H
#define BLAMMO_H

#if defined __cplusplus
#include <string>
using std::string;
int Log(const string& str);		//!< Write to Logger using a C++ string
int Log_now(const string& str);		//!< Write to Logger using a C++ string and immediately flush

extern "C" {
#endif //__cplusplus

/**
 * @brief These represent values returned from Blammo_status and stored in _logStatus.
 * @details They refer to various running states of the Blammo_autoflush thread.  LOG_EXITING should probably never be
 * used manually!  The others can be set externally via Blammo_pause and Blammo_unpause.
 */
enum LogStatuses {
	BLAMMO_UNINIT,	//!< Log not initialized yet
	BLAMMO_STOPPED,	//!< Do not print anything via Blammo_autoflush
	BLAMMO_RUNNING,	//!< Do print things via Blammo_autoflush
	BLAMMO_EXITING	//!< Do not print anything via Blammo_autoflush and cause its thread to exit
};

/**
 * @brief Initialize the Logger
 * @details In the future, this will accept parameters that will specify console output, file output, path of output file
 * and will almost certainly initialize a mutex.
 * @return int 0 for everything went okay, -1 if it's already initialized
 */
int Blammo_init();

/**
 * @brief Lock mutex, then call Blammo_process_unsafe to write pending buffered log entries and clear the buffer
 * @details This function will be primarily called internally but should be safe for general use.
 */
void Blammo_process();

/**
 * @brief Flush the buffer and deinitialize things and such
 */
void Blammo_finish();

/**
 * @brief Add entry to log buffer
 * @details Once the entry is added to the buffer, the buffer size will be checked.  If it is over its predetermined
 * maximum length, Blammo_process() will be called afterward to write the contents of the buffer and clear it, leaving us
 * with an empty buffer.
 * @param str (const char*) the C-string to add to the log buffer
 * @return int -1 = Logger hasn't been initialized yet, 0 = success, 1 = success and the buffer was emptied
 */
int Logger(const char* str);

/**
 * @brief Print this message to the log right now
 * @details Well, literally, add the line to the buffer and immediately flush it with Blammo_process_unsafe
 * @param str (const char*) the C-string to log
 */
void Logger_now(const char* str);

/**
 * @brief Pause the automatic log-processing thread
 * @details Set _logStatus to LOG_STOPPED
 */
void Blammo_pause();

/**
 * @brief Resume the automatic log-processing thread
 * @details Set _logStatus to LOG_RUNNING
 */
void Blammo_unpause();

/**
 * @brief return _logStatus
 */
int Blammo_status();

/**
 * @brief Set the amount of time between automatic logbuffer flushes
 * @param ms (double) The amount of delay in milliseconds
 */
void Blammo_setflushdelay(double ms);

/**
 * @brief Get the currently-set amount of time between automatic logbuffer flushes
 * @return double The amount of delay in milliseconds
 */
double Blammo_getflushdelay();

void Blammo_setmaxlines(int lines);		//!< Set maximum logbuffer line count
void Blammo_setmaxlinelength(int length);	//!< Set maximum logbuffer line length
int Blammo_getmaxlines();			//!< Get current maximum logbuffer line count
int Blammo_getmaxlinelength();			//!< Get current maximum logbuffer line length

#if defined __cplusplus
}
#endif //__cplusplus

#endif //BLAMMO_H
