
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

#include "database/OperationsLocs.hxx"

#include "database/sources.hpp"
#include "mega/values/compilation/source_location.hpp"

#include <common/stash.hpp>

#include <vector>
#include <sstream>
#include <algorithm>

namespace mega::compiler
{
namespace
{
inline bool isNewLine( std::string::const_iterator i )
{
    if( *i == '\n' )
        return true;
    if( ( *i == '\r' ) && ( *( i + 1 ) == '\n' ) )
        return true;
    return false;
}

template < typename KeyType, typename ResultType, typename KeyParserFunctor >
void parseRanges( const std::string& str, const std::string& strBegin, const std::string& strEnd,
                  KeyParserFunctor&& parser, ResultType& ranges )
{
    for( auto i = str.cbegin(), iBegin = str.cbegin(), iEnd = str.cend(); i != iEnd; )
    {
        i = std::search( i, iEnd, strBegin.cbegin(), strBegin.cend() );
        if( i != iEnd )
        {
            const std::string::const_iterator iStart = i;
            i += strBegin.size();

            // get key from the line
            KeyType key;
            {
                auto iLineEnd = i + 1;
                while( !isNewLine( iLineEnd ) )
                {
                    ++iLineEnd;
                }
                key = parser( i, iLineEnd );
                i   = iLineEnd;
            }

            i = std::search( i, iEnd, strEnd.cbegin(), strEnd.cend() );
            VERIFY_RTE_MSG( i != iEnd, "Failed to locate block end" );
            i += strEnd.size();

            const U64 distToStart = static_cast< U64 >( std::distance( iBegin, iStart ) );
            const U64 distToEnd   = static_cast< U64 >( std::distance( iBegin, i ) );

            VERIFY_RTE( ranges.insert( { key, SourceLocation{ distToStart, distToEnd } } ).second );
        }
    }
}
} // namespace

class Task_OperationsLocs : public BaseTask
{
    const mega::io::megaFilePath& m_sourceFilePath;

public:
    Task_OperationsLocs( const TaskArguments& taskArguments, const mega::io::megaFilePath& sourceFilePath )
        : BaseTask( taskArguments )
        , m_sourceFilePath( sourceFilePath )
    {
    }

private:
    using BlockID     = mega::U64;
    using BlockRanges = std::unordered_map< BlockID, SourceLocation >;

public:
    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        const mega::io::CompilationFilePath compilationFile
            = m_environment.OperationsLocs_Locations( m_sourceFilePath );
        start( taskProgress, "Task_OperationsLocs", m_sourceFilePath.path(), compilationFile.path() );

        const task::DeterminantHash determinant(
            { m_toolChain.toolChainHash, m_environment.getBuildHashCode( m_environment.Operations( m_sourceFilePath ) ),
              m_environment.getBuildHashCode( m_environment.AutomataStage_AutomataAnalysis( m_sourceFilePath ) ) } );

        if( m_environment.restore( compilationFile, determinant ) )
        {
            m_environment.setBuildHashCode( compilationFile );
            cached( taskProgress );
            return;
        }

        using namespace OperationsLocs;
        Database database( m_environment, m_sourceFilePath );

        const io::GeneratedHPPSourceFilePath operationsHeader = m_environment.Operations( m_sourceFilePath );

        std::string strOperationsHeaderText;
        boost::filesystem::loadAsciiFile( m_environment.FilePath( operationsHeader ), strOperationsHeaderText );

        using OperationRanges = std::unordered_map< TypeID, SourceLocation, TypeID::Hash >;
        OperationRanges operationRanges;
        {
            using namespace std::string_literals;
            parseRanges< TypeID, OperationRanges >(
                strOperationsHeaderText, "//_MEGAOPERATIONBEGIN"s, "//_MEGAOPERATIONEND"s,
                []( std::string::const_iterator i, std::string::const_iterator iEnd ) -> TypeID
                {
                    std::string        strView( i, iEnd );
                    std::istringstream is( strView );
                    TypeID             typeID;
                    using ::           operator>>;
                    is >> typeID;
                    return typeID;
                },
                operationRanges );
        }

        Operations::Invocations* pInvocations = database.one< Operations::Invocations >( m_sourceFilePath );

        for( auto pInvocationContext : database.many< Interface::InvocationContext >( m_sourceFilePath ) )
        {
            std::vector< Interface::InvocationInstance* > invocationInstances;
            SourceLocation                                operationLoc;

            if( db_cast< Interface::Action >( pInvocationContext )
                || !db_cast< Interface::State >( pInvocationContext ) )
            {
                auto iFind = operationRanges.find( pInvocationContext->get_interface_id() );
                VERIFY_RTE_MSG( iFind != operationRanges.end(),
                                "Failed to locate operation body for: " << pInvocationContext->get_interface_id() );
                operationLoc = iFind->second;
                {
                    for( const auto& [ invocationID, pInvocation ] : pInvocations->get_invocations() )
                    {
                        std::vector< SourceLocation > instances;
                        for( const auto& invocationLoc : pInvocation->get_file_offsets() )
                        {
                            if( operationLoc.contains( invocationLoc ) )
                            {
                                instances.push_back( invocationLoc );
                            }
                        }
                        for( const auto& instanceLoc : instances )
                        {
                            invocationInstances.push_back( database.construct< Interface::InvocationInstance >(
                                Interface::InvocationInstance::Args{ pInvocation, instanceLoc } ) );
                        }
                    }
                }
            }

            database.construct< Interface::InvocationContext >(
                Interface::InvocationContext::Args{ pInvocationContext, invocationInstances, operationLoc } );
        }

        const task::FileHash fileHashCode = database.save_Locations_to_temp();
        m_environment.setBuildHashCode( compilationFile, fileHashCode );
        m_environment.temp_to_real( compilationFile );
        m_environment.stash( compilationFile, determinant );

        succeeded( taskProgress );
    }
};

BaseTask::Ptr create_Task_OperationsLocs( const TaskArguments&          taskArguments,
                                          const mega::io::megaFilePath& sourceFilePath )
{
    return std::make_unique< Task_OperationsLocs >( taskArguments, sourceFilePath );
}

} // namespace mega::compiler
