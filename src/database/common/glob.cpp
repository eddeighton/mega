
#include "database/common/glob.hpp"

#include "common/assert_verify.hpp"
#include "common/terminal.hpp"

#include "boost/filesystem.hpp"
#include "boost/tokenizer.hpp"

#include <sstream>

#define VERIFY_PARSER( expression, msg )                                                        \
    DO_STUFF_AND_REQUIRE_SEMI_COLON( if ( !( expression ) ) {                                   \
        std::ostringstream _os2;                                                                \
        _os2 << common::COLOUR_RED_BEGIN << msg << ". Source Location: " << common::COLOUR_END; \
        throw GlobException( _os2.str() );                                                      \
    } )

namespace mega
{
    namespace io
    {
        void recurseFiles( const boost::filesystem::path&             location,
                           std::vector< std::string >::const_iterator i,
                           std::vector< std::string >::const_iterator iNext,
                           std::vector< std::string >::const_iterator iEnd,
                           bool                                       bCaptureAll,
                           std::vector< boost::filesystem::path >&    results )
        {
            const std::string& str = *i;
            if ( iNext != iEnd )
            {
                for ( auto const& dir_entry : boost::filesystem::directory_iterator( location ) )
                {
                    const boost::filesystem::path& filePath = dir_entry.path();
                    if ( boost::filesystem::is_regular_file( dir_entry ) )
                    {
                        if ( bCaptureAll )
                        {
                            results.push_back( filePath );
                        }
                    }
                    else if ( boost::filesystem::is_directory( dir_entry ) )
                    {
                        if ( bCaptureAll || ( str == "**" ) )
                        {
                            recurseFiles( filePath, ++i, ++iNext, iEnd, true, results );
                        }
                        else if ( ( str == "*" ) || ( str == filePath.filename() ) )
                        {
                            recurseFiles( filePath, ++i, ++iNext, iEnd, false, results );
                        }
                    }
                }
            }
            else
            {
                for ( auto const& dir_entry : boost::filesystem::directory_iterator( location ) )
                {
                    const boost::filesystem::path& filePath = dir_entry.path();
                    if ( boost::filesystem::is_regular_file( dir_entry ) )
                    {
                        if ( ( str == "**" ) || ( str == "*" ) || ( str == filePath.filename() ) || bCaptureAll )
                        {
                            results.push_back( filePath );
                        }
                    }
                    else if ( boost::filesystem::is_directory( dir_entry ) )
                    {
                        if ( ( str == "**" ) || bCaptureAll )
                        {
                            recurseFiles( filePath, i, iNext, iEnd, true, results );
                        }
                    }
                }
            }
        }

        void resolveGlob( const Glob& glob, const boost::filesystem::path& srcDir, FilePathVector& results )
        {
            boost::filesystem::path location = glob.source_file;

            bool bAbsolute = false;
            if ( glob.glob.front() == '/' )
            {
                // use absolute path relative to source directory
                location  = srcDir;
                bAbsolute = true;
            }

            std::vector< std::string > elements;
            {
                boost::char_separator< char > sep( "/" );
                using Tokeniser = boost::tokenizer< boost::char_separator< char > >;
                Tokeniser tokens( glob.glob, sep );
                for ( Tokeniser::iterator tok_iter = tokens.begin(); tok_iter != tokens.end(); ++tok_iter )
                {
                    elements.push_back( *tok_iter );
                }
                VERIFY_PARSER( !elements.empty(), "Glob contains no specification" );
            }

            {
                std::vector< std::string >::const_iterator i = elements.begin(), iNext = elements.begin(), iEnd = elements.end();
                for ( ; i != iEnd; ++i )
                {
                    const std::string& str = *i;

                    if ( iNext != iEnd )
                    {
                        ++iNext;

                        if ( str == ".." )
                        {
                            VERIFY_PARSER( !bAbsolute && location.has_parent_path(),
                                           "Cannot use .. in absolute dependency path of: " << location.string() );
                            location = location.parent_path();
                        }
                        else
                        {
                            // expand to all nested directories
                            recurseFiles( location, i, iNext, iEnd, false, results );
                            break;
                        }
                    }
                    else
                    {
                        if ( str == ".." )
                        {
                            VERIFY_PARSER( false, "Cannot use .. as final element in dependency: " << location.string() );
                        }
                        else if ( str == "**" )
                        {
                            VERIFY_PARSER( false, "Cannot use ** as final element in dependency: " << location.string() );
                        }
                        else
                        {
                            // expand to all nested directories
                            recurseFiles( location, i, iNext, iEnd, false, results );
                            break;
                        }
                    }
                }
            }
        }

    } // namespace io

} // namespace mega