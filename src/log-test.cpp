/*
 *  log-test.cpp
 *  blammo
 *
 *  Created by james on 10/10/20.
 *  Copyright 2020 __MyCompanyName__. All rights reserved.
 *
 */


#include <string>
#include "Logger.h"

int main(int argc, char** argv) {
	Logger_init();
	string msg = "log-test:  ";
	Log(msg + "Log test passed!");
	Log(msg + "This line should be made waaaaaay longer to make sure word wrap actually works.");
	Log_now(msg + "Fluuuuuush.");
	Logger_finish();
	return 0;
}