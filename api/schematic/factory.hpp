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

#ifndef FACTORY_18_09_2013
#define FACTORY_18_09_2013

#include "schematic/file.hpp"
#include "schematic/site.hpp"
#include "schematic/cgalSettings.hpp"

#include "boost/filesystem/path.hpp"

namespace schematic
{

File::Ptr load( const boost::filesystem::path& filePath );
void      load( File::Ptr pFile, const boost::filesystem::path& filePath );
void      save( File::PtrCst pFile, const boost::filesystem::path& filePath );

Site::Ptr construct( Node::Ptr pParent, const format::Site& site );

Polygon formatPolygonFromPath( const format::Path& path );
void    formatPolygonToPath( const Polygon& polygon, format::Path& path );

} // namespace schematic

#endif