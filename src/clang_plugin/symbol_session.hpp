
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

#ifndef GUARD_2023_December_30_symbol_resolving_session
#define GUARD_2023_December_30_symbol_resolving_session

#include "mega/values/compilation/reserved_symbols.hpp"

#include "session.hpp"

namespace clang
{

class SymbolSession : public Session
{
protected:
    const boost::filesystem::path m_srcDir, m_buildDir;
    mega::io::Directories         m_directories;
    mega::io::BuildEnvironment    m_environment;

public:
    using Ptr = std::unique_ptr< Session >;
    SymbolSession( ASTContext* pASTContext, Sema* pSema, const char* strSrcDir, const char* strBuildDir )
        : Session( pASTContext, pSema )
        , m_srcDir( strSrcDir )
        , m_buildDir( strBuildDir )
        , m_directories{ m_srcDir, m_buildDir, "", "" }
        , m_environment( m_directories )
    {
    }

    virtual unsigned int getSymbolID( const std::string& strIdentifier ) const override
    {
        mega::interface::SymbolID symbolID;
        if( mega::interface::getReservedSymbolIDMaybe( strIdentifier, symbolID ) )
        {
            return symbolID.getValue();
        }
        else
        {
            return 0;
        }
    }
};

}

#endif //GUARD_2023_December_30_symbol_resolving_session
