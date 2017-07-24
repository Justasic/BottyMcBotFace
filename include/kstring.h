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

//TO DO:
//Length to spit out len
//Change char at a given index
class kstring
{
    public:
        kstring();
        kstring(const kstring &);
        kstring(const char *);
        kstring(const std::string &str);
        ~kstring();

        // a timing-safe compaison of strings (used for passwords)
        bool securecmp(const kstring &);

        //Assignment Operators
        kstring & operator= (const kstring &);
        kstring & operator= (const std::string &);
        kstring & operator= (const char *);
        kstring & operator+= (const std::string &);
        kstring & operator+= (const kstring s2);

        //Able to use std::ostream and std::istream natively
        friend std::ostream & operator<< (std::ostream &, const kstring&);
        friend std::istream & operator>> (std::istream &, kstring&);

        //Ability to add onto strings, just uses std::strcat
        friend kstring operator+ (const kstring &, char *);
        friend kstring operator+ (char *, const kstring&);

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

        // Casting operators
        inline operator long int() { return strtol(this->str, nullptr, 10); }
        inline operator long long() { return strtoll(this->str, nullptr, 10); }
        inline operator float() { return strtof(this->str, nullptr); }
        inline operator double() { return strtod(this->str, nullptr); }
        inline operator unsigned long () { return strtoul(this->str, nullptr, 10); }

    private:
        char * str;
        //Length of the string, NOT including NULL at the end.
        size_t len;
};

// A case-insensitive map
struct insensitive
{
    inline bool operator()(const kstring &a, const kstring &b) const { return !strcasecmp(a.c_str(), b.c_str()); }
};
template<typename T> class insensitive_map : public std::map<kstring, T, insensitive> { };
