
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


#ifndef NAME_RESOLUTION_8_JUNE_2022
#define NAME_RESOLUTION_8_JUNE_2022

#include "database/model/OperationsStage.hxx"

#include <stdexcept>

namespace mega
{
namespace invocation
{

class NameResolutionException : public std::runtime_error
{
public:
    NameResolutionException( const std::string& strMessage )
        : std::runtime_error( strMessage )
    {
    }
};

OperationsStage::Operations::NameResolution* resolve( OperationsStage::Database&                  database,
                                                      OperationsStage::Operations::Invocation*    pInvocation

);

} // namespace invocation
} // namespace mega

#endif // NAME_RESOLUTION_8_JUNE_2022
