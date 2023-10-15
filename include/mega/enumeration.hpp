
//  Copyright (c) Deighton Systems Limited. 2022. All Rights Reserved.
//  Author: Edward Deighton
//  License: Please see license.txt in the project root folder.

//  Use and copying of this software and preparation of derivative works
//  based upon this software are permitted. Any copy of this software or
//  of any derivative work must include the above copyright notice, this
//  paragraph and the one after it.  Any distribution of this software or
//  derivative works must comply with all applicable laws.

//  This software is made available AS IS, and COPYRIGHT OWNERS DISCLAIMS
//  ALL WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
//  PURPOSE, AND NOTWITHSTANDING ANY OTHER PROVISION CONTAINED HEREIN, ANY
//  LIABILITY FOR DAMAGES RESULTING FROM THE SOFTWARE OR ITS USE IS
//  EXPRESSLY DISCLAIMED, WHETHER ARISING IN CONTRACT, TORT (INCLUDING
//  NEGLIGENCE) OR STRICT LIABILITY, EVEN IF COPYRIGHT OWNERS ARE ADVISED
//  OF THE POSSIBILITY OF SUCH DAMAGES.

#ifndef GUARD_2023_October_14_enumeration
#define GUARD_2023_October_14_enumeration

#ifdef __gnu_linux__
#ifdef __cpp_impl_coroutine
#undef __cpp_impl_coroutine
#endif
#define __cpp_impl_coroutine 1
#endif

#include <coroutine>
#include <exception>

namespace mega
{

template < typename T >
struct Generator
{
    // The class name 'Generator' is our choice and it is not required for coroutine
    // magic. Compiler recognizes coroutine by the presence of 'co_yield' keyword.
    // You can use name 'MyGenerator' (or any other name) instead as long as you include
    // nested struct promise_type with 'MyGenerator get_return_object()' method.

    struct promise_type;
    using handle_type = std::coroutine_handle< promise_type >;

    struct promise_type // required
    {
        T                  value_;
        std::exception_ptr exception_;

        Generator           get_return_object() { return Generator( handle_type::from_promise( *this ) ); }
        std::suspend_always initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        void                unhandled_exception()
        {
            exception_ = std::current_exception();
        } // saving
          // exception

        template < std::convertible_to< T > From > // C++20 concept
        std::suspend_always yield_value( From&& from )
        {
            value_ = std::forward< From >( from ); // caching the result in promise
            return {};
        }
        void return_void() {}
    };

    handle_type h_;

    Generator( handle_type h )
        : h_( h )
    {
    }

    ~Generator() { h_.destroy(); }

    explicit operator bool()
    {
        fill(); // The only way to reliably find out whether or not we finished coroutine,
                // whether or not there is going to be a next value generated (co_yield)
                // in coroutine via C++ getter (operator () below) is to execute/resume
                // coroutine until the next co_yield point (or let it fall off end).
                // Then we store/cache result in promise to allow getter (operator() below
                // to grab it without executing coroutine).
        return !h_.done();
    }

    T operator()()
    {
        fill();
        full_ = false; // we are going to move out previously cached
                       // result to make promise empty again
        return std::move( h_.promise().value_ );
    }

private:
    bool full_ = false;

    void fill()
    {
        if( !full_ )
        {
            h_();
            if( h_.promise().exception_ )
                std::rethrow_exception( h_.promise().exception_ );
            // propagate coroutine exception in called context

            full_ = true;
        }
    }
};

} // namespace mega

#endif // GUARD_2023_October_14_enumeration
