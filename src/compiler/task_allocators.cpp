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

#include "base_task.hpp"

#include "database/model/MemoryStage.hxx"
#include "database/types/sources.hpp"

#include "mega/operation_id.hpp"
#include "mega/allocator.hpp"
#include "mega/types/traits.hpp"
#include "mega/memory.hpp"

#include "common/file.hpp"
#include <common/stash.hpp>

#include <optional>
#include <vector>
#include <string>

namespace MemoryStage
{
#include "compiler/interface.hpp"
}

namespace
{
mega::U64 getBitmask32AllocatorSize( mega::U64 szLocalDomainSize )
{
    // clang-format off
    switch( szLocalDomainSize )
    {
#define ALLOCATOR( manged_name, type, size ) case size: return sizeof( type ); break;
#include "mega/types/allocators_32.xmc"
#undef ALLOCATOR
        default:
            THROW_RTE( "Unsupported allocator type" );
    }
    // clang-format on
}
mega::U64 getBitmask32AllocatorAlignment( mega::U64 szLocalDomainSize )
{
    // clang-format off
    switch( szLocalDomainSize )
    {
#define ALLOCATOR( manged_name, type, size ) case size: return alignof( type ); break;
#include "mega/types/allocators_32.xmc"
#undef ALLOCATOR
        default:
            THROW_RTE( "Unsupported allocator type" );
    }
    // clang-format on
}

mega::U64 getBitmask64AllocatorSize( mega::U64 szLocalDomainSize )
{
    // clang-format off
    switch( szLocalDomainSize )
    {
#define ALLOCATOR( manged_name, type, size ) case size: return sizeof( type ); break;
#include "mega/types/allocators_64.xmc"
#undef ALLOCATOR
        default:
            THROW_RTE( "Unsupported allocator type" );
    }
    // clang-format on
}

mega::U64 getBitmask64AllocatorAlignment( mega::U64 szLocalDomainSize )
{
    // clang-format off
    switch( szLocalDomainSize )
    {
#define ALLOCATOR( manged_name, type, size ) case size: return alignof( type ); break;
#include "mega/types/allocators_64.xmc"
#undef ALLOCATOR
        default:
            THROW_RTE( "Unsupported allocator type" );
    }
    // clang-format on
}

mega::U64 getRingAllocatorSize( mega::U64 szLocalDomainSize )
{
    // clang-format off
    switch( szLocalDomainSize )
    {
#define ALLOCATOR( manged_name, type, size ) case size: return sizeof( type ); break;
#include "mega/types/allocators_128.xmc"
#undef ALLOCATOR
        default:
            THROW_RTE( "Unsupported allocator type" );
    }
    // clang-format on
}

mega::U64 getRingAllocatorAlignment( mega::U64 szLocalDomainSize )
{
    // clang-format off
    switch( szLocalDomainSize )
    {
#define ALLOCATOR( manged_name, type, size ) case size: return alignof( type ); break;
#include "mega/types/allocators_128.xmc"
#undef ALLOCATOR
        default:
            THROW_RTE( "Unsupported allocator type" );
    }
    // clang-format on
}
} // namespace

namespace mega::compiler
{

class Task_Allocators : public BaseTask
{
    const mega::io::megaFilePath& m_sourceFilePath;

public:
    Task_Allocators( const TaskArguments& taskArguments, const mega::io::megaFilePath& sourceFilePath )
        : BaseTask( taskArguments )
        , m_sourceFilePath( sourceFilePath )
    {
    }

    void createallocators( MemoryStage::Database& database, MemoryStage::Concrete::Context* pParentContext,
                           MemoryStage::Concrete::Context* pContext, U64 szTotalSize,
                           std::vector< MemoryStage::Concrete::Dimensions::Allocator* >& allocatorDimensions );

    struct Parts
    {
        using PartVector = std::vector< MemoryStage::MemoryLayout::Part* >;
        // PartVector simpleParts, nonSimpleParts, gpuParts;
        PartVector parts;
    };

    void createParts( MemoryStage::Database& database, MemoryStage::Concrete::Context* pContext, Parts* pParts );
    void createBuffers( MemoryStage::Database& database, MemoryStage::Concrete::Context* pParentContext,
                        MemoryStage::Concrete::Context* pContext, Parts* pParts );

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        const mega::io::CompilationFilePath concrete = m_environment.ConcreteStage_Concrete( m_sourceFilePath );
        const mega::io::CompilationFilePath compilationFile
            = m_environment.MemoryStage_MemoryLayout( m_sourceFilePath );

        start( taskProgress, "Task_Allocators", m_sourceFilePath.path(), compilationFile.path() );

        const task::DeterminantHash determinant(
            { m_toolChain.toolChainHash,
              m_environment.getBuildHashCode( m_environment.ParserStage_AST( m_sourceFilePath ) ),
              m_environment.getBuildHashCode( m_environment.InterfaceStage_Tree( m_sourceFilePath ) ),
              m_environment.getBuildHashCode( concrete ) } );

        if( m_environment.restore( compilationFile, determinant ) )
        {
            m_environment.setBuildHashCode( compilationFile );
            cached( taskProgress );
            return;
        }

        using namespace MemoryStage;

        Database database( m_environment, m_sourceFilePath );

        Concrete::Root* pConcreteRoot = database.one< Concrete::Root >( m_sourceFilePath );
        for( Concrete::ContextGroup* pContextGroup : pConcreteRoot->get_children() )
        {
            if( auto pContext = db_cast< Concrete::Context >( pContextGroup ) )
            {
                std::vector< Concrete::Dimensions::Allocator* > nestedAllocatorDimensions;
                createallocators( database, nullptr, pContext, 1, nestedAllocatorDimensions );
                VERIFY_RTE( nestedAllocatorDimensions.empty() );
            }
        }
        for( Concrete::ContextGroup* pContextGroup : pConcreteRoot->get_children() )
        {
            if( auto pContext = db_cast< Concrete::Context >( pContextGroup ) )
            {
                createBuffers( database, nullptr, pContext, nullptr );
            }
        }

        const task::FileHash fileHashCode = database.save_MemoryLayout_to_temp();
        m_environment.setBuildHashCode( compilationFile, fileHashCode );
        m_environment.temp_to_real( compilationFile );
        m_environment.stash( compilationFile, determinant );

        succeeded( taskProgress );
    }
};

BaseTask::Ptr create_Task_Allocators( const TaskArguments& taskArguments, const mega::io::megaFilePath& sourceFilePath )
{
    return std::make_unique< Task_Allocators >( taskArguments, sourceFilePath );
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
// createallocators
void Task_Allocators::createallocators(
    MemoryStage::Database& database, MemoryStage::Concrete::Context* pParentContext,
    MemoryStage::Concrete::Context* pContext, U64 szTotalSize,
    std::vector< MemoryStage::Concrete::Dimensions::Allocator* >& allocatorDimensions )
{
    using namespace MemoryStage;
    using namespace MemoryStage::Concrete;

    U64 szDomainSize = 0U;
    if( auto pNamespace = db_cast< Namespace >( pContext ) )
    {
    }
    else if( auto pState = db_cast< State >( pContext ) )
    {
        szDomainSize = getSizeTraitSize( pState->get_interface_state() );
        szTotalSize *= szDomainSize;

        pState = database.construct< State >( State::Args{ pState, szDomainSize, szTotalSize } );
    }
    else if( auto pEvent = db_cast< Event >( pContext ) )
    {
        szDomainSize = getSizeTraitSize( pEvent->get_interface_event() );
        szTotalSize *= szDomainSize;

        pEvent = database.construct< Event >( Event::Args{ pEvent, szDomainSize, szTotalSize } );
    }
    else if( auto pInterupt = db_cast< Interupt >( pContext ) )
    {
    }
    else if( auto pFunction = db_cast< Function >( pContext ) )
    {
    }
    else if( auto pObject = db_cast< Object >( pContext ) )
    {
    }
    else
    {
        THROW_RTE( "Unknown context type" );
    }

    std::vector< Dimensions::Allocator* > nestedAllocatorDimensions;

    for( Concrete::ContextGroup* pContextGroup : pContext->get_children() )
    {
        if( auto pChildContext = db_cast< Concrete::Context >( pContextGroup ) )
        {
            createallocators( database, pContext, pChildContext, szTotalSize, nestedAllocatorDimensions );
        }
    }

    {
        using namespace MemoryStage::Allocators;
        Allocator* pAllocator                = nullptr;
        bool       bCreateAllocatorDimension = true;
        if( szDomainSize == 0 )
        {
            pAllocator = database.construct< Nothing >( Nothing::Args{ Allocator::Args{ std::nullopt, pContext } } );
            bCreateAllocatorDimension = false;
        }
        else if( szDomainSize == 1 )
        {
            pAllocator
                = database.construct< Singleton >( Singleton::Args{ Allocator::Args{ pParentContext, pContext } } );
        }
        else if( szDomainSize <= 32 )
        {
            pAllocator = database.construct< Range32 >(
                Range32::Args{ Range::Args{ Allocator::Args{ pParentContext, pContext }, szDomainSize } } );
        }
        else if( szDomainSize <= 64 )
        {
            pAllocator = database.construct< Range64 >(
                Range64::Args{ Range::Args{ Allocator::Args{ pParentContext, pContext }, szDomainSize } } );
        }
        else
        {
            pAllocator = database.construct< RangeAny >(
                RangeAny::Args{ Range::Args{ Allocator::Args{ pParentContext, pContext }, szDomainSize } } );
        }
        VERIFY_RTE( pAllocator );

        if( bCreateAllocatorDimension )
        {
            Dimensions::Allocator* pAllocatorDim = database.construct< Dimensions::Allocator >(
                Dimensions::Allocator::Args{ Dimensions::Allocation::Args{ pContext }, pAllocator } );
            pAllocator->set_dimension( pAllocatorDim );
            allocatorDimensions.push_back( pAllocatorDim );
        }
        else
        {
            pAllocator->set_dimension( std::nullopt );
        }
        pContext = database.construct< Context >( Context::Args{ pContext, pAllocator, nestedAllocatorDimensions } );
    }
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
// createParts

static U64 padToAlignment( U64 alignment, U64 offset )
{
    const U64 diff = offset % alignment;
    if( diff )
    {
        return offset + ( alignment - diff );
    }
    else
    {
        return offset;
    }
}

using namespace MemoryStage;
using namespace MemoryStage::Concrete;

struct PartDimensions
{
    std::vector< Concrete::Dimensions::User* >       user;
    std::vector< Concrete::Dimensions::Link* >       link;
    std::vector< Concrete::Dimensions::Allocation* > alloc;

    bool empty() const { return user.empty() && link.empty() && alloc.empty(); }

    U64 getLocalDomainSize( MemoryStage::Concrete::Context* pContext ) const
    {
        using namespace MemoryStage;

        if( auto pObject = db_cast< Concrete::Object >( pContext ) )
        {
            return 1;
        }
        else if( auto pEvent = db_cast< Concrete::Event >( pContext ) )
        {
            return pEvent->get_local_size();
        }
        else if( auto pState = db_cast< Concrete::State >( pContext ) )
        {
            return pState->get_local_size();
        }
        else
        {
            return 1;
        }
    }

    mega::SizeAlignment calculatePartLayout( MemoryStage::Database& database, MemoryLayout::Part* pPart )
    {
        mega::SizeAlignment result;
        {
            for( auto p : user )
            {
                const U64 szAlign = p->get_interface_dimension()->get_alignment();
                const U64 szSize  = p->get_interface_dimension()->get_size();
                result.alignment  = std::max( result.alignment, szAlign );
                result.size       = padToAlignment( szAlign, result.size );
                database.construct< Concrete::Dimensions::User >(
                    Concrete::Dimensions::User::Args{ p, result.size, pPart } );
                result.size += szSize;
            }
        }
        {
            for( auto pLink : link )
            {
                Concrete::Dimensions::LinkType* pLinkType = nullptr;
                {
                    const U64 szAlign = alignof( mega::TypeID );
                    const U64 szSize  = sizeof( mega::TypeID );
                    result.alignment  = std::max( result.alignment, szAlign );
                    result.size       = padToAlignment( szAlign, result.size );
                
                    pLinkType = database.construct< Concrete::Dimensions::LinkType >(
                        Concrete::Dimensions::LinkType::Args{ 
                            pLink->get_parent_context(), result.size, pPart } 
                    );
                    result.size += szSize;
                }

                if( pLink->get_singular() )
                {
                    const U64 szAlign = alignof( mega::reference );
                    const U64 szSize  = sizeof( mega::reference );
                    result.alignment  = std::max( result.alignment, szAlign );
                    result.size       = padToAlignment( szAlign, result.size );
                    auto pLink2 = database.construct< Concrete::Dimensions::Link >(
                        Concrete::Dimensions::Link::Args{ pLink, result.size, pPart, pLinkType } );
                    pLinkType->set_link( pLink2 );
                    result.size += szSize;
                }
                else
                {
                    const U64 szAlign = mega::DimensionTraits< mega::ReferenceVector >::Alignment;
                    const U64 szSize  = mega::DimensionTraits< mega::ReferenceVector >::Size;
                    result.alignment  = std::max( result.alignment, szAlign );
                    result.size       = padToAlignment( szAlign, result.size );
                    auto pLink2 = database.construct< Concrete::Dimensions::Link >(
                        Concrete::Dimensions::Link::Args{ pLink, result.size, pPart, pLinkType } );
                    pLinkType->set_link( pLink2 );
                    result.size += szSize;
                }
            }
        }
        {
            for( Concrete::Dimensions::Allocation* p : alloc )
            {
                p->set_part( pPart );

                auto pAllocationDimension = db_cast< Concrete::Dimensions::Allocator >( p );
                VERIFY_RTE( pAllocationDimension );

                if( auto pAlloc = db_cast< Allocators::Nothing >( pAllocationDimension->get_allocator() ) )
                {
                    THROW_RTE( "Unreachable" );
                }
                else if( auto pAlloc = db_cast< Allocators::Singleton >( pAllocationDimension->get_allocator() ) )
                {
                    const U64 szAlign = alignof( bool );
                    const U64 szSize  = sizeof( bool );
                    result.alignment  = std::max( result.alignment, szAlign );
                    result.size       = padToAlignment( szAlign, result.size );
                    p->set_offset( result.size );
                    result.size += szSize;
                }
                else if( auto pAlloc = db_cast< Allocators::Range32 >( pAllocationDimension->get_allocator() ) )
                {
                    const U64 szLocalDomainSize = getLocalDomainSize( pAlloc->get_allocated_context() );
                    const U64 szSize            = getBitmask32AllocatorSize( szLocalDomainSize );
                    const U64 szAlign           = getBitmask32AllocatorAlignment( szLocalDomainSize );
                    result.alignment            = std::max( result.alignment, szAlign );
                    result.size                 = padToAlignment( szAlign, result.size );
                    p->set_offset( result.size );
                    result.size += szSize;
                }
                else if( auto pAlloc = db_cast< Allocators::Range64 >( pAllocationDimension->get_allocator() ) )
                {
                    const U64 szLocalDomainSize = getLocalDomainSize( pAlloc->get_allocated_context() );
                    const U64 szSize            = getBitmask64AllocatorSize( szLocalDomainSize );
                    const U64 szAlign           = getBitmask64AllocatorAlignment( szLocalDomainSize );
                    result.alignment            = std::max( result.alignment, szAlign );
                    result.size                 = padToAlignment( szAlign, result.size );
                    p->set_offset( result.size );
                    result.size += szSize;
                }
                else if( auto pAlloc = db_cast< Allocators::RangeAny >( pAllocationDimension->get_allocator() ) )
                {
                    const U64 szLocalDomainSize = getLocalDomainSize( pAlloc->get_allocated_context() );
                    const U64 szSize            = getRingAllocatorSize( szLocalDomainSize );
                    const U64 szAlign           = getRingAllocatorAlignment( szLocalDomainSize );
                    result.alignment            = std::max( result.alignment, szAlign );
                    result.size                 = padToAlignment( szAlign, result.size );
                    p->set_offset( result.size );
                    result.size += szSize;
                }
                else
                {
                    THROW_RTE( "Unknown allocator type" );
                }
            }
        }

        // ensure the entire part is padded to its own alignment to allow array of the part
        result.size = padToAlignment( result.alignment, result.size );
        return result;
    }

    MemoryLayout::Part* createPart( MemoryStage::Database& database, MemoryStage::Concrete::Context* pContext,
                                    U64 szTotalDomainSize )
    {
        MemoryLayout::Part* pContextPart = database.construct< MemoryLayout::Part >(
            MemoryLayout::Part::Args{ szTotalDomainSize, pContext, user, link, alloc } );
        const mega::SizeAlignment sizeAlignment = calculatePartLayout( database, pContextPart );
        pContextPart->set_size( sizeAlignment.size );
        pContextPart->set_alignment( sizeAlignment.alignment );
        return pContextPart;
    }
};

void Task_Allocators::createParts( MemoryStage::Database& database, Concrete::Context* pContext, Parts* pParts )
{
    using namespace MemoryStage;
    using namespace MemoryStage::Concrete;

    PartDimensions simple, nonSimple, gpu;

    U64 szTotalDomainSize;
    {
        if( auto pUserDimensionContext = db_cast< Concrete::UserDimensionContext >( pContext ) )
        {
            for( auto pDim : pUserDimensionContext->get_dimensions() )
            {
                if( pDim->get_interface_dimension()->get_simple() )
                {
                    simple.user.push_back( pDim );
                }
                else
                {
                    nonSimple.user.push_back( pDim );
                }
            }
            for( auto pLink : pUserDimensionContext->get_links() )
            {
                if( pLink->get_singular() )
                {
                    simple.link.push_back( pLink );
                }
                else
                {
                    nonSimple.link.push_back( pLink );
                }
            }
        }

        if( Concrete::Namespace* pNamespace = db_cast< Concrete::Namespace >( pContext ) )
        {
            szTotalDomainSize = 1U;
        }
        else if( Concrete::Object* pObject = db_cast< Concrete::Object >( pContext ) )
        {
            szTotalDomainSize = 1U;
        }
        else if( Concrete::State* pState = db_cast< Concrete::State >( pContext ) )
        {
            szTotalDomainSize = pState->get_total_size();
        }
        else if( Concrete::Event* pEvent = db_cast< Concrete::Event >( pContext ) )
        {
            szTotalDomainSize = pEvent->get_total_size();
        }
        else if( Concrete::Interupt* pFunction = db_cast< Concrete::Interupt >( pContext ) )
        {
            szTotalDomainSize = 1U;
        }
        else if( Concrete::Function* pFunction = db_cast< Concrete::Function >( pContext ) )
        {
            szTotalDomainSize = 1U;
        }
        else
        {
            THROW_RTE( "Unknown context type" );
        }
    }

    for( Concrete::Dimensions::Allocation* pAllocation : pContext->get_allocation_dimensions() )
    {
        simple.alloc.push_back( pAllocation );
    }

    if( !simple.empty() )
    {
        MemoryLayout::Part* pContextPart = simple.createPart( database, pContext, szTotalDomainSize );
        // pParts->simpleParts.push_back( pContextPart );
        pParts->parts.push_back( pContextPart );
    }
    if( !nonSimple.empty() )
    {
        MemoryLayout::Part* pContextPart = nonSimple.createPart( database, pContext, szTotalDomainSize );
        // pParts->nonSimpleParts.push_back( pContextPart );
        pParts->parts.push_back( pContextPart );
    }
    if( !gpu.empty() )
    {
        MemoryLayout::Part* pContextPart = gpu.createPart( database, pContext, szTotalDomainSize );
        // pParts->gpuParts.push_back( pContextPart );
        pParts->parts.push_back( pContextPart );
    }
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
// createBuffers
void Task_Allocators::createBuffers( MemoryStage::Database& database, MemoryStage::Concrete::Context* pParentContext,
                                     MemoryStage::Concrete::Context* pContext, Parts* pParts )
{
    using namespace MemoryStage;
    using namespace MemoryStage::Concrete;

    if( Object* pObject = db_cast< Object >( pContext ) )
    {
        Parts parts;
        {
            createParts( database, pObject, &parts );
            for( Concrete::ContextGroup* pContextGroup : pContext->get_children() )
            {
                if( Concrete::Context* pChildContext = db_cast< Concrete::Context >( pContextGroup ) )
                {
                    createBuffers( database, pContext, pChildContext, &parts );
                }
            }
        }

        std::vector< MemoryLayout::Buffer* > objectBuffers;
        if( !parts.parts.empty() )
        {
            // time for some genius - cannot include mega::runtime::ObjectHeader due to dependencies
            // so redefine
            struct FakeObjectHeader : public mega::ObjectHeaderBase
            {
                std::shared_ptr< void > m_pAllocator;
            };

            U64 szOffset    = sizeof( FakeObjectHeader );
            U64 szAlignment = alignof( FakeObjectHeader );
            for( auto p : parts.parts )
            {
                szOffset = padToAlignment( p->get_alignment(), szOffset );
                p->set_offset( szOffset );
                szOffset += ( p->get_size() * p->get_total_domain_size() );
                szAlignment = std::max( szAlignment, p->get_alignment() );
            }

            MemoryLayout::SimpleBuffer* pSimpleBuffer = database.construct< MemoryLayout::SimpleBuffer >(
                MemoryLayout::SimpleBuffer::Args{ MemoryLayout::Buffer::Args{ parts.parts, szOffset, szAlignment } } );
            objectBuffers.push_back( pSimpleBuffer );

            for( auto p : parts.parts )
            {
                p->set_buffer( pSimpleBuffer );
            }
        }
        /*if ( !parts.nonSimpleParts.empty() )
        {
            U64 szOffset    = 0U;
            U64 szAlignment = 1U;
            for ( auto p : parts.nonSimpleParts )
            {
                szOffset = padToAlignment( p->get_alignment(), szOffset );
                p->set_offset( szOffset );
                szOffset += ( p->get_size() * p->get_total_domain_size() );
                szAlignment = std::max( szAlignment, p->get_alignment() );
            }

            MemoryLayout::NonSimpleBuffer* pNonSimpleBuffer
                = database.construct< MemoryLayout::NonSimpleBuffer >( MemoryLayout::NonSimpleBuffer::Args{
                    MemoryLayout::Buffer::Args{ parts.nonSimpleParts, szOffset, szAlignment } } );
            objectBuffers.push_back( pNonSimpleBuffer );

            for ( auto p : parts.nonSimpleParts )
            {
                p->set_buffer( pNonSimpleBuffer );
            }
        }
        if ( !parts.gpuParts.empty() )
        {
            U64 szOffset    = 0U;
            U64 szAlignment = 1U;
            for ( auto p : parts.gpuParts )
            {
                szOffset = padToAlignment( p->get_alignment(), szOffset );
                p->set_offset( szOffset );
                szOffset += ( p->get_size() * p->get_total_domain_size() );
                szAlignment = std::max( szAlignment, p->get_alignment() );
            }

            MemoryLayout::GPUBuffer* pGPUBuffer = database.construct< MemoryLayout::GPUBuffer >(
                MemoryLayout::GPUBuffer::Args{ MemoryLayout::Buffer::Args{ parts.gpuParts, szOffset, szAlignment } } );
            objectBuffers.push_back( pGPUBuffer );

            for ( auto p : parts.gpuParts )
            {
                p->set_buffer( pGPUBuffer );
            }
        }*/

        pObject = database.construct< Object >( Object::Args{ pObject, objectBuffers } );
    }
    else
    {
        if( pParts )
        {
            if( auto pNamespace = db_cast< Namespace >( pContext ) )
            {
            }
            else if( auto pState = db_cast< State >( pContext ) )
            {
                createParts( database, pState, pParts );
            }
            else if( auto pEvent = db_cast< Event >( pContext ) )
            {
                createParts( database, pEvent, pParts );
            }
            else if( auto pFunction = db_cast< Function >( pContext ) )
            {
            }
            else if( auto pInterupt = db_cast< Interupt >( pContext ) )
            {
            }
            else
            {
                THROW_RTE( "Unknown context type" );
            }
        }

        for( Concrete::ContextGroup* pContextGroup : pContext->get_children() )
        {
            if( auto pChildContext = db_cast< Concrete::Context >( pContextGroup ) )
            {
                createBuffers( database, pContext, pChildContext, pParts );
            }
        }
    }
}

} // namespace mega::compiler
