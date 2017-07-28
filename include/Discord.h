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

// Discord API docs: https://discordapp.com/developers/docs/reference
// OpenSSL API docs: https://wiki.openssl.org/index.php/SSL/TLS_Client
// Discord uses a REST api over HTTPS websockets so this should be fun to connect to.
#pragma once
#include "WebSocket.h"

// The final Discord protocol class. This is built on like 4 other damn protocols
// because ofc we have to reinvent the damn internet over HTTP with WebSockets.
// fucking stupid hipster programmers and retards. I mean I get why y'all want
// web sockets but seriously? doing http? why isnt http a fucking dead protocol...
class Discord : public WebSocket
{
public:
	Discord();
	~Discord();
};
