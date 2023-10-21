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

#ifndef COMPILER_CONFIGURATION_27_MAY_2022
#define COMPILER_CONFIGURATION_27_MAY_2022

#include "mega/values/compilation/compilation_configuration.hpp"

#include "pipeline/configuration.hpp"

#include <boost/filesystem/path.hpp>

#include <vector>
#include <string>

namespace mega::compiler
{

pipeline::Configuration       makePipelineConfiguration( const mega::compiler::Configuration& config );
mega::compiler::Configuration fromPipelineConfiguration( const pipeline::Configuration& pipelineConfig );

} // namespace mega::compiler

#endif // COMPILER_CONFIGURATION_27_MAY_2022
