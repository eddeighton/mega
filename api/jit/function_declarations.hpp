
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

#ifndef GUARD_2023_January_25_function_declarations
#define GUARD_2023_January_25_function_declarations

#include "database/model/FinalStage.hxx"

#include "nlohmann/json.hpp"

#include <set>
#include <string>
#include <sstream>

namespace mega
{

struct FunctionDeclarations
{
    using CallSet        = std::set< const FinalStage::Concrete::Context* >;
    using CopySet        = std::set< std::string >;
    using EventSet       = std::set< std::string >;
    using AllocatorSet   = std::set< std::string >;
    using FreerSet       = std::set< std::string >;
    using ObjectAllocSet = std::set< const FinalStage::Concrete::Context* >;
    using ObjectFreeSet  = std::set< const FinalStage::Concrete::Context* >;
    using ObjectSaveSet  = std::set< const FinalStage::Concrete::Context* >;
    using ObjectLoadSet  = std::set< const FinalStage::Concrete::Context* >;

    CallSet        callSet;
    CopySet        copySet;
    EventSet       eventSet;
    AllocatorSet   allocatorSet;
    FreerSet       freerSet;
    ObjectAllocSet objectAllocSet;
    ObjectFreeSet  objectFreeSet;
    ObjectSaveSet  objectSave;
    ObjectLoadSet  objectLoad;

    nlohmann::json init( const InvocationID& invocationID, std::string& strName ) const
    {
        {
            using ::           operator<<;
            std::ostringstream osName;
            osName << invocationID;
            strName = osName.str();
        }

        return { { "name", strName },
                 { "module_name", strName },
                 { "calls", nlohmann::json::array() },
                 { "copiers", nlohmann::json::array() },
                 { "events", nlohmann::json::array() },
                 { "allocators", nlohmann::json::array() },
                 { "freers", nlohmann::json::array() },
                 { "object_allocators", nlohmann::json::array() },
                 { "variables", nlohmann::json::array() },
                 { "assignments", nlohmann::json::array() },
                 { "object_savers", nlohmann::json::array() } };
    }

    void generate( nlohmann::json& data )
    {
        for( auto pCall : callSet )
        {
            data[ "calls" ].push_back( pCall->get_concrete_id() );
        }
        for( auto copy : copySet )
        {
            data[ "copiers" ].push_back( copy );
        }
        for( auto event : eventSet )
        {
            data[ "events" ].push_back( event );
        }
        for( auto allocator : allocatorSet )
        {
            data[ "allocators" ].push_back( allocator );
        }
        for( auto freer : freerSet )
        {
            data[ "freers" ].push_back( freer );
        }
        for( auto pObject : objectAllocSet )
        {
            data[ "object_allocators" ].push_back( pObject->get_concrete_id() );
        }
        for( auto pObject : objectSave )
        {
            data[ "object_savers" ].push_back( pObject->get_concrete_id() );
        }
        for( auto pObject : objectLoad )
        {
            data[ "object_loaders" ].push_back( pObject->get_concrete_id() );
        }
    }
};

}

#endif //GUARD_2023_January_25_function_declarations
