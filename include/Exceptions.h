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
#include <exception>
#include <string>
#include "tinyformat.h"

class BasicException : public std::exception
{
protected:
	const std::string err;
public:
	BasicException(const std::string &message) : err(message) {}

	// Formatted constructor for ease of use,
	template<typename... Args> BasicException(const std::string &message, const Args&... args) : err(tfm::format(message, args...)) { }

	virtual ~BasicException() throw() { };

	virtual const char* what() const noexcept
	{
		return this->err.c_str();
	}
};

class SQLException : public BasicException
{
	const std::string query;
public:
	SQLException(const std::string &mstr, const std::string &query = "") : BasicException(mstr), query(query)
	{
	}

	template<typename... Args> SQLException(const std::string &message, const std::string &query, const Args&... args) : BasicException(message, args...), query(query) { }

	~SQLException()
	{
	}

	const char *Query() const noexcept
	{
		return this->query.c_str();
	}

};

class ModuleException : public BasicException
{
	const std::string module;
public:
	ModuleException(const std::string &err, const std::string &module) : BasicException(err), module(module) {}
	ModuleException(const std::string &err) : BasicException(err), module("unknown") {}
	template<typename... Args> ModuleException(const std::string &message, const std::string &module, const Args&... args) : BasicException(message, args...), module(module) { }

	const char *Module() const noexcept
	{
		return this->module.c_str();
	}
};

class SocketException : public BasicException
{
public:
	SocketException(const std::string &err) : BasicException(err) { }
	template<typename... Args> SocketException(const std::string &message, const Args&... args) : BasicException(message, args...) { }

};

class DNSException : public BasicException
{
public:
	DNSException(const std::string &err) : BasicException(err) { }
	template<typename... Args> DNSException(const std::string &message, const Args&... args) : BasicException(message, args...) { }
};
