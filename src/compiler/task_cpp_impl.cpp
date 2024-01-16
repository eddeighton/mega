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

#include "database/ObjectStage.hxx"

#include "compiler/clang_compilation.hpp"

#include "mega/common_strings.hpp"

#include "mega/values/compilation/hyper_graph.hpp"
#include "mega/values/runtime/pointer.hpp"
#include "mega/make_unique_without_reorder.hpp"

#include "nlohmann/json.hpp"

#include "inja/inja.hpp"
#include "inja/environment.hpp"
#include "inja/template.hpp"

#include <memory>

namespace ObjectStage
{
class ImplementationGen
{
    class TemplateEngine
    {
        const mega::io::StashEnvironment& m_environment;
        ::inja::Environment&              m_injaEnvironment;
        ::inja::Template                  m_implTemplate;

    public:
        TemplateEngine( const mega::io::StashEnvironment& buildEnvironment, ::inja::Environment& injaEnv )
            : m_environment( buildEnvironment )
            , m_injaEnvironment( injaEnv )
            , m_implTemplate( m_injaEnvironment.parse_template( m_environment.ImplementationTemplate().string() ) )
        {
        }

        void renderImpl( const nlohmann::json& data, std::ostream& os ) const
        {
            m_injaEnvironment.render_to( os, m_implTemplate, data );
        }
    };

    const mega::io::StashEnvironment& m_environment;
    std::string                       m_strUnitName;

    /*
        std::string typeToString( Functions::ReturnTypes::ReturnType* pReturnType )
        {
            std::ostringstream os;
            return os.str();
        }*/
public:
    ImplementationGen( const mega::io::StashEnvironment& buildEnvironment, const std::string& strUnitName )
        : m_environment( buildEnvironment )
        , m_strUnitName( strUnitName )
    {
    }

    void generate( Functions::Invocations* pInvocations, std::ostream& os )
    {
        ::inja::Environment injaEnvironment;
        {
            injaEnvironment.set_trim_blocks( true );
        }

        TemplateEngine templateEngine( m_environment, injaEnvironment );

        nlohmann::json implData( { { "unitname", m_strUnitName }, { "invocations", nlohmann::json::array() } } );

        for( auto& [ id, pInvocation ] : pInvocations->get_invocations() )
        {
            /*std::ostringstream osContext;
            {
                osContext << mega::MEGA_POINTER << "< ";
                bool bFirst = true;
                for( mega::interface::TypeID typeID : id.m_context )
                {
                    if( bFirst )
                        bFirst = false;
                    else
                        osContext << ',';
                    osContext << "0x" << std::hex << std::setw( 8 ) << std::setfill( '0' ) << typeID.getValue();
                }
                osContext << " >";
            }

            std::ostringstream osSymbolPath;
            {
                bool bFirst = true;
                for( mega::interface::SymbolID symbolID : id.m_symbols )
                {
                    if( bFirst )
                        bFirst = false;
                    else
                        osSymbolPath << ',';
                    osContext << "0x" << std::hex << std::setw( 8 ) << std::setfill( '0' ) << symbolID.getValue();
                }
            }*/

            std::ostringstream osID;
            {
                osID << "{";
                {
                    bool bFirst = true;
                    for( mega::interface::TypeID typeID : id.m_context )
                    {
                        if( !typeID.valid() )
                            break;
                        if( bFirst )
                            bFirst = false;
                        else
                            osID << ',';
                        osID << typeID;
                    }
                }
                osID << "},{";
                {
                    bool bFirst = true;
                    for( mega::interface::SymbolID symbolID : id.m_symbols )
                    {
                        if( symbolID == mega::interface::NULL_SYMBOL_ID )
                            break;
                        if( bFirst )
                            bFirst = false;
                        else
                            osID << ',';
                        osID << symbolID;
                    }
                }
                osID << "}," << ( id.m_bHasParams ? "true" : "false" );
            }

            std::ostringstream osReturnType;
            std::ostringstream osParameterType;
            std::ostringstream osFunctionType;

            {
                auto pReturnType = pInvocation->get_return_type();

                osReturnType << pReturnType->get_canonical_type();

                if( auto pVoid = db_cast< Functions::ReturnTypes::Void >( pReturnType ) )
                {
                    osReturnType << "void";
                }
                else if( auto pDimensionReturnType = db_cast< Functions::ReturnTypes::Dimension >( pReturnType ) )
                {
                    std::set< std::string > canonicalTypes;
                    for( auto pDim : pDimensionReturnType->get_dimensions() )
                    {
                        canonicalTypes.insert( pDim->get_cpp_data_type()->get_type_info()->get_canonical() );
                    }
                    VERIFY_RTE_MSG( canonicalTypes.size() == 1,
                                    "Cannot generate runtime return type for non-homogeneous dimension" );
                    osParameterType << *canonicalTypes.begin();
                }
                else if( auto pFunctionReturnType = db_cast< Functions::ReturnTypes::Function >( pReturnType ) )
                {
                    std::set< Interface::CPP::TypeInfo* > dataTypes;
                    for( auto pFunction : pFunctionReturnType->get_functions() )
                    {
                        dataTypes.insert( pFunction->get_cpp_function_type()->get_return_type_info() );
                    }

                    std::set< std::string >                returnTypes;
                    std::set< std::vector< std::string > > parameterTypes;
                    for( Interface::Function* pFunction : pFunctionReturnType->get_functions() )
                    {
                        returnTypes.insert(
                            pFunction->get_cpp_function_type()->get_return_type_info()->get_canonical() );
                        std::vector< std::string > params;
                        for( auto pParam : pFunction->get_cpp_function_type()->get_parameter_type_infos() )
                        {
                            params.push_back( pParam->get_canonical() );
                        }
                        parameterTypes.insert( params );
                    }
                    VERIFY_RTE_MSG(
                        returnTypes.size() == 1, "Cannot generate runtime return type for non-homogeneous function" );
                    VERIFY_RTE_MSG(
                        parameterTypes.size() == 1, "Cannot generate runtime parameters for non-homogeneous function" );

                    const std::string&                strReturnType = *returnTypes.begin();
                    const std::vector< std::string >& parameters    = *parameterTypes.begin();

                    // define function pointer type
                    os << strReturnType << "(*)( mega::runtime::Pointer* ";

                    for( const std::string& strType : parameters )
                        os << "," << strType;

                    os << ")";
                }
                else
                {
                    THROW_RTE( "Unknown return type" );
                }
            }

            nlohmann::json invocation( {

                { "context", pInvocation->get_canonical_context() },
                { "type_path", pInvocation->get_canonical_type_path() },
                { "operation", pInvocation->get_canonical_operation() },

                { "explicit_operation", mega::getExplicitOperationString( pInvocation->get_explicit_operation() ) },
                { "id", osID.str() },

                { "return_type", osReturnType.str() },
                { "parameter_type", osParameterType.str() }, // use in write
                { "function_type", osFunctionType.str() }    // use in call

            } );

            implData[ "invocations" ].push_back( invocation );
        }

        templateEngine.renderImpl( implData, os );
    }
};
} // namespace ObjectStage

using namespace ObjectStage; // used as view of CPPSourceStage

namespace mega::compiler
{

class Task_CPP_Impl : public BaseTask
{
    const mega::io::cppFilePath& m_sourceFilePath;

public:
    Task_CPP_Impl( const TaskArguments& taskArguments, const mega::io::cppFilePath& sourceFilePath )
        : BaseTask( taskArguments )
        , m_sourceFilePath( sourceFilePath )
    {
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        const mega::io::GeneratedCPPSourceFilePath cppImplFile = m_environment.CPPImpl( m_sourceFilePath );
        start( taskProgress, "Task_CPP_Impl", m_sourceFilePath.path(), cppImplFile.path() );

        const task::DeterminantHash determinant( { m_toolChain.toolChainHash } );

        /*if( m_environment.restore( cppImplFile, determinant ) )
        {
            m_environment.setBuildHashCode( cppImplFile );
            cached( taskProgress );
            return;
        }*/

        Database database( m_environment, m_sourceFilePath, true );

        auto pInvocations = database.one< Functions::Invocations >( m_sourceFilePath );
        VERIFY_RTE( pInvocations );

        std::ostringstream os;
        {
            ImplementationGen implGen( m_environment, m_sourceFilePath.path().string() );
            implGen.generate( pInvocations, os );
            os << "\n";
        }

        if( boost::filesystem::updateFileIfChanged( m_environment.FilePath( cppImplFile ), os.str() ) )
        {
            m_environment.setBuildHashCode( cppImplFile );
            m_environment.stash( cppImplFile, determinant );
            succeeded( taskProgress );
        }
        else
        {
            m_environment.setBuildHashCode( cppImplFile );
            cached( taskProgress );
        }
    }
};

BaseTask::Ptr create_Task_CPP_Impl( const TaskArguments& taskArguments, const mega::io::cppFilePath& sourceFilePath )
{
    return std::make_unique< Task_CPP_Impl >( taskArguments, sourceFilePath );
}

} // namespace mega::compiler
