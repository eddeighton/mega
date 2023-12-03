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

#include "mega/common_strings.hpp"
#include "mega/values/compilation/symbol_table.hpp"

#include "database/SymbolAnalysis.hxx"
#include "database/SymbolRollout.hxx"
#include "database/manifest.hxx"

#include "environment/environment_archive.hpp"
#include "database/exception.hpp"

namespace SymbolAnalysis
{
#include "compiler/interface_printer.hpp"
#include "compiler/interface_typeid_sequence.hpp"
} // namespace SymbolAnalysis
namespace SymbolRollout
{
#include "compiler/interface_printer.hpp"
#include "compiler/interface_typeid_sequence.hpp"
} // namespace SymbolRollout

namespace mega::compiler
{

class Task_SymbolAnalysis : public BaseTask
{
    const mega::io::Manifest m_manifest;

public:
    struct InterfaceHashCodeGenerator
    {
        const mega::io::StashEnvironment& env;
        task::DeterminantHash             toolChainHash;
        InterfaceHashCodeGenerator( const mega::io::StashEnvironment& env, task::DeterminantHash toolChainHash )
            : env( env )
            , toolChainHash( toolChainHash )
        {
        }
        inline task::DeterminantHash operator()( const mega::io::megaFilePath& megaFilePath ) const
        {
            return task::DeterminantHash( { toolChainHash, env.getBuildHashCode( env.ParserStage_AST( megaFilePath ) ),
                                            env.getBuildHashCode( env.InterfaceStage_Tree( megaFilePath ) ) } );
        }
    };

    Task_SymbolAnalysis( const TaskArguments& taskArguments, const mega::io::manifestFilePath& manifestFilePath )
        : BaseTask( taskArguments )
        , m_manifest( m_environment, manifestFilePath )
    {
    }

    using PathSet = std::set< mega::io::megaFilePath >;
    PathSet getSortedSourceFiles( const io::Manifest& manifest ) const
    {
        PathSet sourceFiles;
        for( const mega::io::megaFilePath& sourceFilePath : manifest.getMegaSourceFiles() )
        {
            sourceFiles.insert( sourceFilePath );
        }
        return sourceFiles;
    }

    SymbolAnalysis::Symbols::SymbolTable* requestNewSymbols( SymbolAnalysis::Database& newDatabase )
    {
        using namespace SymbolAnalysis;

        SymbolTable symbolTable = m_environment.getSymbolTable();

        const auto sourceFiles = getSortedSourceFiles( m_manifest );

        // request new symbols
        {
            SymbolRequest request;
            auto          addSymbol = [ & ]( const std::string& str )
            {
                auto symbolIDOpt = symbolTable.findSymbol( str );
                if( !symbolIDOpt.has_value() )
                {
                    request.newSymbols.insert( str );
                }
            };

            for( const mega::io::megaFilePath& newSourceFile : sourceFiles )
            {
                for( Interface::IContext* pContext : newDatabase.many< Interface::IContext >( newSourceFile ) )
                {
                    addSymbol( Interface::getIdentifier( pContext ) );
                }
                for( Interface::DimensionTrait* pDimension :
                     newDatabase.many< Interface::DimensionTrait >( newSourceFile ) )
                {
                    addSymbol( Interface::getIdentifier( pDimension ) );
                }
                for( Interface::LinkTrait* pLink : newDatabase.many< Interface::LinkTrait >( newSourceFile ) )
                {
                    addSymbol( Interface::getIdentifier( pLink ) );
                }
            }

            if( !request.newSymbols.empty() )
            {
                symbolTable = m_environment.newSymbols( request );
            }
        }

        // construct symbols
        using NewSymbolNames   = std::map< std::string, Symbols::SymbolTypeID* >;
        using NewSymbolTypeIDs = std::map< mega::TypeID, Symbols::SymbolTypeID* >;

        NewSymbolNames   newSymbolNames;
        NewSymbolTypeIDs newSymbolTypeIDs;

        auto addSymbol = [ & ]( const std::string& str ) -> Symbols::SymbolTypeID*
        {
            Symbols::SymbolTypeID* pSymbol = nullptr;
            {
                auto iFind = newSymbolNames.find( str );
                if( iFind == newSymbolNames.end() )
                {
                    // using namespace SymbolAnalysis;
                    auto symbolIDOpt = symbolTable.findSymbol( str );
                    VERIFY_RTE( symbolIDOpt.has_value() );
                    pSymbol = newDatabase.construct< Symbols::SymbolTypeID >(
                        Symbols::SymbolTypeID::Args{ str, symbolIDOpt.value(), {}, {}, {} } );

                    VERIFY_RTE( newSymbolNames.insert( { str, pSymbol } ).second );
                    VERIFY_RTE( newSymbolTypeIDs.insert( { symbolIDOpt.value(), pSymbol } ).second );
                }
                else
                {
                    pSymbol = iFind->second;
                }
                return pSymbol;
            }
        };

        for( const mega::io::megaFilePath& newSourceFile : sourceFiles )
        {
            for( Interface::IContext* pContext : newDatabase.many< Interface::IContext >( newSourceFile ) )
            {
                auto pSymbol = addSymbol( Interface::getIdentifier( pContext ) );
                pSymbol->push_back_contexts( pContext );
            }
            for( Interface::DimensionTrait* pDimension :
                 newDatabase.many< Interface::DimensionTrait >( newSourceFile ) )
            {
                auto pSymbol = addSymbol( Interface::getIdentifier( pDimension ) );
                pSymbol->push_back_dimensions( pDimension );
            }
            for( Interface::LinkTrait* pLink : newDatabase.many< Interface::LinkTrait >( newSourceFile ) )
            {
                auto pSymbol = addSymbol( Interface::getIdentifier( pLink ) );
                pSymbol->push_back_links( pLink );
            }
        }

        const TypeIDSequenceGen idSequenceGen( newSymbolNames );

        // request new interface types
        {
            SymbolRequest request;
            auto          addInterfaceType = [ & ]( const SymbolTraits::SymbolIDVectorPair& sequencePair )
            {
                VERIFY_RTE( !sequencePair.first.empty() );
                if( auto pInterfaceObject = symbolTable.findInterfaceObject( sequencePair.first ) )
                {
                    if( !sequencePair.second.empty() )
                    {
                        if( TypeID{} == pInterfaceObject->find( sequencePair.second ) )
                        {
                            request.newInterfaceElements.insert( sequencePair );
                        }
                    }
                }
                else
                {
                    request.newInterfaceObjects.insert( sequencePair.first );
                    if( !sequencePair.second.empty() )
                    {
                        request.newInterfaceElements.insert( sequencePair );
                    }
                }
            };

            for( const mega::io::megaFilePath& newSourceFile : sourceFiles )
            {
                for( Interface::IContext* pContext : newDatabase.many< Interface::IContext >( newSourceFile ) )
                {
                    addInterfaceType( idSequenceGen( pContext ) );
                }
                for( Interface::DimensionTrait* pDimension :
                     newDatabase.many< Interface::DimensionTrait >( newSourceFile ) )
                {
                    addInterfaceType( idSequenceGen( pDimension ) );
                }
                for( Interface::LinkTrait* pLink : newDatabase.many< Interface::LinkTrait >( newSourceFile ) )
                {
                    addInterfaceType( idSequenceGen( pLink ) );
                }
            }

            if( !request.newInterfaceObjects.empty() || !request.newInterfaceElements.empty() )
            {
                symbolTable = m_environment.newSymbols( request );
            }
        }

        std::map< TypeIDSequence, Symbols::InterfaceTypeID* > newInterfaceTypeIDSequences;
        std::map< TypeID, Symbols::InterfaceTypeID* >         newInterfaceTypeIDs;

        // add interface types
        {
            auto addInterfaceType =

                [ & ]( const SymbolTraits::SymbolIDVectorPair& sequencePair,
                       std::optional< Interface::IContext* >
                           contextOpt,
                       std::optional< Interface::DimensionTrait* >
                           dimensionOpt,
                       std::optional< Interface::LinkTrait* >
                           linkOpt )
            {
                VERIFY_RTE( !sequencePair.first.empty() );

                TypeID         interfaceTypeID;
                TypeIDSequence typeIDSequence;
                {
                    auto pInterfaceObject = symbolTable.findInterfaceObject( sequencePair.first );
                    VERIFY_RTE( pInterfaceObject );
                    std::copy(
                        sequencePair.first.begin(), sequencePair.first.end(), std::back_inserter( typeIDSequence ) );
                    if( !sequencePair.second.empty() )
                    {
                        std::copy( sequencePair.second.begin(),
                                   sequencePair.second.end(),
                                   std::back_inserter( typeIDSequence ) );
                        interfaceTypeID = pInterfaceObject->find( sequencePair.second );
                    }
                    else
                    {
                        interfaceTypeID = TypeID::make_object_from_objectID( pInterfaceObject->getObjectID() );
                    }
                }
                VERIFY_RTE( interfaceTypeID != TypeID{} );

                auto pInterfaceTypeID
                    = newDatabase.construct< Symbols::InterfaceTypeID >( Symbols::InterfaceTypeID::Args{
                        typeIDSequence, interfaceTypeID, contextOpt, dimensionOpt, linkOpt } );

                VERIFY_RTE( newInterfaceTypeIDSequences.insert( { typeIDSequence, pInterfaceTypeID } ).second );
                VERIFY_RTE_MSG( newInterfaceTypeIDs.insert( { interfaceTypeID, pInterfaceTypeID } ).second,
                                "Duplicate interface typeID: " << interfaceTypeID );
            };

            for( const mega::io::megaFilePath& newSourceFile : sourceFiles )
            {
                for( Interface::IContext* pContext : newDatabase.many< Interface::IContext >( newSourceFile ) )
                {
                    addInterfaceType( idSequenceGen( pContext ), pContext, std::nullopt, std::nullopt );
                }
                for( Interface::DimensionTrait* pDimension :
                     newDatabase.many< Interface::DimensionTrait >( newSourceFile ) )
                {
                    addInterfaceType( idSequenceGen( pDimension ), std::nullopt, pDimension, std::nullopt );
                }
                for( Interface::LinkTrait* pLink : newDatabase.many< Interface::LinkTrait >( newSourceFile ) )
                {
                    addInterfaceType( idSequenceGen( pLink ), std::nullopt, std::nullopt, pLink );
                }
            }
        }

        return newDatabase.construct< Symbols::SymbolTable >( Symbols::SymbolTable::Args{
            newSymbolNames, newSymbolTypeIDs, newInterfaceTypeIDSequences, newInterfaceTypeIDs } );
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        const mega::io::manifestFilePath    manifestFilePath = m_environment.project_manifest();
        const mega::io::CompilationFilePath symbolCompilationFile
            = m_environment.SymbolAnalysis_SymbolTable( manifestFilePath );
        start( taskProgress, "Task_SymbolAnalysis", manifestFilePath.path(), symbolCompilationFile.path() );

        InterfaceHashCodeGenerator hashCodeGenerator( m_environment, m_toolChain.toolChainHash );

        task::DeterminantHash determinant( m_toolChain.toolChainHash );
        for( const mega::io::megaFilePath& sourceFilePath : m_manifest.getMegaSourceFiles() )
        {
            determinant ^= hashCodeGenerator( sourceFilePath );
        }

        if( m_environment.restore( symbolCompilationFile, determinant ) )
        {
            m_environment.setBuildHashCode( symbolCompilationFile );
            cached( taskProgress );
            return;
        }

        using namespace SymbolAnalysis;
        using namespace SymbolAnalysis::Symbols;

        Database database( m_environment, manifestFilePath );

        requestNewSymbols( database );

        const task::FileHash fileHashCode = database.save_SymbolTable_to_temp();
        m_environment.setBuildHashCode( symbolCompilationFile, fileHashCode );
        m_environment.temp_to_real( symbolCompilationFile );
        m_environment.stash( symbolCompilationFile, determinant );
        succeeded( taskProgress );
    }
};

BaseTask::Ptr create_Task_SymbolAnalysis( const TaskArguments&              taskArguments,
                                          const mega::io::manifestFilePath& manifestFilePath )
{
    return std::make_unique< Task_SymbolAnalysis >( taskArguments, manifestFilePath );
}

class Task_SymbolRollout : public BaseTask
{
public:
    Task_SymbolRollout( const TaskArguments& taskArguments, const mega::io::megaFilePath& sourceFilePath )
        : BaseTask( taskArguments )
        , m_sourceFilePath( sourceFilePath )
    {
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        const mega::io::CompilationFilePath symbolAnalysisFilePath
            = m_environment.SymbolAnalysis_SymbolTable( m_environment.project_manifest() );
        const mega::io::CompilationFilePath symbolRolloutFilePath
            = m_environment.SymbolRollout_PerSourceSymbols( m_sourceFilePath );
        start( taskProgress, "Task_SymbolRollout", m_sourceFilePath.path(), symbolRolloutFilePath.path() );

        Task_SymbolAnalysis::InterfaceHashCodeGenerator hashGen( m_environment, m_toolChain.toolChainHash );
        const task::DeterminantHash                     determinant = hashGen( m_sourceFilePath );

        if( m_environment.restore( symbolRolloutFilePath, determinant ) )
        {
            m_environment.setBuildHashCode( symbolRolloutFilePath );
            cached( taskProgress );
            return;
        }

        using namespace SymbolRollout;

        Database database( m_environment, m_sourceFilePath );

        Symbols::SymbolTable* pSymbolTable = database.one< Symbols::SymbolTable >( m_environment.project_manifest() );
        const auto            symbolNames  = pSymbolTable->get_symbol_names();
        const auto            interfaceTypeIDs = pSymbolTable->get_interface_type_id_sequences();

        const TypeIDSequenceGen idSequenceGen( symbolNames );

        auto typeIDSeqFromSymbolSeqPair = [ & ]( auto pContext ) -> TypeIDSequence
        {
            TypeIDSequence typeIDSequence;
            const auto     idSeq = idSequenceGen( pContext );
            std::copy( idSeq.first.begin(), idSeq.first.end(), std::back_inserter( typeIDSequence ) );
            std::copy( idSeq.second.begin(), idSeq.second.end(), std::back_inserter( typeIDSequence ) );
            return typeIDSequence;
        };

        for( Interface::IContext* pContext : database.many< Interface::IContext >( m_sourceFilePath ) )
        {
            TypeID symbolID;
            {
                auto iFind = symbolNames.find( Interface::getIdentifier( pContext ) );
                VERIFY_RTE( iFind != symbolNames.end() );
                symbolID = iFind->second->get_id();
            }

            TypeID interfaceTypeID;
            {
                auto iFind = interfaceTypeIDs.find( typeIDSeqFromSymbolSeqPair( pContext ) );
                VERIFY_RTE( iFind != interfaceTypeIDs.end() );
                Symbols::InterfaceTypeID* pInterfaceTypeID = iFind->second;
                interfaceTypeID                            = pInterfaceTypeID->get_id();
            }

            VERIFY_RTE( symbolID.isSymbolID() );
            VERIFY_RTE( interfaceTypeID.isContextID() );

            database.construct< Interface::IContext >(
                Interface::IContext::Args{ pContext, symbolID, interfaceTypeID } );
        }

        for( Interface::DimensionTrait* pDimension : database.many< Interface::DimensionTrait >( m_sourceFilePath ) )
        {
            TypeID symbolID;
            {
                auto iFind = symbolNames.find( Interface::getIdentifier( pDimension ) );
                VERIFY_RTE( iFind != symbolNames.end() );
                symbolID = iFind->second->get_id();
            }

            TypeID interfaceTypeID;
            {
                auto iFind = interfaceTypeIDs.find( typeIDSeqFromSymbolSeqPair( pDimension ) );
                VERIFY_RTE( iFind != interfaceTypeIDs.end() );
                Symbols::InterfaceTypeID* pInterfaceTypeID = iFind->second;
                interfaceTypeID                            = pInterfaceTypeID->get_id();
            }

            database.construct< Interface::DimensionTrait >(
                Interface::DimensionTrait::Args{ pDimension, symbolID, interfaceTypeID } );
        }

        for( Interface::LinkTrait* pLink : database.many< Interface::LinkTrait >( m_sourceFilePath ) )
        {
            TypeID symbolID;
            {
                auto iFind = symbolNames.find( Interface::getIdentifier( pLink ) );
                VERIFY_RTE( iFind != symbolNames.end() );
                symbolID = iFind->second->get_id();
            }

            TypeID interfaceTypeID;
            {
                auto iFind = interfaceTypeIDs.find( typeIDSeqFromSymbolSeqPair( pLink ) );
                VERIFY_RTE( iFind != interfaceTypeIDs.end() );
                Symbols::InterfaceTypeID* pInterfaceTypeID = iFind->second;
                interfaceTypeID                            = pInterfaceTypeID->get_id();
            }

            database.construct< Interface::LinkTrait >(
                Interface::LinkTrait::Args{ pLink, symbolID, interfaceTypeID } );
        }

        const task::FileHash fileHashCode = database.save_PerSourceSymbols_to_temp();
        m_environment.setBuildHashCode( symbolRolloutFilePath, fileHashCode );
        m_environment.temp_to_real( symbolRolloutFilePath );
        m_environment.stash( symbolRolloutFilePath, determinant );
        succeeded( taskProgress );
    }
    const mega::io::megaFilePath& m_sourceFilePath;
};

BaseTask::Ptr create_Task_SymbolRollout( const TaskArguments&          taskArguments,
                                         const mega::io::megaFilePath& sourceFilePath )
{
    return std::make_unique< Task_SymbolRollout >( taskArguments, sourceFilePath );
}

} // namespace mega::compiler
