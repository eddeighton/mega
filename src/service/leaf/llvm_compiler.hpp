
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

#ifndef GUARD_2022_October_22_LLVMComiler
#define GUARD_2022_October_22_LLVMComiler

#include "jit/code_generator.hpp"

#include <boost/filesystem/operations.hpp>

namespace mega
{
namespace network
{
class LogicalThreadBase;
class Sender;
} // namespace network
namespace service
{

class LLVMCompilerImpl : public runtime::CodeGenerator::LLVMCompiler
{
public:
    LLVMCompilerImpl( network::LogicalThreadBase& logicalthread, std::shared_ptr< mega::network::Sender > pSender,
                      const mega::MegastructureInstallation& megaInstall, const mega::Project& project,
                      boost::asio::yield_context& yield_ctx )
        : m_logicalthread( logicalthread )
        , m_pSender( pSender )
        , m_megaInstall( megaInstall )
        , m_tempDir( project.getProjectTempDir() )
        , m_clangPath( megaInstall.getClangPath() )
        , m_yield_ctx( yield_ctx )
    {
        if( !boost::filesystem::exists( m_tempDir ) )
        {
            boost::filesystem::create_directories( m_tempDir );
        }
    }

    virtual void compileToLLVMIR( const std::string& strName, const std::string& strCPPCode, std::ostream& osIR,
                                  std::optional< const FinalStage::Components::Component* > pComponent ) const override;

    const mega::MegastructureInstallation& getMegaInstall() const { return m_megaInstall; }
    const boost::filesystem::path&         getTempDir() const { return m_tempDir; }
    const boost::filesystem::path&         getClangPath() const { return m_clangPath; }

    void stash( const std::string& filePath, mega::U64 determinant ) const;
    bool restore( const std::string& filePath, mega::U64 determinant ) const;

private:
    network::LogicalThreadBase&              m_logicalthread;
    std::shared_ptr< mega::network::Sender > m_pSender;
    mega::MegastructureInstallation          m_megaInstall;
    boost::filesystem::path                  m_clangPath;
    boost::filesystem::path                  m_tempDir;
    boost::asio::yield_context&              m_yield_ctx;
};
} // namespace service
} // namespace mega

#endif // GUARD_2022_October_22_LLVMComiler
