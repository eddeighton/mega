#ifndef IO_GENERICS_MAR_25_2022
#define IO_GENERICS_MAR_25_2022

#include "object.hpp"

#include <array>
#include <iterator>

namespace mega
{
namespace io
{
    template < typename Iterator >
    struct Range
    {
        using iterator_type = Iterator;
        Iterator _begin, _end;
        Range( Iterator _begin, Iterator _end )
            : _begin( _begin )
            , _end( _end )
        {
        }
        Iterator begin() const { return _begin; }
        Iterator end() const { return _end; }

        bool operator=( const Range& cmp ) const
        {
            return _begin == cmp._begin && _end == cmp._end;
        }
    };

    template < class IteratorType >
    class MultiRangeIter : public std::iterator< std::forward_iterator_tag, typename IteratorType::value_type >
    {
    public:
        using RangeType = Range< IteratorType >;
        using RangeVector = std::vector< RangeType >;
        using value_type = typename IteratorType::value_type;

    private:
        RangeVector  ranges;
        std::size_t  szIndex = 0U;
        IteratorType iter;

    public:
        inline MultiRangeIter()
            : ranges( { RangeType{ IteratorType{}, IteratorType{} } } )
            , szIndex( 0 )
        {
        }

        inline MultiRangeIter( const MultiRangeIter& cpy )
            : ranges( cpy.ranges )
            , szIndex( cpy.szIndex )
            , iter( cpy.iter )
        {
        }

        inline MultiRangeIter( const RangeVector& iters )
            : ranges( iters )
            , szIndex( 0U )
        {
            while ( szIndex != ranges.size() )
            {
                iter = ranges[ szIndex ].begin();
                if ( iter == ranges[ szIndex ].end() )
                    ++szIndex;
                else
                    break;
            }
        }

        inline MultiRangeIter( const RangeVector& iters, bool atEnd )
            : ranges( iters )
            , szIndex( ranges.size() )
        {
            if( !ranges.empty() )
            {
                iter = ranges.back().end();
            }
        }

        inline MultiRangeIter& operator++()
        {
            ++iter;
            while ( ( szIndex != ranges.size() ) && ( iter == ranges[ szIndex ].end() ) )
            {
                ++szIndex;
                if ( szIndex != ranges.size() )
                    iter = ranges[ szIndex ].begin();
            }
            return *this;
        }

        inline MultiRangeIter operator++( int )
        {
            MultiRangeIter tmp( *this );
            this->         operator++();
            return tmp;
        }

        inline bool operator==( const MultiRangeIter& rhs ) const
        {
            return ( szIndex == rhs.szIndex ) && ( ranges.size() == rhs.ranges.size() ) &&
                   // NOTE: either the index is at end ( then does not matter if iter uninitialized
                   // OR the iterators are exactly the same value ( and must be initialized )
                   ( ( szIndex == ranges.size() ) || ( iter == rhs.iter ) );
        }
        inline bool operator!=( const MultiRangeIter& rhs ) const { return !( rhs == *this ); }

        inline const value_type operator*() const
        {
            return *iter;
        }
    };

    template < typename T, typename RangeType >
    inline std::vector< T* > many( const RangeType& range )
    {
        std::vector< T* > found;
        for ( auto value : range )
        {
            if ( T* p = dynamic_cast< T* >( value ) )
            {
                found.push_back( p );
            }
        }
        return found;
    }

    template < typename T, typename RangeType >
    inline T* one( const RangeType& range )
    {
        std::vector< T* > found = many< T >( range );
        VERIFY_RTE( found.size() == 1U );
        return found.front();
    }

    template < typename T, typename RangeType >
    inline T* oneOpt( const RangeType& range )
    {
        std::vector< T* > found = many< T >( range );
        VERIFY_RTE( found.size() <= 1U );
        if( !found.empty() )
            return found.back();
        else
            return nullptr;
    }

} // namespace io
} // namespace mega

#endif // IO_GENERICS_MAR_25_2022
