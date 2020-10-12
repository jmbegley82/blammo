/* StaticLogger.cpp
 *
 */

#include "StaticLogger.h"
#include "Logger.h"

static StaticLogger _staticlogger;

StaticLogger::StaticLogger() {
	Logger_init();
	Logger("StaticLogger initialized!");
}

StaticLogger::~StaticLogger() {
	Logger("StaticLogger exiting.");
	Logger_finish();
}
