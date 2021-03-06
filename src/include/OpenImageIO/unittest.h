/*
  Copyright 2010 Larry Gritz and the other authors and contributors.
  All Rights Reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are
  met:
  * Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in the
  documentation and/or other materials provided with the distribution.
  * Neither the name of the software's owners nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.
  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

  (This is the Modified BSD License)
*/

#pragma once

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>

#include <OpenImageIO/strutil.h>
#include <OpenImageIO/sysutil.h>


OIIO_NAMESPACE_BEGIN

namespace simd {
// Force a float-based abs and max to appear in namespace simd
inline float
abs(float x)
{
    return std::abs(x);
}
inline float
max(float x, float y)
{
    return std::max(x, y);
}
}  // namespace simd

namespace pvt {

class UnitTestFailureCounter {
public:
    UnitTestFailureCounter() noexcept
        : m_failures(0)
    {
    }
    ~UnitTestFailureCounter()
    {
        if (m_failures) {
            std::cout << Sysutil::Term(std::cout).ansi("red", "ERRORS!\n");
            std::exit(m_failures != 0);
        } else {
            std::cout << Sysutil::Term(std::cout).ansi("green", "OK\n");
        }
    }
    const UnitTestFailureCounter& operator++() noexcept  // prefix
    {
        ++m_failures;
        return *this;
    }
    int operator++(int) noexcept { return m_failures++; }  // postfix
    UnitTestFailureCounter operator+=(int i) noexcept
    {
        m_failures += i;
        return *this;
    }
    operator int() const noexcept { return m_failures; }

private:
    int m_failures = 0;
};


template<typename X, typename Y>
inline bool
equal_approx(const X& x, const Y& y)
{
    using namespace simd;
    return all(abs((x) - (y)) <= 0.001f * max(abs(x), abs(y)));
}


}  // end namespace pvt

OIIO_NAMESPACE_END

// Helper: print entire vectors. This makes the OIIO_CHECK_EQUAL macro
// work for std::vector!
template<typename T>
inline std::ostream&
operator<<(std::ostream& out, const std::vector<T>& v)
{
    out << '{' << OIIO::Strutil::join(v, ",") << '}';
    return out;
}

static OIIO::pvt::UnitTestFailureCounter unit_test_failures;



/// OIIO_CHECK_* macros checks if the conditions is met, and if not,
/// prints an error message indicating the module and line where the
/// error occurred, but does NOT abort.  This is helpful for unit tests
/// where we do not want one failure.
#define OIIO_CHECK_ASSERT(x)                                                   \
    ((x) ? ((void)0)                                                           \
         : ((std::cout << OIIO::Sysutil::Term(std::cout).ansi("red,bold")      \
                       << __FILE__ << ":" << __LINE__ << ":\n"                 \
                       << "FAILED: "                                           \
                       << OIIO::Sysutil::Term(std::cout).ansi("normal") << #x  \
                       << "\n"),                                               \
            (void)++unit_test_failures))

#define OIIO_CHECK_EQUAL(x, y)                                                 \
    (((x) == (y))                                                              \
         ? ((void)0)                                                           \
         : ((std::cout << OIIO::Sysutil::Term(std::cout).ansi("red,bold")      \
                       << __FILE__ << ":" << __LINE__ << ":\n"                 \
                       << "FAILED: "                                           \
                       << OIIO::Sysutil::Term(std::cout).ansi("normal") << #x  \
                       << " == " << #y << "\n"                                 \
                       << "\tvalues were '" << (x) << "' and '" << (y)         \
                       << "'\n"),                                              \
            (void)++unit_test_failures))

#define OIIO_CHECK_EQUAL_THRESH(x, y, eps)                                     \
    ((std::abs((x) - (y)) <= eps)                                              \
         ? ((void)0)                                                           \
         : ((std::cout << OIIO::Sysutil::Term(std::cout).ansi("red,bold")      \
                       << __FILE__ << ":" << __LINE__ << ":\n"                 \
                       << "FAILED: "                                           \
                       << OIIO::Sysutil::Term(std::cout).ansi("normal") << #x  \
                       << " == " << #y << "\n"                                 \
                       << "\tvalues were '" << (x) << "' and '" << (y) << "'"  \
                       << ", diff was " << std::abs((x) - (y)) << "\n"),       \
            (void)++unit_test_failures))

#define OIIO_CHECK_EQUAL_APPROX(x, y)                                          \
    (OIIO::pvt::equal_approx(x, y)                                             \
         ? ((void)0)                                                           \
         : ((std::cout << OIIO::Sysutil::Term(std::cout).ansi("red,bold")      \
                       << __FILE__ << ":" << __LINE__ << ":\n"                 \
                       << "FAILED: "                                           \
                       << OIIO::Sysutil::Term(std::cout).ansi("normal") << #x  \
                       << " == " << #y << "\n"                                 \
                       << "\tvalues were '" << (x) << "' and '" << (y) << "'"  \
                       << ", diff was " << ((x) - (y)) << "\n"),               \
            (void)++unit_test_failures))

#define OIIO_CHECK_NE(x, y)                                                    \
    (((x) != (y))                                                              \
         ? ((void)0)                                                           \
         : ((std::cout << OIIO::Sysutil::Term(std::cout).ansi("red,bold")      \
                       << __FILE__ << ":" << __LINE__ << ":\n"                 \
                       << "FAILED: "                                           \
                       << OIIO::Sysutil::Term(std::cout).ansi("normal") << #x  \
                       << " != " << #y << "\n"                                 \
                       << "\tvalues were '" << (x) << "' and '" << (y)         \
                       << "'\n"),                                              \
            (void)++unit_test_failures))

#define OIIO_CHECK_LT(x, y)                                                    \
    (((x) < (y))                                                               \
         ? ((void)0)                                                           \
         : ((std::cout << OIIO::Sysutil::Term(std::cout).ansi("red,bold")      \
                       << __FILE__ << ":" << __LINE__ << ":\n"                 \
                       << "FAILED: "                                           \
                       << OIIO::Sysutil::Term(std::cout).ansi("normal") << #x  \
                       << " < " << #y << "\n"                                  \
                       << "\tvalues were '" << (x) << "' and '" << (y)         \
                       << "'\n"),                                              \
            (void)++unit_test_failures))

#define OIIO_CHECK_GT(x, y)                                                    \
    (((x) > (y))                                                               \
         ? ((void)0)                                                           \
         : ((std::cout << OIIO::Sysutil::Term(std::cout).ansi("red,bold")      \
                       << __FILE__ << ":" << __LINE__ << ":\n"                 \
                       << "FAILED: "                                           \
                       << OIIO::Sysutil::Term(std::cout).ansi("normal") << #x  \
                       << " > " << #y << "\n"                                  \
                       << "\tvalues were '" << (x) << "' and '" << (y)         \
                       << "'\n"),                                              \
            (void)++unit_test_failures))

#define OIIO_CHECK_LE(x, y)                                                    \
    (((x) <= (y))                                                              \
         ? ((void)0)                                                           \
         : ((std::cout << OIIO::Sysutil::Term(std::cout).ansi("red,bold")      \
                       << __FILE__ << ":" << __LINE__ << ":\n"                 \
                       << "FAILED: "                                           \
                       << OIIO::Sysutil::Term(std::cout).ansi("normal") << #x  \
                       << " <= " << #y << "\n"                                 \
                       << "\tvalues were '" << (x) << "' and '" << (y)         \
                       << "'\n"),                                              \
            (void)++unit_test_failures))

#define OIIO_CHECK_GE(x, y)                                                    \
    (((x) >= (y))                                                              \
         ? ((void)0)                                                           \
         : ((std::cout << OIIO::Sysutil::Term(std::cout).ansi("red,bold")      \
                       << __FILE__ << ":" << __LINE__ << ":\n"                 \
                       << "FAILED: "                                           \
                       << OIIO::Sysutil::Term(std::cout).ansi("normal") << #x  \
                       << " >= " << #y << "\n"                                 \
                       << "\tvalues were '" << (x) << "' and '" << (y)         \
                       << "'\n"),                                              \
            (void)++unit_test_failures))


// Special SIMD related equality checks that use all()
#define OIIO_CHECK_SIMD_EQUAL(x, y)                                            \
    (all((x) == (y))                                                           \
         ? ((void)0)                                                           \
         : ((std::cout << OIIO::Sysutil::Term(std::cout).ansi("red,bold")      \
                       << __FILE__ << ":" << __LINE__ << ":\n"                 \
                       << "FAILED: "                                           \
                       << OIIO::Sysutil::Term(std::cout).ansi("normal") << #x  \
                       << " == " << #y << "\n"                                 \
                       << "\tvalues were '" << (x) << "' and '" << (y)         \
                       << "'\n"),                                              \
            (void)++unit_test_failures))

#define OIIO_CHECK_SIMD_EQUAL_THRESH(x, y, eps)                                \
    (all(abs((x) - (y)) < (eps))                                               \
         ? ((void)0)                                                           \
         : ((std::cout << OIIO::Sysutil::Term(std::cout).ansi("red,bold")      \
                       << __FILE__ << ":" << __LINE__ << ":\n"                 \
                       << "FAILED: "                                           \
                       << OIIO::Sysutil::Term(std::cout).ansi("normal") << #x  \
                       << " == " << #y << "\n"                                 \
                       << "\tvalues were '" << (x) << "' and '" << (y)         \
                       << "'\n"),                                              \
            (void)++unit_test_failures))
