
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

#ifndef GUARD_2023_January_22_invoke_fixture
#define GUARD_2023_January_22_invoke_fixture

#include "mega/values/compilation/tool_chain_hash.hpp"

#include "compiler/configuration.hpp"

#include "pipeline/configuration.hpp"
#include "pipeline/pipeline.hpp"

#include "environment/environment_build.hpp"
#include "database/serialisation.hpp"
#include "database/sources.hpp"

#include "database/manifest.hxx"
#include "database/environment.hxx"

#include "common/file.hpp"
#include "common/string.hpp"
#include "common/assert_verify.hpp"

#include <boost/filesystem/path.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

#include <gtest/gtest.h>
#include <gtest/gtest-param-test.h>

#include <memory>
#include <iostream>
#include <utility>

extern mega::utilities::ToolChain             g_toolChain;
extern std::vector< std::string >             g_cppFlags;
extern std::vector< std::string >             g_cppDefines;
extern std::vector< boost::filesystem::path > g_includeDirectories;
extern boost::filesystem::path                g_templatesDir;
extern boost::filesystem::path                g_stashDir;

template < const char* TEST_NAME, const char* CODE, typename TestDataType >
class InvocationTestFixture : public ::testing::TestWithParam< TestDataType >
{
public:
    struct Impl
    {
        using Ptr = std::unique_ptr< Impl >;

        bool m_bSuccess = false;

        mega::io::Directories         m_directories;
        mega::io::BuildEnvironment    m_environment;
        mega::pipeline::Configuration m_pipeline;
        boost::filesystem::path       m_srcFilePath;
        mega::io::megaFilePath        m_megaSrcPath;

        Impl( mega::io::Directories directories )
            : m_directories{ std::move( directories ) }
            , m_environment( m_directories )
        {
            {
                std::ostringstream osFileName;
                osFileName << TEST_NAME << ".mega";
                // create test code source file
                m_srcFilePath = m_directories.srcDir / osFileName.str();
            }

            {
                auto pSrcFileStream = boost::filesystem::createNewFileStream( m_srcFilePath );
                *pSrcFileStream << CODE;
            }
            m_megaSrcPath = m_environment.megaFilePath_fromPath( m_srcFilePath );

            const mega::io::ComponentListingFilePath componentListingFilePath
                = m_environment.ComponentListingFilePath_fromPath( m_directories.buildDir );

            std::vector< mega::io::ComponentInfo > componentInfos;
            {
                std::ostringstream osComponentName;
                osComponentName << TEST_NAME << ".so";
                const boost::filesystem::path componentFilePath = m_directories.buildDir / osComponentName.str();
                const mega::io::ComponentInfo componentInfo(
                    mega::ComponentType::eInterface, TEST_NAME, componentFilePath, g_cppFlags, g_cppDefines,
                    m_directories.srcDir, m_directories.buildDir, { m_srcFilePath }, {}, g_includeDirectories );
                componentInfos.emplace_back( std::move( componentInfo ) );
            }
            m_environment.temp_to_real( componentListingFilePath );
            const boost::filesystem::path actualComponentInfoFilePath
                = m_directories.buildDir / componentListingFilePath.path();
            VERIFY_RTE( boost::filesystem::exists( actualComponentInfoFilePath ) );

            const mega::compiler::Configuration config = { { g_toolChain.megaCompilerPath.string(), mega::Version{} },
                                                           TEST_NAME,
                                                           componentInfos,
                                                           m_directories };

            m_pipeline = mega::compiler::makePipelineConfiguration( config );
        }

        mega::pipeline::PipelineResult runPipeline()
        {
            return mega::pipeline::runPipelineLocally( g_stashDir, std::nullopt, g_toolChain, m_pipeline,
                                                       "Task_OperationsPCH", "", {}, false, true, true, std::cout );
        };
    };

    static void SetUpTestSuite()
    {
        try
        {
            // create directories
            boost::filesystem::path testFolder
                = boost::filesystem::temp_directory_path() / "invocation_tests" / TEST_NAME;
            boost::filesystem::create_directories( testFolder );
            VERIFY_RTE_MSG(
                boost::filesystem::exists( testFolder ), "Failed to create temporary folder: " << testFolder.string() );

            boost::filesystem::path srcDir = testFolder / "src";
            boost::filesystem::create_directories( srcDir );
            boost::filesystem::path buildDir = testFolder / "build";
            boost::filesystem::create_directories( buildDir );
            boost::filesystem::path installDir = testFolder / "install";
            boost::filesystem::create_directories( installDir );

            m_pImpl = std::make_unique< Impl >( mega::io::Directories{ srcDir, buildDir, installDir, g_templatesDir } );

            mega::pipeline::PipelineResult result = m_pImpl->runPipeline();
            ASSERT_TRUE( result.m_bSuccess );
        }
        catch( std::exception& ex )
        {
            m_pImpl.reset();
            FAIL() << "Setup failed with exception: " << ex.what() << std::endl;
        }
        catch( ... )
        {
            m_pImpl.reset();
            FAIL() << "Setup failed with unknown exception" << std::endl;
        }
    }

    static void TearDownTestSuite() { m_pImpl.reset(); }

    // Sets up the test fixture.
    virtual void SetUp() {}

    // Tears down the test fixture.
    virtual void TearDown() {}

    static typename Impl::Ptr m_pImpl;
};

#endif // GUARD_2023_January_22_invoke_fixture
