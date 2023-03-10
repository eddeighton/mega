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




#ifndef OBJECT_LOADER_9_APRIL_2022
#define OBJECT_LOADER_9_APRIL_2022

#include "database/common/api.hpp"
#include "object_info.hpp"
#include "object.hpp"

namespace data
{
    class EGDB_EXPORT ObjectPartLoader
    {
    public:
        virtual ~ObjectPartLoader() {}
        virtual mega::io::Object* load( const mega::io::ObjectInfo& objectInfo ) = 0;
    };

    class EGDB_EXPORT NullObjectPartLoader : public ObjectPartLoader
    {
        virtual mega::io::Object* load( const mega::io::ObjectInfo& objectInfo )
        {
            THROW_RTE( "load called on NullObjectPartLoader" );
        }
    };
}

#endif //OBJECT_LOADER_9_APRIL_2022
