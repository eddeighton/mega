#ifndef IO_GENERICS_MAR_25_2022
#define IO_GENERICS_MAR_25_2022

#include "object.hpp"

namespace mega
{
namespace io
{
    template < typename T >
    inline std::vector< T* > many( const Object::Array& objects )
    {
        std::vector< T* > found;
        for ( Object* pObject : objects )
        {
            if ( T* p = dynamic_cast< T* >( pObject ) )
            {
                found.push_back( p );
            }
        }
        return found;
    }

    template < typename T >
    inline T* one( const Object::Array& objects )
    {
        std::vector< T* > found = many< T >( objects );
        VERIFY_RTE( found.size() == 1U );
        return found.front();
    }

    template < typename T >
    inline T* oneOpt( const Object::Array& objects )
    {
        std::vector< T* > found = many< T >( objects );
        if ( !found.empty() )
        {
            VERIFY_RTE( found.size() == 1U );
            return found.front();
        }
        else
        {
            return nullptr;
        }
    }

    template < typename T >
    inline T* root( const Object::Array& objects )
    {
        T* pFound = nullptr;
        for ( Object* pObject : objects )
        {
            if ( T* p = dynamic_cast< T* >( pObject ) )
            {
                if ( p->getParent() == nullptr )
                {
                    VERIFY_RTE( pFound == nullptr );
                    pFound = p;
                }
            }
        }
        VERIFY_RTE( pFound );
        return pFound;
    }

    template < typename T >
    inline std::vector< const T* > many_cst( const Object::Array& objects )
    {
        std::vector< const T* > found;
        for ( Object* pObject : objects )
        {
            if ( const T* p = dynamic_cast< const T* >( pObject ) )
            {
                found.push_back( p );
            }
        }
        return found;
    }

    template < typename T >
    inline const T* one_cst( const Object::Array& objects )
    {
        std::vector< const T* > found = many< const T >( objects );
        VERIFY_RTE( found.size() == 1U );
        return found.front();
    }

    template < typename T >
    inline const T* oneOpt_cst( const Object::Array& objects )
    {
        std::vector< const T* > found = many< const T >( objects );
        if ( !found.empty() )
        {
            VERIFY_RTE( found.size() == 1U );
            return found.front();
        }
        else
        {
            return nullptr;
        }
    }

    template < typename T >
    inline const T* root_cst( const Object::Array& objects )
    {
        const T* pFound = nullptr;
        for ( const Object* pObject : objects )
        {
            if ( const T* p = dynamic_cast< const T* >( pObject ) )
            {
                if ( p->getParent() == nullptr )
                {
                    VERIFY_RTE( pFound == nullptr );
                    pFound = p;
                }
            }
        }
        VERIFY_RTE( pFound );
        return pFound;
    }

    struct CompareIndexedObjects
    {
        inline bool operator()( const Object* pLeft, const Object* pRight ) const
        {
            VERIFY_RTE( pLeft && pRight );
            return ( pLeft->getFileID() != pRight->getFileID() ) ? ( pLeft->getFileID() < pRight->getFileID() ) : ( pLeft->getIndex() < pRight->getIndex() );
        }
    };
} // namespace io
} // namespace mega

#endif // IO_GENERICS_MAR_25_2022
