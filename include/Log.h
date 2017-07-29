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
#include "kstring.h"
#include <mutex>

typedef enum
{
	LOG_INFO, // General messages ("hello!")
	LOG_WARN, // warnings ("This config value is unset, using default!")
	LOG_ERR,  // general errors ("Cannot create file: Permission denied.")
	LOG_CRIT  // irrecoverable error ("program encountered a segmentation fault")
} loglevel_t;

class Log
{
	// Our message to print.
	kstring message;
	// Our log level.
	loglevel_t level;
public:
	// Defined for user-defined literal.
	Log(const char *str, size_t len, loglevel_t level) noexcept(false);
	// C++ likes us to have the call operator defined.
	template<typename... Args>
	Log & operator () (const Args&... args) noexcept(false)
	{
		try {
			// Local lock for thread safety.
			extern std::mutex loglock;
			std::unique_lock<std::mutex> lock(loglock);
			this->message = this->message.fmt(args...);
		} catch (const std::system_error &e)
		{
			// Well... something fucked up.
			printf("Caught system error in log class: %s (%d)\n", e.what(), e.code().value());
		}
		return *this;
	}

	// When we actually call the print function.
	~Log() noexcept(false);
};

// Allow for "Here is the value: %s!"_l("the value!"); style logs.
// This is the info/generic operator.
inline Log operator "" _l (const char *str, size_t len) { return Log(str, len, LOG_INFO); }
inline Log operator "" _lw(const char *str, size_t len) { return Log(str, len, LOG_WARN); }
inline Log operator "" _le(const char *str, size_t len) { return Log(str, len, LOG_ERR);  }
inline Log operator "" _lc(const char *str, size_t len) { return Log(str, len, LOG_CRIT); }
