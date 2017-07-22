//William Haugen
//
//The purpose of this file is to hold all of the functions from the kstring.h file.
//It will overload =, +=, +, <, <=, >, >= operators in the string library, so it
//will preform deep copies and comparisons, it WILL NOT just check addresses.

#include "kstring.h"
#include "ManagedBuffer.h"

//Default constructor
kstring::kstring() : str(NULL), len(0)
{
};

//Copy constructor for a kstring.
kstring::kstring(const kstring &copy_from)
{
    len = copy_from.len;
    str = new char[len +1];
    std::strcpy(str, copy_from.str);
};

//Copy constructor for an array of chars
kstring::kstring(const char* copy_from)
{
    len = std::strlen(copy_from);
    str = new char[len +1];
    std::strcpy(str, copy_from);
};

//Deconstructor
kstring::~kstring()
{
    delete [] str;
    str = NULL;
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
    //Check for self assignment.
    if(this == &op2)
        return *this;

    //Clear up memory, if any is in use.
    delete [] str;

    //Set str and len to op2's data.
    len = op2.len;
    str = new char[len +1];
    std::strcpy(str, op2.str);

    return *this;
};

kstring & kstring::operator= (const char * op2)
{
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
    len += s2.len;
    char * temp = new char[len +s2.len +1];
    std::strcpy(temp, str);
    std::strcat(temp, s2.str);
    delete []str;
    str = temp;
    return *this;
};

std::ostream & operator<< (std::ostream &op1, const kstring& op2) 
{
    op1 <<op2.str;
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

//    char temp[1024]; // 1K buffeer
//    op1 >>temp;
//    op2.len = std::strlen(temp);
    if(op2.str)
        delete []op2.str;
    op2.str = new char [op2.len +1];
//    std::strncpy(op2.str, temp);
	memcpy(reinterpret_cast<void*>(op2.str), *mb, mb.size());

    return op1;
};

//Pass by value to allow for chaining.
kstring operator+ (const kstring &s, char *c)
{
    kstring temp;
    temp.len = s.len + std::strlen(c);
    temp.str = new char[temp.len +1];
    std::strcpy(temp.str, s.str);
    std::strcat(temp.str, c);
    return temp;
};

kstring operator+ (char *c, const kstring &s)
{
    return s + c;
};


//----RELATIONAL SECTION----
bool operator < (const kstring & s1, char *c)
{
    return(std::strcmp(s1.str,c) < 0);
};

bool operator < (char *c, const kstring &s1)
{
    return(std::strcmp(c,s1.str) <0);
};

bool operator < (const kstring & s1, const kstring & s2)
{
    return(std::strcmp(s1.str,s2.str) < 0);
};

bool operator <= (const kstring & s1, char *c)
{
    return(std::strcmp(s1.str,c) <= 0);
};

bool operator <= (char *c, const kstring &s1)
{
    return(std::strcmp(c,s1.str) <= 0);
};

bool operator <= (const kstring & s1, const kstring & s2)
{
    return(std::strcmp(s1.str,s2.str) <= 0);
};

bool operator > (const kstring & s1, char *c)
{
    return(std::strcmp(s1.str,c) > 0);
};

bool operator > (char *c, const kstring &s1)
{
    return(std::strcmp(c,s1.str) > 0);
};

bool operator > (const kstring & s1, const kstring & s2)
{
    return(std::strcmp(s1.str,s2.str) > 0);
};

bool operator >= (const kstring & s1, char *c)
{
    return(std::strcmp(s1.str,c) >= 0);
};

bool operator >= (char *c, const kstring &s1)
{
    return(std::strcmp(c, s1.str) >= 0);
};

bool operator >= (const kstring & s1, const kstring & s2)
{
    return(std::strcmp(s1.str,s2.str) >= 0);
};

bool operator != (const kstring & s1, char *c)
{
    return(std::strcmp(s1.str,c));
};

bool operator != (char *c, const kstring &s1)
{
    return(s1 != c);
};

bool operator != (const kstring & s1, const kstring & s2)
{
    return(std::strcmp(s1.str,s2.str));
};

bool operator == (const kstring & s1, char *c)
{
    return(std::strcmp(s1.str,c) == 0);
};

bool operator == (char *c, const kstring &s1)
{
    return(s1 == c);
};

bool operator == (const kstring & s1, const kstring & s2)
{
    return(std::strcmp(s1.str,s2.str) == 0);
};
//----END RELATIONAL SECTION----


//READ ONLY, Will NOT be able to modify at the index.
char & kstring::operator [] (int index)
{
    return str[index];
};

const char & kstring::operator [] (int index) const
{
	return str[index];
}
