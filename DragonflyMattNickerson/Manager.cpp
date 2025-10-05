#include "Manager.h"
#include <iostream>

int Manager::startUp() {
	if (started) {
		std::cerr << "Manager already started" << std::endl;
		return -1;
	}
	started = true;
	return 0;
}

void Manager::shutDown() {
	started = false;
}

