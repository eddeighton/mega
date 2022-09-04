
#include "code_generator.hpp"
#include "database/common/environment_archive.hpp"

#include "database/model/FinalStage.hxx"
#include "service/network/log.hpp"

#include "common/file.hpp"
#include "common/string.hpp"

#include "nlohmann/json.hpp"

#include "inja/inja.hpp"
#include "inja/environment.hpp"
#include "inja/template.hpp"

#include "boost/process.hpp"

#include <string>
#include <istream>
#include <sstream>

namespace mega
{
namespace runtime
{
namespace
{

class GeneratorUtil
{
public:
    GeneratorUtil( const boost::filesystem::path& clangPath, const boost::filesystem::path& tempDir )
        : m_clangPath( clangPath )
        , m_uuid( common::uuid() )
        , m_sourcePath( tempDir / ( m_uuid + ".cpp" ) )
        , m_irPath( tempDir / ( m_uuid + ".ir" ) )
    {
    }

    std::unique_ptr< boost::filesystem::ofstream > createSourceFile()
    {
        return boost::filesystem::createNewFileStream( m_sourcePath );
    }

    void compile( const std::string& strCmd, std::ostream& os )
    {
        SPDLOG_INFO( "Compiling: {}", strCmd );

        std::ostringstream osOutput, osError;
        int                iCompilationResult = 0;
        {
            namespace bp = boost::process;

            bp::ipstream errStream, outStream; // reading pipe-stream
            bp::child    c( strCmd, bp::std_out > outStream, bp::std_err > errStream );

            std::string strOutputLine;
            while ( c.running() && std::getline( outStream, strOutputLine ) )
            {
                if ( !strOutputLine.empty() )
                {
                    osOutput << "\nOUT    : " << strOutputLine;
                }
            }

            c.wait();
            iCompilationResult = c.exit_code();
            if ( iCompilationResult )
            {
                osError << common::COLOUR_RED_BEGIN << "FAILED : " << m_sourcePath.string();
                while ( errStream && std::getline( errStream, strOutputLine ) )
                {
                    if ( !strOutputLine.empty() )
                    {
                        osError << "\nERROR  : " << strOutputLine;
                    }
                }
                osError << common::COLOUR_END;
            }
        }
        VERIFY_RTE_MSG( iCompilationResult == 0, "Error compilation invocation: " << osError.str() );

        boost::filesystem::loadAsciiFile( m_irPath, os );
    }

    void compile( const FinalStage::Components::Component* pComponent, std::ostream& os )
    {
        std::ostringstream osCmd;

        osCmd << m_clangPath << " -S -emit-llvm ";

        // flags
        for ( const std::string& flag : pComponent->get_cpp_flags() )
        {
            VERIFY_RTE( !flag.empty() );
            osCmd << "-" << flag << " ";
        }

        // defines
        for ( const std::string& strDefine : pComponent->get_cpp_defines() )
        {
            VERIFY_RTE( !strDefine.empty() );
            osCmd << "-D" << strDefine << " ";
        }

        // include directories
        for ( const boost::filesystem::path& includeDir : pComponent->get_include_directories() )
        {
            osCmd << "-I " << includeDir.native() << " ";
        }

        osCmd << "-o " << m_irPath.native() << " -c " << m_sourcePath.native();
        compile( osCmd.str(), os );
    }

    void compile( std::ostream& os )
    {
        std::ostringstream osCmd;
        osCmd << m_clangPath << " -S -emit-llvm -o " << m_irPath.native() << " -c " << m_sourcePath.native();
        compile( osCmd.str(), os );
    }

private:
    const boost::filesystem::path& m_clangPath;
    const std::string              m_uuid;
    const boost::filesystem::path  m_sourcePath;
    const boost::filesystem::path  m_irPath;
};

} // namespace

class CodeGenerator::Pimpl
{
    const mega::network::MegastructureInstallation m_megastructureInstallation;
    ::inja::Environment                            m_injaEnvironment;
    ::inja::Template                               m_allocationTemplate;
    ::inja::Template                               m_allocateTemplate;
    ::inja::Template                               m_readTemplate;

public:
    Pimpl( const mega::network::MegastructureInstallation& megastructureInstallation )
        : m_megastructureInstallation( megastructureInstallation )
    {
        m_injaEnvironment.set_trim_blocks( true );

        m_allocationTemplate
            = m_injaEnvironment.parse_template( m_megastructureInstallation.getRuntimeTemplateAllocation().native() );
        m_allocateTemplate
            = m_injaEnvironment.parse_template( m_megastructureInstallation.getRuntimeTemplateAllocate().native() );
        m_readTemplate
            = m_injaEnvironment.parse_template( m_megastructureInstallation.getRuntimeTemplateRead().native() );
    }

    void render_allocation( const nlohmann::json& data, std::ostream& os )
    {
        m_injaEnvironment.render_to( os, m_allocationTemplate, data );
    }
    void render_allocate( const nlohmann::json& data, std::ostream& os )
    {
        m_injaEnvironment.render_to( os, m_allocateTemplate, data );
    }
    void render_read( const nlohmann::json& data, std::ostream& os )
    {
        m_injaEnvironment.render_to( os, m_readTemplate, data );
    }
};

CodeGenerator::CodeGenerator( const mega::network::MegastructureInstallation& megastructureInstallation,
                              const mega::network::Project&                   project )
    : m_pPimpl( std::make_shared< Pimpl >( megastructureInstallation ) )
    , m_tempDir( project.getProjectTempDir() )
    , m_clangPath( megastructureInstallation.getClangPath() )
{
    if ( !boost::filesystem::exists( m_tempDir ) )
    {
        boost::filesystem::create_directories( m_tempDir );
    }
}

void CodeGenerator::generate_allocation( const DatabaseInstance& database, mega::TypeID objectTypeID, std::ostream& os )
{
    SPDLOG_INFO( "generate_allocation: {}", objectTypeID );

    const FinalStage::Concrete::Object*      pObject    = database.getObject( objectTypeID );
    const FinalStage::Components::Component* pComponent = pObject->get_component();

    std::ostringstream osObjectTypeID;
    osObjectTypeID << objectTypeID;
    nlohmann::json data( { { "ObjectTypeID", osObjectTypeID.str() },
                           { "shared_parts", nlohmann::json::array() },
                           { "heap_parts", nlohmann::json::array() } } );

    {
        using namespace FinalStage;
        for ( auto pBuffer : pObject->get_buffers() )
        {
            bool bBufferIsShared = false;
            if ( dynamic_database_cast< MemoryLayout::SimpleBuffer >( pBuffer ) )
                bBufferIsShared = true;

            for ( auto pPart : pBuffer->get_parts() )
            {
                std::ostringstream osPartType;
                std::ostringstream osPartName;
                // osPart << "part_" << pPart->get_context()->get_concrete_id();
                osPartType << "_Part" << pPart->get_context()->get_interface()->get_identifier();
                osPartName << pPart->get_context()->get_interface()->get_identifier();

                const std::size_t szTotalDomainSize
                    = database.getConcreteContextTotalAllocation( pPart->get_context()->get_concrete_id() );

                nlohmann::json part( { { "type", osPartType.str() },
                                       { "name", osPartName.str() },
                                       { "size", szTotalDomainSize },
                                       { "inits", std::vector< int >( szTotalDomainSize, 0 ) },
                                       { "members", nlohmann::json::array() } } );

                for ( auto pUserDim : pPart->get_user_dimensions() )
                {
                    nlohmann::json member( { { "type", pUserDim->get_interface_dimension()->get_canonical_type() },
                                             { "name", pUserDim->get_interface_dimension()->get_id()->get_str() } } );
                    part[ "members" ].push_back( member );
                }
                for ( auto pLinkDim : pPart->get_link_dimensions() )
                {
                    if ( Concrete::Dimensions::LinkMany* pLinkMany
                         = dynamic_database_cast< Concrete::Dimensions::LinkMany >( pLinkDim ) )
                    {
                        std::ostringstream osLinkName;
                        osLinkName << "link_" << pLinkMany->get_link()->get_concrete_id();
                        nlohmann::json member( { { "type", "mega::reference" }, { "name", osLinkName.str() } } );
                        part[ "members" ].push_back( member );
                    }
                    else if ( Concrete::Dimensions::LinkSingle* pLinkSingle
                              = dynamic_database_cast< Concrete::Dimensions::LinkSingle >( pLinkDim ) )
                    {
                        std::ostringstream osLinkName;
                        osLinkName << "link_" << pLinkSingle->get_link()->get_concrete_id();
                        nlohmann::json member( { { "type", "mega::ReferenceVector" }, { "name", osLinkName.str() } } );
                        part[ "members" ].push_back( member );
                    }
                    else
                    {
                        THROW_RTE( "Unknown link type" );
                    }
                }
                for ( auto pAllocDim : pPart->get_allocation_dimensions() )
                {
                    if ( Concrete::Dimensions::Allocator* pAllocator
                         = dynamic_database_cast< Concrete::Dimensions::Allocator >( pAllocDim ) )
                    {
                        Allocators::Allocator* pAllocBase = pAllocator->get_allocator();

                        if ( Allocators::Nothing* pAlloc = dynamic_database_cast< Allocators::Nothing >( pAllocBase ) )
                        {
                            // do nothing
                        }
                        else if ( Allocators::Singleton* pAlloc
                                  = dynamic_database_cast< Allocators::Singleton >( pAllocBase ) )
                        {
                            std::ostringstream osAllocName;
                            osAllocName << "alloc_" << pAlloc->get_allocated_context()->get_concrete_id();
                            nlohmann::json member( { { "type", "bool" }, { "name", osAllocName.str() } } );
                            part[ "members" ].push_back( member );
                        }
                        else if ( Allocators::Range32* pAlloc
                                  = dynamic_database_cast< Allocators::Range32 >( pAllocBase ) )
                        {
                            std::ostringstream osAllocName;
                            osAllocName << "alloc_" << pAlloc->get_allocated_context()->get_concrete_id();
                            std::ostringstream osTypeName;
                            osTypeName << "Bitmask32Allocator< "
                                       << database.getConcreteContextTotalAllocation(
                                              pAlloc->get_allocated_context()->get_concrete_id() )
                                       << " >";
                            nlohmann::json member( { { "type", osTypeName.str() }, { "name", osAllocName.str() } } );
                            part[ "members" ].push_back( member );
                        }
                        else if ( Allocators::Range64* pAlloc
                                  = dynamic_database_cast< Allocators::Range64 >( pAllocBase ) )
                        {
                            std::ostringstream osAllocName;
                            osAllocName << "alloc_" << pAlloc->get_allocated_context()->get_concrete_id();
                            std::ostringstream osTypeName;
                            osTypeName << "Bitmask64Allocator< "
                                       << database.getConcreteContextTotalAllocation(
                                              pAlloc->get_allocated_context()->get_concrete_id() )
                                       << " >";
                            nlohmann::json member( { { "type", osTypeName.str() }, { "name", osAllocName.str() } } );
                            part[ "members" ].push_back( member );
                        }
                        else if ( Allocators::RangeAny* pAlloc
                                  = dynamic_database_cast< Allocators::RangeAny >( pAllocBase ) )
                        {
                            std::ostringstream osAllocName;
                            osAllocName << "alloc_" << pAlloc->get_allocated_context()->get_concrete_id();
                            std::ostringstream osTypeName;
                            osTypeName << "RingAllocator< mega::Instance, "
                                       << database.getConcreteContextTotalAllocation(
                                              pAlloc->get_allocated_context()->get_concrete_id() )
                                       << " >";
                            nlohmann::json member( { { "type", osTypeName.str() }, { "name", osAllocName.str() } } );
                            part[ "members" ].push_back( member );
                        }
                        else
                        {
                            THROW_RTE( "Unknown allocator type" );
                        }
                    }
                    else
                    {
                        THROW_RTE( "Unknown allocation dimension type" );
                    }
                }

                if ( bBufferIsShared )
                    data[ "shared_parts" ].push_back( part );
                else
                    data[ "heap_parts" ].push_back( part );
            }
        }
    }

    GeneratorUtil generator( m_clangPath, m_tempDir );
    {
        auto pFileStream = generator.createSourceFile();
        m_pPimpl->render_allocation( data, *pFileStream );
    }
    generator.compile( pComponent, os );
}

void CodeGenerator::generate_allocate( const DatabaseInstance& database, const mega::InvocationID& invocationID,
                                       std::ostream& os )
{
    SPDLOG_INFO( "generate_allocate: {}", invocationID );
    // const FinalStage::Operations::Invocation* pInvocation = database.getInvocation( invocation );
    // using namespace FinalStage::Invocations;
    // Variables::Context* pRootVariable = pInvocation->get_root_variable();
    // pInvocation->get_root_instruction()

    std::ostringstream osName;
    osName << invocationID;
    nlohmann::json data( { { "name", osName.str() } } );

    GeneratorUtil generator( m_clangPath, m_tempDir );
    {
        auto pFileStream = generator.createSourceFile();
        m_pPimpl->render_allocate( data, *pFileStream );
    }
    generator.compile( os );
}

void CodeGenerator::generate_read( const DatabaseInstance& database, const mega::InvocationID& invocationID,
                                   std::ostream& os )
{
    SPDLOG_INFO( "generate_read: {}", invocationID );
    // const FinalStage::Operations::Invocation* pInvocation = database.getInvocation( invocation );
    // using namespace FinalStage::Invocations;
    // Variables::Context* pRootVariable = pInvocation->get_root_variable();
    // pInvocation->get_root_instruction()

    std::ostringstream osName;
    osName << invocationID;

    nlohmann::json data( { { "name", osName.str() } } );

    GeneratorUtil generator( m_clangPath, m_tempDir );
    {
        auto pFileStream = generator.createSourceFile();
        m_pPimpl->render_read( data, *pFileStream );
    }
    generator.compile( os );
}
} // namespace runtime
} // namespace mega
