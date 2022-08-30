
#include "base_task.hpp"

#include "database/model/MemoryStage.hxx"

#include "database/types/sources.hpp"

#include "common/file.hpp"
#include <common/stash.hpp>

#include <optional>
#include <vector>
#include <string>

namespace mega
{
namespace compiler
{

template < typename TContext >
inline std::size_t getSizeTraitSize( const TContext* pInterfaceContext )
{
    using namespace MemoryStage;
    std::size_t                            allocationSize = 1U;
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
                           MemoryStage::Concrete::Context* pContext, std::size_t szTotalSize,
                           std::vector< MemoryStage::Concrete::Dimensions::Allocator* >& allocatorDimensions )
    {
        using namespace MemoryStage;
        using namespace MemoryStage::Concrete;

        std::size_t szSize = 0U;
        if ( Namespace* pNamespace = dynamic_database_cast< Namespace >( pContext ) )
        {
        }
        else if ( Action* pAction = dynamic_database_cast< Action >( pContext ) )
        {
            szSize = getSizeTraitSize( pAction->get_interface_action() );
            szTotalSize *= szSize;

            pAction = database.construct< Action >( Action::Args{ pAction, szSize, szTotalSize } );
        }
        else if ( Event* pEvent = dynamic_database_cast< Event >( pContext ) )
        {
            szSize = getSizeTraitSize( pEvent->get_interface_event() );
            szTotalSize *= szSize;

            pEvent = database.construct< Event >( Event::Args{ pEvent, szSize, szTotalSize } );
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
            szSize  = 1U;
            pBuffer = database.construct< Buffer >( Buffer::Args{ pBuffer, szTotalSize } );
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
            if ( szSize == 0 )
            {
                pAllocator
                    = database.construct< Nothing >( Nothing::Args{ Allocator::Args{ std::nullopt, pContext } } );
                bCreateAllocatorDimension = false;
            }
            else if ( szSize == 1 )
            {
                pAllocator
                    = database.construct< Singleton >( Singleton::Args{ Allocator::Args{ pParentContext, pContext } } );
            }
            else if ( szSize <= 32 )
            {
                pAllocator = database.construct< Range32 >(
                    Range32::Args{ Range::Args{ Allocator::Args{ pParentContext, pContext } } } );
            }
            else if ( szSize <= 64 )
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

        struct Dimensions
        {
            std::vector< Concrete::Dimensions::User* >          user;
            std::vector< Concrete::Dimensions::LinkReference* > link;
            std::vector< Concrete::Dimensions::Allocation* >    alloc;

            bool empty() const { return user.empty() && link.empty() && alloc.empty(); }

            void setParts( MemoryStage::Database& database, MemoryLayout::Part* pPart )
            {
                for ( auto p : user )
                {
                    database.construct< Concrete::Dimensions::User >( Concrete::Dimensions::User::Args{ p, pPart } );
                }
                for ( auto p : link )
                    p->set_part( pPart );
                for ( auto p : alloc )
                    p->set_part( pPart );
            }
        };
        Dimensions simple, nonSimple, gpu;

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
                Interface::DimensionTrait* pTrait = pDim->get_interface_dimension();
                if ( pTrait->get_simple() )
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
                MemoryLayout::Part::Args{ pContext, simple.user, simple.link, simple.alloc } );
            simple.setParts( database, pContextPart );
            pParts->simpleParts.push_back( pContextPart );
        }
        if ( !nonSimple.empty() )
        {
            MemoryLayout::Part* pContextPart = database.construct< MemoryLayout::Part >(
                MemoryLayout::Part::Args{ pContext, nonSimple.user, nonSimple.link, nonSimple.alloc } );
            nonSimple.setParts( database, pContextPart );
            pParts->nonSimpleParts.push_back( pContextPart );
        }
        if ( !gpu.empty() )
        {
            MemoryLayout::Part* pContextPart = database.construct< MemoryLayout::Part >(
                MemoryLayout::Part::Args{ pContext, gpu.user, gpu.link, gpu.alloc } );
            gpu.setParts( database, pContextPart );
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
                MemoryLayout::SimpleBuffer* pSimpleBuffer = database.construct< MemoryLayout::SimpleBuffer >(
                    MemoryLayout::SimpleBuffer::Args{ MemoryLayout::Buffer::Args{ parts.simpleParts } } );
                objectBuffers.push_back( pSimpleBuffer );
            }
            if ( !parts.nonSimpleParts.empty() )
            {
                MemoryLayout::NonSimpleBuffer* pNonSimpleBuffer = database.construct< MemoryLayout::NonSimpleBuffer >(
                    MemoryLayout::NonSimpleBuffer::Args{ MemoryLayout::Buffer::Args{ parts.nonSimpleParts } } );
                objectBuffers.push_back( pNonSimpleBuffer );
            }
            if ( !parts.gpuParts.empty() )
            {
                MemoryLayout::GPUBuffer* pGPUBuffer = database.construct< MemoryLayout::GPUBuffer >(
                    MemoryLayout::GPUBuffer::Args{ MemoryLayout::Buffer::Args{ parts.gpuParts } } );
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
