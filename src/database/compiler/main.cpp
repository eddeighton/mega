

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

db::schema::Schema loadSchema( const std::vector< boost::filesystem::path >& inputSourceFiles )
{
    db::schema::Schema schema;
    for ( const boost::filesystem::path& sourceFilePath : inputSourceFiles )
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
                const int         distance      = std::distance( strFileContents.cbegin(), iterReached );
                const int         distanceToEnd = std::distance( iterReached, strFileContents.cend() );
                const std::string strError( strFileContents.cbegin() + std::max( 0, distance - 30 ),
                                            iterReached + std::min( distanceToEnd - distance, 30 ) );
                THROW_RTE( "Failed to load schema file: " << sourceFilePath.string() << " Could not parse beyond line: "
                                                          << result.iterReached.position() << "\n"
                                                          << strError << "\n"
                                                          << osError.str() );
            }
        }
        else
        {
            THROW_RTE( "Failed to load schema file: " << sourceFilePath.string() << " Error: " << osError.str() );
        }
    }
    return schema;
}

struct OutputFiles
{
    std::vector< std::string > headerFiles;
    std::vector< std::string > sourceFiles;
};
void calculateOutputFiles( const db::schema::Schema& schema, OutputFiles& outputFiles )
{
    using namespace std::literals;
    static const std::vector< std::string > constantHeaderFiles
        = { "environment.hxx"s, "file_info.hxx"s, "manifest.hxx"s, "data.hxx"s };
    static const std::vector< std::string > constantSourceFiles
        = { "data.cxx"s, "environment.cxx"s, "file_info.cxx"s, "manifest.cxx"s };

    outputFiles.headerFiles = constantHeaderFiles;
    outputFiles.sourceFiles = constantSourceFiles;

    struct Visitor
    {
        OutputFiles& outputFiles;
        Visitor( OutputFiles& outputFiles )
            : outputFiles( outputFiles )
        {
        }
        void operator()( const db::schema::Stage& stage ) const
        {
            // view header file
            {
                std::ostringstream os;
                os << stage.m_name << ".hxx";
                outputFiles.headerFiles.push_back( os.str() );
            }
            // view source file
            {
                std::ostringstream os;
                os << stage.m_name << ".cxx";
                outputFiles.sourceFiles.push_back( os.str() );
            }
        }
        void operator()( const db::schema::Namespace& namespace_ ) const {}
    } visitor( outputFiles );

    for ( const auto& element : schema.m_elements )
    {
        boost::apply_visitor( visitor, element );
    }
}

std::optional< task::Stash > g_stashOpt;

bool restore( const boost::filesystem::path& outputAPIFolderPath, const boost::filesystem::path& outputSrcFolderPath,
              const task::DeterminantHash& determinant, const OutputFiles& outputFiles )
{
    bool bRestored = false;
    if ( g_stashOpt.has_value() )
    {
        bRestored = true;
        for ( const auto& headerFile : outputFiles.headerFiles )
        {
            if ( !g_stashOpt.value().restore( outputAPIFolderPath / headerFile, determinant ) )
            {
                bRestored = false;
                break;
            }
        }
        for ( const auto& sourceFileName : outputFiles.sourceFiles )
        {
            if ( !g_stashOpt.value().restore( outputSrcFolderPath / sourceFileName, determinant ) )
            {
                bRestored = false;
                break;
            }
        }
    }
    return bRestored;
}

void stash( const boost::filesystem::path& outputAPIFolderPath, const boost::filesystem::path& outputSrcFolderPath,
            const task::DeterminantHash& determinant, const OutputFiles& outputFiles )
{
    if ( g_stashOpt.has_value() )
    {
        for ( const auto& headerFile : outputFiles.headerFiles )
        {
            g_stashOpt.value().stash( outputAPIFolderPath / headerFile, determinant );
        }
        for ( const auto& sourceFileName : outputFiles.sourceFiles )
        {
            g_stashOpt.value().stash( outputSrcFolderPath / sourceFileName, determinant );
        }
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
        std::string                outputAPIDir, outputSrcDir, dataDir, injaDir, databaseCompilerLib, stashDir;

        po::options_description commandOptions( " Commands" );
        {
            commandOptions.add_options()
                // clang-format off
            ( "help",   po::bool_switch( &bHelp ), "Print command line help info." )
            ( "wait",   po::bool_switch( &bGeneralWait ), "Wait at startup for attaching a debugger" )

            ( "api",    po::value< std::string >( &outputAPIDir ),          "Output folder to generate API" )
            ( "src",    po::value< std::string >( &outputSrcDir ),          "Output folder to generate source" )
            ( "dblib",  po::value< std::string >( &databaseCompilerLib ),   "Path to database compiler library" )
            ( "stash",  po::value< std::string >( &stashDir ),              "Stash directory" )

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

            const Path outputAPIFolderPath     = inputStringToPath( outputAPIDir );
            const Path outputSrcFolderPath     = inputStringToPath( outputSrcDir );
            const Path databaseCompilerLibPath = inputStringToPath( databaseCompilerLib );
            const Path dataFolderPath          = inputStringToPath( dataDir );
            const Path injaFolderPath          = inputStringToPath( injaDir );

            {
                PathVector inputSourceFiles;
                for ( const std::string& strFile : sourceFiles )
                {
                    inputSourceFiles.push_back( inputStringToPath( strFile ) );
                }

                task::DeterminantHash determinant{ databaseCompilerLibPath };
                for ( const Path& sourceFilePath : inputSourceFiles )
                {
                    determinant ^= sourceFilePath;
                }
                // add all templates in injaFolderPath
                {
                    for ( boost::filesystem::directory_iterator i( injaFolderPath ), iEnd; i != iEnd; ++i )
                    {
                        const auto filePath = i->path();
                        if ( boost::filesystem::is_regular_file( filePath ) )
                        {
                            VERIFY_RTE( boost::filesystem::extension( filePath ) == ".jinja" );
                            determinant ^= filePath;
                        }
                    }
                }

                const db::schema::Schema schema = loadSchema( inputSourceFiles );
                OutputFiles              outputFiles;
                calculateOutputFiles( schema, outputFiles );

                if ( !stashDir.empty() )
                {
                    g_stashOpt = task::Stash( stashDir );
                }

                if ( !restore( outputAPIFolderPath, outputSrcFolderPath, determinant, outputFiles ) )
                {
                    // generate json
                    {
                        // std::cout << "Parsed schema:\n" << schema << std::endl;
                        db::model::Schema::Ptr pSchema = db::model::from_ast( schema );

                        // write schema to json data
                        db::jsonconv::toJSON( dataFolderPath, pSchema );
                    }

                    // generate source code
                    {
                        const db::gen::Environment env{
                            outputAPIFolderPath, outputSrcFolderPath, dataFolderPath, injaFolderPath };
                        db::gen::generate( env );
                    }

                    stash( outputAPIFolderPath, outputSrcFolderPath, determinant, outputFiles );

                    std::cout << "Database compiler regenerated: "
                              << outputFiles.headerFiles.size() + outputFiles.sourceFiles.size()
                              << " database source files" << std::endl;
                }
                else
                {
                    std::cout << "Database compiler restored: "
                              << outputFiles.headerFiles.size() + outputFiles.sourceFiles.size()
                              << " database source files without recompiling" << std::endl;
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
