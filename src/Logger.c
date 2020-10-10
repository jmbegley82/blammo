/* Logger.c
 *
 */

//#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "Logger.h"
#include "Timing.h"

#if !defined DEFAULT_MAXLINELENGTH
//! The default maximum length of a logbuffer entry
#define DEFAULT_MAXLINELENGTH 128
#endif //DEFAULT_MAXLINELENGTH

#if !defined DEFAULT_MAXLINES
//! The default maximum number of logbuffer entries
#define DEFAULT_MAXLINES 128
#endif //DEFAULT_MAXLINES

#if !defined DEFAULT_AUTOFLUSHSLEEP
//! The default number of milliseconds to sleep after a timed automatic flush of the buffer
#define DEFAULT_AUTOFLUSHSLEEP 1000
#endif

pthread_mutex_t _logMutex;	//!< This mutex is used to cover _logbuffer and _logCurrentLine
char** _logbuffer;		//!< This buffer holds our log entries
int _logMaxLineLength;
int _logMaxLines;
int _logCurrentLine;		//!< This index represents the next entry in _logbuffer that will be written
int _logAutoflushSleep;
int _logStatus = LOG_UNINIT;	//!< This is used to pause/resume/exit the Logger_autoflush thread
pthread_t _logFlushThread;	//!< This pthread_t holds the Logger_autoflush thread

/**
 * @brief Simple thread that calls Logger_process and then waits a specified amount of time
 * @details If _logStatus is LOG_STOPPED, nothing will be printed.  If _logStatus is LOG_RUNNING,
 * lines will be printed.  If _logStatus is LOG_EXITING, nothing will be printed and this thread will exit.
 */
void* Logger_autoflush(void* arg) {
/*	while(_logStatus != LOG_EXITING) {
		if(_logStatus == LOG_RUNNING) {
			Logger_process();
		}
		SleepMsec(_logAutoflushSleep);
	}
*/
	pthread_mutex_lock(&_logMutex);
	int cachedStatus = _logStatus;
	int cachedSleep = _logAutoflushSleep;
	pthread_mutex_unlock(&_logMutex);
	while(cachedStatus != LOG_EXITING) {
		if(cachedStatus == LOG_RUNNING) {
			Logger_process();
		}
		SleepMsec(cachedSleep);
		pthread_mutex_lock(&_logMutex);
		cachedStatus = _logStatus;
		cachedSleep = _logAutoflushSleep;
		pthread_mutex_unlock(&_logMutex);
	}
#if defined DEBUGEXTRA
	Logger("Logger_autoflush has begun to exit correctly.");
#endif //DEBUGEXTRA
	pthread_exit((void*)0);
}

void Logger_createbuffer_unsafe() {
	_logbuffer = malloc(_logMaxLines * sizeof(char**));
	for(int i=0; i<_logMaxLines; i++) {
		_logbuffer[i] = malloc(_logMaxLineLength+1);
		memset(_logbuffer[i], 0, _logMaxLineLength+1);
	}
}

void Logger_destroybuffer_unsafe() {
	for(int i=0; i<_logMaxLines; i++) {
		free((void*)_logbuffer[i]);
	}
	free((void*)_logbuffer);
}

int Logger_init() {
	pthread_mutex_init(&_logMutex, NULL);
	pthread_mutex_lock(&_logMutex);
	_logStatus = LOG_STOPPED;
	_logMaxLineLength = DEFAULT_MAXLINELENGTH;
	_logMaxLines = DEFAULT_MAXLINES;
	_logAutoflushSleep = DEFAULT_AUTOFLUSHSLEEP;
	Logger_createbuffer_unsafe();
	_logCurrentLine = 0;
	_logStatus = LOG_RUNNING;
	pthread_mutex_unlock(&_logMutex);
	pthread_create(&_logFlushThread, NULL, Logger_autoflush, NULL);
	//pthread_setname_np(_logFlushThread, "LogFlusher");
	return 0;
}

/**
 * @brief Write pending buffered log entries and clear the buffer.
 * @details This function should only be used internally because it does not lock the mutex!
 */
void Logger_process_unsafe() {
	for(int i=0; i<_logCurrentLine; i++) {
		printf("%s\n", _logbuffer[i]);
		memset(_logbuffer[i], 0, _logMaxLineLength);
	}
//#if defined DEBUGEXTRA
//	printf("Bawooooooosh!\n");
//#endif //DEBUGEXTRA
	_logCurrentLine = 0;
}

void Logger_process() {
	pthread_mutex_lock(&_logMutex);
	Logger_process_unsafe();
	pthread_mutex_unlock(&_logMutex);
}

void Logger_finish() {
	pthread_mutex_lock(&_logMutex);
	_logStatus = LOG_EXITING;
	pthread_mutex_unlock(&_logMutex);
	void* status;
	pthread_join(_logFlushThread, &status);
	pthread_mutex_lock(&_logMutex);
	Logger_process_unsafe();
	Logger_destroybuffer_unsafe();
	pthread_mutex_unlock(&_logMutex);
	pthread_mutex_destroy(&_logMutex);
}

/**
 * @brief Add entry to _logbuffer the dangerous way
 * @details Break str into _logMaxLineLength-sized chunks and add them to _logbuffer.  When necessary,
 * flush it with Logger_process_unsafe.  Internal use only!
 * @param str (const char*) the C-string to add to the log buffer
 * @return int 0 = success, 1 = success and the buffer was emptied after the last line was added
 */
int Logger_unsafe(const char* str) {
	int retval = 0;
	assert(_logCurrentLine < _logMaxLines);  // if at any point this happens, something is very wrong
	for(int i=0; i<strlen(str); i += _logMaxLineLength) {
		strncpy(_logbuffer[_logCurrentLine], str+i, _logMaxLineLength);
		_logCurrentLine++;
		if(_logCurrentLine >= _logMaxLines) {
			Logger_process_unsafe();
			retval = 1;
		} else retval = 0;
	}
	return retval;
}

int Logger(const char* str) {
	if(_logStatus != LOG_RUNNING) return -1;
	pthread_mutex_lock(&_logMutex);
	int retval = Logger_unsafe(str);
	pthread_mutex_unlock(&_logMutex);
	return retval;
}

void Logger_now(const char* str) {
	pthread_mutex_lock(&_logMutex);
	int status = Logger_unsafe(str);
	// if the log didn't flush, do it now:
	if(status != 1) Logger_process_unsafe();
	pthread_mutex_unlock(&_logMutex);
}

void Logger_pause() {
	_logStatus = LOG_STOPPED;
}

void Logger_unpause() {
	_logStatus = LOG_RUNNING;
}

int Logger_status() {
	return _logStatus;
}

void Logger_setflushdelay(double ms) {
	pthread_mutex_lock(&_logMutex);
	if(ms > 0) _logAutoflushSleep = ms;
	pthread_mutex_unlock(&_logMutex);
}

double Logger_getflushdelay() {
	return _logAutoflushSleep;
}

void Logger_setmaxlines(int lines) {
	if(lines <= 0 || lines == _logMaxLines) return;
	pthread_mutex_lock(&_logMutex);
	_logStatus = LOG_STOPPED;
	//flush
	Logger_process_unsafe();
	//destroy
	Logger_destroybuffer_unsafe();
#if defined DEBUGEXTRA
	printf("_logMaxLines is %d and is about to be %d\n", _logMaxLines, lines);
#endif //DEBUGEXTRA
	_logMaxLines = lines;
	//create
	Logger_createbuffer_unsafe();
	_logStatus = LOG_RUNNING;
	pthread_mutex_unlock(&_logMutex);
}

void Logger_setmaxlinelength(int length) {
	if(length <= 0 || length == _logMaxLineLength) return;
	pthread_mutex_lock(&_logMutex);
	_logStatus = LOG_STOPPED;
	//flush
	Logger_process_unsafe();
	//destroy
	Logger_destroybuffer_unsafe();
#if defined DEBUGEXTRA
	printf("_logMaxLineLength is %d and is about to be %d\n", _logMaxLineLength, length);
#endif //DEBUGEXTRA
	_logMaxLineLength = length;
	//create
	Logger_createbuffer_unsafe();
	_logStatus = LOG_RUNNING;
	pthread_mutex_unlock(&_logMutex);
}

int Logger_getmaxlines() {
	return _logMaxLines;
}

int Logger_getmaxlinelength() {
	return _logMaxLineLength;
}
