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

// Synopsis - Justin:
//
// The purpose of this test is to ensure that kstring operates as designed
// so that any changes that happen are caught at build time when we do development.
//
#include <cstring>
#include <string>
#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include "kstring.h"


TEST_CASE("kstring accepts char*", "[kstring]")
{
	kstring string;
	const char str[] = "The Quick Brown Fox Jumps Over the Tall Red Fence.";

	SECTION("constructor accepts const char*")
	{
		string = kstring(str);
		REQUIRE(!memcmp(string.c_str(), str, sizeof(str)));
	}

	SECTION("operator = accepts const char*")
	{
		string = str;
		const char finalstr[] = "The Quick Brown Fox Jumps Over the Tall Red Fence... however he will not jump over the small curb!";
		REQUIRE(memcmp(string.c_str(), finalstr, sizeof(finalstr)));
	}

	SECTION("string length is measured correctly")
	{
		string = str;
		// We use sizeof(str) - 1 here because kstring does not include null-terminator.
		REQUIRE(string.size() == sizeof(str)-1);
	}

	SECTION("operator += accepts const char*")
	{
		string += ".. however he will not jump over the small curb!";
		const char finalstr[] = "The Quick Brown Fox Jumps Over the Tall Red Fence... however he will not jump over the small curb!";
		REQUIRE(memcmp(string.c_str(), finalstr, sizeof(finalstr)));
	}

	SECTION("operator = doesn't crash on null argument")
	{
		string = str;
		const char *nulled = nullptr;
		kstring teststr = nulled;

		kstring empty;
		teststr = empty;
		// This test is runtime either it segfaults or it doesnt.
		REQUIRE(true);
	}

	SECTION("operator + doesn't crash on null argument")
	{
		string = str;
		const char *nulled = nullptr;
		kstring teststr = string + nulled;

		kstring empty;
		teststr = string + empty;
		// This test is runtime either it segfaults or it doesnt.
		REQUIRE((teststr == string));
	}

	SECTION("operator += doesn't crash on null argument")
	{
		string = str;
		kstring empty;
		kstring teststr = str;
		teststr += empty;
		// This test is runtime either it segfaults or it doesnt.
		REQUIRE(teststr == string);
	}

	SECTION("operator == doesn't crash on null argument")
	{
		string = str;
		const char *nulled = nullptr;
		kstring empty;
		// This test is runtime either it segfaults or it doesnt.
		REQUIRE(!(string == empty));
		REQUIRE(!(string == nulled));
	}

	SECTION("operator != doesn't crash on null argument")
	{
		string = str;
		const char *nulled = nullptr;
		kstring empty;
		// This test is runtime either it segfaults or it doesnt.
		REQUIRE(string != empty);
		REQUIRE(string != nulled);
	}

}

TEST_CASE("kstring can accept std::string", "[kstring]")
{
	kstring string;
	const char str[] = "The Quick Brown Fox Jumps Over the Tall Red Fence.";
	const std::string teststr = str;

	SECTION("constructor accepts std::string")
	{
		string = kstring(teststr);
		REQUIRE(!memcmp(string.c_str(), str, sizeof(str)));
	}

	SECTION("operator = will accept std::string")
	{
		string = teststr;
		REQUIRE(!memcmp(string.c_str(), teststr.c_str(), teststr.size()));
	}

	SECTION("operator += will accept std::string")
	{
		string += ".. however he will not jump over the small curb!";
		const std::string final = "The Quick Brown Fox Jumps Over the Tall Red Fence... however he will not jump over the small curb!";
		REQUIRE(memcmp(string.c_str(), final.c_str(), final.size()));
	}

	SECTION("operator != will compare std::string")
	{
		std::string garbage = "akwdhjgfskjqwge";
		string = str;
		REQUIRE(string != garbage);
	}

	SECTION("operator == will compare std::string")
	{
		// Reset the string.
		string = str;
		std::string test = str;
		printf("%ld: %s %s\n", test.size(), test.c_str(), string == test ? "true" : "false");
		REQUIRE(string == test);
	}

	SECTION("operator != will compare to null")
	{
		std::string garbage = "sadjkfhg";
		kstring nullstr;
		REQUIRE(nullstr != garbage);
	}

	SECTION("operator == will compare to null")
	{
		std::string garbage = "sadkfgjh";
		kstring nullstr;
		REQUIRE(!(nullstr == garbage));
	}
}

TEST_CASE("kstring can convert numbers", "[kstring]")
{
	kstring string;
	const char number[] = "9001";
	const char floatnumber[] = "3.14159";

	// TODO: Catch exceptions which should be thrown in these conditions.
	const char toobigint[] = "65537"; // max size is 65535 (unsigned)
	const char toobiglong[] = "4294967297"; // max size is 4294967295 (unsigned)
	const char toobiglonglong[] = "18446744073709551617"; // max size is 18446744073709551615 (unsigned)

	SECTION("cast to int")
	{
		string = number;
		int test = static_cast<int>(string);
		REQUIRE(test == 9001);

		// Test signedness
		string = "-9001";
		test = static_cast<int>(string);
		REQUIRE(test == -9001);
	}

	SECTION("cast to long int")
	{
		string = number;
		long int test = static_cast<long int>(string);
		REQUIRE(test == 9001);

		// test signedness
		string = "-9001";
		test = static_cast<long int>(string);
		REQUIRE(test == -9001);
	}

	SECTION("cast to long long int")
	{
		string = number;
		long long int test = static_cast<long long int>(string);
		REQUIRE(test == 9001);

		// test signedness
		string = "-9001";
		test = static_cast<long long int>(string);
		REQUIRE(test == -9001);
	}


	SECTION("cast to unsigned int")
	{
		string = number;
		unsigned test = static_cast<unsigned int>(string);
		REQUIRE(test == 9001);
	}

	SECTION("cast to unsigned long int")
	{
		string = number;
		unsigned long int test = static_cast<unsigned long int>(string);
		REQUIRE(test == 9001);
	}

	SECTION("cast to unsigned long long int")
	{
		string = number;
		unsigned long long int test = static_cast<unsigned long long int>(string);
		REQUIRE(test == 9001);
	}

	SECTION("cast to float")
	{
		string = floatnumber;
		float test = static_cast<float>(string);

		REQUIRE(test == 3.14159f);
	}

	SECTION("cast to double")
	{
		string = floatnumber;
		double test = static_cast<double>(string);
		REQUIRE(test == (double)3.14159);
	}


}
