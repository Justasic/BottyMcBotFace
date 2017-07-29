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

        // Standard functions.
        size_t find(const kstring &, size_t = 0) const;
        kstring substr(size_t = 0, size_t = npos) const;

        // a timing-safe compaison of strings (used for passwords)
        bool securecmp(const kstring &);

        // Vector functions (useful for tokenization)
        std::vector<kstring> expand(const kstring &delim) const;
        kstring contract(const std::vector<kstring> &_vec, const kstring &delim);

        // Make use of tinyformat here.
        template<typename... Args>
        kstring fmt(const Args&... args) { return tfm::format(this->c_str(), args...); }

        //Assignment Operators
        kstring & operator= (const kstring &);
        kstring & operator= (const std::string &);
        kstring & operator= (const char *);
        kstring & operator= (const char);
        kstring & operator+= (const std::string &);
        kstring & operator+= (const char *);
        kstring & operator+= (const char);
        kstring & operator+= (const kstring s2);

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
        inline size_t size() const { return this->len; }
        inline const char *c_str() const { return this->str; }
        inline bool isnull() const { return !this->str; }
        inline bool empty() const { return !this->len; }
        inline void clear() { bzero(this->str, this->len); this->len = 0; }

        // Casting operators
        inline explicit operator int()                    { return static_cast<int>(strtol(this->str, nullptr, 10)); }
        inline explicit operator long int()               { return strtol(this->str, nullptr, 10); }
        inline explicit operator long long int()          { return strtoll(this->str, nullptr, 10); }
        // C/C++ does not have a strtou function cuz apparently being consistent isn't top
        // priority for ISOCPP group. So we have to do a truncating cast from unsigned long.
        inline explicit operator unsigned int()           { return static_cast<unsigned int>(strtoul(this->str, nullptr, 10)); }
        inline explicit operator unsigned long int()      { return strtoul(this->str, nullptr, 10); }
        inline explicit operator unsigned long long int() { return strtoull(this->str, nullptr, 10); }
        inline explicit operator float()                  { return strtof(this->str, nullptr); }
        inline explicit operator double()                 { return strtod(this->str, nullptr); }
        inline explicit operator long double()            { return strtold(this->str, nullptr); }


    private:
        char * str;
        //Length of the string, NOT including NULL at the end.
        size_t len;
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
