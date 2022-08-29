
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

    void recurse( MemoryStage::Database& database, MemoryStage::Concrete::Context* pParentContext,
                  MemoryStage::Concrete::Context* pContext, std::size_t szTotalSize )
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
            szSize = getSizeTraitSize( pAction->get_interface_action() );
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
            //pLink = database.construct< Link >( Link::Args{ pLink, szTotalSize } );
        }
        else if ( Buffer* pBuffer = dynamic_database_cast< Buffer >( pContext ) )
        {
            szSize = 1U;
            pBuffer = database.construct< Buffer >( Buffer::Args{ pBuffer, szTotalSize } );
        }
        else
        {
            THROW_RTE( "Unknown context type" );
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

            using namespace MemoryStage::Concrete;

            if ( bCreateAllocatorDimension )
            {
                Dimensions::Allocator* pAllocatorDim = database.construct< Dimensions::Allocator >(
                    Dimensions::Allocator::Args{ Dimensions::Allocation::Args{ pContext }, pAllocator } );
                pContext
                    = database.construct< Context >( Context::Args{ pContext, pAllocator, { pAllocatorDim }, {} } );
            }
            else
            {
                pContext = database.construct< Context >( Context::Args{ pContext, pAllocator, {}, {} } );
            }
        }

        for ( Concrete::ContextGroup* pContextGroup : pContext->get_children() )
        {
            if ( Concrete::Context* pChildContext = dynamic_database_cast< Concrete::Context >( pContextGroup ) )
            {
                recurse( database, pContext, pChildContext, szTotalSize );
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
            { m_toolChain.toolChainHash, m_environment.getBuildHashCode( concrete ) } );

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
                recurse( database, nullptr, pContext, 1 );
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
