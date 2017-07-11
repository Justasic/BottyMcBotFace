/*************************************************************************
 *
 * CONFIDENTIAL
 * __________________
 *
 *  2017 Justin Crawford
 *  All Rights Reserved.
 *
 * NOTICE:  All information contained herein is, and remains
 * the property of Justin Crawford, The intellectual and technical
 * concepts contained herein are proprietary to Justin Crawford
 * and his suppliers and may be covered by U.S. and Foreign Patents,
 * patents in process, and are protected by trade secret or copyright law.
 * Dissemination of this information or reproduction of this material
 * is strictly forbidden unless prior written permission is obtained
 * from Justin Crawford.
 */
/* RequireFunction: epoll_wait */
#include <Nitrogen.h>
#include <sys/epoll.h>

typedef struct epoll_event epoll_t;
std::shared_mutex mux;

class EPollMultiplexer : public SocketMultiplexer
{
	int epollhandle;
	std::vector<epoll_t> Events;
public:
	EPollMultiplexer(Module *m) : Provider(m, "EPOLL", PR_MULTIPLEXER), SocketMultiplexer(m), epollhandle(-1)
	{
	

	// Initalizers
	void Initialize() override
	{
		Log(LOG_SOCKET) << "Using EPoll Multiplexing engine...";
		// well... it had to be greater than zero....
		this->epollhandle = epoll_create(9001);

		if (this->epollhandle == -1)
			throw SocketException("Failed to initialize EPoll Multiplexing engine");

		// Allocate some space
		Events.resize(10);
	}

	void Terminate() override
	{
		Log(LOG_SOCKET) << "EPoll Multiplexer shutting down...";
		close(this->epollhandle);
	}

	// Sockets interact with these functions.
	bool AddSocket(Socket *s) override
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

	bool RemoveSocket(Socket *s) override
	{
		std::unique_lock<std::shared_mutex> lck{mux};
		epoll_t ev;
		memset(&ev, 0, sizeof(epoll_t));
		ev.data.fd = s->GetFD();

		auto it = std::find(this->Sockets.begin(), this->Sockets.end(), s);
		if(it != this->Sockets.end())
				this->Sockets.erase(it);
		else
				Log(LOG_WARN) << "Could not find " << s->GetFD() << " in known sockets!";

		if (epoll_ctl(this->epollhandle, EPOLL_CTL_DEL, ev.data.fd, &ev))
			return false;
		return true;
	}

	bool SetSocketStatus(Socket *s, flags_t status) override
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
	void Multiplex(time_t sleep) override
	{
		mux.lock();
		errno = 0;
		Log(LOG_VERBOSE, this->GetModule()) << "[Socket Engine] Entering EPoll Wait...";
		// Make sure we have room for epoll events.
		if (this->Sockets.size() > Events.size())
			Events.resize(this->Events.size() * 2);

		// Check for epoll events, wait for 1 second, then return.
		int total = epoll_wait(this->epollhandle, &Events.front(), Events.size(), sleep * 1000);

		if (total == -1)
		{
			if (errno != EINTR)
			{
				Log(LOG_WARN, this->GetModule()) << "[Socket Engine] EPollMultiplexer::Process() error: " << strerror(errno);
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
				Log(LOG_VERBOSE, this->GetModule()) << "[Socket Engine] Unknown socket " << ev.data.fd << ", removing from EPoll and ignoring...";
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
};

class EPollModule : public Module
{
	EPollMultiplexer epoll;
public:
	EPollModule(const std::string &Name) : Module(Name, MOD_SOCKETENGINE), epoll(this)
	{
		this->SetAuthor("Justin Crawford");
		this->SetDescription("The EPoll socket multiplexer");
		this->SetVersion(VERSION_FULL);
		epoll.Initialize();
	}

	~EPollModule()
	{
		epoll.Terminate();
	}
};

REGISTER_MODULE(EPollModule)
