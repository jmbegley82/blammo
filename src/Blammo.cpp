/* Blammo.cpp
 *
 */

#include <string>
#include "Blammo.h"

using std::string;

int Log(const string& str) {
	return Logger(str.c_str());
}

int Log_now(const string& str) {
	Logger_now(str.c_str());
	return 0;
}
