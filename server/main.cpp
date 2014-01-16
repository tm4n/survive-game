#include <string>
#include <sstream>
#include <iostream>


#include "include/gameServer.h"


int main( int argc, char* argv[] )
{
	// start networked server
	gameServer *sv = new gameServer(true);

    delete sv;

    return 0;
}
