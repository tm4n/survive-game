#ifndef ANDROID

#include <string>
#include <sstream>
#include <iostream>


#include "gameServer.h"


int main( int argc, char* argv[] )
{
	// start networked server
	gameServer *sv = new gameServer();
	
	sv->run();

    delete sv;

    return 0;
}

#endif