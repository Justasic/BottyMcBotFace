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
#pragma once
#include "HTTPParser.h"
#include "Socket.h"

enum WebSocketOpcode_t
{
	// Continuation frame
	WSFRAME_CONT = 0x0,
	// Non-control frames
	WSFRAME_TEXT = 0x1,
	WSFRAME_DATA = 0x2,
	// Extras we accept but ignore.
	WSFRAME_UNUSED3 = 0x3,
	WSFRAME_UNUSED4 = 0x4,
	WSFRAME_UNUSED5 = 0x5,
	WSFRAME_UNUSED6 = 0x6,
	WSFRAME_UNUSED7 = 0x7,
	// Control frames
	WSFRAME_CLOSED  = 0x8,
	WSFRAME_PING    = 0x9,
	WSFRAME_PONG    = 0xA,
	// Unused futures we ignore
	WSFRAME_CONTR_B = 0xB,
	WSFRAME_CONTR_C = 0xC,
	WSFRAME_CONTR_D = 0xD,
	WSFRAME_CONTR_E = 0xE,
	WSFRAME_CONTR_F = 0xF,
};

//      0                   1                   2                   3
//      0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//     +-+-+-+-+-------+-+-------------+-------------------------------+
//     |F|R|R|R| opcode|M| Payload len |    Extended payload length    |
//     |I|S|S|S|  (4)  |A|     (7)     |             (16/64)           |
//     |N|V|V|V|       |S|             |   (if payload len==126/127)   |
//     | |1|2|3|       |K|             |                               |
//     +-+-+-+-+-------+-+-------------+ - - - - - - - - - - - - - - - +
//     |     Extended payload length continued, if payload len == 127  |
//     + - - - - - - - - - - - - - - - +-------------------------------+
//     |                               |Masking-key, if MASK set to 1  |
//     +-------------------------------+-------------------------------+
//     | Masking-key (continued)       |          Payload Data         |
//     +-------------------------------- - - - - - - - - - - - - - - - +
//     :                     Payload Data continued ...                :
//     + - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +
//     |                     Payload Data continued ...                |
//     +---------------------------------------------------------------+

typedef struct 
{
	// boolean indicating this is the final fragment of the message.
	uint8_t FIN  : 1;
	// Extensions (must be configured ahead of time or terminate the socket)
	uint8_t RSV1 : 1;
	uint8_t RSV2 : 1;
	uint8_t RSV3 : 1;
	// The type of payload (see rfc)
	uint8_t opcode : 4;

	// Boolean indicating that we have masked data
	uint8_t mask : 1;
	// Payload length
	uint8_t length : 7;
	// We cannot continue to define the structure in C++ but in memory it will continue as follows:
	//
	// If length is 126, use a uint16, if 127 use 64-bit int. This is payload length.
	// if (this->length == 126)
	// 	uint16_t length;
	// else if(this->length == 127)
	// 	uint64_t length;
	//
	// if (this->mask)
	// 	uint32_t maskkey;
	//
	// Payload data is defined as being extension data and application data.
	// struct payload
	// {
	// 		Length of extension data is 0 unless negotiated.
	// 		void *extensions;
	// 		Length of application data is (sizeof(*this) - len(this->payload.extensions))
	// 		void *application;
	// }
	//
	// This is now the end of the websocket frame
} wsframe_t;

class WebSocket : public SecureBufferedSocket
{
public:
	WebSocket();
	~WebSocket();

	// Handle connecting.

	// 
	// WebSocket Connection Control functions
	void SendPing();
	void SendPong();
	void ConnectionClosed();
};
