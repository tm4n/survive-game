#ifndef ANDROID

#include <string>
#include <sstream>
#include <iostream>
#include <string.h>
#include "helper.h"

#include "gameServer.h"


int main( int argc, char* argv[] )
{
	// parse input arguments
	for (int i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], "--debug") == 0) log_output_level = 0;
	}
	
	// start networked server
	gameServer *sv = new gameServer();
	
	sv->run();

    delete sv;

    return 0;
}

#endif
