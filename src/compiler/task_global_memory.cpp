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

#include "mega/relation_id.hpp"
#include "mega/common_strings.hpp"

#include "database/model/MemoryStage.hxx"
#include "database/model/GlobalMemoryStage.hxx"
#include "database/model/GlobalMemoryStageRollout.hxx"
#include "database/model/manifest.hxx"

#include "database/common/environment_archive.hpp"
#include "database/common/exception.hpp"

namespace GlobalMemoryStage
{
#include "compiler/interface_printer.hpp"
}

namespace mega::compiler
{

class Task_GlobalMemoryStage : public BaseTask
{
    const mega::io::Manifest m_manifest;

public:
    Task_GlobalMemoryStage( const TaskArguments& taskArguments, const mega::io::manifestFilePath& manifestFilePath )
        : BaseTask( taskArguments )
        , m_manifest( m_environment, manifestFilePath )
    {
    }

    std::optional< GlobalMemoryStage::Interface::SizeTrait* >
    getSizeTrait( GlobalMemoryStage::Interface::IContext* pContext )
    {
        using namespace GlobalMemoryStage;
        std::optional< Interface::SizeTrait* > sizeOpt;
        {
            if( Interface::Object* pObject = db_cast< Interface::Object >( pContext ) )
            {
                sizeOpt = pObject->get_size_trait();
            }
        }
        {
            if( Interface::Abstract* pInterface = db_cast< Interface::Abstract >( pContext ) )
            {
                sizeOpt = pInterface->get_size_trait();
            }
        }
        return sizeOpt;
    }

    GlobalMemoryStage::Interface::IContext* getObjectMemoryMappingBase( GlobalMemoryStage::Concrete::Object* pObject )
    {
        using namespace GlobalMemoryStage;
        Interface::IContext* pMemoryMappedContext = nullptr;
        for( auto pContext : pObject->get_inheritance() )
        {
            std::optional< Interface::SizeTrait* > sizeOpt = getSizeTrait( pContext );
            if( sizeOpt.has_value() )
            {
                VERIFY_RTE_MSG( !pMemoryMappedContext,
                                "Type: " << Interface::printIContextFullType( pObject->get_interface() )
                                         << " has MULTIPLE memory mapping size specifications" );
                pMemoryMappedContext = pContext;
            }
        }
        return pMemoryMappedContext;
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        const mega::io::manifestFilePath    manifestFilePath = m_environment.project_manifest();
        const mega::io::CompilationFilePath globalMemoryCompilationFile
            = m_environment.GlobalMemoryStage_GlobalMemoryLayout( manifestFilePath );
        start( taskProgress, "Task_GlobalMemoryStage", manifestFilePath.path(), globalMemoryCompilationFile.path() );

        task::DeterminantHash determinant( m_toolChain.toolChainHash );
        for( const mega::io::megaFilePath& sourceFilePath : m_manifest.getMegaSourceFiles() )
        {
            determinant ^= m_environment.getBuildHashCode( m_environment.MemoryStage_MemoryLayout( sourceFilePath ) );
        }

        if( m_environment.restore( globalMemoryCompilationFile, determinant ) )
        {
            m_environment.setBuildHashCode( globalMemoryCompilationFile );
            cached( taskProgress );
            return;
        }

        using namespace GlobalMemoryStage;

        Database database( m_environment, manifestFilePath );
        {
            using MappedObjects = std::multimap< Interface::IContext*, Concrete::Object* >;
            MappedObjects mappedObjects;

            for( const mega::io::megaFilePath& sourceFilePath : m_manifest.getMegaSourceFiles() )
            {
                for( Concrete::Object* pObject : database.many< Concrete::Object >( sourceFilePath ) )
                {
                    Interface::IContext* pContext = getObjectMemoryMappingBase( pObject );
                    mappedObjects.insert( { pContext, pObject } );
                }
            }

            for( auto i = mappedObjects.begin(), iEnd = mappedObjects.end(); i != iEnd; )
            {
                if( ::GlobalMemoryStage::Interface::IContext* pInterface = i->first )
                {
                    std::optional< Interface::SizeTrait* > sizeOpt = getSizeTrait( pInterface );
                    VERIFY_RTE( sizeOpt.has_value() );

                    std::vector< Concrete::Object* > concrete;
                    mega::U64                        block_size       = 0;
                    mega::U64                        block_alignment  = 1;
                    const mega::U64                  fixed_allocation = sizeOpt.value()->get_size();

                    for( auto iUpper = mappedObjects.upper_bound( i->first ); i != iUpper; ++i )
                    {
                        Concrete::Object* pObject = i->second;
                        concrete.push_back( pObject );

                        for( MemoryLayout::Buffer* pBuffer : pObject->get_buffers() )
                        {
                            block_size      = std::max( block_size, pBuffer->get_size() );
                            block_alignment = std::max( block_alignment, pBuffer->get_alignment() );
                        }
                    }

                    MemoryLayout::MemoryMap* pMemoryMap
                        = database.construct< MemoryLayout::MemoryMap >( MemoryLayout::MemoryMap::Args{
                            pInterface, block_size, block_alignment, fixed_allocation, concrete } );
                }
                else
                {
                    i = mappedObjects.upper_bound( i->first );
                }
            }
        }

        const task::FileHash fileHashCode = database.save_GlobalMemoryLayout_to_temp();
        m_environment.setBuildHashCode( globalMemoryCompilationFile, fileHashCode );
        m_environment.temp_to_real( globalMemoryCompilationFile );
        m_environment.stash( globalMemoryCompilationFile, determinant );

        succeeded( taskProgress );
    }
};

BaseTask::Ptr create_Task_GlobalMemoryStage( const TaskArguments&              taskArguments,
                                             const mega::io::manifestFilePath& manifestFilePath )
{
    return std::make_unique< Task_GlobalMemoryStage >( taskArguments, manifestFilePath );
}

class Task_GlobalMemoryStageRollout : public BaseTask
{
    const mega::io::megaFilePath& m_sourceFilePath;

public:
    Task_GlobalMemoryStageRollout( const TaskArguments&          taskArguments,
                                   const mega::io::megaFilePath& megaSourceFilePath )
        : BaseTask( taskArguments )
        , m_sourceFilePath( megaSourceFilePath )
    {
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        mega::io::CompilationFilePath globalMemoryCompilationFile
            = m_environment.GlobalMemoryStage_GlobalMemoryLayout( m_environment.project_manifest() );

        mega::io::CompilationFilePath rolloutCompilationFile
            = m_environment.GlobalMemoryStageRollout_GlobalMemoryRollout( m_sourceFilePath );

        start( taskProgress,
               "Task_GlobalMemoryStageRollout",
               m_environment.project_manifest().path(),
               // globalMemoryCompilationFile.path(),
               rolloutCompilationFile.path() );

        const task::DeterminantHash determinant = { m_environment.getBuildHashCode( globalMemoryCompilationFile ) };

        if( m_environment.restore( rolloutCompilationFile, determinant ) )
        {
            m_environment.setBuildHashCode( rolloutCompilationFile );
            cached( taskProgress );
            return;
        }

        using namespace GlobalMemoryStageRollout;

        Database database( m_environment, m_sourceFilePath );
        {
            std::map< Interface::IContext*, MemoryLayout::MemoryMap* > memoryMaps;
            {
                for( MemoryLayout::MemoryMap* pMemoryMap :
                     database.many< MemoryLayout::MemoryMap >( m_environment.project_manifest() ) )
                {
                    memoryMaps.insert( { pMemoryMap->get_interface(), pMemoryMap } );
                }
            }

            for( Concrete::Object* pObject : database.many< Concrete::Object >( m_sourceFilePath ) )
            {
                MemoryLayout::MemoryMap* pMemoryMap = nullptr;
                for( auto pContext : pObject->get_inheritance() )
                {
                    auto iFind = memoryMaps.find( pContext );
                    if( iFind != memoryMaps.end() )
                    {
                        VERIFY_RTE( pMemoryMap == nullptr );
                        pMemoryMap = iFind->second;
                    }
                }

                if( pMemoryMap )
                {
                    database.construct< Concrete::MemoryMappedObject >(
                        Concrete::MemoryMappedObject::Args{ pObject, pMemoryMap } );
                }
            }
        }

        const task::FileHash fileHashCode = database.save_GlobalMemoryRollout_to_temp();
        m_environment.setBuildHashCode( rolloutCompilationFile, fileHashCode );
        m_environment.temp_to_real( rolloutCompilationFile );
        m_environment.stash( rolloutCompilationFile, determinant );
        succeeded( taskProgress );
    }
};

BaseTask::Ptr create_Task_GlobalMemoryStageRollout( const TaskArguments&          taskArguments,
                                                    const mega::io::megaFilePath& megaSourceFilePath )
{
    return std::make_unique< Task_GlobalMemoryStageRollout >( taskArguments, megaSourceFilePath );
}

} // namespace mega::compiler
