//William Haugen
//
//The purpose of this class, kstring, is to overload the standard
//cstring library's operators to preform deep copies and have
//it act like it is part of the standard library.

#include <cstring>
#include <cctype>
#include <iostream>


//TO DO:
//Length to spit out len
//Change char at a given index
class kstring
{
    public:
        kstring();
        kstring(const kstring &);
        kstring(const char *);
        ~kstring();

        //Assignment Operators
        kstring & operator= (const kstring &);
        kstring & operator= (const char *);
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

    private:
        char * str;
        //Length of the string, NOT including NULL at the end.
        int len;
};
