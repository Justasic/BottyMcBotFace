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
#include "SocketMultiplexer.h"
#include <shared_mutex> // obvious
#include <mutex> // for unique_lock
#include <unistd.h> // for close()
#include <cstring> // for memset
#include <sys/epoll.h> // obvious
#include "Log.h"

std::shared_mutex mux;
/**
 * This file is entirely a stub to make sure that the application links correctly.
 * These are actually implemented in the socket engine modules.
 */
 // Well.. I had to give the base class *SOMETHING* as an argument
 // or the compiler whines about the implicit deletion of the
 // default constructor of the Provider class.
SocketMultiplexer::SocketMultiplexer()
{
}

 SocketMultiplexer::~SocketMultiplexer()
 {
	 this->Terminate();
 }

 Socket *SocketMultiplexer::FindSocket(int sock_fd)
 {
	 for (auto it : this->Sockets)
	 {
		 if (it->GetFD() == sock_fd)
		 	return it;
	 }
	 return nullptr;
 }

// Initalizers
void SocketMultiplexer::Initialize()
{
	//Log(LOG_SOCKET) << "Using EPoll Multiplexing engine...";
    "Using EPoll Multiplexing engine..."_l;
	// well... it had to be greater than zero....
	this->epollhandle = epoll_create(9001);

	if (this->epollhandle == -1)
		throw SocketException("Failed to initialize EPoll Multiplexing engine");

	// Allocate some space
	Events.resize(10);
}

void SocketMultiplexer::Terminate()
{
	"EPoll Multiplexer shutting down..."_l;
	close(this->epollhandle);
}

// Sockets interact with these functions.
bool SocketMultiplexer::AddSocket(Socket *s)
{
	std::unique_lock<std::shared_mutex> lck{mux};
	epoll_t ev;
	memset(&ev, 0, sizeof(epoll_t));

	ev.events = EPOLLIN;
	ev.data.fd = s->GetFD();

	// Add to the socket thing.
	this->Sockets.push_back(s);

	if (epoll_ctl(this->epollhandle, EPOLL_CTL_ADD, ev.data.fd, &ev) == -1)
		return false;
	return true;
}

bool SocketMultiplexer::RemoveSocket(Socket *s)
{
	std::unique_lock<std::shared_mutex> lck{mux};
	epoll_t ev;
	memset(&ev, 0, sizeof(epoll_t));
	ev.data.fd = s->GetFD();

	if (auto it = std::find(this->Sockets.begin(), this->Sockets.end(), s); it != this->Sockets.end())
			this->Sockets.erase(it);
	else
			//Log(LOG_WARN) << "Could not find " << s->GetFD() << " in known sockets!";
            "Could not find %d in known sockets!"_l(s->GetFD());

	if (epoll_ctl(this->epollhandle, EPOLL_CTL_DEL, ev.data.fd, &ev))
		return false;
	return true;
}

bool SocketMultiplexer::SetSocketStatus(Socket *s, flags_t status)
{
	// Do not lock a mutex in this. Threads use this to mark sockets as
	// read to write, if we lock a mutex then there will be gridlock in
	// the application over when data is processed.
	epoll_t ev;
	memset(&ev, 0, sizeof(epoll_t));

	ev.events = (status & MX_READABLE ? EPOLLIN : 0) | (status & MX_WRITABLE ? EPOLLOUT : 0);
	ev.data.fd = s->GetFD();
	s->status = status;

	if (epoll_ctl(this->epollhandle, EPOLL_CTL_MOD, ev.data.fd, &ev) == -1)
		return false;
	return true;
}


// This is called in the event loop to slow the program down and process sockets.
void SocketMultiplexer::Multiplex(time_t sleep)
{
		mux.lock();
		errno = 0;
		//Log(LOG_VERBOSE, this->GetModule()) << "[Socket Engine] Entering EPoll Wait...";
        "[Socket Engine] Entering EPoll Wait..."_l;
		// Make sure we have room for epoll events.
		if (this->Sockets.size() > Events.size())
			Events.resize(this->Events.size() * 2);

		// Check for epoll events, wait for 1 second, then return.
		int total = epoll_wait(this->epollhandle, &Events.front(), Events.size(), sleep * 1000);

		if (total == -1)
		{
			if (errno != EINTR)
			{
				//Log(LOG_WARN, this->GetModule()) << "[Socket Engine] EPollMultiplexer::Process() error: " << strerror(errno);
                "[Socket Engine] EPollMultiplexer::Process() error: %s"_l(strerror(errno));
				::sleep(sleep); // prevents infinite loops my terminal has a hard time exiting from .
			}
		}
		mux.unlock();

		for (int i = 0; i < total; ++i)
		{
			mux.lock();
			epoll_t &ev = Events[i];

			Socket *s = this->FindSocket(ev.data.fd);
			if (!s)
			{
				//Log(LOG_VERBOSE, this->GetModule()) << "[Socket Engine] Unknown socket " << ev.data.fd << ", removing from EPoll and ignoring...";
                "[Socket Engine] Unknown socket %d, removing from EPoll and ignoring..."_l(ev.data.fd);
				epoll_ctl(this->epollhandle, EPOLL_CTL_DEL, ev.data.fd, &ev);
				mux.unlock();
				continue;
			}
			mux.unlock();

			if (ev.events & (EPOLLHUP | EPOLLERR))
			{
				s->MultiplexError();
				delete s;
				continue;
			}

			if (!s->MultiplexEvent())
			{
				if (s->status & SS_DEAD)
					delete s;
				continue;
			}

			if ((ev.events & EPOLLIN) && !s->MultiplexRead())
				s->status |= SS_DEAD;

			if ((ev.events & EPOLLOUT) && !s->MultiplexWrite())
				s->status |= SS_DEAD;

			if (s->status & SS_DEAD)
				delete s;
		}
}
