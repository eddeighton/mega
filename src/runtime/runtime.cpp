
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

#include "runtime/runtime.hpp"
#include "runtime/functor_dispatch.hpp"
#include "runtime/functor_id.hxx"
#include "runtime/clang.hpp"

#include "environment/environment.hpp"

#include "il/backend/backend.hpp"

namespace mega::runtime
{

Runtime::Runtime( const boost::filesystem::path& tempDir, const MegastructureInstallation& megaInstall )
    : m_tempDir( tempDir )
    , m_clang( megaInstall.getClangPath() )
    , m_orc( megaInstall )
{
    if( !boost::filesystem::exists( m_tempDir ) )
    {
        boost::filesystem::create_directories( m_tempDir );
    }
}

void Runtime::loadProgram( const service::Program& program )
{
    // if existing program then migrate...
    const auto                     programManifest = service::Environment::load( program );
    std::unique_ptr< JITDatabase > pNewDatabase    = std::make_unique< JITDatabase >( programManifest.getDatabase() );

    m_pDatabase.swap( pNewDatabase );
    m_program = program;
}
void Runtime::unloadProgram()
{
    m_program = service::Program{};
    m_pDatabase.reset();
}

service::Program Runtime::getProgram() const
{
    return m_program;
}

void Runtime::getFunction( service::StashProvider& stashProvider, const FunctorID& functionID, void** ppFunction )
{
    // attempt to find function in hash table
    auto iFind = m_materialisedFunctions.find( functionID );
    if( iFind == m_materialisedFunctions.end() )
    {
        VERIFY_RTE_MSG(
            m_pDatabase, "No program database loaded when attempting to materialise function: " << functionID );

        std::ostringstream osIR;
        {
            const auto functionDef = dispatchFactory( *m_pDatabase, m_materialisedFunctionFactory, functionID );

            const std::string           strCPPCode = il::generateCPP( functionDef );
            const task::DeterminantHash determinant{ strCPPCode };

            std::ostringstream osFunctionID;
            osFunctionID << m_program << '_' << functionID;
            const boost::filesystem::path irFilePath = m_tempDir / ( osFunctionID.str() + ".ir" );

            if( stashProvider.restore( irFilePath.string(), determinant.get() ) )
            {
                boost::filesystem::loadAsciiFile( irFilePath, osIR );
            }
            else
            {
                const boost::filesystem::path inputCPPFilePath = m_tempDir / ( osFunctionID.str() + ".cpp" );
                {
                    auto pFStream = boost::filesystem::createNewFileStream( inputCPPFilePath );
                    *pFStream << strCPPCode;
                }
                m_clang.compileToLLVMIR( inputCPPFilePath, irFilePath, nullptr );

                stashProvider.stash( irFilePath.string(), determinant.get() );
                boost::filesystem::loadAsciiFile( irFilePath, osIR );
            }
        }

        auto pModule = m_orc.compile( osIR.str() );

        void* pFunction = pModule->get( functionID );
        VERIFY_RTE_MSG( pFunction, "Failed to compiled function: " << functionID );

        FunctionInfo functionInfo{ pFunction, std::move( pModule ) };
        iFind = m_materialisedFunctions.insert( { functionID, std::move( functionInfo ) } ).first;
    }

    *ppFunction = iFind->second.pFunction;

    // record the pointer to the function pointer so can reset when reprogram
    iFind->second.functionPointers.push_back( ppFunction );
}

} // namespace mega::runtime
