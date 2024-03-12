
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

#include "mega/reports.hpp"

#include "compiler/configuration.hpp"

#include "pipeline/configuration.hpp"
#include "pipeline/pipeline.hpp"

#include "environment/environment_build.hpp"
#include "database/serialisation.hpp"
#include "database/sources.hpp"

#include "database/manifest.hxx"
#include "database/environment.hxx"

#include "database_reporters/factory.hpp"

#include "report/renderer_html.hpp"

#include "mega/reports.hpp"

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
extern boost::filesystem::path                g_compiler_templatesDir;
extern boost::filesystem::path                g_report_templatesDir;
extern boost::filesystem::path                g_stashDir;
extern boost::filesystem::path                g_inputDir;
extern boost::filesystem::path                g_expectedDir;
extern boost::filesystem::path                g_resultDir;

class Compilation
{
public:
    using Ptr = std::unique_ptr< Compilation >;

    bool m_bSuccess = false;

    mega::io::Directories                  m_directories;
    mega::io::BuildEnvironment             m_environment;
    mega::pipeline::Configuration          m_pipeline;
    std::vector< boost::filesystem::path > m_sourceFiles;
    std::vector< boost::filesystem::path > m_includeDirs;

    const char* TEST_NAME;
    const char* TASK_NAME;

    Compilation( mega::io::Directories directories, std::vector< boost::filesystem::path >& sourceFiles,
                 std::vector< boost::filesystem::path >& includeDirs, const char* TEST_NAME, const char* TASK_NAME )
        : m_directories{ std::move( directories ) }
        , m_environment( m_directories )
        , m_sourceFiles( sourceFiles )
        , m_includeDirs( includeDirs )
        , TEST_NAME( TEST_NAME )
        , TASK_NAME( TASK_NAME )
    {
        std::vector< mega::io::ComponentInfo > componentInfos;
        {
            std::ostringstream osComponentName;
            osComponentName << TEST_NAME << ".so";
            const boost::filesystem::path componentFilePath = m_directories.buildDir / osComponentName.str();
            mega::io::ComponentInfo       componentInfo( mega::ComponentType::eInterface, TEST_NAME, componentFilePath,
                                                         g_cppFlags, g_cppDefines, m_directories.srcDir,
                                                         m_directories.buildDir, m_sourceFiles, {}, m_includeDirs );
            componentInfos.emplace_back( std::move( componentInfo ) );
        }

        const mega::io::Manifest manifest( m_environment, m_directories.srcDir, componentInfos );
        {
            const mega::io::manifestFilePath projectManifestPath = m_environment.project_manifest();
            manifest.save_temp( m_environment, projectManifestPath );
            m_environment.temp_to_real( projectManifestPath );
        }

        const mega::compiler::Configuration config = {

            { g_toolChain.megaCompilerPath.string(), mega::Version{} },
            TEST_NAME,
            componentInfos,
            m_directories,
            manifest };

        m_pipeline = mega::compiler::makePipelineConfiguration( config );
    }

    mega::pipeline::PipelineResult runPipeline( const std::string& strSourceFile )
    {
        return mega::pipeline::runPipelineLocally( g_stashDir, std::nullopt, g_toolChain, m_pipeline, TASK_NAME,
                                                   strSourceFile, {}, true, true, true, std::cout );
    };

    void generateReport( const std::string& strURL, const std::string& strFileNameNoExt )
    {
        std::ostringstream osOutFile;
        osOutFile << strFileNameNoExt << ".html";
        const boost::filesystem::path resultFile = g_resultDir / osOutFile.str();

        try
        {
            const mega::URL url = report::fromString( strURL );

            mega::io::Manifest manifest( m_environment, m_environment.project_manifest() );

            std::optional< mega::Report > resultOpt
                = generateCompilationReport( url, mega::reporters::CompilationReportArgs{ manifest, m_environment } );
            VERIFY_RTE_MSG( resultOpt.has_value(), "Failed to generate any report for: " << url.c_str() );

            // HTMLRenderer::JavascriptShortcuts shortcuts;
            report::HTMLTemplateEngine templateEngine( g_report_templatesDir, true );

            std::ostringstream os;
            report::renderHTML( resultOpt.value(), os, templateEngine );

            boost::filesystem::updateFileIfChanged( resultFile, os.str() );
        }
        catch( std::exception& ex )
        {
            THROW_RTE( "Error generating report: " << resultFile.string() << " exception: " << ex.what() );
        }
    }
};

inline Compilation::Ptr createBuildAndRun( const std::vector< std::string >& sourceFiles,
                                           const std::vector< std::string >& includeFiles, const char* TEST_NAME,
                                           const char* TASK_NAME )
{
    using CompilationType = Compilation;
    CompilationType::Ptr pImpl;
    try
    {
        // create directories
        boost::filesystem::path testFolder = boost::filesystem::temp_directory_path() / "compiler_tests" / TEST_NAME;
        boost::filesystem::create_directories( testFolder );
        VERIFY_RTE_MSG(
            boost::filesystem::exists( testFolder ), "Failed to create temporary folder: " << testFolder.string() );
        std::cout << "Created test: " << testFolder.string() << std::endl;

        boost::filesystem::path srcDir = testFolder / "src";
        boost::filesystem::create_directories( srcDir );
        boost::filesystem::path buildDir = testFolder / "build";
        boost::filesystem::create_directories( buildDir );
        boost::filesystem::path installDir = testFolder / "install";
        boost::filesystem::create_directories( installDir );

        mega::io::Directories directories{ srcDir, buildDir, installDir, g_compiler_templatesDir };

        // create files
        std::vector< boost::filesystem::path > sourceFilePaths;
        {
            for( const auto& strSourceFile : sourceFiles )
            {
                const boost::filesystem::path inputMegaSourceFile = g_inputDir / strSourceFile;
                VERIFY_RTE_MSG( boost::filesystem::exists( inputMegaSourceFile ),
                                "Failed to locate test input source file: " << inputMegaSourceFile.string() );

                // create test code source file by copying input test file
                const auto megaSourceFilePath = srcDir / strSourceFile;
                boost::filesystem::copyFileIfChanged( inputMegaSourceFile, megaSourceFilePath );
                sourceFilePaths.push_back( megaSourceFilePath );
            }
            for( const auto& includeFile : includeFiles )
            {
                const boost::filesystem::path inputMegaSourceFile = g_inputDir / includeFile;
                VERIFY_RTE_MSG( boost::filesystem::exists( inputMegaSourceFile ),
                                "Failed to locate test input source file: " << inputMegaSourceFile.string() );

                // create test code source file by copying input test file
                const auto megaSourceFilePath = srcDir / includeFile;
                boost::filesystem::copyFileIfChanged( inputMegaSourceFile, megaSourceFilePath );
            }
        }

        pImpl = std::make_unique< CompilationType >(
            std::move( directories ), sourceFilePaths, g_includeDirectories, TEST_NAME, TASK_NAME );
    }
    catch( std::exception& ex )
    {
        pImpl.reset();
        std::cout << "Setup failed with exception: " << ex.what() << std::endl;
        throw;
    }
    catch( ... )
    {
        pImpl.reset();
        std::cout << "Setup failed with unknown exception" << std::endl;
        throw;
    }
    return pImpl;
}

inline Compilation::Ptr createBuildAndRun( const std::string& srcFileName, const char* TEST_NAME,
                                           const char* TASK_NAME )
{
    return createBuildAndRun( { srcFileName }, {}, TEST_NAME, TASK_NAME );
}

template < typename TestDataType >
class CompilerTestFixture : public ::testing::TestWithParam< TestDataType >
{
public:
    static void  SetUpTestSuite() {}
    static void  TearDownTestSuite() {}
    virtual void SetUp() {}
    virtual void TearDown() {}
};

#endif // GUARD_2023_January_22_invoke_fixture
