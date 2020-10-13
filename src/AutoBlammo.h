/* AutoBlammo.h
 *
 */

#if !defined AUTOBLAMMO_H
#define AUTOBLAMMO_H

#include "Blammo.h"

/**
 * A statically-allocated class that automatically initializes and deinitializes Blammo
 */
class AutoBlammo {
public:
	AutoBlammo();
	~AutoBlammo();
};

#endif //AUTOBLAMMO_H
