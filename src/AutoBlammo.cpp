/* StaticLogger.cpp
 *
 */

#include "AutoBlammo.h"
#include "Blammo.h"

static StaticLogger _staticlogger;

StaticLogger::StaticLogger() {
	Logger_init();
	Logger("StaticLogger initialized!");
}

StaticLogger::~StaticLogger() {
	Logger("StaticLogger exiting.");
	Logger_finish();
}
