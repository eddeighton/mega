
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

#ifndef GUARD_2024_March_11_reports
#define GUARD_2024_March_11_reports

#include "mega/values/value.hpp"

#include "report/report.hpp"
#include "report/colours.hxx"
#include "report/url.hpp"

namespace mega
{

using URL    = report::URL;
using Colour = report::Colour;

using ValueVector     = report::ValueVector< mega::Value >;
using ContainerVector = report::ContainerVector< mega::Value >;

using Report    = report::Container< mega::Value >;
using Branch    = report::Branch< mega::Value >;
using Line      = report::Line< mega::Value >;
using Multiline = report::Multiline< mega::Value >;
using Table     = report::Table< mega::Value >;
using Graph     = report::Graph< mega::Value >;

} // namespace mega

#endif // GUARD_2024_March_11_reports
