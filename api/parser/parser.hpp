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

#ifndef PARSER_14_SEPT_2020
#define PARSER_14_SEPT_2020

#include <boost/filesystem/path.hpp>

#include <ostream>
#include <memory>

#if defined( _MSC_VER )

// Microsoft
#ifdef EG_DATABASE_SHARED_MODULE
#define EGDB_EXPORT __declspec( dllexport )
#else
#define EGDB_EXPORT __declspec( dllimport )
#endif

#elif defined( __GNUC__ )

// GCC
#ifdef EG_DATABASE_SHARED_MODULE
#define EGDB_EXPORT __attribute__( ( visibility( "default" ) ) )
#else
#define EGDB_EXPORT
#endif

#endif

namespace ParserStage
{
class Database;

namespace Parser
{
class ContextDef;
}
} // namespace ParserStage

struct EGDB_EXPORT EG_PARSER_INTERFACE
{
    virtual ParserStage::Parser::ContextDef*
    parseEGSourceFile( ParserStage::Database&                        database,
                       const boost::filesystem::path&                sourceFilePath,
                       const std::vector< boost::filesystem::path >& includeDirectories,
                       std::ostream&                                 osError,
                       std::ostream&                                 osWarn )
        = 0;
};

//std::unique_ptr< EG_PARSER_INTERFACE > getParser();

#endif // PARSER_14_SEPT_2020