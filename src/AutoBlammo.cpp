/* AutoBlammo.cpp
 *
 */

#include "AutoBlammo.h"
#include "Blammo.h"

static AutoBlammo _autoblammo;

AutoBlammo::AutoBlammo() {
	Blammo_init();
	Logger("AutoBlammo initialized!");
}

AutoBlammo::~AutoBlammo() {
	Logger("AutoBlammo exiting.");
	Blammo_finish();
}
