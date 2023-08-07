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

#ifndef EG_COROUTINE
#define EG_COROUTINE

#include "return_reason.hpp"

#ifdef __gnu_linux__
#ifdef __cpp_impl_coroutine
#undef __cpp_impl_coroutine
#endif
#define __cpp_impl_coroutine 1
#endif

#include <coroutine>

namespace mega
{
struct ActionCoroutine
{
    struct promise_type
    {
        ReturnReason m_reason;

        ActionCoroutine get_return_object()
        {
            return ActionCoroutine( std::coroutine_handle< promise_type >::from_promise( *this ), &m_reason );
        }

        auto initial_suspend() { return std::suspend_always{}; } // suspend_never
        auto final_suspend() noexcept { return std::suspend_always{}; }
        void unhandled_exception() 
        {
            throw std::current_exception();
        }

        auto return_value( ReturnReason reason )
        {
            m_reason = reason;
            return std::suspend_always{};
        }
        auto yield_value( ReturnReason reason )
        {
            m_reason = reason;
            return std::suspend_always{};
        }
    };

    ReturnReason*                         m_pReason = nullptr;
    std::coroutine_handle< promise_type > m_coroutine;

    const ReturnReason& getReason() const
    {
        if( m_pReason )
            return *m_pReason;
        else
        {
            static ReturnReason defaultReturnReason;
            return defaultReturnReason;
        }
    }

    ActionCoroutine()                                    = default;
    ActionCoroutine( ActionCoroutine const& )            = delete;
    ActionCoroutine& operator=( ActionCoroutine const& ) = delete;

    explicit ActionCoroutine( std::coroutine_handle< promise_type > coroutine, ReturnReason* pReason )
        : m_coroutine( coroutine )
        , m_pReason( pReason )
    {
    }

    ActionCoroutine( ActionCoroutine&& other )
        : m_coroutine( other.m_coroutine )
        , m_pReason( other.m_pReason )
    {
        other.m_coroutine = nullptr;
    }

    ActionCoroutine& operator=( ActionCoroutine&& other )
    {
        if( &other != this )
        {
            m_coroutine       = other.m_coroutine;
            m_pReason         = other.m_pReason;
            other.m_coroutine = nullptr;
        }
        return *this;
    }

    ~ActionCoroutine()
    {
        if( m_coroutine && !m_coroutine.done() )
        {
            m_coroutine.destroy();
            m_coroutine = nullptr;
        }
    }

    void resume()
    {
        if( m_coroutine )
            m_coroutine.resume();
    }

    void destroy()
    {
        if( m_coroutine )
        {
            m_coroutine.destroy();
            m_coroutine = nullptr;
        }
    }

    bool started() { return m_coroutine ? true : false; }

    bool done()
    {
        if( m_coroutine )
            return m_coroutine.done();
        else
            return true;
    }

    // void await_transform( std::coroutine_handle< promise_type > coroutine )
    //{
    // }
    // bool await_ready( )
    //{
    //     return false;
    // }
    // void await_suspend( std::coroutine_handle< promise_type > coro )
    //{
    // }
    // void await_resume( )
    //{
    // }
};

/*
co_await std::experimental::suspend_always{}
*/

} // namespace mega

#endif // EG_COROUTINE