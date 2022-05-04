#ifndef DATA_POINTER_7_APRIL_2022
#define DATA_POINTER_7_APRIL_2022

#include "loader.hpp"
#include "object_info.hpp"
#include "object.hpp"
#include "object_loader.hpp"
#include "serialisation.hpp"

#include "common/assert_verify.hpp"

#include <variant>
#include <type_traits>

namespace data
{
    template < typename T >
    class Ptr
    {
    public:
        Ptr()
            : m_pLoader( nullptr )
        {
        }

        Ptr( ObjectPartLoader& loader )
            : m_pLoader( &loader )
        {
        }

        Ptr( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
            : m_pLoader( &loader )
            , m_objectInfo( objectInfo )
        {
        }

        Ptr( Ptr& old, const mega::io::ObjectInfo& objectInfo )
            : m_pLoader( old.m_pLoader )
            , m_objectInfo( objectInfo )
        {
        }

        template < typename TOld >
        Ptr( Ptr< TOld >& oldPointerWithLoader, T* pObjectPart )
            : m_pLoader( oldPointerWithLoader.dontUseThisGetLoader() )
            , m_objectInfo( pObjectPart->getObjectInfo() )
            , m_pObjectPart( pObjectPart )
        {
        }

        Ptr( ObjectPartLoader& loader, T* pObjectPart )
            : m_pLoader( &loader )
            , m_objectInfo( pObjectPart->getObjectInfo() )
            , m_pObjectPart( pObjectPart )
        {
        }

        Ptr( T* pObjectPart )
            : m_objectInfo( pObjectPart->getObjectInfo() )
            , m_pObjectPart( pObjectPart )
        {
            VERIFY_RTE( m_pObjectPart );
            VERIFY_RTE( m_pLoader );
        }

        Ptr( const Ptr& copy, const Ptr& copy2 )
            : m_pLoader( copy.m_pLoader )
            , m_objectInfo( copy.m_objectInfo )
            , m_pObjectPart( copy.m_pObjectPart )
        {
        }

        Ptr( const Ptr& copy )
            : m_pLoader( copy.m_pLoader )
            , m_objectInfo( copy.m_objectInfo )
            , m_pObjectPart( copy.m_pObjectPart )
        {
        }

        Ptr& operator=( const Ptr& copy )
        {
            VERIFY_RTE( m_pLoader == copy.m_pLoader );
            if ( this != &copy )
            {
                m_objectInfo  = copy.m_objectInfo;
                m_pObjectPart = copy.m_pObjectPart;
            }
            return *this;
        }

        inline T& operator*() const { return *get(); }
        inline T* operator->() const { return get(); }

        inline bool operator<( const Ptr& cmp ) const { return m_objectInfo < cmp.m_objectInfo; }
        inline bool operator==( const Ptr& cmp ) const { return m_objectInfo == cmp.m_objectInfo; }

        const mega::io::ObjectInfo& getObjectInfo() const { return m_objectInfo; }

        template < class Archive >
        void save( Archive& ar, const unsigned int version ) const
        {
            ar& m_objectInfo;
        }
        template < class Archive >
        void load( Archive& ar, const unsigned int version )
        {
            ar&                           m_objectInfo;
            boost::archive::MegaIArchive& mar = dynamic_cast< boost::archive::MegaIArchive& >( ar );
            m_objectInfo                      = mega::io::ObjectInfo(
                                     m_objectInfo.getType(), mar.m_fileIDLoadedToRuntime[ m_objectInfo.getFileID() ], m_objectInfo.getIndex() );
            m_pLoader = &mar.m_loader;
        }
        BOOST_SERIALIZATION_SPLIT_MEMBER()

        inline ObjectPartLoader* dontUseThisGetLoader() { return m_pLoader; }

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
                VERIFY_RTE( m_pLoader );
                m_pObjectPart = dynamic_cast< T* >( m_pLoader->load( m_objectInfo ) );
                VERIFY_RTE( m_pObjectPart );
            }
            return m_pObjectPart;
        }
        ObjectPartLoader*    m_pLoader;
        mega::io::ObjectInfo m_objectInfo;
        mutable T*           m_pObjectPart = nullptr;
    };

    template < typename T >
    inline void to_json( nlohmann::json& j, const Ptr< T >& p )
    {
        const mega::io::ObjectInfo& objectInfo = p.getObjectInfo();
        j = nlohmann::json{ { "type", objectInfo.getType() }, { "fileID", objectInfo.getFileID() }, { "index", objectInfo.getIndex() } };
    }


    template < typename TTo, typename TFrom >
    inline Ptr< TTo > convert( const Ptr< TFrom >& from )
    {
        if constexpr( std::is_same< TTo, TFrom >::value )
        {
            return from;
        }
        THROW_RTE( "Invalid conversion" );
    }
    
    template < typename TTo, typename... TFromTypes >
    inline Ptr< TTo > convert( const std::variant< TFromTypes... >& from )
    {
        return std::visit( []( auto&& arg ) -> Ptr< TTo > { return convert< TTo >( arg ); }, from );
    }

    template < typename TVariantType, typename TTo, typename TFrom >
    struct UpCast
    {
        inline TVariantType operator()( TFrom& from ) const { return from; }
    };

} // namespace data

#endif // DATA_POINTER_7_APRIL_2022
