/*************************************************************************
 * BSD 2-Clause License
 *
 * Copyright (c) 2017, Justin Crawford
 * Copyright (c) 2017, Billy Haugen
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
#pragma once
#include "Socket.h"
#include <list>
#include <vector>

// So we know what statuses we have for whether a socket is writable or readable.
enum
{
	MX_WRITABLE = 1,
	MX_READABLE = 2
};
// Easy of use
typedef struct epoll_event epoll_t;

class SocketMultiplexer
{
protected:
	// Used for finding sockets as well as handling other things
	// like initialization of sockets.
	std::list<Socket*> Sockets;
	// EPoll specific
	int epollhandle;
	std::vector<epoll_t> Events;
public:
	SocketMultiplexer();
	~SocketMultiplexer();

	// Initalizers
	void Initialize();
	void Terminate();

	// Sockets interact with these functions.
	bool AddSocket(Socket *s);
	bool RemoveSocket(Socket *s);
	Socket *FindSocket(int sock_fd);
	bool SetSocketStatus(Socket *s, flags_t);

	// This is called in the event loop to slow the program down and process sockets.
	void Multiplex(time_t sleep);
};

// Global variable.
extern SocketMultiplexer *mplexer;
