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




#ifndef EG_ITERATORS_02_05_2019
#define EG_ITERATORS_02_05_2019

#include "mega/clock.hpp"

#include <array>
#include <vector>

template< typename T >
using __mega_vector = std::vector< T >;

/*
template< typename ReferenceType >
inline mega::TimeStamp getTimestamp( mega::TypeID type, mega::Instance instance );

template< typename ReferenceType >
inline mega::ActionState getState( mega::TypeID type, mega::Instance instance );

template< typename ReferenceType >
inline mega::TimeStamp getStopCycle( mega::TypeID type, mega::Instance instance );

template< typename ReferenceType >
inline bool isActionActive( mega::TypeID type, mega::Instance instance )
{
    switch( getState< ReferenceType >( type, instance ) )
    {
        case mega::action_running : return true;
        case mega::action_paused  : return true;
        case mega::action_stopped :
            if( getStopCycle< ReferenceType >( type, instance ) == clock::cycle( type ) )
                return true;
            else
                return false;
        case mega::TOTAL_ACTION_STATES  : return false;
    }
}

template< class ReferenceType >
class __eg_ReferenceRawIterator : public std::iterator< std::forward_iterator_tag, ReferenceType >
{
public:
    using value_type = ReferenceType;
    mega::Instance instance;
    mega::Instance sentinal;
    mega::TypeID type;
    inline __eg_ReferenceRawIterator( mega::Instance instance, mega::Instance sentinal, mega::TypeID type ) : instance( instance ), sentinal( sentinal ), type( type ) {}
    inline __eg_ReferenceRawIterator( const __eg_ReferenceRawIterator& from ) : instance( from.instance ), sentinal( from.sentinal ), type( from.type ) {}
    inline __eg_ReferenceRawIterator& operator++()
    {
        ++instance;
        return *this;
    }
    inline __eg_ReferenceRawIterator operator++(int) {__eg_ReferenceRawIterator tmp(*this); operator++(); return tmp;}
    inline bool operator==(const __eg_ReferenceRawIterator& rhs) const {return (instance==rhs.instance) && (type==rhs.type);}
    inline bool operator!=(const __eg_ReferenceRawIterator& rhs) const {return !(rhs==*this);}
    inline const value_type operator*() const
    {
        return mega::Pointer{ instance, type, getTimestamp< ReferenceType >( type, instance ) };
    }
};

template< class ReferenceType >
class __eg_ReferenceIterator : public std::iterator< std::forward_iterator_tag, ReferenceType >
{
public:
    using value_type = ReferenceType;
    mega::Instance instance;
    mega::Instance sentinal;
    mega::TypeID type;
    inline __eg_ReferenceIterator( mega::Instance instance, mega::Instance sentinal, mega::TypeID type ) : instance( instance ), sentinal( sentinal ), type( type ) {}
    inline __eg_ReferenceIterator( const __eg_ReferenceIterator& from ) : instance( from.instance ), sentinal( from.sentinal ), type( from.type ) {}
    inline __eg_ReferenceIterator& operator++()
    {
        while( true )
        {
            ++instance;
            if( ( instance == sentinal ) || ( isActionActive< ReferenceType >( type, instance ) ) )
                break;
        }
        return *this;
    }
    inline __eg_ReferenceIterator operator++(int) {__eg_ReferenceIterator tmp(*this); operator++(); return tmp;}
    inline bool operator==(const __eg_ReferenceIterator& rhs) const {return (instance==rhs.instance) && (type==rhs.type);}
    inline bool operator!=(const __eg_ReferenceIterator& rhs) const {return !(rhs==*this);}
    inline const value_type operator*() const
    {
        return mega::Pointer{ instance, type, getTimestamp< ReferenceType >( type, instance ) };
    }
};

template< class IteratorType, mega::Instance SIZE >
class __eg_MultiIterator : public std::iterator< std::forward_iterator_tag, typename IteratorType::value_type >
{
public:
    using IteratorArray = std::array< IteratorType, SIZE >;
    using value_type = typename IteratorType::value_type;
private:
    IteratorArray iterators;
    mega::Instance szIndex = 0U;
public:

    inline __eg_MultiIterator()
        :   szIndex( SIZE )
    {
    }
    
    inline __eg_MultiIterator( const __eg_MultiIterator& cpy )
        :   iterators( cpy.iterators ),
            szIndex( cpy.szIndex )
    {
        
    }
    
    inline __eg_MultiIterator( const IteratorArray& iters ) 
        :   iterators( iters ),
            szIndex( 0U )
    {
        while( szIndex != SIZE )
        {
            if( iterators[ szIndex ].instance == iterators[ szIndex ].sentinal )
                ++szIndex;
            else
                break;
        }
    }
    
    inline __eg_MultiIterator& operator++()
    {
        if( iterators[ szIndex ].instance != iterators[ szIndex ].sentinal )
            ++iterators[ szIndex ];
        if( iterators[ szIndex ].instance == iterators[ szIndex ].sentinal )
        {
            while( ( szIndex != SIZE ) && ( iterators[ szIndex ].instance == iterators[ szIndex ].sentinal ) )
                ++szIndex;
        }
        return *this;
    }
    
    inline __eg_MultiIterator operator++(int) 
    {
        __eg_MultiIterator tmp(*this); 
        this->operator++(); 
        return tmp;
    }
    
    inline bool operator==(const __eg_MultiIterator& rhs) const 
    {
        return (szIndex==rhs.szIndex) && 
            ( ( szIndex >= SIZE) || ( iterators[ szIndex ] == rhs.iterators[ szIndex ] ) );
    }
    inline bool operator!=(const __eg_MultiIterator& rhs) const {return !(rhs==*this);}
    
    inline const value_type operator*() const
    {
        return *iterators[ szIndex ];
    }
};

template< typename Iterator >
struct __eg_Range
{
    using iterator_type = Iterator;
    Iterator _begin, _end;
    __eg_Range( Iterator _begin, Iterator _end ) : _begin( _begin ), _end( _end ) {}
    Iterator begin() const { return _begin; }
    Iterator end() const { return _end; }
}; 
    
namespace mega
{
    template< typename Result, typename RangeType >
    inline Result one( const RangeType& range )
    {
        for( typename RangeType::iterator_type 
            i = range.begin(), iEnd = range.end();
            i!=iEnd;  )
        {
            return *i;
        }
        return Result();
    }
    
    template< typename RangeType >
    inline mega::U64 count( const RangeType& range )
    {
        return std::distance( range.begin(), range.end() );
    }
               

} //namespace mega

*/

#endif //EG_ITERATORS_02_05_2019