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

#ifndef CODE_GENERATOR_16_AUG_2022
#define CODE_GENERATOR_16_AUG_2022

#include "database/database.hpp"

#include "service/protocol/common/megastructure_installation.hpp"
#include "service/protocol/common/project.hpp"

#include <memory>

namespace mega::runtime
{
class CodeGenerator
{
    class Pimpl;

public:
    CodeGenerator( const mega::network::MegastructureInstallation& megastructureInstallation,
                   const mega::network::Project&                   project );

    void generate_allocation( const DatabaseInstance& database, mega::TypeID objectTypeID, std::ostream& os );
    void generate_allocate( const DatabaseInstance& database, const mega::InvocationID& invocationID,
                            std::ostream& os );
    void generate_read( const DatabaseInstance& database, const mega::InvocationID& invocationID, std::ostream& os );
    void generate_write( const DatabaseInstance& database, const mega::InvocationID& invocationID, std::ostream& os );
    void generate_call( const DatabaseInstance& database, const mega::InvocationID& invocationID, std::ostream& os );
    void generate_start( const DatabaseInstance& database, const mega::InvocationID& invocationID, std::ostream& os );
    void generate_stop( const DatabaseInstance& database, const mega::InvocationID& invocationID, std::ostream& os );

private:
    nlohmann::json generate( const DatabaseInstance& database, const mega::InvocationID& invocationID,
                             std::string& strName ) const;
    void compileToIR( const boost::filesystem::path& sourcePath, const boost::filesystem::path& irPath ) const;



private:
    std::shared_ptr< Pimpl > m_pPimpl;
};
} // namespace mega::runtime

#endif // CODE_GENERATOR_16_AUG_2022
