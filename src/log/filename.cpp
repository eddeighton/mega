
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

#include "log/filename.hpp"

#include "common/assert_verify.hpp"

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_eoi.hpp>
#include <boost/spirit/include/support_line_pos_iterator.hpp>

#include <boost/phoenix/core.hpp>
#include <boost/phoenix/stl.hpp>
#include <boost/phoenix/fusion.hpp>
#include <boost/phoenix/operator.hpp>
#include <boost/phoenix/object.hpp>

#include <sstream>
#include <iomanip>

namespace mega::log
{

struct FileNameID
{
    std::string filename;
    U32         id;
};

} // namespace mega::log

// clang-format off
using MegaLogFileNameID = mega::log::FileNameID;
using MegaU32 = mega::U32;
BOOST_FUSION_ADAPT_STRUCT( MegaLogFileNameID,
    ( std::string, filename )
    ( MegaU32, id ) )
// clang-format on

namespace mega::log
{

const char* LOG_FILE_EXTENSION = ".log";

template < typename Iterator >
class FileNameGrammar : public boost::spirit::qi::grammar< Iterator, FileNameID() >
{
public:
    FileNameGrammar()
        : FileNameGrammar::base_type( m_main_rule, "filename" )
    {
        using namespace boost::spirit;
        using namespace boost::spirit::qi;
        using namespace boost::phoenix;
        m_identifier
            = char_( "a-zA-Z" )[ push_back( _val, qi::_1 ) ] >> *( char_( "a-zA-Z0-9" )[ push_back( _val, qi::_1 ) ] );
        m_main_rule = lexeme[ m_identifier[ at_c< 0 >( _val ) = qi::_1 ] >> lit( '_' )
                              >> m_logFileIndex_rule[ at_c< 1 >( _val ) = qi::_1 ] >> lit( LOG_FILE_EXTENSION ) ];
    }

    using LogFileIndexParser = boost::spirit::qi::uint_parser< U32, 10, LogFileIndexDigits, LogFileIndexDigits >;

    boost::spirit::qi::rule< Iterator, std::string() > m_identifier;
    LogFileIndexParser                                 m_logFileIndex_rule;
    boost::spirit::qi::rule< Iterator, FileNameID() >  m_main_rule;
};

boost::filesystem::path toFilePath( const boost::filesystem::path& logFolderPath, const std::string& strFileType,
                                    BufferIndex fileIndex )
{
    std::ostringstream os;
    os << strFileType << '_' << std::setfill( '0' ) << std::setw( LogFileIndexDigits ) << fileIndex.get()
       << LOG_FILE_EXTENSION;
    return logFolderPath / os.str();
}

bool fromFilePath( const boost::filesystem::path& logFilePath, std::string& strFileType, BufferIndex& index )
{
    const std::string                              strFileName = logFilePath.filename().string();
    FileNameGrammar< std::string::const_iterator > filenameGrammar;
    FileNameID                                     result;
    auto                                           iIter = strFileName.begin(), iEnd = strFileName.end();
    if ( boost::spirit::qi::parse(
             iIter, iEnd, filenameGrammar[ boost::phoenix::ref( result ) = boost::spirit::qi::_1 ] ) )
    {
        if ( iIter == iEnd )
        {
            strFileType = result.filename;
            index       = result.id;
            return true;
        }
    }

    return false;
}

} // namespace mega::log
