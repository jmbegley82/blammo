/*
 *  Blammo-test.cpp
 *  blammo
 *
 *  Created by james on 10/10/20.
 *  Copyright 2020 __MyCompanyName__. All rights reserved.
 *
 */


#include <string>
#include "Blammo.h"

int main(int argc, char** argv) {
	string msg = "Blammo-test:  ";
	Log(msg + "Log test passed!");
	Log(msg + "This line should be made waaaaaay longer to make sure word wrap actually works.  For example, one solution might be to dump the contents of dmesg or ioreg or the entire contents of the GNU General Public License.  Any text of sufficient length should suffice.  The exact length currently escapes me, but I'm really hoping that it is at most 1/2 the length of this text.");
	Log_now(msg + "Fluuuuuush.");
	return 0;
}
