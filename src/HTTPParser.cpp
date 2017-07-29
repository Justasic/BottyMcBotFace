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
#include "HTTPParser.h"
#include "Exceptions.h"


// In this constructor we assume that all HTTP data (at least
// all the data from the header and the \r\n is here) is inside
// the managed buffer. As we read and interpret this buffer we fill
// out our class to be more useful.
HTTPData::HTTPData(const ManagedBuffer &httpdata)
{
	// First, start by splitting the header up from content and header.
	// HTTP deliminates header from content with "\r\n\r\n" characters.
	// Finding these character sequences will tell us how long the header
	// is and how much data to add in. Everything after those bytes is just
	// copied into the other managed buffer for "content". Since we're
	// looking for those bytes, we have a buffer of only 4 bytes so we
	// examine the stream at 4 bytes each.
	const char findstr[] = { '\r', '\n', '\r', '\n' };
	char buffer[sizeof(findstr)] = {0};
	size_t cur = sizeof(findstr); // curser position.

	memcpy(buffer, findstr, sizeof(findstr));
	// Evaluate each 4 bytes, copy 4 more bytes if true.
	while(memcmp(buffer, findstr, sizeof(findstr)) != 0)
	{
		// Make sure we don't buffer overflow.
		size_t copy = std::min(sizeof(findstr), (httpdata.size() - cur));
		memcpy(buffer, *httpdata, copy);
		cur += copy;
	}

	// We didn't find shit, better blow up.
	if (cur == httpdata.size())
		throw BasicException("FIXME: throw invalid data exception in HTTPData constructor");

	// Tell kstring that our string is only as big as our curser to truncate it.
	kstring httpheadersstr = kstring(reinterpret_cast<const char*>(*httpdata), cur);
	// tokenize our string into individual lines.
	kvector httpheaderlines = httpheadersstr.expand("\r\n");
	// tokenize to a map.
	for (auto &it : httpheaderlines)
	{
		auto items = it.expand(":");
		this->header[items[0]] = items[1];
	}

	// Now copy all the rest of the data (aka. "content") to the content variable.
	size_t cpysize = httpdata.size() - cur;
	this->content.AllocateAhead(cpysize);
	// Dirty hack but whatever.
	memcpy(*this->content, reinterpret_cast<uint8_t*>(*httpdata) + cur, cpysize);
}

HTTPData::HTTPData(const HTTPData &other)
{
	this->header = other.header;
	this->status = other.status;

	// Copy content.
	this->content.AllocateAhead(other.content.size());
	memcpy(*this->content, *other.content, this->content.size());
}

HTTPData::~HTTPData()
{
}

kstring HTTPData::GetField(const kstring &str) const
{
	for (auto&& [key, value] : this->header)
	{
		if (key == str)
			return value;
	}
	return "";
}
