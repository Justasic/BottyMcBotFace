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
//The purpose of this file is to hold all of the functions from the kstring.h file.
//It will overload =, +=, +, <, <=, >, >= operators in the string library, so it
//will preform deep copies and comparisons, it WILL NOT just check addresses.

#include "kstring.h"
#include "ManagedBuffer.h"

inline bool isequalnull(bool s1, bool s2)
{
    // if either is null, return false.
    // if both are null, return true (matching strings "technically")
    if ((s1 && !s2) || (!s1 && s2))
        return false;

    return true;
}

//Default constructor
kstring::kstring() : str(nullptr), len(0)
{
};

//Copy constructor for a kstring.
kstring::kstring(const kstring &copy_from) : str(nullptr), len(0)
{
    if (!copy_from.str)
        return;

    len = copy_from.len;
    str = new char[len +1];
    std::strcpy(str, copy_from.str);
};

//Copy constructor for an array of chars
kstring::kstring(const char* copy_from) : str(nullptr), len(0)
{
    if (!copy_from)
        return;

    len = std::strlen(copy_from);
    str = new char[len +1];
    std::strcpy(str, copy_from);
};

// Copy constructor for a std::string
kstring::kstring(const std::string &copy_from) : str(nullptr), len(0)
{
    this->len = copy_from.size();
    this->str = new char[this->len + 1];
    bzero(this->str, this->len + 1);
    memcpy(this->str, copy_from.c_str(), this->len);
}

//Deconstructor
kstring::~kstring()
{
    if (str)
        delete [] str;
    str = nullptr;
    len = 0;
};

bool kstring::securecmp(const kstring &otherstr)
{
    size_t tmax = otherstr.size() - 1;
    int ret = 0;

    for (size_t n = 0; n < this->len; ++n)
    {
		// FIXME: don't call the operator.
        ret |= (this->operator[](n) ^ (n <= tmax ? otherstr[n] : otherstr[tmax]));
    }

    return !ret;
}

kstring & kstring::operator= (const kstring & op2)
{
    // Idiot check
    if (!op2.str)
        return *this;
    //Check for self assignment.
    if(this == &op2)
        return *this;

    //Clear up memory, if any is in use.
    delete [] str;

    //Set str and len to op2's data.
    len = op2.len;
    str = new char[len +1];
    bzero(this->str, this->len + 1);
    std::strcpy(str, op2.str);

    return *this;
};

kstring & kstring::operator= (const std::string &op2)
{
    delete [] this->str;

    this->len = op2.size();
    this->str = new char[this->len + 1];
    // Zero memory so we're always null-terminated
    bzero(this->str, this->len + 1);
    memcpy(this->str, op2.c_str(), this->len);

    return *this;
}

kstring & kstring::operator= (const char * op2)
{
    if (!op2)
        return *this;
    //Check if str is already in use.
    if(str)
        delete []str;

    //Get data for kstring's members
    len = std::strlen(op2);
    str = new char[len +1];
    std::strcpy(str, op2);

    return *this;
};

kstring & kstring::operator+= (const kstring s2)
{
    if (!s2.str)
        return *this;

    len += s2.len;
    char * temp = new char[len + 1];
    std::strcpy(temp, str);
    std::strcat(temp, s2.str);
    delete []str;
    str = temp;
    return *this;
};

kstring & kstring::operator+= (const char *s2)
{
    if (!s2)
        return *this;

    size_t s2len = std::strlen(s2);
    char *tmp = new char[this->len + s2len + 1];
    memcpy(tmp, this->str, this->len);
    std::strncat(tmp, s2, this->len + s2len);
    this->len = this->len + s2len;
    delete [] str;
    str = tmp;
    return *this;
}

std::ostream & operator<< (std::ostream &op1, const kstring& op2)
{
    if (!op2.str)
        return op1;
    op1 << op2.str;
    return op1;
};

std::istream & operator>> (std::istream &op1, kstring& op2)
{
    //To match with Discord's Char Limit
	ManagedBuffer mb;

	while (!op1.eof())
	{
		char ch = op1.get();
		mb.Write(reinterpret_cast<const void*>(&ch), sizeof(std::istream::int_type));
	}

    if(op2.str)
        delete []op2.str;
    op2.str = new char [op2.len +1];
	memcpy(reinterpret_cast<void*>(op2.str), *mb, mb.size());

    return op1;
};

//Pass by value to allow for chaining.
kstring operator+ (const kstring &s, char *c)
{
    // Noop, just return the passed string I guess.
    if (!s.str || !c)
        return s;
    kstring temp;
    temp.len = s.len + std::strlen(c);
    temp.str = new char[temp.len +1];
    std::strcpy(temp.str, s.str);
    std::strcat(temp.str, c);
    return temp;
};

kstring operator+ (char *c, const kstring &s)
{
    if (!c || !s.str)
        return s;

    return s + c;
};

kstring operator+ (const kstring &s, const char *c)
{
    if (!c || !s.str)
        return s;

    kstring temp;
    temp.len = s.len + std::strlen(c);
    temp.str = new char[temp.len +1];
    std::strcpy(temp.str, s.str);
    std::strcat(temp.str, c);
    return temp;
}

kstring operator+ (const char *c, const kstring &s)
{
    if (!c || !s.str)
        return s;

    return s + c;
}

kstring operator+ (const kstring &s1, const kstring &s2)
{
    if (!s1.str || !s2.str)
        return s1;

    return s1 + s2.str;
}


//----RELATIONAL SECTION----
bool operator < (const kstring & s1, char *c)
{
    if (!s1.str || !c)
        return false;

    return std::strcmp(s1.str,c) < 0;
};

bool operator < (char *c, const kstring &s1)
{
    if (!c || s1.str)
        return false;

    return std::strcmp(c,s1.str) <0;
};

bool operator < (const kstring & s1, const kstring & s2)
{
    if (!s1.str || !s2.str)
        return false;

    return std::strcmp(s1.str,s2.str) < 0;
};

bool operator <= (const kstring & s1, char *c)
{
    if (!s1.str || !c)
        return false;

    return std::strcmp(s1.str,c) <= 0;
};

bool operator <= (char *c, const kstring &s1)
{
    if (!c || s1.str)
        return false;

    return std::strcmp(c,s1.str) <= 0;
};

bool operator <= (const kstring & s1, const kstring & s2)
{
    if (!s1.str || !s2.str)
        return false;

    return std::strcmp(s1.str,s2.str) <= 0;
};

bool operator > (const kstring & s1, char *c)
{
    if (!c || s1.str)
        return false;

    return std::strcmp(s1.str,c) > 0;
};

bool operator > (char *c, const kstring &s1)
{
    if (!c || s1.str)
        return false;

    return std::strcmp(c,s1.str) > 0;
};

bool operator > (const kstring & s1, const kstring & s2)
{
    if (!s1.str || !s2.str)
        return false;

    return std::strcmp(s1.str,s2.str) > 0;
};

bool operator >= (const kstring & s1, char *c)
{
    if (!c || s1.str)
        return false;

    return std::strcmp(s1.str,c) >= 0;
};

bool operator >= (char *c, const kstring &s1)
{
    if (!c || s1.str)
        return false;

    return std::strcmp(c, s1.str) >= 0;
};

bool operator >= (const kstring & s1, const kstring & s2)
{
    if (!s1.str || !s2.str)
        return false;

    return std::strcmp(s1.str,s2.str) >= 0;
};

bool operator != (const kstring & s1, char *c)
{
    if (!s1.str || !c)
        // if one is null but the other is not, return true.
        return !isequalnull(s1.str, c);

    return std::strcmp(s1.str,c);
};

bool operator != (char *c, const kstring &s1)
{
    return s1 != c;
};

bool operator != (const kstring & s1, const kstring & s2)
{
    if (!s1.str || !s2.str)
        // if one is null but the other is not, return true.
        return !isequalnull(s1.str, s2.str);

    return std::strcmp(s1.str, s2.str);
};

bool operator == (const kstring & s1, char *c)
{
    if (!s1.str || !c)
        // if one is null but the other is not, return true.
        return isequalnull(s1.str, c);

    return std::strcmp(s1.str,c) == 0;
};

bool operator == (char *c, const kstring &s1)
{
    return s1 == c;
};

bool operator == (const kstring & s1, const kstring & s2)
{
    if (!s1.str || !s2.str)
        // if one is null but the other is not, return true.
        return isequalnull(s1.str, s2.str);

    return std::strcmp(s1.str, s2.str) == 0;
};
//----END RELATIONAL SECTION----


//READ ONLY, Will NOT be able to modify at the index.
char & kstring::operator [] (int index)
{
    static char ch = 0;
    if (this->str)
        return str[index];
    else
        return ch;
};

const char & kstring::operator [] (int index) const
{
    static char ch = 0;
    if (this->str)
        return str[index];
    else
        return ch;
}
