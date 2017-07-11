#include <cstdio>
#include <cstdlib>
#include "SocketMultiplexer.h"

// Global var, include SocketMultiplexer.h to access it
SocketMultiplexer *mplexer;

int main(int argc, char **argv)
{
	tfm::printf("Hello! :D\n");
	
	SocketMultiplexer m;
	mplexer = &m;

	mplexer->Initialize();

	while (true)
	{
		// Iterate the event loop every 5 seconds unless
		// an event is happening.
		mplexer->Multiplex(5);
	}

	return EXIT_SUCCESS;
};
