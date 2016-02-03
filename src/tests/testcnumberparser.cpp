// This file is part of the SpeedCrunch project
// Copyright (C) 2004-2006 Ariya Hidayat <ariya@kde.org>
// Copyright (C) 2007-2009, 2013-2014 Helder Correia <helder.pereira.correia@gmail.com>
// Copyright (C) 2015 Hadrien Theveneau <theveneau@gmail.com>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; see the file COPYING.  If not, write to
// the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
// Boston, MA 02110-1301, USA.

#include "math/cmath.h"
#include "math/floatconst.h"

#include <cstdlib>
#include <cstring>
#include <iostream>

using namespace std;

#define CHECK(x,y) check_value(__FILE__,__LINE__,#x,x,y)
#define CHECK_FORMAT(f,p,x,y) check_format(__FILE__,__LINE__,#x,x,f,p,y)
#define CHECK_PRECISE(x,y) check_precise(__FILE__,__LINE__,#x,x,y)
#define CHECK_KNOWN_ISSUE(x,y,n) check_value(__FILE__,__LINE__,#x,x,y,n)

static int cmath_total_tests  = 0;
static int cmath_failed_tests = 0;
static int cmath_new_failed_tests = 0;

static void check_value(const char*, int line, const char* msg, const CNumber& n, const char* expected, int issue = 0)
{
    ++cmath_total_tests;
    char* result = CMath::format(n, 'f');
    if (strcmp(result, expected)) {
        ++cmath_failed_tests;
        cerr << "[Line " << line << "]\t" << msg << "\tResult: " << result;
        cerr << "\tExpected: " << expected;
        if (issue)
            cerr << "\t[ISSUE " << issue << "]";
        else {
            cerr << "\t[NEW]";
            ++cmath_new_failed_tests;
        }
        cerr << endl;
    }
    free(result);
}

static void check_format(const char* file, int line, const char* msg, const CNumber& n, char format, int prec, const char* expected)
{
    ++cmath_total_tests;
    char* result = CMath::format(n, format, prec);
    if (strcmp(result, expected)) {
        ++cmath_failed_tests;
        cerr << file << "[" << line << "]: " << msg << endl
             << "  Result  : " << result
             << endl << "  Expected: " << expected << endl << endl;
    }
    free(result);
}

static void check_precise(const char* file, int line, const char* msg, const CNumber& n, const char* expected)
{
    ++cmath_total_tests;
    char* result = CMath::format(n, 'f', 50);
    if (strcmp(result, expected)) {
        ++cmath_failed_tests;
        cerr << file << "[" << line << "]: " << msg << endl
             << "  Result  : " << result << endl
             << "  Expected: " << expected << endl << endl;
    }
    free(result);
}

void test_create()
{
    // Real numbers
    CHECK(CNumber("1"),      "1");
    CHECK(CNumber("2"),      "2");
    CHECK(CNumber("1.0"),    "1");
    CHECK(CNumber("1e-3"),   "0.001");
    CHECK(CNumber("1.0e-3"), "0.001");

    // Complex number
    CHECK(CNumber("0+1j"),           "1j");
    CHECK(CNumber("1+1j"),           "1+1j");
    CHECK(CNumber("2+2j"),           "2+2j");
    CHECK(CNumber("0+1.0j"),         "1j");
    CHECK(CNumber("0.0+1.0j"),       "1j");
    CHECK(CNumber("1.0+1.0j"),       "1+1j");
    CHECK(CNumber("1e-3+1e-3j"),     "0.001+0.001j");
    CHECK(CNumber("1.0e-3+1.0e-3j"), "0.001+0.001j");   

    // Too large or too small.
    CHECK(HNumber("1e1000000000"), "NaN");
    CHECK(HNumber("1e-1000000000"), "NaN");
    CHECK_FORMAT('e', 2, HNumber("1e1000000000"), "NaN");
    CHECK_FORMAT('e', 2, HNumber("1e-1000000000"), "NaN");

    // Long numbers, like in session history
    CHECK(CNumber("0+1.000000000000000000000000000000000000000000000000000000000000000000000000000000j"), "1j");
}

int main(int, char**)
{
    cmath_total_tests  = 0;
    cmath_failed_tests = 0;

    floatmath_init();

    test_create();

    if (cmath_failed_tests)
        cerr << cmath_total_tests  << " total, "
             << cmath_failed_tests << " failed, "
             << cmath_new_failed_tests << " new" << endl;

  return cmath_failed_tests;
}

