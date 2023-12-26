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
#include "compiler/type_path.hpp"
} // namespace SymbolRollout

#include <map>

namespace mega::compiler
{

class Task_SymbolAnalysis : public BaseTask
{
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
            return task::DeterminantHash(
                { toolChainHash, env.getBuildHashCode( env.ParserStage_AST( megaFilePath ) ),
                  env.getBuildHashCode( env.InterfaceStage_Tree( env.project_manifest() ) ) } );
        }
    };

    Task_SymbolAnalysis( const TaskArguments& taskArguments )
        : BaseTask( taskArguments )
    {
    }

    SymbolAnalysis::Symbols::SymbolTable* requestNewSymbols( SymbolAnalysis::Database& database )
    {
        using namespace SymbolAnalysis;

        SymbolTable symbolTable = m_environment.getSymbolTable();

        const auto sourceFiles = getSortedSourceFiles();

        // request new symbols
        using NewSymbolNamess = std::map< std::string, Symbols::SymbolID* >;
        using NewSymbolIDs    = std::map< mega::interface::SymbolID, Symbols::SymbolID* >;
        NewSymbolNamess newSymbolNames;
        NewSymbolIDs    newSymbolIDs;

        {
            using SymbolMap = std::multimap< std::string, Parser::Symbol* >;

            SymbolRequest request;
            SymbolMap     symbols;

            for( const mega::io::megaFilePath& newSourceFile : sourceFiles )
            {
                for( Parser::Symbol* pSymbol : database.many< Parser::Symbol >( newSourceFile ) )
                {
                    auto token       = pSymbol->get_token();
                    auto symbolIDOpt = symbolTable.findSymbol( token );
                    if( !symbolIDOpt.has_value() )
                    {
                        request.newSymbols.insert( token );
                    }
                    symbols.insert( { token, pSymbol } );
                }
            }

            if( !request.newSymbols.empty() )
            {
                symbolTable = m_environment.newSymbols( request );
            }

            // add NULL_SYMBOL
            /*{
                auto symbolIDOpt = symbolTable.findSymbol( "" );
                VERIFY_RTE( symbolIDOpt.has_value() );
                auto pSymbol
                    = database.construct< Symbols::SymbolID >( Symbols::SymbolID::Args{ "", symbolIDOpt.value(), {} } );
                newSymbolIDs.insert( { symbolIDOpt.value(), pSymbol } );
            }*/

            for( auto i = symbols.begin(), iEnd = symbols.end(); i != iEnd; )
            {
                const auto& token       = i->first;
                auto        symbolIDOpt = symbolTable.findSymbol( token );
                VERIFY_RTE( symbolIDOpt.has_value() );
                auto pSymbol = database.construct< Symbols::SymbolID >(
                    Symbols::SymbolID::Args{ token, symbolIDOpt.value(), {}, {} } );
                newSymbolNames.insert( { token, pSymbol } );
                newSymbolIDs.insert( { symbolIDOpt.value(), pSymbol } );

                for( auto iUpper = symbols.upper_bound( i->first ); i != iUpper; ++i )
                {
                    pSymbol->push_back_symbols( i->second );
                }
            }
        }

        std::map< interface::SymbolIDSequence, Symbols::InterfaceTypeID* > newInterfaceTypeIDSequences;
        std::map< interface::TypeID, Symbols::InterfaceTypeID* >           newInterfaceTypeIDs;

        {
            const TypeIDSequenceGen idSequenceGen( newSymbolNames );
            {
                SymbolRequest request;

                for( auto pNode : database.many< Interface::Node >( m_environment.project_manifest() ) )
                {
                    auto sequencePair = idSequenceGen( pNode );
                    if( auto pInterfaceObject = symbolTable.findInterfaceObject( sequencePair.first ) )
                    {
                        if( !sequencePair.second.empty() )
                        {
                            if( interface::TypeID{} == pInterfaceObject->find( sequencePair.second ) )
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
                }

                if( !request.newInterfaceObjects.empty() || !request.newInterfaceElements.empty() )
                {
                    symbolTable = m_environment.newSymbols( request );
                }
            }

            for( auto pNode : database.many< Interface::Node >( m_environment.project_manifest() ) )
            {
                auto sequencePair = idSequenceGen( pNode );

                interface::TypeID                 interfaceTypeID;
                std::vector< Symbols::SymbolID* > symbolSequence;
                {
                    auto pInterfaceObject = symbolTable.findInterfaceObject( sequencePair.first );
                    VERIFY_RTE( pInterfaceObject );
                    {
                        for( const auto& symbolID : sequencePair.first )
                        {
                            auto iFind = newSymbolIDs.find( symbolID );
                            VERIFY_RTE( iFind != newSymbolIDs.end() );
                            symbolSequence.push_back( iFind->second );
                        }
                    }
                    if( !sequencePair.second.empty() )
                    {
                        for( const auto& symbolID : sequencePair.second )
                        {
                            auto iFind = newSymbolIDs.find( symbolID );
                            VERIFY_RTE( iFind != newSymbolIDs.end() );
                            symbolSequence.push_back( iFind->second );
                        }
                        interfaceTypeID = pInterfaceObject->find( sequencePair.second );
                    }
                    else
                    {
                        interfaceTypeID
                            = interface::TypeID( pInterfaceObject->getObjectID(), interface::NULL_SUB_OBJECT_ID );
                    }
                }
                VERIFY_RTE( interfaceTypeID != interface::NULL_TYPE_ID );

                auto pInterfaceTypeID = database.construct< Symbols::InterfaceTypeID >(
                    Symbols::InterfaceTypeID::Args{ symbolSequence, interfaceTypeID, pNode } );

                interface::SymbolIDSequence symbolIDSequence = sequencePair.first;
                std::copy(
                    sequencePair.second.begin(), sequencePair.second.end(), std::back_inserter( symbolIDSequence ) );

                VERIFY_RTE( newInterfaceTypeIDSequences.insert( { symbolIDSequence, pInterfaceTypeID } ).second );
                VERIFY_RTE_MSG( newInterfaceTypeIDs.insert( { interfaceTypeID, pInterfaceTypeID } ).second,
                                "Duplicate interface typeID: " << interfaceTypeID );

                database.construct< Interface::Node >( Interface::Node::Args{ pNode, pInterfaceTypeID } );
            }
        }

        // record the interfaceID last symbol
        {
            for( const auto [ typeID, pInterfaceID ] : newInterfaceTypeIDs )
            {
                auto symbolIDs = pInterfaceID->get_symbol_ids();
                if( !symbolIDs.empty() )
                {
                    auto pLast = symbolIDs.back();
                    pLast->push_back_interfaceIDs( pInterfaceID );
                }
            }
        }

        return database.construct< Symbols::SymbolTable >( Symbols::SymbolTable::Args{
            newSymbolNames, newSymbolIDs, newInterfaceTypeIDSequences, newInterfaceTypeIDs } );
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

BaseTask::Ptr create_Task_SymbolAnalysis( const TaskArguments& taskArguments )
{
    return std::make_unique< Task_SymbolAnalysis >( taskArguments );
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
        const auto                          manifestFilePath = m_environment.project_manifest();
        const mega::io::CompilationFilePath symbolAnalysisFilePath
            = m_environment.SymbolAnalysis_SymbolTable( manifestFilePath );
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

        Symbols::SymbolTable* pSymbolTable     = database.one< Symbols::SymbolTable >( manifestFilePath );
        const auto            symbolNames      = pSymbolTable->get_symbol_names();
        const auto            interfaceTypeIDs = pSymbolTable->get_interface_symbol_id_sequences();

        // reconstruct all symbols with their associated symbolID
        for( Parser::Symbol* pSymbol : database.many< Parser::Symbol >( m_sourceFilePath ) )
        {
            auto token = pSymbol->get_token();
            auto iFind = symbolNames.find( token );
            VERIFY_RTE( iFind != symbolNames.end() );
            database.construct< Parser::Symbol >( Parser::Symbol::Args{ pSymbol, iFind->second } );
        }

        // determine absolute types
        for( auto pAbsoluteTypePath : database.many< Parser::Type::Absolute >( m_sourceFilePath ) )
        {
            auto pNode = Interface::resolve( interfaceTypeIDs, pAbsoluteTypePath );
            database.construct< Parser::Type::Absolute >( Parser::Type::Absolute::Args{ pAbsoluteTypePath, pNode } );
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
