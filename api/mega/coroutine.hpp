

#ifndef EG_COROUTINE
#define EG_COROUTINE

#include "common.hpp"
#include "event.hpp"

#ifdef __cpp_impl_coroutine
#undef __cpp_impl_coroutine
#endif

#define __cpp_impl_coroutine 1
#include <coroutine>

#include <chrono>
#include <optional>
#include <vector>

namespace mega
{
enum Reason
{
    eReason_Wait,
    eReason_Wait_All,
    eReason_Wait_Any,
    eReason_Sleep,
    eReason_Sleep_All,
    eReason_Sleep_Any,
    eReason_Timeout,
    eReason_Terminated
};

struct ReturnReason
{
    Reason                                                 reason;
    std::vector< Event >                                   events;
    std::optional< std::chrono::steady_clock::time_point > timeout;

    ReturnReason()
        : reason( eReason_Terminated )
    {
    }

    ReturnReason( Reason _reason )
        : reason( _reason )
    {
    }

    ReturnReason( Reason _reason, const Event& event )
        : reason( _reason )
        , events( 1, event )
    {
    }

    ReturnReason( Reason _reason, std::initializer_list< Event > _events )
        : reason( _reason )
        , events( _events )
    {
    }

    ReturnReason( const std::chrono::steady_clock::time_point& _timeout )
        : reason( eReason_Timeout )
        , timeout( _timeout )
    {
    }
};

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
        void unhandled_exception() {}

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

    const ReturnReason getReason() const
    {
        if ( m_pReason )
            return *m_pReason;
        else
            return ReturnReason();
    }

    ActionCoroutine()                         = default;
    ActionCoroutine( ActionCoroutine const& ) = delete;
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
        if ( &other != this )
        {
            m_coroutine       = other.m_coroutine;
            m_pReason         = other.m_pReason;
            other.m_coroutine = nullptr;
        }
        return *this;
    }

    ~ActionCoroutine()
    {
        if ( m_coroutine && !m_coroutine.done() )
        {
            m_coroutine.destroy();
            m_coroutine = nullptr;
        }
    }

    void resume()
    {
        if ( m_coroutine )
            m_coroutine.resume();
    }

    void destroy()
    {
        if ( m_coroutine )
        {
            m_coroutine.destroy();
            m_coroutine = nullptr;
        }
    }

    bool started() { return m_coroutine ? true : false; }

    bool done()
    {
        if ( m_coroutine )
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