// Copyright 2013, 2014 by Martin Moene
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifdef __clang__
# pragma clang diagnostic ignored "-Wfloat-equal"
#elif defined __GNUC__
# pragma GCC   diagnostic ignored "-Wfloat-equal"
#endif

#include "lest/lest_decompose.hpp"
#include <set>

#ifdef __clang__
# pragma clang diagnostic ignored "-Wmissing-braces"
#elif defined __GNUC__
# pragma GCC   diagnostic ignored "-Wmissing-braces"
#endif

const lest::test no_using_namespace_lest[] =
{
    CASE( "Ensure namespace lest is specified correctly in lest_decompose.hpp [compile-only]" )
    {
        EXPECT(  true );
        EXPECT_NO_THROW( true );
        EXPECT_THROWS( true );
        EXPECT_THROWS_AS( true, std::exception );
    },
};

using namespace lest;

struct S { void f(){} };

const lest::test specification[] =
{
    CASE( "Function to suppress warning \"expression has no effect\" acts as identity function" )
    {
        EXPECT( false == is_true( false ) );
        EXPECT(  true == is_true( true  ) );
    },

    CASE( "Function with_message() returns correct string" )
    {
        std::string msg = "Let writing tests become irresistibly easy and attractive.";
        EXPECT( ( "with message \"" + msg + "\"" ) == with_message( msg ) );
    },

    CASE( "Function of_type() returns correct string" )
    {
        std::string msg = "this_type";
        EXPECT( ( "of type " + msg ) == of_type( msg ) );
    },

    CASE( "Function pluralise() adds 's' except for 1 item" )
    {
        std::string word = "hammer";
        EXPECT( word == pluralise( word, 1 ) );
        for ( auto i : {0,2,3,4,5,6,7,8,9,10,11,12} )
        {
            EXPECT( ( word + "s" ) == pluralise( word, i ) );
        }
    },

    CASE( "Location constructs properly" )
    {
        char const * file = __FILE__; int line = __LINE__;
        location where{ file, line };
        EXPECT( file == where.file );
        EXPECT( line == where.line );
    },

    CASE( "Comment constructs properly" )
    {
        std::string info = __FILE__;
        comment note = info;
        EXPECT( info == note.info );
    },

    CASE( "Comment converted to bool indicates absence or presence of comment" )
    {
        EXPECT( false == bool( comment( "" ) ) );
        EXPECT(  true == bool( comment("x" ) ) );
    },

    CASE( "Failure exception type constructs and prints properly" )
    {
        std::string name = "test-name";
        failure msg( location{"filename.cpp", 765}, "expression", "decomposition" );

        std::ostringstream os;
        report( os, msg, name );

#ifndef __GNUG__
        EXPECT( os.str() == "filename.cpp(765): failed: test-name: expression for decomposition\n" );
#else
        EXPECT( os.str() == "filename.cpp:765: failed: test-name: expression for decomposition\n" );
#endif
    },

    CASE( "Expected exception type constructs and prints properly" )
    {
        std::string name = "test-name";
        expected msg( location{"filename.cpp", 765}, "expression" );

        std::ostringstream os;
        report( os, msg, name );

#ifndef __GNUG__
        EXPECT( os.str() == "filename.cpp(765): failed: didn't get exception: test-name: expression\n" );
#else
        EXPECT( os.str() == "filename.cpp:765: failed: didn't get exception: test-name: expression\n" );
#endif
    },

    CASE( "Unexpected exception type constructs and prints properly" )
    {
        std::string name = "test-name";
        lest::unexpected msg( location{"filename.cpp", 765}, "expression", "exception-type" );

        std::ostringstream os;
        report( os, msg, name );

#ifndef __GNUG__
        EXPECT( os.str() == "filename.cpp(765): failed: got unexpected exception exception-type: test-name: expression\n" );
#else
        EXPECT( os.str() == "filename.cpp:765: failed: got unexpected exception exception-type: test-name: expression\n" );
#endif
    },

    CASE( "Expect generates no message exception for a succeeding test" )
    {
        test pass = { CASE( "P" ) { EXPECT( true  ); } };

        try { pass.behaviour(); }
        catch(...) { throw failure(location{__FILE__,__LINE__}, "unexpected error generated", "true" ); }
    },

    CASE( "Expect generates a message exception for a failing test" )
    {
        test fail = { CASE( "F" ) { EXPECT( false ); } };

        for (;;)
        {
            try { fail.behaviour(); } catch ( message & ) { break; }
            throw failure(location{__FILE__,__LINE__}, "no error generated", "false" );
        }
    },

    CASE( "Expect succeeds for success (true) and failure (false)" )
    {
        test pass[] = {{ CASE( "P" ) { EXPECT( true  ); } }};
        test fail[] = {{ CASE( "F" ) { EXPECT( false ); } }};

        std::ostringstream os;

        EXPECT( 0 == run( pass, os ) );
        EXPECT( 1 == run( fail, os ) );
    },

    CASE( "Expect succeeds for integer comparison" )
    {
        test pass  [] = {{ CASE( "P"  ) { EXPECT( 7 == 7 ); EXPECT( 7 != 8 );
                                          EXPECT( 7 >= 6 ); EXPECT( 7 <= 8 );
                                          EXPECT( 7 >  6 ); EXPECT( 7 <  8 ); } }};
        test fail_1[] = {{ CASE( "F1" ) { EXPECT( 7 == 8 ); } }};
        test fail_2[] = {{ CASE( "F2" ) { EXPECT( 7 != 7 ); } }};
        test fail_3[] = {{ CASE( "F3" ) { EXPECT( 7 <= 6 ); } }};
        test fail_4[] = {{ CASE( "F4" ) { EXPECT( 7 >= 8 ); } }};
        test fail_5[] = {{ CASE( "F5" ) { EXPECT( 7 <  6 ); } }};
        test fail_6[] = {{ CASE( "F6" ) { EXPECT( 7 >  8 ); } }};

        std::ostringstream os;

        EXPECT( 0 == run( pass  , os ) );
        EXPECT( 1 == run( fail_1, os ) );
        EXPECT( 1 == run( fail_2, os ) );
        EXPECT( 1 == run( fail_3, os ) );
        EXPECT( 1 == run( fail_4, os ) );
        EXPECT( 1 == run( fail_5, os ) );
        EXPECT( 1 == run( fail_6, os ) );
    },

    CASE( "Expect succeeds for mixed integer, real comparison" )
    {
        test pass  [] = {{ CASE( "P"  ) { EXPECT( 7.0 == 7   ); EXPECT( 7.0 != 8   );
                                          EXPECT( 7   == 7.0 ); EXPECT( 7   != 8.0 );} }};
        test fail_1[] = {{ CASE( "F1" ) { EXPECT( 7.0 == 8   ); } }};
        test fail_2[] = {{ CASE( "F2" ) { EXPECT( 7  !=  7.0 ); } }};

        std::ostringstream os;

        EXPECT( 0 == run( pass  , os ) );
        EXPECT( 1 == run( fail_1, os ) );
        EXPECT( 1 == run( fail_2, os ) );
    },

    CASE( "Expect succeeds for string comparison" )
    {
        std::string a("a" ); std::string b("b" );
        test pass  [] = {{ CASE_ON( "P", =  ) { EXPECT( a == a ); EXPECT( a != b );
                                                EXPECT( b >= a ); EXPECT( a <= b );
                                                EXPECT( b >  a ); EXPECT( a <  b ); } }};
        test fail_1[] = {{ CASE_ON( "F1", = ) { EXPECT( a == b ); } }};
        test fail_2[] = {{ CASE_ON( "F2", = ) { EXPECT( a != a ); } }};
        test fail_3[] = {{ CASE_ON( "F3", = ) { EXPECT( b <= a ); } }};
        test fail_4[] = {{ CASE_ON( "F4", = ) { EXPECT( a >= b ); } }};
        test fail_5[] = {{ CASE_ON( "F5", = ) { EXPECT( b <  a ); } }};
        test fail_6[] = {{ CASE_ON( "F6", = ) { EXPECT( a >  b ); } }};

        std::ostringstream os;

        EXPECT( 0 == run( pass  , os ) );
        EXPECT( 1 == run( fail_1, os ) );
        EXPECT( 1 == run( fail_2, os ) );
        EXPECT( 1 == run( fail_3, os ) );
        EXPECT( 1 == run( fail_4, os ) );
        EXPECT( 1 == run( fail_5, os ) );
        EXPECT( 1 == run( fail_6, os ) );
    },

    CASE( "Expect expression RHS can use *, / %, +, -" )
    {
        EXPECT( 7 == 1 * 7 );
        EXPECT( 7 == 7 / 1 );
        EXPECT( 0 == 7 % 1 );
        EXPECT( 7 == 1 + 6 );
        EXPECT( 7 == 8 - 1 );
    },

    CASE( "Expect expression LHS can use *, / %, +, -" )
    {
        EXPECT( 1 * 7 == 7 );
        EXPECT( 7 / 1 == 7 );
        EXPECT( 7 % 1 == 0 );
        EXPECT( 1 + 6 == 7 );
        EXPECT( 8 - 1 == 7 );
    },

    CASE( "Function run() returns the right failure count" )
    {
        test pass  [] = {{ CASE( "P"  ) { EXPECT( 1==1 ); } }};
        test fail_1[] = {{ CASE( "F1" ) { EXPECT( 0==1 ); } }};
        test fail_3[] = {{ CASE( "F1" ) { EXPECT( 0==1 ); } },
                         { CASE( "F2" ) { EXPECT( 0==1 ); } },
                         { CASE( "F3" ) { EXPECT( 0==1 ); } },};

        std::ostringstream os;

        EXPECT( 0 == run( pass  , os ) );
        EXPECT( 1 == run( fail_1, os ) );
        EXPECT( 3 == run( fail_3, os ) );
    },

    CASE( "Expect reports an unexpected standard exception" )
    {
        std::string text = "hello-world";
        test fail[] = {{ CASE_ON( "F", = ) { EXPECT( (throw std::runtime_error(text), true) ); } }};

        std::ostringstream os;

        EXPECT( 1 == run( fail, os ) );
        EXPECT( std::string::npos != os.str().find(text) );
    },

    CASE( "Expect reports an unexpected non-standard exception" )
    {
        test fail[] = {{ CASE( "P" ) { EXPECT( (throw 77, true) ); } }};

        std::ostringstream os;

        EXPECT( 1 == run( fail, os ) );
    },

    CASE( "Expect_no_throw succeeds without an exception" )
    {
        test pass[] = {{ CASE( "P" ) { EXPECT_NO_THROW( true ); } }};

        std::ostringstream os;

        EXPECT( 0 == run( pass, os ) );
    },

    CASE( "Expect_no_throw reports a standard exception" )
    {
        std::string text = "hello-world";
        test fail[] = {{ CASE_ON( "F", = ) { EXPECT_NO_THROW( throw std::runtime_error(text) ); } }};

        std::ostringstream os;

        EXPECT( 1 == run( fail, os ) );
        EXPECT( std::string::npos != os.str().find(text) );
    },

    CASE( "Expect_no_throw reports a non-standard exception" )
    {
        test fail[] = {{ CASE( "F" ) { EXPECT_NO_THROW( (throw 77, true) ); } }};

        std::ostringstream os;

        EXPECT( 1 == run( fail, os ) );
    },

    CASE( "Expect_throws reports a missing exception" )
    {
        test fail[] = {{ CASE( "F" ) { EXPECT_THROWS( true ); } }};

        std::ostringstream os;

        EXPECT( 1 == run( fail, os ) );
    },

    CASE( "Expect_throws succeeds with a standard exception" )
    {
        std::string text = "hello-world";
        test pass[] = {{ CASE_ON( "P", = ) { EXPECT_THROWS( throw std::runtime_error(text) ); } }};

        std::ostringstream os;

        EXPECT( 0 == run( pass, os ) );
    },

    CASE( "Expect_throws succeeds with a non-standard exception" )
    {
        test pass[] = {{ CASE( "P" ) { EXPECT_THROWS( throw 77 ); } }};

        std::ostringstream os;

        EXPECT( 0 == run( pass, os ) );
    },

    CASE( "Expect_throws_as reports a missing exception" )
    {
        test fail[] = {{ CASE( "F" ) { EXPECT_THROWS_AS( true, std::runtime_error ); } }};

        std::ostringstream os;

        EXPECT( 1 == run( fail, os ) );
    },

    CASE( "Expect_throws_as reports getting a different exception" )
    {
        test fail[] = {{ CASE( "F" ) { EXPECT_THROWS_AS( throw std::bad_alloc(), std::runtime_error ); } }};

        std::ostringstream os;

        EXPECT( 1 == run( fail, os ) );
    },

    CASE( "Expect_throws_as succeeds with a specific standard exception" )
    {
        test pass[] = {{ CASE( "P" ) { EXPECT_THROWS_AS( throw std::bad_alloc(), std::bad_alloc ); } }};

        std::ostringstream os;

        EXPECT( 0 == run( pass, os ) );
    },

    CASE( "Expect_throws_as succeeds with a specific non-standard exception" )
    {
        test pass[] = {{ CASE( "P" ) { EXPECT_THROWS_AS( throw 77, int ); } }};

        std::ostringstream os;

        EXPECT( 0 == run( pass, os ) );
    },

    CASE( "Decomposition formats nullptr as string" )
    {
        test pass[] = {{ CASE( "P" ) { EXPECT( nullptr == nullptr  ); } }};
        test fail[] = {{ CASE( "F" ) { EXPECT( nullptr == reinterpret_cast<void*>(1) ); } }};

        std::ostringstream os;

        EXPECT( 0 == run( pass, os ) );
        EXPECT( 1 == run( fail, os ) );

        EXPECT( std::string::npos != os.str().find( "nullptr" ) );
    },

    CASE( "Decomposition formats a pointer as hexadecimal number" )
    {
        test fail[] = {{ CASE( "F" ) { EXPECT( nullptr == reinterpret_cast<void*>(1) ); } }};

        std::ostringstream os;

        EXPECT( 1 == run( fail, os ) );

        EXPECT( std::string::npos != os.str().find( "0x000" /*...001*/) );
    },

    CASE( "Decomposition formats a member function pointer as hexadecimal number" )
    {
        test fail[] = {{ CASE( "F" ) { void (S::*p)() = &S::f; EXPECT( p != p ); } }};

        std::ostringstream os;

        EXPECT( 1 == run( fail, os ) );

        EXPECT( std::string::npos != os.str().find( "0x" ) );
    },

    CASE( "Decomposition formats boolean as strings true and false" )
    {
        test pass[] = {{ CASE( "P" ) { EXPECT( true == true  ); } }};
        test fail[] = {{ CASE( "F" ) { EXPECT( true == false ); } }};

        std::ostringstream os;

        EXPECT( 0 == run( pass, os ) );
        EXPECT( 1 == run( fail, os ) );

        EXPECT( std::string::npos != os.str().find( "true == false for true == false" ) );
    },

    CASE( "Decomposition formats character with single quotes" )
    {
        test pass[] = {{ CASE( "P" ) { EXPECT( 'a' < 'b' ); } }};
        test fail[] = {{ CASE( "F" ) { EXPECT( 'b' < 'a' ); } }};

        std::ostringstream os;

        EXPECT( 0 == run( pass, os ) );
        EXPECT( 1 == run( fail, os ) );

        EXPECT( std::string::npos != os.str().find( "'b' < 'a' for 'b' < 'a'" ) );
    },

    CASE( "Decomposition formats std::string with double quotes" )
    {
        std::string hello( "hello" );
        std::string world( "world" );

        test pass[] = {{ CASE_ON( "P", = ) { EXPECT( hello < "world" ); } }};
        test fail[] = {{ CASE_ON( "F", = ) { EXPECT( world < "hello" ); } }};

        std::ostringstream os;

        EXPECT( 0 == run( pass, os ) );
        EXPECT( 1 == run( fail, os ) );

        EXPECT( std::string::npos != os.str().find( "world < \"hello\" for \"world\" < \"hello\"" ) );
    },

    CASE( "Decomposition formats C string with double quotes" )
    {
        char const * hello( "hello" ); std::string std_hello( "hello" );
        char const * world( "world" ); std::string std_world( "world" );

        test pass[] = {{ CASE_ON( "P", = ) { EXPECT( hello < std_world ); } }};
        test fail[] = {{ CASE_ON( "F", = ) { EXPECT( world < std_hello ); } }};

        std::ostringstream os;

        EXPECT( 0 == run( pass, os ) );
        EXPECT( 1 == run( fail, os ) );

        EXPECT( std::string::npos != os.str().find( "world < std_hello for \"world\" < \"hello\"" ) );
    },

    CASE( "Decomposition formats container with curly braces" )
    {
        std::set<int> s{ 1, 2, 3, };
        std::set<int> t{ 2, 1, 0, };

        test pass[] = {{ CASE_ON( "P", = ) { EXPECT( s == s ); } }};
        test fail[] = {{ CASE_ON( "F", = ) { EXPECT( s == t ); } }};

        std::ostringstream os;

        EXPECT( 0 == run( pass, os ) );
        EXPECT( 1 == run( fail, os ) );

        EXPECT( std::string::npos != os.str().find( "{ 1, 2, 3, }" ) );
        EXPECT( std::string::npos != os.str().find( "{ 0, 1, 2, }" ) );
    },

    CASE( "Has single expression evaluation" )
    {
        test pass[] = {{ CASE( "P" ) { int n = 0; EXPECT( 1 == ++n ); } }};
        test fail[] = {{ CASE( "F" ) { int n = 0; EXPECT( 2 == ++n ); } }};

        std::ostringstream os;

        EXPECT( 0 == run( pass, os ) );
        EXPECT( 1 == run( fail, os ) );

        EXPECT( std::string::npos != os.str().find( "for 2 == 1" ) );
    },
};

int main()
{
    return lest::run( specification );
}

// cl -nologo -Wall -EHsc -I../include test_lest_decompose.cpp && test_lest_decompose
// g++ -Wall -Wextra -std=c++11 -I../include -o test_lest_decompose.exe test_lest_decompose.cpp && test_lest_decompose

