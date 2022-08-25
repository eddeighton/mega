

#include "database/compiler/json_converter.hpp"
#include "database/compiler/generator.hpp"
#include "database/compiler/grammar.hpp"

#include "common/file.hpp"
#include "common/hash.hpp"
#include "common/stash.hpp"
#include "common/assert_verify.hpp"

#include "nlohmann/json.hpp"

#include <boost/filesystem/operations.hpp>
#include <boost/program_options.hpp>
#include <boost/stacktrace.hpp>
#include <boost/filesystem.hpp>

#include <iostream>
#include <iterator>
#include <algorithm>

boost::filesystem::path inputStringToPath( const std::string strPath )
{
    VERIFY_RTE_MSG( !strPath.empty(), "No output folder specified" );
    const boost::filesystem::path actualPath
        = boost::filesystem::edsCannonicalise( boost::filesystem::absolute( strPath ) );
    {
        VERIFY_RTE_MSG( boost::filesystem::exists( actualPath ),
                        "Could not locate output folder directory: " << actualPath.string() );
    }
    return actualPath;
}

using namespace std::literals;

static const std::vector< std::string > headerFiles = {
    //
    "ComponentListing.hxx"s,
    "ComponentListingView.hxx"s,
    "ConcreteStage.hxx"s,
    "ConcreteTypeAnalysis.hxx"s,
    "ConcreteTypeAnalysisView.hxx"s,
    "ConcreteTypeRollout.hxx"s,
    "data_AST.hxx"s,
    "data_Body.hxx"s,
    "data_Clang.hxx"s,
    "data_Components.hxx"s,
    "data_Concrete.hxx"s,
    "data_ConcreteTable.hxx"s,
    "data_Derivations.hxx"s,
    "data_DPGraph.hxx"s,
    "data_MemoryLayout.hxx"s,
    "data_Model.hxx"s,
    "data_Operations.hxx"s,
    "data_PerSourceConcreteTable.hxx"s,
    "data_PerSourceDerivations.hxx"s,
    "data_PerSourceModel.hxx"s,
    "data_PerSourceSymbols.hxx"s,
    "data_SymbolTable.hxx"s,
    "data_Tree.hxx"s,
    "DependencyAnalysis.hxx"s,
    "DependencyAnalysisView.hxx"s,
    "DerivationAnalysis.hxx"s,
    "DerivationAnalysisRollout.hxx"s,
    "DerivationAnalysisView.hxx"s,
    "environment.hxx"s,
    "file_info.hxx"s,
    "FinalStage.hxx"s,
    "HyperGraphAnalysis.hxx"s,
    "HyperGraphAnalysisRollout.hxx"s,
    "HyperGraphAnalysisView.hxx"s,
    "InterfaceAnalysisStage.hxx"s,
    "InterfaceStage.hxx"s,
    "manifest.hxx"s,
    "MemoryStage.hxx"s,
    "OperationsStage.hxx"s,
    "ParserStage.hxx"s,
    "SymbolAnalysis.hxx"s,
    "SymbolAnalysisView.hxx"s,
    "SymbolRollout.hxx"s };

static const std::vector< std::string > sourceFiles = {
    //
    "ComponentListing.cxx"s,
    "ComponentListingView.cxx"s,
    "ConcreteStage.cxx"s,
    "ConcreteTypeAnalysis.cxx"s,
    "ConcreteTypeAnalysisView.cxx"s,
    "ConcreteTypeRollout.cxx"s,
    "data.cxx"s,
    "DependencyAnalysis.cxx"s,
    "DependencyAnalysisView.cxx"s,
    "DerivationAnalysis.cxx"s,
    "DerivationAnalysisRollout.cxx"s,
    "DerivationAnalysisView.cxx"s,
    "environment.cxx"s,
    "file_info.cxx"s,
    "FinalStage.cxx"s,
    "HyperGraphAnalysis.cxx"s,
    "HyperGraphAnalysisRollout.cxx"s,
    "HyperGraphAnalysisView.cxx"s,
    "InterfaceAnalysisStage.cxx"s,
    "InterfaceStage.cxx"s,
    "manifest.cxx"s,
    "MemoryStage.cxx"s,
    "OperationsStage.cxx"s,
    "ParserStage.cxx"s,
    "SymbolAnalysis.cxx"s,
    "SymbolAnalysisView.cxx"s,
    "SymbolRollout.cxx"s,
};

bool restore( const boost::filesystem::path& stashDirectory, const boost::filesystem::path& outputAPIFolderPath,
              const boost::filesystem::path& outputSrcFolderPath, const task::DeterminantHash& determinant )
{
    task::Stash stash( stashDirectory );

    bool bRestored = true;
    for ( const auto& headerFile : headerFiles )
    {
        bRestored = bRestored && stash.restore( outputAPIFolderPath / headerFile, determinant );
    }
    for ( const auto& sourceFileName : sourceFiles )
    {
        bRestored = bRestored && stash.restore( outputSrcFolderPath / sourceFileName, determinant );
    }

    return bRestored;
}

void stash( const boost::filesystem::path& stashDirectory, const boost::filesystem::path& outputAPIFolderPath,
            const boost::filesystem::path& outputSrcFolderPath, const task::DeterminantHash& determinant )
{
    task::Stash stash( stashDirectory );
    for ( const auto& headerFile : headerFiles )
    {
        stash.stash( outputAPIFolderPath / headerFile, determinant );
    }
    for ( const auto& sourceFileName : sourceFiles )
    {
        stash.stash( outputSrcFolderPath / sourceFileName, determinant );
    }
}

int main( int argc, const char* argv[] )
{
    namespace po = boost::program_options;
    po::variables_map vm;
    try
    {
        bool bHelp        = false;
        bool bGeneralWait = false;

        std::vector< std::string > sourceFiles;
        std::string                outputAPIDir, outputSrcDir, dataDir, injaDir, stashDir;

        po::options_description commandOptions( " Commands" );
        {
            commandOptions.add_options()
                // clang-format off
            ( "help",   po::bool_switch( &bHelp ), "Print command line help info." )
            ( "wait",   po::bool_switch( &bGeneralWait ), "Wait at startup for attaching a debugger" )

            ( "api",    po::value< std::string >( &outputAPIDir ),  "Output folder to generate API" )
            ( "src",    po::value< std::string >( &outputSrcDir ),  "Output folder to generate source" )
            ( "stash",  po::value< std::string >( &stashDir ),      "Stash directory" )

            ( "data_dir", po::value< std::string >( &dataDir ), "Directory for inja templates and data sub directories" )
            ( "inja_dir", po::value< std::string >( &injaDir ), "Directory for inja templates and data sub directories" )

            ( "input",  po::value< std::vector< std::string > >( &sourceFiles ), "Input source file" )
            ;
            // clang-format on
        }

        po::positional_options_description p;
        p.add( "input", -1 );

        po::variables_map vm;
        po::store( po::command_line_parser( argc, argv ).options( commandOptions ).positional( p ).run(), vm );
        po::notify( vm );

        if ( bHelp )
        {
            std::cout << commandOptions << "\n";
        }
        else
        {
            using Path       = boost::filesystem::path;
            using PathVector = std::vector< Path >;

            const Path outputAPIFolderPath = inputStringToPath( outputAPIDir );
            const Path outputSrcFolderPath = inputStringToPath( outputSrcDir );

            const Path dataFolderPath = inputStringToPath( dataDir );
            const Path injaFolderPath = inputStringToPath( injaDir );

            {
                PathVector inputSourceFiles;
                for ( const std::string& strFile : sourceFiles )
                {
                    inputSourceFiles.push_back( inputStringToPath( strFile ) );
                }

                task::DeterminantHash determinant;
                for ( const Path& sourceFilePath : inputSourceFiles )
                {
                    determinant ^= sourceFilePath;
                }

                bool bRestored = true;
                if ( !stashDir.empty() )
                {
                    bRestored = restore( stashDir, outputAPIFolderPath, outputSrcFolderPath, determinant );
                }

                if ( !bRestored )
                {
                    db::schema::Schema schema;
                    for ( const Path& sourceFilePath : inputSourceFiles )
                    {
                        std::string strFileContents;
                        boost::filesystem::loadAsciiFile( sourceFilePath, strFileContents, true );

                        std::ostringstream      osError;
                        db::schema::Schema      fileSchema;
                        db::schema::ParseResult result = db::schema::parse( strFileContents, fileSchema, osError );
                        if ( result.bSuccess )
                        {
                            std::string::const_iterator iterReached = result.iterReached.base();
                            if ( iterReached == strFileContents.end() )
                            {
                                std::copy( fileSchema.m_elements.begin(), fileSchema.m_elements.end(),
                                           std::back_inserter( schema.m_elements ) );
                            }
                            else
                            {
                                const int         distance = std::distance( strFileContents.cbegin(), iterReached );
                                const int         distanceToEnd = std::distance( iterReached, strFileContents.cend() );
                                const std::string strError( strFileContents.cbegin() + std::max( 0, distance - 30 ),
                                                            iterReached + std::min( distanceToEnd - distance, 30 ) );
                                THROW_RTE( "Failed to load schema file: " << sourceFilePath.string()
                                                                          << " Could not parse beyond line: "
                                                                          << result.iterReached.position() << "\n"
                                                                          << strError << "\n"
                                                                          << osError.str() );
                            }
                        }
                        else
                        {
                            THROW_RTE( "Failed to load schema file: " << sourceFilePath.string()
                                                                      << " Error: " << osError.str() );
                        }
                    }

                    // std::cout << "Parsed schema:\n" << schema << std::endl;
                    db::model::Schema::Ptr pSchema = db::model::from_ast( schema );

                    // write schema to json data
                    db::jsonconv::toJSON( dataFolderPath, pSchema );

                    const db::gen::Environment env{
                        outputAPIFolderPath, outputSrcFolderPath, dataFolderPath, injaFolderPath };
                    db::gen::generate( env, pSchema );

                    if ( !stashDir.empty() )
                    {
                        stash( stashDir, outputAPIFolderPath, outputSrcFolderPath, determinant );
                    }
                }
            }
        }
    }
    catch ( boost::program_options::error& e )
    {
        std::cout << "Invalid input. " << e.what() << "\nType '--help' for options" << std::endl;
        std::cout << boost::stacktrace::stacktrace() << std::endl;
        return 1;
    }
    catch ( std::exception& e )
    {
        std::cout << "Error: " << e.what() << std::endl;
        std::cout << boost::stacktrace::stacktrace() << std::endl;
        return 1;
    }
    catch ( ... )
    {
        std::cout << "Unknown error.\n" << std::endl;
        std::cout << boost::stacktrace::stacktrace() << std::endl;
        return 1;
    }

    return 0;
}
