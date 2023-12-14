
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

#ifndef GUARD_2023_November_27_generator
#define GUARD_2023_November_27_generator

#include "model.hpp"

#include <inja/inja.hpp>
#include <inja/environment.hpp>

#include "nlohmann/json.hpp"

#include <boost/filesystem/path.hpp>

#include <vector>

namespace il_gen
{
nlohmann::json generateJSON( const Model& model, const std::vector< boost::filesystem::path >& includes );

void generate( inja::Environment& injaEnv, const nlohmann::json& model, const std::string& strTemplate,
               const boost::filesystem::path& outputFile );

} // namespace il_gen

#endif // GUARD_2023_November_27_generator
