
#include "base_task.hpp"

#include "database/model/MemoryStage.hxx"

#include "database/types/sources.hpp"

#include "common/file.hpp"

#include "mega/common.hpp"
#include "mega/allocator.hpp"
#include "mega/default_traits.hpp"

#include <common/stash.hpp>

#include <optional>
#include <vector>
#include <string>

namespace mega
{
namespace compiler
{

template < typename TContext >
inline mega::U64 getSizeTraitSize( const TContext* pInterfaceContext )
{
    using namespace MemoryStage;
    mega::U64                            allocationSize = 1U;
    std::optional< Interface::SizeTrait* > sizeTraitOpt   = pInterfaceContext->get_size_trait();
    if ( sizeTraitOpt.has_value() )
    {
        allocationSize = sizeTraitOpt.value()->get_size();
        VERIFY_RTE_MSG( allocationSize > 0U, "Invalid size for: " << pInterfaceContext->get_identifier() );
    }
    return allocationSize;
}

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
                           MemoryStage::Concrete::Context* pContext, mega::U64 szTotalSize,
                           std::vector< MemoryStage::Concrete::Dimensions::Allocator* >& allocatorDimensions )
    {
        using namespace MemoryStage;
        using namespace MemoryStage::Concrete;

        mega::U64 szDomainSize = 0U;
        if ( Namespace* pNamespace = dynamic_database_cast< Namespace >( pContext ) )
        {
        }
        else if ( Action* pAction = dynamic_database_cast< Action >( pContext ) )
        {
            szDomainSize = getSizeTraitSize( pAction->get_interface_action() );
            szTotalSize *= szDomainSize;

            pAction = database.construct< Action >( Action::Args{ pAction, szDomainSize, szTotalSize } );
        }
        else if ( Event* pEvent = dynamic_database_cast< Event >( pContext ) )
        {
            szDomainSize = getSizeTraitSize( pEvent->get_interface_event() );
            szTotalSize *= szDomainSize;

            pEvent = database.construct< Event >( Event::Args{ pEvent, szDomainSize, szTotalSize } );
        }
        else if ( Function* pFunction = dynamic_database_cast< Function >( pContext ) )
        {
        }
        else if ( Object* pObject = dynamic_database_cast< Object >( pContext ) )
        {
        }
        else if ( Link* pLink = dynamic_database_cast< Link >( pContext ) )
        {
            HyperGraph::Relation* pRelation = pLink->get_link()->get_relation();

            Concrete::Dimensions::LinkReference* pLinkRef = nullptr;
            if ( pRelation->get_source_interface()->get_link_trait()->get_cardinality().maximum().isMany() )
            {
                // range
                pLinkRef = database.construct< Concrete::Dimensions::LinkMany >(
                    Concrete::Dimensions::LinkMany::Args{ Concrete::Dimensions::LinkReference::Args{ pLink } } );
            }
            else
            {
                // singular
                pLinkRef = database.construct< Concrete::Dimensions::LinkSingle >(
                    Concrete::Dimensions::LinkSingle::Args{ Concrete::Dimensions::LinkReference::Args{ pLink } } );
            }

            pLink = database.construct< Link >( Link::Args{ pLink, szTotalSize, pLinkRef } );
        }
        else if ( Buffer* pBuffer = dynamic_database_cast< Buffer >( pContext ) )
        {
            szDomainSize = 1U;
            pBuffer      = database.construct< Buffer >( Buffer::Args{ pBuffer, szTotalSize } );
        }
        else
        {
            THROW_RTE( "Unknown context type" );
        }

        std::vector< Dimensions::Allocator* > nestedAllocatorDimensions;

        for ( Concrete::ContextGroup* pContextGroup : pContext->get_children() )
        {
            if ( Concrete::Context* pChildContext = dynamic_database_cast< Concrete::Context >( pContextGroup ) )
            {
                createallocators( database, pContext, pChildContext, szTotalSize, nestedAllocatorDimensions );
            }
        }

        {
            using namespace MemoryStage::Allocators;
            Allocator* pAllocator                = nullptr;
            bool       bCreateAllocatorDimension = true;
            if ( szDomainSize == 0 )
            {
                pAllocator
                    = database.construct< Nothing >( Nothing::Args{ Allocator::Args{ std::nullopt, pContext } } );
                bCreateAllocatorDimension = false;
            }
            else if ( szDomainSize == 1 )
            {
                pAllocator
                    = database.construct< Singleton >( Singleton::Args{ Allocator::Args{ pParentContext, pContext } } );
            }
            else if ( szDomainSize <= 32 )
            {
                pAllocator = database.construct< Range32 >(
                    Range32::Args{ Range::Args{ Allocator::Args{ pParentContext, pContext } } } );
            }
            else if ( szDomainSize <= 64 )
            {
                pAllocator = database.construct< Range64 >(
                    Range64::Args{ Range::Args{ Allocator::Args{ pParentContext, pContext } } } );
            }
            else
            {
                pAllocator = database.construct< RangeAny >(
                    RangeAny::Args{ Range::Args{ Allocator::Args{ pParentContext, pContext } } } );
            }
            VERIFY_RTE( pAllocator );

            if ( bCreateAllocatorDimension )
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
            pContext
                = database.construct< Context >( Context::Args{ pContext, pAllocator, nestedAllocatorDimensions } );
        }
    }

    struct Parts
    {
        using PartVector = std::vector< MemoryStage::MemoryLayout::Part* >;
        PartVector simpleParts, nonSimpleParts, gpuParts;
    };

    template < typename TContextType >
    void createParts( MemoryStage::Database& database, TContextType* pContext, Parts* pParts )
    {
        using namespace MemoryStage;
        using namespace MemoryStage::Concrete;

        struct PartDimensions
        {
            std::vector< Concrete::Dimensions::User* >          user;
            std::vector< Concrete::Dimensions::LinkReference* > link;
            std::vector< Concrete::Dimensions::Allocation* >    alloc;

            bool empty() const { return user.empty() && link.empty() && alloc.empty(); }

            mega::U64 getLocalDomainSize( MemoryStage::Concrete::Context* pContext ) const
            {
                using namespace MemoryStage;

                if ( Concrete::Object* pObject = dynamic_database_cast< Concrete::Object >( pContext ) )
                {
                    return 1;
                }
                else if ( Concrete::Event* pEvent = dynamic_database_cast< Concrete::Event >( pContext ) )
                {
                    return pEvent->get_local_size();
                }
                else if ( Concrete::Action* pAction = dynamic_database_cast< Concrete::Action >( pContext ) )
                {
                    return pAction->get_local_size();
                }
                else if ( Concrete::Link* pLink = dynamic_database_cast< Concrete::Link >( pContext ) )
                {
                    return 1;
                }
                else
                {
                    return 1;
                }
            }

            mega::U64 setParts( MemoryStage::Database& database, MemoryLayout::Part* pPart )
            {
                mega::U64 szOffset = 0U;
                for ( auto p : user )
                {
                    database.construct< Concrete::Dimensions::User >(
                        Concrete::Dimensions::User::Args{ p, szOffset, pPart } );
                    szOffset += p->get_interface_dimension()->get_size();
                }
                for ( auto p : link )
                {
                    p->set_offset( szOffset );
                    p->set_part( pPart );
                    if ( dynamic_database_cast< Concrete::Dimensions::LinkSingle >( p ) )
                    {
                        szOffset += sizeof( mega::reference );
                    }
                    else if ( dynamic_database_cast< Concrete::Dimensions::LinkMany >( p ) )
                    {
                        szOffset += mega::DimensionTraits< mega::ReferenceVector >::Size;
                        static_assert( mega::DimensionTraits< mega::ReferenceVector >::Size == 32U,
                                       "Something is wrong with mega::DimensionTraits< mega::ReferenceVector >::Size" );
                    }
                    else
                    {
                        THROW_RTE( "Unknown link reference type" );
                    }
                }
                for ( Concrete::Dimensions::Allocation* p : alloc )
                {
                    p->set_offset( szOffset );
                    p->set_part( pPart );

                    Concrete::Dimensions::Allocator* pAllocationDimension
                        = dynamic_database_cast< Concrete::Dimensions::Allocator >( p );
                    VERIFY_RTE( pAllocationDimension );

                    if ( Allocators::Nothing* pAlloc
                         = dynamic_database_cast< Allocators::Nothing >( pAllocationDimension->get_allocator() ) )
                    {
                        THROW_RTE( "Unreachable" );
                    }
                    else if ( Allocators::Singleton* pAlloc = dynamic_database_cast< Allocators::Singleton >(
                                  pAllocationDimension->get_allocator() ) )
                    {
                        szOffset += sizeof( bool );
                    }
                    else if ( Allocators::Range32* pAlloc
                              = dynamic_database_cast< Allocators::Range32 >( pAllocationDimension->get_allocator() ) )
                    {
                        // pAlloc->get_allocated_context()
                        const mega::U64 szLocalDomainSize = getLocalDomainSize( pAlloc->get_allocated_context() );

                        // clang-format off
                        switch( szLocalDomainSize )
                        {
                            case 2:  szOffset += sizeof( mega::Bitmask32Allocator< 2 > ); break;
                            case 3:  szOffset += sizeof( mega::Bitmask32Allocator< 3 > ); break;
                            case 4:  szOffset += sizeof( mega::Bitmask32Allocator< 4 > ); break;
                            case 5:  szOffset += sizeof( mega::Bitmask32Allocator< 5 > ); break;
                            case 6:  szOffset += sizeof( mega::Bitmask32Allocator< 6 > ); break;
                            case 7:  szOffset += sizeof( mega::Bitmask32Allocator< 7 > ); break;
                            case 8:  szOffset += sizeof( mega::Bitmask32Allocator< 8 > ); break;
                            case 9:  szOffset += sizeof( mega::Bitmask32Allocator< 9 > ); break;
                            case 10: szOffset += sizeof( mega::Bitmask32Allocator< 10 > ); break;
                            case 11: szOffset += sizeof( mega::Bitmask32Allocator< 11 > ); break;
                            case 12: szOffset += sizeof( mega::Bitmask32Allocator< 12 > ); break;
                            case 13: szOffset += sizeof( mega::Bitmask32Allocator< 13 > ); break;
                            case 14: szOffset += sizeof( mega::Bitmask32Allocator< 14 > ); break;
                            case 15: szOffset += sizeof( mega::Bitmask32Allocator< 15 > ); break;
                            case 16: szOffset += sizeof( mega::Bitmask32Allocator< 16 > ); break;
                            case 17: szOffset += sizeof( mega::Bitmask32Allocator< 17 > ); break;
                            case 18: szOffset += sizeof( mega::Bitmask32Allocator< 18 > ); break;
                            case 19: szOffset += sizeof( mega::Bitmask32Allocator< 19 > ); break;
                            case 20: szOffset += sizeof( mega::Bitmask32Allocator< 20 > ); break;
                            case 21: szOffset += sizeof( mega::Bitmask32Allocator< 21 > ); break;
                            case 22: szOffset += sizeof( mega::Bitmask32Allocator< 22 > ); break;
                            case 23: szOffset += sizeof( mega::Bitmask32Allocator< 23 > ); break;
                            case 24: szOffset += sizeof( mega::Bitmask32Allocator< 24 > ); break;
                            case 25: szOffset += sizeof( mega::Bitmask32Allocator< 25 > ); break;
                            case 26: szOffset += sizeof( mega::Bitmask32Allocator< 26 > ); break;
                            case 27: szOffset += sizeof( mega::Bitmask32Allocator< 27 > ); break;
                            case 28: szOffset += sizeof( mega::Bitmask32Allocator< 28 > ); break;
                            case 29: szOffset += sizeof( mega::Bitmask32Allocator< 29 > ); break;
                            case 30: szOffset += sizeof( mega::Bitmask32Allocator< 30 > ); break;
                            case 31: szOffset += sizeof( mega::Bitmask32Allocator< 31 > ); break;
                            default:
                                THROW_RTE( "Unsupported allocator type" );
                        }
                        // clang-format on
                    }
                    else if ( Allocators::Range64* pAlloc
                              = dynamic_database_cast< Allocators::Range64 >( pAllocationDimension->get_allocator() ) )
                    {
                        const mega::U64 szLocalDomainSize = getLocalDomainSize( pAlloc->get_allocated_context() );

                        // clang-format off
                        switch( szLocalDomainSize )
                        {
                            case 32: szOffset += sizeof( mega::Bitmask64Allocator< 32 > ); break;
                            case 33: szOffset += sizeof( mega::Bitmask64Allocator< 33 > ); break;
                            case 34: szOffset += sizeof( mega::Bitmask64Allocator< 34 > ); break;
                            case 35: szOffset += sizeof( mega::Bitmask64Allocator< 35 > ); break;
                            case 36: szOffset += sizeof( mega::Bitmask64Allocator< 36 > ); break;
                            case 37: szOffset += sizeof( mega::Bitmask64Allocator< 37 > ); break;
                            case 38: szOffset += sizeof( mega::Bitmask64Allocator< 38 > ); break;
                            case 39: szOffset += sizeof( mega::Bitmask64Allocator< 39 > ); break;
                            case 40: szOffset += sizeof( mega::Bitmask64Allocator< 40 > ); break;
                            case 41: szOffset += sizeof( mega::Bitmask64Allocator< 41 > ); break;
                            case 42: szOffset += sizeof( mega::Bitmask64Allocator< 42 > ); break;
                            case 43: szOffset += sizeof( mega::Bitmask64Allocator< 43 > ); break;
                            case 44: szOffset += sizeof( mega::Bitmask64Allocator< 44 > ); break;
                            case 45: szOffset += sizeof( mega::Bitmask64Allocator< 45 > ); break;
                            case 46: szOffset += sizeof( mega::Bitmask64Allocator< 46 > ); break;
                            case 47: szOffset += sizeof( mega::Bitmask64Allocator< 47 > ); break;
                            case 48: szOffset += sizeof( mega::Bitmask64Allocator< 48 > ); break;
                            case 49: szOffset += sizeof( mega::Bitmask64Allocator< 49 > ); break;
                            case 50: szOffset += sizeof( mega::Bitmask64Allocator< 50 > ); break;
                            case 51: szOffset += sizeof( mega::Bitmask64Allocator< 51 > ); break;
                            case 52: szOffset += sizeof( mega::Bitmask64Allocator< 52 > ); break;
                            case 53: szOffset += sizeof( mega::Bitmask64Allocator< 53 > ); break;
                            case 54: szOffset += sizeof( mega::Bitmask64Allocator< 54 > ); break;
                            case 55: szOffset += sizeof( mega::Bitmask64Allocator< 55 > ); break;
                            case 56: szOffset += sizeof( mega::Bitmask64Allocator< 56 > ); break;
                            case 57: szOffset += sizeof( mega::Bitmask64Allocator< 57 > ); break;
                            case 58: szOffset += sizeof( mega::Bitmask64Allocator< 58 > ); break;
                            case 59: szOffset += sizeof( mega::Bitmask64Allocator< 59 > ); break;
                            case 60: szOffset += sizeof( mega::Bitmask64Allocator< 60 > ); break;
                            case 61: szOffset += sizeof( mega::Bitmask64Allocator< 61 > ); break;
                            case 62: szOffset += sizeof( mega::Bitmask64Allocator< 62 > ); break;
                            case 63: szOffset += sizeof( mega::Bitmask64Allocator< 63 > ); break;
                            default:
                                THROW_RTE( "Unsupported allocator type" );
                        }
                        // clang-format on
                    }
                    else if ( Allocators::RangeAny* pAlloc
                              = dynamic_database_cast< Allocators::RangeAny >( pAllocationDimension->get_allocator() ) )
                    {
                        const mega::U64 szLocalDomainSize = getLocalDomainSize( pAlloc->get_allocated_context() );

                        // clang-format off
                        switch( szLocalDomainSize )
                        {
                            case 64: szOffset += sizeof( mega::RingAllocator< mega::Instance, 64 > ); break;
                            case 80: szOffset += sizeof( mega::RingAllocator< mega::Instance, 80 > ); break;
                            case 96: szOffset += sizeof( mega::RingAllocator< mega::Instance, 96 > ); break;
                            case 112: szOffset += sizeof( mega::RingAllocator< mega::Instance, 112 > ); break;
                            case 128: szOffset += sizeof( mega::RingAllocator< mega::Instance, 128 > ); break;
                            case 144: szOffset += sizeof( mega::RingAllocator< mega::Instance, 144 > ); break;
                            case 160: szOffset += sizeof( mega::RingAllocator< mega::Instance, 160 > ); break;
                            case 176: szOffset += sizeof( mega::RingAllocator< mega::Instance, 176 > ); break;
                            case 192: szOffset += sizeof( mega::RingAllocator< mega::Instance, 192 > ); break;
                            case 208: szOffset += sizeof( mega::RingAllocator< mega::Instance, 208 > ); break;
                            case 224: szOffset += sizeof( mega::RingAllocator< mega::Instance, 224 > ); break;
                            case 240: szOffset += sizeof( mega::RingAllocator< mega::Instance, 240 > ); break;
                            case 256: szOffset += sizeof( mega::RingAllocator< mega::Instance, 256 > ); break;
                            case 272: szOffset += sizeof( mega::RingAllocator< mega::Instance, 272 > ); break;
                            case 288: szOffset += sizeof( mega::RingAllocator< mega::Instance, 288 > ); break;
                            case 304: szOffset += sizeof( mega::RingAllocator< mega::Instance, 304 > ); break;
                            case 320: szOffset += sizeof( mega::RingAllocator< mega::Instance, 320 > ); break;
                            case 336: szOffset += sizeof( mega::RingAllocator< mega::Instance, 336 > ); break;
                            case 352: szOffset += sizeof( mega::RingAllocator< mega::Instance, 352 > ); break;
                            case 368: szOffset += sizeof( mega::RingAllocator< mega::Instance, 368 > ); break;
                            case 384: szOffset += sizeof( mega::RingAllocator< mega::Instance, 384 > ); break;
                            case 400: szOffset += sizeof( mega::RingAllocator< mega::Instance, 400 > ); break;
                            case 416: szOffset += sizeof( mega::RingAllocator< mega::Instance, 416 > ); break;
                            case 432: szOffset += sizeof( mega::RingAllocator< mega::Instance, 432 > ); break;
                            case 448: szOffset += sizeof( mega::RingAllocator< mega::Instance, 448 > ); break;
                            case 464: szOffset += sizeof( mega::RingAllocator< mega::Instance, 464 > ); break;
                            case 480: szOffset += sizeof( mega::RingAllocator< mega::Instance, 480 > ); break;
                            case 496: szOffset += sizeof( mega::RingAllocator< mega::Instance, 496 > ); break;
                            case 512: szOffset += sizeof( mega::RingAllocator< mega::Instance, 512 > ); break;
                            case 528: szOffset += sizeof( mega::RingAllocator< mega::Instance, 528 > ); break;
                            case 544: szOffset += sizeof( mega::RingAllocator< mega::Instance, 544 > ); break;
                            case 560: szOffset += sizeof( mega::RingAllocator< mega::Instance, 560 > ); break;
                            case 576: szOffset += sizeof( mega::RingAllocator< mega::Instance, 576 > ); break;
                            case 592: szOffset += sizeof( mega::RingAllocator< mega::Instance, 592 > ); break;
                            case 608: szOffset += sizeof( mega::RingAllocator< mega::Instance, 608 > ); break;
                            case 624: szOffset += sizeof( mega::RingAllocator< mega::Instance, 624 > ); break;
                            case 640: szOffset += sizeof( mega::RingAllocator< mega::Instance, 640 > ); break;
                            case 656: szOffset += sizeof( mega::RingAllocator< mega::Instance, 656 > ); break;
                            case 672: szOffset += sizeof( mega::RingAllocator< mega::Instance, 672 > ); break;
                            case 688: szOffset += sizeof( mega::RingAllocator< mega::Instance, 688 > ); break;
                            case 704: szOffset += sizeof( mega::RingAllocator< mega::Instance, 704 > ); break;
                            case 720: szOffset += sizeof( mega::RingAllocator< mega::Instance, 720 > ); break;
                            case 736: szOffset += sizeof( mega::RingAllocator< mega::Instance, 736 > ); break;
                            case 752: szOffset += sizeof( mega::RingAllocator< mega::Instance, 752 > ); break;
                            case 768: szOffset += sizeof( mega::RingAllocator< mega::Instance, 768 > ); break;
                            case 784: szOffset += sizeof( mega::RingAllocator< mega::Instance, 784 > ); break;
                            case 800: szOffset += sizeof( mega::RingAllocator< mega::Instance, 800 > ); break;
                            case 816: szOffset += sizeof( mega::RingAllocator< mega::Instance, 816 > ); break;
                            case 832: szOffset += sizeof( mega::RingAllocator< mega::Instance, 832 > ); break;
                            case 848: szOffset += sizeof( mega::RingAllocator< mega::Instance, 848 > ); break;
                            case 864: szOffset += sizeof( mega::RingAllocator< mega::Instance, 864 > ); break;
                            case 880: szOffset += sizeof( mega::RingAllocator< mega::Instance, 880 > ); break;
                            case 896: szOffset += sizeof( mega::RingAllocator< mega::Instance, 896 > ); break;
                            case 912: szOffset += sizeof( mega::RingAllocator< mega::Instance, 912 > ); break;
                            case 928: szOffset += sizeof( mega::RingAllocator< mega::Instance, 928 > ); break;
                            case 944: szOffset += sizeof( mega::RingAllocator< mega::Instance, 944 > ); break;
                            case 960: szOffset += sizeof( mega::RingAllocator< mega::Instance, 960 > ); break;
                            case 976: szOffset += sizeof( mega::RingAllocator< mega::Instance, 976 > ); break;
                            case 992: szOffset += sizeof( mega::RingAllocator< mega::Instance, 992 > ); break;
                            case 1008: szOffset += sizeof( mega::RingAllocator< mega::Instance, 1008 > ); break;
                            case 1024: szOffset += sizeof( mega::RingAllocator< mega::Instance, 1024 > ); break;
                            default:
                                THROW_RTE( "Unsupported allocator type" );
                        }
                        // clang-format on
                    }
                    else
                    {
                        THROW_RTE( "Unknown allocator type" );
                    }
                }
                return szOffset;
            }
        };
        PartDimensions simple, nonSimple, gpu;

        mega::U64 szTotalDomainSize;
        {
            if constexpr ( std::is_same< TContextType, Concrete::Object >::value )
            {
                szTotalDomainSize = 1U;
            }
            if constexpr ( std::is_same< TContextType, Concrete::Event >::value )
            {
                szTotalDomainSize = pContext->get_total_size();
            }
            if constexpr ( std::is_same< TContextType, Concrete::Action >::value )
            {
                szTotalDomainSize = pContext->get_total_size();
            }
            if constexpr ( std::is_same< TContextType, Concrete::Link >::value )
            {
                szTotalDomainSize = pContext->get_total_size();
            }
            if constexpr ( std::is_same< TContextType, Concrete::Buffer >::value )
            {
                szTotalDomainSize = pContext->get_total_size();
            }
            else
            {
                szTotalDomainSize = 1U;
            }
        }

        for ( Concrete::Dimensions::Allocation* pAllocation : pContext->get_allocation_dimensions() )
        {
            simple.alloc.push_back( pAllocation );
        }

        if constexpr ( std::is_same< TContextType, Concrete::Link >::value )
        {
            simple.link.push_back( pContext->get_link_reference() );
        }
        else
        {
            for ( Concrete::Dimensions::User* pDim : pContext->get_dimensions() )
            {
                if ( pDim->get_interface_dimension()->get_simple() )
                {
                    simple.user.push_back( pDim );
                }
                else
                {
                    nonSimple.user.push_back( pDim );
                }
            }
        }

        if ( !simple.empty() )
        {
            MemoryLayout::Part* pContextPart = database.construct< MemoryLayout::Part >(
                MemoryLayout::Part::Args{ szTotalDomainSize, pContext, simple.user, simple.link, simple.alloc } );
            const mega::U64 szSize = simple.setParts( database, pContextPart );
            pContextPart->set_size( szSize );
            pParts->simpleParts.push_back( pContextPart );
        }
        if ( !nonSimple.empty() )
        {
            MemoryLayout::Part* pContextPart = database.construct< MemoryLayout::Part >( MemoryLayout::Part::Args{
                szTotalDomainSize, pContext, nonSimple.user, nonSimple.link, nonSimple.alloc } );
            const mega::U64   szSize       = nonSimple.setParts( database, pContextPart );
            pContextPart->set_size( szSize );
            pParts->nonSimpleParts.push_back( pContextPart );
        }
        if ( !gpu.empty() )
        {
            MemoryLayout::Part* pContextPart = database.construct< MemoryLayout::Part >(
                MemoryLayout::Part::Args{ szTotalDomainSize, pContext, gpu.user, gpu.link, gpu.alloc } );
            const mega::U64 szSize = gpu.setParts( database, pContextPart );
            pContextPart->set_size( szSize );
            pParts->gpuParts.push_back( pContextPart );
        }
    }

    void createBuffers( MemoryStage::Database& database, MemoryStage::Concrete::Context* pParentContext,
                        MemoryStage::Concrete::Context* pContext, Parts* pParts )
    {
        using namespace MemoryStage;
        using namespace MemoryStage::Concrete;

        if ( Object* pObject = dynamic_database_cast< Object >( pContext ) )
        {
            Parts parts;

            createParts( database, pObject, &parts );

            for ( Concrete::ContextGroup* pContextGroup : pContext->get_children() )
            {
                if ( Concrete::Context* pChildContext = dynamic_database_cast< Concrete::Context >( pContextGroup ) )
                {
                    createBuffers( database, pContext, pChildContext, &parts );
                }
            }

            std::vector< MemoryLayout::Buffer* > objectBuffers;
            if ( !parts.simpleParts.empty() )
            {
                mega::U64 szOffset = 0U;
                for ( auto p : parts.simpleParts )
                {
                    p->set_offset( szOffset );
                    szOffset += ( p->get_size() * p->get_total_domain_size() );
                }

                MemoryLayout::SimpleBuffer* pSimpleBuffer = database.construct< MemoryLayout::SimpleBuffer >(
                    MemoryLayout::SimpleBuffer::Args{ MemoryLayout::Buffer::Args{ parts.simpleParts, szOffset } } );
                objectBuffers.push_back( pSimpleBuffer );
            }
            if ( !parts.nonSimpleParts.empty() )
            {
                mega::U64 szOffset = 0U;
                for ( auto p : parts.nonSimpleParts )
                {
                    p->set_offset( szOffset );
                    szOffset += ( p->get_size() * p->get_total_domain_size() );
                }

                MemoryLayout::NonSimpleBuffer* pNonSimpleBuffer = database.construct< MemoryLayout::NonSimpleBuffer >(
                    MemoryLayout::NonSimpleBuffer::Args{ MemoryLayout::Buffer::Args{ parts.nonSimpleParts, szOffset } } );
                objectBuffers.push_back( pNonSimpleBuffer );
            }
            if ( !parts.gpuParts.empty() )
            {
                mega::U64 szOffset = 0U;
                for ( auto p : parts.gpuParts )
                {
                    p->set_offset( szOffset );
                    szOffset += ( p->get_size() * p->get_total_domain_size() );
                }

                MemoryLayout::GPUBuffer* pGPUBuffer = database.construct< MemoryLayout::GPUBuffer >(
                    MemoryLayout::GPUBuffer::Args{ MemoryLayout::Buffer::Args{ parts.gpuParts, szOffset } } );
                objectBuffers.push_back( pGPUBuffer );
            }

            pObject = database.construct< Object >( Object::Args{ pObject, objectBuffers } );
        }
        else
        {
            if ( pParts )
            {
                if ( Namespace* pNamespace = dynamic_database_cast< Namespace >( pContext ) )
                {
                }
                else if ( Action* pAction = dynamic_database_cast< Action >( pContext ) )
                {
                    createParts( database, pAction, pParts );
                }
                else if ( Event* pEvent = dynamic_database_cast< Event >( pContext ) )
                {
                    createParts( database, pEvent, pParts );
                }
                else if ( Function* pFunction = dynamic_database_cast< Function >( pContext ) )
                {
                }
                else if ( Link* pLink = dynamic_database_cast< Link >( pContext ) )
                {
                    createParts( database, pLink, pParts );
                }
                else if ( Buffer* pBuffer = dynamic_database_cast< Buffer >( pContext ) )
                {
                }
                else
                {
                    THROW_RTE( "Unknown context type" );
                }
            }

            for ( Concrete::ContextGroup* pContextGroup : pContext->get_children() )
            {
                if ( Concrete::Context* pChildContext = dynamic_database_cast< Concrete::Context >( pContextGroup ) )
                {
                    createBuffers( database, pContext, pChildContext, pParts );
                }
            }
        }
    }

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

        if ( m_environment.restore( compilationFile, determinant ) )
        {
            m_environment.setBuildHashCode( compilationFile );
            cached( taskProgress );
            return;
        }

        using namespace MemoryStage;

        Database database( m_environment, m_sourceFilePath );

        Concrete::Root* pConcreteRoot = database.one< Concrete::Root >( m_sourceFilePath );
        for ( Concrete::ContextGroup* pContextGroup : pConcreteRoot->get_children() )
        {
            if ( Concrete::Context* pContext = dynamic_database_cast< Concrete::Context >( pContextGroup ) )
            {
                std::vector< Concrete::Dimensions::Allocator* > nestedAllocatorDimensions;
                createallocators( database, nullptr, pContext, 1, nestedAllocatorDimensions );
                VERIFY_RTE( nestedAllocatorDimensions.empty() );
            }
        }
        for ( Concrete::ContextGroup* pContextGroup : pConcreteRoot->get_children() )
        {
            if ( Concrete::Context* pContext = dynamic_database_cast< Concrete::Context >( pContextGroup ) )
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

} // namespace compiler
} // namespace mega
