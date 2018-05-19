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

// Justin Crawford
//
// We cannot use the C++ new operator as it does not allow for `realloc' to be used
// on the pointers it creates, this unfortunately means we must do some nasty amounts
// of reinterpret_casts to get type compliance. I may create a special allocator in
// the future which allows for only looking at one set of reinterpret_casts instead of
// messy pointer hacking everywhere.

#include "kstring.h"
#include "ManagedBuffer.h"


// "Typesafe" allocators
template<typename ret> ret tmalloc(size_t sz) { return reinterpret_cast<ret>(malloc(sz)); }
template<typename old> auto trealloc(old o, size_t newsz) { return reinterpret_cast<old>(realloc(reinterpret_cast<void*>(o), newsz)); }
template<typename old> void tfree(old o) { free(reinterpret_cast<void*>(o)); }

inline bool isequalnull(bool s1, bool s2)
{
    // if either is null, return false.
    // if both are null, return true (matching strings "technically")
    if ((s1 && !s2) || (!s1 && s2))
        return false;

    return true;
}

//Default constructor
kstring::kstring() : str(nullptr)
{
};

kstring::kstring(const char *copy_from, size_t len) : str(nullptr)
{
    if (!copy_from)
        return;

    this->str = tmalloc<_string_t*>(sizeof(kstring::_string_t) + len);
    this->str->allocatedsz = sizeof(kstring::_string_t) + len;
    this->str->length = len;
    this->str->refs++;
    memcpy(this->str->str, copy_from, len);
}

//Copy constructor for a kstring.
kstring::kstring(const kstring &copy_from) : str(nullptr)
{
    if (!copy_from.str)
        return;

	// hahaha this one is easy ;D
	this->str = tmalloc<_string_t*>(copy_from.str->allocatedsz);
	memcpy(this->str, copy_from.str, copy_from.str->allocatedsz);
	this->str->refs = 1;
};

//Copy constructor for an array of chars
kstring::kstring(const char* copy_from) : str(nullptr)
{
    if (!copy_from)
        return;

    size_t len = std::strlen(copy_from);
    this->str = tmalloc<_string_t*>(sizeof(kstring::_string_t) + len);
    this->str->allocatedsz = sizeof(kstring::_string_t) + len;
    this->str->length = len;
    this->str->refs++;
    memcpy(this->str->str, copy_from, len);
};

// Copy constructor for a std::string
kstring::kstring(const std::string &copy_from) : str(nullptr)
{
    this->str = tmalloc<_string_t*>(sizeof(kstring::_string_t) + copy_from.size());
    this->str->allocatedsz = sizeof(kstring::_string_t) + copy_from.size();
    this->str->length = copy_from.size();
    this->str->refs++;
    memcpy(this->str->str, copy_from.data(), this->str->length);
}

//Deconstructor
kstring::~kstring()
{
    if (this->str)
    {
        // Only free our string if the reference count is zero
        this->str->refs--;
        if (!this->str->refs)
        {
            tfree(this->str);
            this->str = nullptr;
        }
    }
};


size_t kstring::find(const kstring& op2, size_t start) const
{
    int index = start;
    //If either string is null, or the length is equal to 0, leave.
    if ((!op2.str || !this->str) )
        return kstring::npos;

	if ((this->str->length == 0 || op2.str->length == 0) || start > this->str->length)
		return kstring::npos;

    bool is_match = false;

    //go through this->str and see if there is a match for the first
    //element of the for loop.
    for (size_t i = index; i < this->str->length && !is_match; ++i)
    {
        //if there is a match, set is_match to true
        if (this->str->str[i] == op2.str->str[0] && (this->str->length - i) >= op2.str->length)
        {
            is_match = true;
            //go through the rest of op2.str to make sure it is a match
            //skips index 0 since it was already compared to.
            for (size_t j = 1, k = i+1; j < op2.str->length && is_match; ++j, ++k)
            {
                //if there isn't a match, set is_match to false
                if (this->str->str[k] != op2.str->str[j])
                    is_match = false;
            }
        }

        //if op2.str was a match, index i where it was found.
        if (is_match)
            index = i;
    }

    //return the index if there was a match.
    //return npos if no match was found.
    if (is_match)
        return index;
    return kstring::npos;
}

kstring kstring::substr(size_t begin, size_t end) const
{
	if (this->isnull())
		return *this;

    //Sets all the variables for ints for ease of use.
    size_t length = this->str->length;
    //if end is == -1 then copy the rest of the string.
    if (end == -1U)
        end = length;

    //Checks all the bad inputs/ info before getting to the function.
    if ((begin >= length) || (end > length || begin > end))
        return *this;

    //Creates a temp array of the max value possible for substr
    char array[length + 1];

    //Copies the chars to the temp array
    for (size_t i = begin, j = 0; i <= end; ++i, ++j)
        array[j] = this->str->str[i];

    //set null at the very last possible index.
    int endArray = end - begin;
    array[++endArray] = '\0';

    //create a kstring with array to be returned.
    kstring temp(array);

    //returns the new kstring.
    return temp;
}

bool kstring::securecmp(const kstring &otherstr)
{
    size_t tmax = otherstr.size() - 1;
    int ret = 0;

    for (size_t n = 0; n < this->str->length; ++n)
    {
		// FIXME: don't call the operator.
        ret |= (this->operator[](n) ^ (n <= tmax ? otherstr[n] : otherstr[tmax]));
    }

    return !ret;
}

kvector kstring::expand(const kstring &delim) const
{
    size_t start = 0, end = 0;
    kvector ret;

    while (end != kstring::npos)
    {
        end = this->find(delim, start);

        // If at end, use length=maxLength.  Else use length=end-start.
        ret.push_back(this->substr(start, (end == kstring::npos) ? kstring::npos : end - start));

        // If at end, use start=maxSize.  Else use start=end+delimiter.
        start = ((end > (kstring::npos - delim.size())) ? kstring::npos : end + delim.size());
    }

    return ret;
}

kstring kstring::contract(const kvector &_vec, const kstring &delim)
{
    for (auto it = _vec.begin(), it_end = _vec.end(); it != it_end; ++it)
    {
        if (it + 1 == it_end)
            *this += *it;
        else
            *this += (*it) + delim;
    }

    return *this;
}

kstring & kstring::operator= (const kstring & op2)
{
    // Idiot check
    if (!op2.str)
        return *this;

	if (!this->isnull())
		free(this->str);

	// Honestly, it's easier to call our own constructor cuz I'm lazy
	// This does, however, make a deep copy which is not efficient
	// but is the same behavior as std::string
	this->kstring(op2);

    return *this;
};

kstring & kstring::operator= (const std::string &op2)
{
	if (op2.empty())
		return *this;

	if (!this->isnull())
		free(this->str);
    
	this->kstring(op2);

    return *this;
}

kstring & kstring::operator= (const char * op2)
{
    if (!op2)
        return *this;
    if (this->str)
        free(this->str);

	this->kstring(op2);

    return *this;
};

kstring & kstring::operator= (const char ch)
{
    if (this->str)
        free(this->str);
	
	this->kstring(ch);

    return *this;
}

kstring & kstring::operator+= (const kstring &s2)
{
    if (!s2.str)
        return *this;

	size_t newsz = this->str->allocatedsz + s2.str->length;

	_string_t *s = trealloc(this->str, newsz);
	if (!s)
		throw std::bad_alloc();

	s->allocatedsz = newsz;
	std::strncat(s->str, s2.str->str, s2.str->length);
	s->length += s2.str->length;

	this->str = s;

    return *this;
};

kstring & kstring::operator+= (const char *s2)
{
    if (!s2)
        return *this;

    size_t s2len = std::strlen(s2);
	size_t newsz = this->str->allocatedsz + s2len;

	_string_t *s = trealloc(this->str, newsz);
	if (!s)
		throw std::bad_alloc();

	s->allocatedsz = newsz;
	s->length += s2len;
	std::strncat(s->str, s2, s2len);

	this->str = s;

    return *this;
}

std::ostream & operator<< (std::ostream &op1, const kstring& op2)
{
    if (!op2.str)
        return op1;

    op1 << op2.str->str;

    return op1;
};

std::istream & operator>> (std::istream &op1, kstring& op2)
{
	ManagedBuffer mb;

	while (!op1.eof())
	{
		char ch = op1.get();
		mb.Write(reinterpret_cast<const void*>(&ch), sizeof(std::istream::int_type));
	}


	if (op2.str)
		tfree(op2.str);

	kstring::_string_t *s = tmalloc<kstring::_string_t*>(mb.size());

	if (!s)
		throw std::bad_alloc();

	memcpy(reinterpret_cast<void*>(s->str), *mb, mb.size());

	op2.str = s;

    return op1;
};

//Pass by value to allow for chaining.
kstring operator+ (const kstring &s, char *c)
{
    // Noop, just return the passed string I guess.
    if (!s.str || !c)
        return s;

    kstring temp = s;
	size_t len = std::strlen(c);
	// TODO: Use kstring::reserve() in the future here.
	kstring::_string_t *ns = trealloc(temp.str, s.str->allocatedsz + len);
	if (!ns)
		throw std::bad_alloc();

    std::strcat(temp.str->str, c);
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
    // Noop, just return the passed string I guess.
    if (!s.str || !c)
        return s;

    kstring temp = s;
	size_t len = std::strlen(c);
	// TODO: Use kstring::reserve() in the future here.
	kstring::_string_t *ns = trealloc(temp.str, s.str->allocatedsz + len);
	if (!ns)
		throw std::bad_alloc();

    std::strcat(temp.str->str, c);
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

    return s1 + s2.str->str;
}


//----RELATIONAL SECTION----
bool operator < (const kstring & s1, char *c)
{
    if (!s1.str || !c)
        return false;

    return std::strcmp(s1.str->str, c) < 0;
};

bool operator < (char *c, const kstring &s1)
{
    if (!c || s1.str)
        return false;

    return std::strcmp(c, s1.str->str) <0;
};

bool operator < (const kstring & s1, const kstring & s2)
{
    if (!s1.str || !s2.str)
        return false;

    return std::strcmp(s1.str->str, s2.str->str) < 0;
};

bool operator <= (const kstring & s1, char *c)
{
    if (!s1.str || !c)
        return false;

    return std::strcmp(s1.str->str, c) <= 0;
};

bool operator <= (char *c, const kstring &s1)
{
    if (!c || s1.str)
        return false;

    return std::strcmp(c, s1.str->str) <= 0;
};

bool operator <= (const kstring & s1, const kstring & s2)
{
    if (!s1.str || !s2.str)
        return false;

    return std::strcmp(s1.str->str, s2.str->str) <= 0;
};

bool operator > (const kstring & s1, char *c)
{
    if (!c || s1.str)
        return false;

    return std::strcmp(s1.str->str, c) > 0;
};

bool operator > (char *c, const kstring &s1)
{
    if (!c || s1.str)
        return false;

    return std::strcmp(c, s1.str->str) > 0;
};

bool operator > (const kstring & s1, const kstring & s2)
{
    if (!s1.str || !s2.str)
        return false;

    return std::strcmp(s1.str->str, s2.str->str) > 0;
};

bool operator >= (const kstring & s1, char *c)
{
    if (!c || s1.str)
        return false;

    return std::strcmp(s1.str->str, c) >= 0;
};

bool operator >= (char *c, const kstring &s1)
{
    if (!c || s1.str)
        return false;

    return std::strcmp(c, s1.str->str) >= 0;
};

bool operator >= (const kstring & s1, const kstring & s2)
{
    if (!s1.str || !s2.str)
        return false;

    return std::strcmp(s1.str->str, s2.str->str) >= 0;
};

bool operator != (const kstring & s1, char *c)
{
    if (!s1.str || !c)
        // if one is null but the other is not, return true.
        return !isequalnull(s1.str, c);

    return std::strcmp(s1.str->str, c);
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

    return std::strcmp(s1.str->str, s2.str->str);
};

bool operator == (const kstring & s1, char *c)
{
    if (!s1.str || !c)
        // if one is null but the other is not, return true.
        return isequalnull(s1.str, c);

    return std::strcmp(s1.str->str, c) == 0;
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

    return std::strcmp(s1.str->str, s2.str->str) == 0;
};
//----END RELATIONAL SECTION----


//READ ONLY, Will NOT be able to modify at the index.
char & kstring::operator [] (int index)
{
    static char ch = 0;
    if (this->str)
        return this->str->str[index];
    else
        return ch;
};

const char & kstring::operator [] (int index) const
{
    static char ch = 0;
    if (this->str)
        return this->str->str[index];
    else
        return ch;
};
