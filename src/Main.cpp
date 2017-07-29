/*************************************************************************
 * BSD 2-Clause License
 *
 * Copyright (c) 2017, Justin Crawford
 * Copyright (c) 2017, William Haugen
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <cstdio>
#include <cstdlib>
#include "SocketMultiplexer.h"
#include "json.hpp"
#include "ThreadEngine.h"
#include "TimerReactor.h"
#include "Log.h"

// Global var, include SocketMultiplexer.h to access it
SocketMultiplexer *mplexer;
ThreadHandler *thread;
TimerReactor *reactor;

int main(int argc, char **argv)
{
	try
	{
		"Hello World! :D"_l;

		// Initialize the thread engine first.
		ThreadHandler engine;
		engine.Initialize();
		thread = &engine;

		// Now that our thread engine is up, initialize
		// the timer reactor.
		TimerReactor tr;
		reactor = &tr;

		// Now initialize the socket engine.
		SocketMultiplexer m;
		mplexer = &m;

		mplexer->Initialize();

		while (true)
		{
			// Iterate the event loop every 5 seconds unless
			// an event is happening.
			mplexer->Multiplex(5);
		}
	} catch (const BasicException &ex)
	{
		"Basic Exception was caught at highest level of the application with the following error message: %s"_lc(ex.what());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
};
