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


#include "compiler/codegen/codegen.hpp"

#include "compiler/eg.hpp"
#include "compiler/input.hpp"
#include "compiler/interface.hpp"
#include "database/concrete.hpp"
#include "compiler/derivation.hpp"
#include "database/layout.hpp"
#include "compiler/identifiers.hpp"
#include "compiler/derivation.hpp"
#include "compiler/invocation.hpp"

#include <boost/algorithm/string.hpp>

#include <sstream>
#include <cctype>

namespace eg
{
    
    void generateBufferStructures( std::ostream& os, const ReadStage& program )
    {
        const Layout& layout = program.getLayout();
        
        generateIncludeGuard( os, "STRUCTURES" );
        
        os << "//data structures\n";
        for( const Buffer* pBuffer : layout.getBuffers() )
        {
            os << "\n//Buffer: " << pBuffer->getTypeName();
            if( const concrete::Action* pAction = pBuffer->getAction() )
            {
                os << " type: " << pAction->getIndex();
            }
            
            os << /*" stride: " << pBuffer->getStride() <<*/ " size: " << pBuffer->getSize() << "\n";
            os << "struct " << pBuffer->getTypeName() << "\n{\n";
            for( const DataMember* pDataMember : pBuffer->getDataMembers() )
            {
                os << "    ";
                generateDataMemberType( os, pDataMember );
                os << " " << pDataMember->getName() << ";\n";
            }
            
            os << "};\n";
            os << "extern " << pBuffer->getTypeName() << " *" << pBuffer->getVariableName() << ";\n";
            
        }
        
        os << "\n" << pszLine << pszLine;
        os << "#endif\n";
    }
}