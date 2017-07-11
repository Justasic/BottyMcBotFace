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
#pragma once
#include "Socket.h"
#include "Provider.h"
#include <list>

// So we know what statuses we have for whether a socket is writable or readable.
enum
{
	MX_WRITABLE = 1,
	MX_READABLE = 2
};

class SocketMultiplexer : public virtual Provider
{
protected:
	// Used for finding sockets as well as handling other things
	// like initialization of sockets.
	std::list<Socket*> Sockets;
public:
	SocketMultiplexer(Module *m);
	virtual ~SocketMultiplexer();

	// Initalizers
	virtual void Initialize();
	virtual void Terminate();

	// Sockets interact with these functions.
	virtual bool AddSocket(Socket *s);
	virtual bool RemoveSocket(Socket *s);
	Socket *FindSocket(int sock_fd);
	virtual bool SetSocketStatus(Socket *s, flags_t);

	// This is called in the event loop to slow the program down and process sockets.
	virtual void Multiplex(time_t sleep);
};
