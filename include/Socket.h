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
#include "Flux.h"
#include <arpa/inet.h>

typedef union {
	struct sockaddr_in ipv4;
	struct sockaddr_in6 ipv6;
	struct sockaddr sa;
} sockaddr_t;

typedef enum
{
	// Misc socket statuses.
	SS_DEAD       = 1 << 1,
	SS_WRITABLE   = 1 << 2,
	// For connecting sockets
	SS_CONNECTING = 1 << 3,
	SS_CONNECTED  = 1 << 4,
	// For Binding sockets.
	SS_ACCEPTING  = 1 << 5,
	SS_ACCEPTED   = 1 << 6,
	// If we're IPv6 or not.
	SS_ISIPV6     = 1 << 7
} socketstatus_t;

class Socket
{
protected:
	int sock_fd;
	sockaddr_t sa;
public:
	// Delete the default constructor because it causes all kinds of fucking issues.
	Socket() = delete;
	Socket(int sock, bool isipv6 = false, int type = SOCK_STREAM);
	virtual ~Socket();

	// Status of the socket
	flags_t status;

	// Socket Flags
	void SetNonBlocking(bool status);

	// I/O functions, these are overwritten in the sub-class sockets.
	virtual size_t Write(const void *data, size_t len);
	virtual size_t Read(void *data, size_t len);

	// Getters/Setters
	inline int GetFD() { return this->sock_fd; }

	// Interaction with the SocketMultiplexer
	virtual bool MultiplexEvent();
	virtual void MultiplexError();
	virtual bool MultiplexRead();
	virtual bool MultiplexWrite();

	static std::string GetAddress(sockaddr_t saddr);
	static short GetPort(sockaddr_t s);
	static sockaddr_t GetSockAddr(int type, const std::string &addr, int port);
};

class ConnectionSocket : public virtual Socket
{
	std::string address;
	short port;
public:
	ConnectionSocket(bool ipv6);
	virtual ~ConnectionSocket();

	bool MultiplexEvent();
	void MultiplexError();

	void Connect(const std::string &address, short port);

	virtual void OnConnect() = 0;
	virtual void OnError(const std::string &str);
};

class ClientSocket;

class ListeningSocket : public virtual Socket
{
protected:
	std::string address;
	short port;
	bool ipv6;
public:
	ListeningSocket(const std::string &bindaddr, short port, bool ipv6);
	virtual ~ListeningSocket();
	bool MultiplexRead();

	virtual ClientSocket *OnAccept(int fd, const sockaddr_t &addr) = 0;
};


class ClientSocket : public virtual Socket
{
public:
	ListeningSocket *ls;
	ClientSocket(ListeningSocket *ls, int sock_fd, const sockaddr_t &addr, bool ipv6);
	bool MultiplexEvent();
	void MultiplexError();
	virtual void OnAccept();
	virtual void OnError(const std::string &str);
};
