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
#include "Flux.h"
#include "SocketMultiplexer.h"

/**
 * This file is entirely a stub to make sure that the application links correctly.
 * These are actually implemented in the socket engine modules.
 */
 // Well.. I had to give the base class *SOMETHING* as an argument
 // or the compiler whines about the implicit deletion of the
 // default constructor of the Provider class.
SocketMultiplexer::SocketMultiplexer(Module *m) : Provider(m, "SocketMultiplexer", PR_MULTIPLEXER)
{
	// Make sure another Multiplexer isnt already loaded.
	// Multiplexers are always modules and therefore this is a
	// module exception.
	if (ProviderHandler::FindProviders(PR_MULTIPLEXER).size() >= 2)
		throw ModuleException("You cannot load two Multiplexers!");
}

 SocketMultiplexer::~SocketMultiplexer()
 {
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
void SocketMultiplexer::Initialize() {}
void SocketMultiplexer::Terminate()  {}

// Sockets interact with these functions.
bool SocketMultiplexer::AddSocket(Socket *s)    { return false; }
bool SocketMultiplexer::RemoveSocket(Socket *s) { return false; }
bool SocketMultiplexer::SetSocketStatus(Socket *s, flags_t) { return false; }

// This is called in the event loop to slow the program down and process sockets.
void SocketMultiplexer::Multiplex(time_t sleep)
{
}
