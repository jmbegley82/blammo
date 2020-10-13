/* StaticLogger.h
 *
 */

#include "Blammo.h"

/**
 * A statically-allocated class that automatically initializes and deinitializes Logger
 */
class StaticLogger {
public:
	StaticLogger();
	~StaticLogger();
};
