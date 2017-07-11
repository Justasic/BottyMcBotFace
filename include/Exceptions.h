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
#include <exception>
#include <string>

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
