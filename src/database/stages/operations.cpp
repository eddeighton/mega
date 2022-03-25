//  Copyright (c) Deighton Systems Limited. 2019. All Rights Reserved.
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

#include "database/stages/operations.hpp"

namespace mega
{
namespace Stages
{
    Operations::Operations( const boost::filesystem::path& treePath,
                            const boost::filesystem::path& tuPath,
                            io::Object::FileID             fileID )
        : Creating( io::File::FileIDtoPathMap{ { io::Object::NO_FILE, treePath } }, fileID )
        , m_tuPath( tuPath )
        , m_pDerivationAnalysis( io::one_cst< DerivationAnalysis >( getMaster() ) )
        , m_invocations( *this, *m_pDerivationAnalysis )
    {
        m_pIdentifiers = io::one_cst< Identifiers >( getMaster() );
    }

    void Operations::store() const
    {
        Creating::store( m_tuPath );
    }

    const Identifiers& Operations::getIdentifiers() const
    {
        return *io::one< Identifiers >( getObjects( io::Object::NO_FILE ) );
    }

    const InvocationSolution* Operations::getInvocation(
        const InvocationSolution::InvocationID& invocationID, const std::vector< mega::TypeID >& implicitTypePath )
    {
        return m_invocations.getInvocation( invocationID, implicitTypePath );
    }
} // namespace Stages
} // namespace mega
