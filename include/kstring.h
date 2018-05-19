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

//William Haugen
//
//The purpose of this class, kstring, is to overload the standard
//cstring library's operators to preform deep copies and have
//it act like it is part of the standard library.

#include <cstring>
#include <cctype>
#include <iostream>
#include <map>
#include <strings.h>
#include <vector>
#include "tinyformat.h"


//TO DO:
//Length to spit out len
//Change char at a given index
class kstring
{
	public:
		// Define npos.
		static const size_t npos = -1;

		// Standard constructors
		kstring();
		kstring(const kstring &);
		kstring(const char *, size_t);
		kstring(const char *);
		kstring(const char);
		kstring(const std::string &str);

		// Numerical conversions
		explicit kstring(int i);
		explicit kstring(long int i);
		explicit kstring(long long int i);
		explicit kstring(unsigned int i);
		explicit kstring(unsigned long int i);
		explicit kstring(unsigned long long int i);
		explicit kstring(float i);
		explicit kstring(double i);
		explicit kstring(long double i);

		// Destructor
		~kstring();

		// TODO: Implement these -- Remove inline once finished.
		inline char *data() {}
		inline char &front() {}
		inline const char &front() const {}
		inline char &back() {}
		inline const char &back() const {}
		inline void reserve(size_t sz) {}
		inline size_t capacity() const {}
		inline void shrink_to_fit() {}
		// TODO: All insert() functions.
		inline kstring &erase(size_t index = 0, size_t count = kstring::npos) {}
		inline void push_back(char ch) {}
		inline void pop_back() { this->erase(this->size()-1, 1); }
		// TODO: all append() functions.
		// TODO: all operator += functions.
		// TODO: all compare() functions.
		inline bool starts_with(kstring x) const noexcept {}
		inline bool starts_with(char x) const noexcept {}
		inline bool starts_with(const char *x) const {}
		inline bool ends_with(kstring x) const noexcept {}
		inline bool ends_with(char x) const noexcept {}
		inline bool ends_with(const char *x) const {}
		// TODO: all replace() functions.
		kstring substr(size_t = 0, size_t = npos) const;
		inline void resize(size_t count) {}
		inline void resize(size_t count, char ch) {}
		inline void swap(kstring &other) {}

		size_t copy(char *dest, size_t count, size_t pos = 0) const;
		// Make a deep copy of the string (aka, allocate new memory and copy the entire string byte-by-byte)
		kstring deepcopy();

		// a timing-safe compaison of strings (used for passwords)
		bool securecmp(const kstring &);

		// Vector functions (useful for tokenization)
		std::vector<kstring> expand(const kstring &delim) const;
		kstring contract(const std::vector<kstring> &_vec, const kstring &delim);

		// Make use of tinyformat here.
		template<typename... Args>
		kstring fmt(const Args&... args) { return tfm::format(this->c_str(), args...); }

		// Search functions
		size_t find(const kstring &, size_t = 0) const;
		inline size_t find(const char *s, size_t pos, size_t count) const {}
		inline size_t find(const char *s, size_t pos = 0) const {}
		inline size_t find(char ch, size_t pos = 0) const {}

		// Reverse find
		inline size_t rfind(const kstring &, size_t = 0) const {}
		inline size_t rfind(const char *s, size_t pos, size_t count) const {}
		inline size_t rfind(const char *s, size_t pos = 0) const {}
		inline size_t rfind(char ch, size_t pos = 0) const {}

		inline size_t find_first_of(const kstring &str, size_t pos = 0) const {}
		inline size_t find_first_of(const char *s, size_t pos, size_t count) const {}
		inline size_t find_first_of(const char *s, size_t pos = 0) const {}
		inline size_t find_first_of(char ch, size_t pos = 0) const {}

		inline size_t find_first_not_of(const kstring &str, size_t pos = 0) const {}
		inline size_t find_first_not_of(const char *s, size_t pos, size_t count) const {}
		inline size_t find_first_not_of(const char *s, size_t pos = 0) const {}
		inline size_t find_first_not_of(char ch, size_t pos = 0) const {}

		inline size_t find_last_of(const kstring &str, size_t pos = 0) const {}
		inline size_t find_last_of(const char *s, size_t pos, size_t count) const {}
		inline size_t find_last_of(const char *s, size_t pos = 0) const {}
		inline size_t find_last_of(char ch, size_t pos = 0) const {}

		inline size_t find_last_not_of(const kstring &str, size_t pos = 0) const {}
		inline size_t find_last_not_of(const char *s, size_t pos, size_t count) const {}
		inline size_t find_last_not_of(const char *s, size_t pos = 0) const {}
		inline size_t find_last_not_of(char ch, size_t pos = 0) const {}

		//Assignment Operators
		kstring & operator= (const kstring &);
		kstring & operator= (const std::string &);
		kstring & operator= (const char *);
		kstring & operator= (const char);
		kstring & operator+= (const std::string &);
		kstring & operator+= (const char *);
		kstring & operator+= (const char);
		kstring & operator+= (const kstring &s2);

		//Able to use std::ostream and std::istream natively
		friend std::ostream & operator<< (std::ostream &, const kstring&);
		friend std::istream & operator>> (std::istream &, kstring&);

		//Ability to add onto strings, just uses std::strcat
		friend kstring operator+ (const kstring &, char *);
		friend kstring operator+ (char *, const kstring&);
		friend kstring operator+ (const kstring &, const char *);
		friend kstring operator+ (const char *, const kstring&);
		friend kstring operator+ (const kstring&, const kstring&);


		//Using strcmp to determine all the relations.
		friend bool operator < (const kstring &, char *);
		friend bool operator < (char *, const kstring &);
		friend bool operator < (const kstring &, const kstring &);

		friend bool operator <= (const kstring &, char *);
		friend bool operator <= (char *, const kstring &);
		friend bool operator <= (const kstring &, const kstring &);

		friend bool operator > (const kstring &, char *);
		friend bool operator > (char *, const kstring &);
		friend bool operator > (const kstring &, const kstring &);

		friend bool operator >= (const kstring &, char *);
		friend bool operator >= (char *, const kstring &);
		friend bool operator >= (const kstring &, const kstring &);

		friend bool operator != (const kstring &, char *);
		friend bool operator != (char *, const kstring &);
		friend bool operator != (const kstring &, const kstring &);

		friend bool operator == (const kstring &, char *);
		friend bool operator == (char *, const kstring &);
		friend bool operator == (const kstring &, const kstring &);

		//READ ONLY for the subscript
		char & operator [] (int);
		const char & operator [] (int) const;

		// Getters/Setters
		inline size_t size() const { return this->isnull() ? 0 : this->str->length; }
		inline const char *c_str() const { return this->isnull() ? nullptr : this->str->str; }
		inline bool isnull() const { return !this->str; }
		inline bool empty() const { return this->isnull() ? true : !this->str->length; }
		inline void clear() { if (this->str) { bzero(this->str->str, this->str->allocatedsz); this->str->length = 0; } }

		// Casting operators
		inline explicit operator int()                    { return this->isnull() ? 0 : static_cast<int>(strtol(this->str->str, nullptr, 10)); }
		inline explicit operator long int()               { return this->isnull() ? 0 : strtol(this->str->str, nullptr, 10); }
		inline explicit operator long long int()          { return this->isnull() ? 0 : strtoll(this->str->str, nullptr, 10); }
		// C/C++ does not have a strtou function cuz apparently being consistent isn't top
		// priority for ISOCPP group. So we have to do a truncating cast from unsigned long.
		inline explicit operator unsigned int()           { return this->isnull() ? 0 : static_cast<unsigned int>(strtoul(this->str->str, nullptr, 10)); }
		inline explicit operator unsigned long int()      { return this->isnull() ? 0 : strtoul(this->str->str, nullptr, 10); }
		inline explicit operator unsigned long long int() { return this->isnull() ? 0 : strtoull(this->str->str, nullptr, 10); }
		inline explicit operator float()                  { return this->isnull() ? 0.0 : strtof(this->str->str, nullptr); }
		inline explicit operator double()                 { return this->isnull() ? 0.0 : strtod(this->str->str, nullptr); }
		inline explicit operator long double()            { return this->isnull() ? 0.0 : strtold(this->str->str, nullptr); }

	private:

		// This structure should never be exposed to the user!
		typedef struct
		{
			// We reference count to dynamically expand the buffer, this is
			// also why we require an internal object.
			size_t refs;        // Used for reference counting
			size_t allocatedsz; // Used to get allocated size
			size_t length;      // Length of the string
			// Okay, this takes some explaining. I got this trick from
			// https://stackoverflow.com/a/599441
			// and basically what is going on is we allocate this struct
			// for the size of the string plus the size of the struct.
			// This will allow us to keep all the data related to the actual
			// bytes of the string tied together so it isn't lost, even if
			// we return somestringobj->str; as a pointer to the user.
			// The user can use pointer arithmatic to get the length or size
			// without having an out of bounds deference.
			char str[1]; // String itself.
		} _string_t;

		_string_t *str;
};

// Useful :p
typedef std::vector<kstring> kvector;

// user-defined literal
inline kstring operator "" _k(const char *str, size_t len) { return kstring(str, len); }

// A case-insensitive map
struct insensitive
{
	inline bool operator()(const kstring &a, const kstring &b) const { return !strcasecmp(a.c_str(), b.c_str()); }
};
template<typename T> class kmap : public std::map<kstring, T, insensitive> { };
