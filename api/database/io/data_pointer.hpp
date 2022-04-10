#ifndef DATA_POINTER_7_APRIL_2022
#define DATA_POINTER_7_APRIL_2022

#include "object_info.hpp"
#include "object.hpp"
#include "object_loader.hpp"

#include "common/assert_verify.hpp"

#include <variant>

namespace data
{
    template < typename T >
    class Ptr
    {
    public:
        Ptr( ObjectPartLoader& loader )
            : m_loader( loader )
        {
        }
        Ptr( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
            : m_loader( loader )
            , m_objectInfo( objectInfo )
        {
        }
        Ptr( ObjectPartLoader& loader, T* pObjectPart )
            : m_loader( loader )
            , m_objectInfo( pObjectPart->getObjectInfo() )
            , m_pObjectPart( pObjectPart )
        {
        }

        Ptr( const Ptr& copy )
            : m_loader( copy.m_loader )
            , m_objectInfo( copy.m_objectInfo )
            , m_pObjectPart( copy.m_pObjectPart )
        {

        }

        Ptr& operator=( const Ptr& copy )
        {
            VERIFY_RTE( &m_loader == &copy.m_loader );
            if( this != &copy )
            {
                m_objectInfo = copy.m_objectInfo;
                m_pObjectPart = copy.m_pObjectPart;
            }
            return *this;
        }

        inline T& operator*() const { return *get(); }
        inline T* operator->() const { return get(); }

        const mega::io::ObjectInfo& getObjectInfo() const { return m_objectInfo; }

    private:
        T* get() const
        {
            if ( m_pObjectPart )
            {
                return m_pObjectPart;
            }
            else
            {
                // load the object
                m_pObjectPart = dynamic_cast<T*>( m_loader.load( m_objectInfo ) );
            }
            return m_pObjectPart;
        }
        ObjectPartLoader&    m_loader;
        mega::io::ObjectInfo m_objectInfo;
        mutable T*           m_pObjectPart = nullptr;
    };

    template < typename TTo, typename TFrom >
    inline Ptr< TTo > convert( Ptr< TFrom >& from )
    {
        THROW_RTE( "Invalid conversion" );
    }

    template < typename TTo, typename... TFromTypes >
    inline Ptr< TTo > convert( std::variant< TFromTypes... >& from )
    {
        return std::visit( []( auto&& arg ) -> Ptr< TTo > { return convert< TTo >( arg ); }, from );
    }

} // namespace data

#endif // DATA_POINTER_7_APRIL_2022
