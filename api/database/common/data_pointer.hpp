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
#include <memory>

namespace data
{
template < typename T >
class Ptr
{
    using ObjectInfoPtr = std::shared_ptr< mega::io::ObjectInfo >;

public:
    Ptr()
        : m_pLoader( nullptr )
        , m_pObjectInfo( std::make_shared< mega::io::ObjectInfo >() )
    {
    }

    Ptr( ObjectPartLoader& loader )
        : m_pLoader( &loader )
        , m_pObjectInfo( std::make_shared< mega::io::ObjectInfo >() )
    {
    }

    Ptr( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        : m_pLoader( &loader )
        , m_pObjectInfo( std::make_shared< mega::io::ObjectInfo >( objectInfo ) )
    {
    }

    Ptr( Ptr& old, const mega::io::ObjectInfo& objectInfo )
        : m_pLoader( old.m_pLoader )
        , m_pObjectInfo( std::make_shared< mega::io::ObjectInfo >( objectInfo ) )
    {
    }

    template < typename TOld >
    Ptr( Ptr< TOld >& oldPointerWithLoader, T* pObjectPart )
        : m_pLoader( oldPointerWithLoader.dontUseThisGetLoader() )
        , m_pObjectInfo( std::make_shared< mega::io::ObjectInfo >( pObjectPart->getObjectInfo() ) )
        , m_pObjectPart( pObjectPart )
    {
    }

    Ptr( ObjectPartLoader& loader, T* pObjectPart )
        : m_pLoader( &loader )
        , m_pObjectInfo( std::make_shared< mega::io::ObjectInfo >( pObjectPart->getObjectInfo() ) )
        , m_pObjectPart( pObjectPart )
    {
    }

    Ptr( T* pObjectPart )
        : m_pObjectInfo( std::make_shared< mega::io::ObjectInfo >( pObjectPart->getObjectInfo() ) )
        , m_pObjectPart( pObjectPart )
    {
        VERIFY_RTE( m_pObjectPart );
        VERIFY_RTE( m_pLoader );
    }

    Ptr( const Ptr& copy, const Ptr& copy2 )
        : m_pLoader( copy.m_pLoader )
        , m_pObjectInfo( copy.m_pObjectInfo )
        , m_pObjectPart( copy.m_pObjectPart )
    {
    }

    Ptr( const Ptr& copy )
        : m_pLoader( copy.m_pLoader )
        , m_pObjectInfo( copy.m_pObjectInfo )
        , m_pObjectPart( copy.m_pObjectPart )
    {
    }

    Ptr& operator=( const Ptr& copy )
    {
        if ( m_pLoader == nullptr )
        {
            m_pLoader = copy.m_pLoader;
        }
        VERIFY_RTE( m_pLoader == copy.m_pLoader );
        if ( this != &copy )
        {
            m_pObjectInfo = copy.m_pObjectInfo;
            m_pObjectPart = copy.m_pObjectPart;
        }
        return *this;
    }

    inline T& operator*() const { return *get(); }
    inline T* operator->() const { return get(); }

    const mega::io::ObjectInfo& getObjectInfo() const { return *m_pObjectInfo; }

    inline bool operator<( const Ptr& cmp ) const { return getObjectInfo() < cmp.getObjectInfo(); }
    inline bool operator==( const Ptr& cmp ) const { return getObjectInfo() == cmp.getObjectInfo(); }

    template < typename Archive >
    void save( Archive& ar, const unsigned int version ) const
    {
        ar&* m_pObjectInfo;

        boost::archive::MegaOArchive& mar = dynamic_cast< boost::archive::MegaOArchive& >( ar );
        mar.objectInfo( m_pObjectInfo.get() );
    }

    template < typename Archive >
    void load( Archive& ar, const unsigned int version )
    {
        ar&* m_pObjectInfo;

        boost::archive::MegaIArchive& mar = dynamic_cast< boost::archive::MegaIArchive& >( ar );
        mar.objectInfo( m_pObjectInfo.get() );
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
            VERIFY_RTE( m_pLoader && m_pObjectInfo );
            mega::io::Object* pObject = m_pLoader->load( *m_pObjectInfo );
            // using c-cast here circumvents the need for undefined types
            m_pObjectPart = ( T* )pObject;
            VERIFY_RTE( m_pObjectPart );
        }
        return m_pObjectPart;
    }
    ObjectPartLoader* m_pLoader;
    ObjectInfoPtr     m_pObjectInfo;
    mutable T*        m_pObjectPart = nullptr;
};

template < typename T >
inline void to_json( nlohmann::json& j, const Ptr< T >& p )
{
    const mega::io::ObjectInfo& objectInfo = p.getObjectInfo();
    j                                      = nlohmann::json{
        { "type", objectInfo.getType() }, { "fileID", objectInfo.getFileID() }, { "index", objectInfo.getIndex() } };
}

template < typename TTo, typename TFrom >
inline Ptr< TTo > convert( const Ptr< TFrom >& from )
{
    if constexpr ( std::is_same< TTo, TFrom >::value )
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

template < typename TVariantTo, typename TVariantFrom >
inline TVariantTo coerceVariant( const TVariantFrom& variantFrom )
{
    return std::visit( []( auto&& arg ) -> TVariantTo 
    { 
        if constexpr( std::is_convertible< TVariantFrom, TVariantTo >::value )
        {
            return TVariantTo{ arg }; 
        }
        if constexpr( !std::is_convertible< TVariantFrom, TVariantTo >::value )
        {
            THROW_RTE( "to_upper variant" );
        }
    }, variantFrom );
}

template < typename TVariant >
inline TVariant to_upper( TVariant& from )
{
    TVariant up_most = from;
    for ( ;; )
    {
        TVariant up_most_next = std::visit(
            []( auto&& arg ) -> TVariant { return coerceVariant< TVariant >( arg->m_inheritance ); }, up_most );
        if ( up_most_next == up_most )
            break;
        up_most = up_most_next;
    }
    return up_most;
}

class Factory
{
public:
    static mega::io::Object* create( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
};

} // namespace data

#endif // DATA_POINTER_7_APRIL_2022
