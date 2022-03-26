//  Copyright (c) Deighton Systems Limited. 2019. All Rights Reserved.
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

#ifndef ALLOCATOR_23_JUNE_2020
#define ALLOCATOR_23_JUNE_2020

#include "database/model/objects.hpp"

namespace mega
{
namespace Stages
{
    class Interface;
}

namespace concrete
{
    class Action;
    class Dimension_Generated;
/*
    class Allocator : public io::Object
    {
        friend class mega::Stages::Interface;

    protected:
        Allocator( const io::Object& indexedObject )
            : io::Object( indexedObject )
        {
        }

        virtual void load( io::Loader& loader );
        virtual void store( io::Storer& storer ) const;

        virtual void constructDimensions( Stages::Interface& stage ) = 0;

    public:
        const Action* getAllocated() const { return m_pContext_Allocated; }
        const Action* getAllocating() const { return m_pContext_Allocating; }

    protected:
        Action* m_pContext_Allocated = nullptr;
        Action* m_pContext_Allocating = nullptr;
    };

    class NothingAllocator : public Allocator
    {
        friend class mega::Stages::Interface;
        friend Allocator* chooseAllocator( Stages::Interface&, Action*, Action* );

    public:
        static const ObjectType Type = eConcreteAllocator_Nothing;

    protected:
        NothingAllocator( const io::Object& indexedObject )
            : Allocator( indexedObject )
        {
        }

        virtual void load( io::Loader& loader );
        virtual void store( io::Storer& storer ) const;

        virtual void constructDimensions( Stages::Interface& stage );
    };

    class SingletonAllocator : public Allocator
    {
        friend class mega::Stages::Interface;
        friend Allocator* chooseAllocator( Stages::Interface&, Action*, Action* );

    public:
        static const ObjectType Type = eConcreteAllocator_Singleton;

    protected:
        SingletonAllocator( const io::Object& indexedObject )
            : Allocator( indexedObject )
        {
        }

        virtual void load( io::Loader& loader );
        virtual void store( io::Storer& storer ) const;

        virtual void constructDimensions( Stages::Interface& stage );
    };

    class RangeAllocator : public Allocator
    {
        friend class mega::Stages::Interface;
        friend Allocator* chooseAllocator( Stages::Interface&, Action*, Action* );

    public:
        static const ObjectType Type = eConcreteAllocator_Range;

    protected:
        RangeAllocator( const io::Object& indexedObject )
            : Allocator( indexedObject )
        {
        }

        virtual void load( io::Loader& loader );
        virtual void store( io::Storer& storer ) const;

    public:
        const Dimension_Generated* getAllocatorData() const { return m_pAllocatorData; }
        std::string                getAllocatorType() const;

    protected:
        virtual void constructDimensions( Stages::Interface& stage );

        Dimension_Generated* m_pAllocatorData = nullptr;
    };

    Allocator* chooseAllocator( Stages::Interface& stage, Action* pParent, Action* pChild );
*/
} // namespace concrete
} // namespace mega

#endif // ALLOCATOR_23_JUNE_2020