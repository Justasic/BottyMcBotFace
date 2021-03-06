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
#include "Log.h"
#include <unistd.h>
#include <mutex>

// See https://stackoverflow.com/a/38237385
//constexpr const char* str_end(const char *str) { return *str ? str_end(str + 1) : str; }
//constexpr bool str_slant(const char *str) { return *str == '/' ? true : (*str ? str_slant(str + 1) : false); }
//constexpr const char* r_slant(const char* str) { return *str == '/' ? (str + 1) : r_slant(str - 1); }
//constexpr const char* file_name(const char* str) { return str_slant(str) ? r_slant(str_end(str)) : str; }

std::mutex loglock;

const char levelstrings[][10] = {
	"INFO",    // LOG_INFO
	"WARNING", // LOG_WARN
	"ERROR",   // LOG_ERR
	"CRITICAL" // LOG_CRIT
};

Log::Log(const char *str, size_t len, loglevel_t level) noexcept(false): level(level)
{
	this->message = kstring(str, len);
}

Log::~Log() noexcept(false)
{
	try {
		std::unique_lock<std::mutex> lock(loglock);
		if (this->message.isnull() || this->message.empty())
			return;

		tfm::printf("[%d %d %d] %s: %s\n" , time(nullptr), getpid(), getuid(), levelstrings[this->level], this->message);
	} catch(const std::system_error &e)
	{
		printf("Received a system error exception in log class: %s (%d)\n", e.what(), e.code().value());
	}
	catch (const std::exception &e)
	{
		printf("Received a general exception in the log class: %s\n", e.what());
	}
}
