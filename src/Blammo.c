/* Blammo.c
 *
 */

//#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "Blammo.h"
#include <Agnostic/Timing.h>

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
int _logStatus = BLAMMO_UNINIT;	//!< This is used to pause/resume/exit the Blammo_autoflush thread
pthread_t _logFlushThread;	//!< This pthread_t holds the Blammo_autoflush thread

/**
 * @brief Simple thread that calls Blammo_process and then waits a specified amount of time
 * @details If _logStatus is BLAMMO_STOPPED, nothing will be printed.  If _logStatus is BLAMMO_RUNNING,
 * lines will be printed.  If _logStatus is BLAMMO_EXITING, nothing will be printed and this thread will exit.
 */
void* Blammo_autoflush(void* arg) {
/*	while(_logStatus != BLAMMO_EXITING) {
		if(_logStatus == BLAMMO_RUNNING) {
			Blammo_process();
		}
		SleepMsec(_logAutoflushSleep);
	}
*/
	pthread_mutex_lock(&_logMutex);
	int cachedStatus = _logStatus;
	int cachedSleep = _logAutoflushSleep;
	pthread_mutex_unlock(&_logMutex);
	while(cachedStatus != BLAMMO_EXITING) {
		if(cachedStatus == BLAMMO_RUNNING) {
			Blammo_process();
		}
		SleepMsec(cachedSleep);
		pthread_mutex_lock(&_logMutex);
		cachedStatus = _logStatus;
		cachedSleep = _logAutoflushSleep;
		pthread_mutex_unlock(&_logMutex);
	}
#if defined DEBUGEXTRA
	Logger("Blammo_autoflush has begun to exit correctly.");
#endif //DEBUGEXTRA
	pthread_exit((void*)0);
}

void Blammo_createbuffer_unsafe() {
	_logbuffer = malloc(_logMaxLines * sizeof(char**));
	for(int i=0; i<_logMaxLines; i++) {
		_logbuffer[i] = malloc(_logMaxLineLength+1);
		memset(_logbuffer[i], 0, _logMaxLineLength+1);
	}
}

void Blammo_destroybuffer_unsafe() {
	for(int i=0; i<_logMaxLines; i++) {
		free((void*)_logbuffer[i]);
	}
	free((void*)_logbuffer);
}

int Blammo_init() {
	pthread_mutex_init(&_logMutex, NULL);
	pthread_mutex_lock(&_logMutex);
	_logStatus = BLAMMO_STOPPED;
	_logMaxLineLength = DEFAULT_MAXLINELENGTH;
	_logMaxLines = DEFAULT_MAXLINES;
	_logAutoflushSleep = DEFAULT_AUTOFLUSHSLEEP;
	Blammo_createbuffer_unsafe();
	_logCurrentLine = 0;
	_logStatus = BLAMMO_RUNNING;
	pthread_mutex_unlock(&_logMutex);
	pthread_create(&_logFlushThread, NULL, Blammo_autoflush, NULL);
	//pthread_setname_np(_logFlushThread, "LogFlusher");
	return 0;
}

/**
 * @brief Write pending buffered log entries and clear the buffer.
 * @details This function should only be used internally because it does not lock the mutex!
 */
void Blammo_process_unsafe() {
	for(int i=0; i<_logCurrentLine; i++) {
		printf("%s\n", _logbuffer[i]);
		memset(_logbuffer[i], 0, _logMaxLineLength);
	}
//#if defined DEBUGEXTRA
//	printf("Bawooooooosh!\n");
//#endif //DEBUGEXTRA
	_logCurrentLine = 0;
}

void Blammo_process() {
	pthread_mutex_lock(&_logMutex);
	Blammo_process_unsafe();
	pthread_mutex_unlock(&_logMutex);
}

void Blammo_finish() {
	pthread_mutex_lock(&_logMutex);
	_logStatus = BLAMMO_EXITING;
	pthread_mutex_unlock(&_logMutex);
	void* status;
	pthread_join(_logFlushThread, &status);
	pthread_mutex_lock(&_logMutex);
	Blammo_process_unsafe();
	Blammo_destroybuffer_unsafe();
	pthread_mutex_unlock(&_logMutex);
	pthread_mutex_destroy(&_logMutex);
}

/**
 * @brief Add entry to _logbuffer the dangerous way
 * @details Break str into _logMaxLineLength-sized chunks and add them to _logbuffer.  When necessary,
 * flush it with Blammo_process_unsafe.  Internal use only!
 * @param str (const char*) the C-string to add to the log buffer
 * @return int 0 = success, 1 = success and the buffer was emptied after the last line was added
 */
int Blammo_unsafe(const char* str) {
	int retval = 0;
	assert(_logCurrentLine < _logMaxLines);  // if at any point this happens, something is very wrong
	for(int i=0; i<strlen(str); i += _logMaxLineLength) {
		strncpy(_logbuffer[_logCurrentLine], str+i, _logMaxLineLength);
		_logCurrentLine++;
		if(_logCurrentLine >= _logMaxLines) {
			Blammo_process_unsafe();
			retval = 1;
		} else retval = 0;
	}
	return retval;
}

int Logger(const char* str) {
	if(_logStatus != BLAMMO_RUNNING) return -1;
	pthread_mutex_lock(&_logMutex);
	int retval = Blammo_unsafe(str);
	pthread_mutex_unlock(&_logMutex);
	return retval;
}

void Logger_now(const char* str) {
	pthread_mutex_lock(&_logMutex);
	int status = Blammo_unsafe(str);
	// if the log didn't flush, do it now:
	if(status != 1) Blammo_process_unsafe();
	pthread_mutex_unlock(&_logMutex);
}

void Blammo_pause() {
	_logStatus = BLAMMO_STOPPED;
}

void Blammo_unpause() {
	_logStatus = BLAMMO_RUNNING;
}

int Blammo_status() {
	return _logStatus;
}

void Blammo_setflushdelay(double ms) {
	pthread_mutex_lock(&_logMutex);
	if(ms > 0) _logAutoflushSleep = ms;
	pthread_mutex_unlock(&_logMutex);
}

double Blammo_getflushdelay() {
	return _logAutoflushSleep;
}

void Blammo_setmaxlines(int lines) {
	if(lines <= 0 || lines == _logMaxLines) return;
	pthread_mutex_lock(&_logMutex);
	_logStatus = BLAMMO_STOPPED;
	//flush
	Blammo_process_unsafe();
	//destroy
	Blammo_destroybuffer_unsafe();
#if defined DEBUGEXTRA
	printf("_logMaxLines is %d and is about to be %d\n", _logMaxLines, lines);
#endif //DEBUGEXTRA
	_logMaxLines = lines;
	//create
	Blammo_createbuffer_unsafe();
	_logStatus = BLAMMO_RUNNING;
	pthread_mutex_unlock(&_logMutex);
}

void Blammo_setmaxlinelength(int length) {
	if(length <= 0 || length == _logMaxLineLength) return;
	pthread_mutex_lock(&_logMutex);
	_logStatus = BLAMMO_STOPPED;
	//flush
	Blammo_process_unsafe();
	//destroy
	Blammo_destroybuffer_unsafe();
#if defined DEBUGEXTRA
	printf("_logMaxLineLength is %d and is about to be %d\n", _logMaxLineLength, length);
#endif //DEBUGEXTRA
	_logMaxLineLength = length;
	//create
	Blammo_createbuffer_unsafe();
	_logStatus = BLAMMO_RUNNING;
	pthread_mutex_unlock(&_logMutex);
}

int Blammo_getmaxlines() {
	return _logMaxLines;
}

int Blammo_getmaxlinelength() {
	return _logMaxLineLength;
}
