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

#ifndef COMPONENT_MANAGER_20_JUNE_2022
#define COMPONENT_MANAGER_20_JUNE_2022

#include "functions.hpp"

#include "database/database.hpp"

#include "service/protocol/common/project.hpp"

#include <boost/filesystem/path.hpp>
#include <boost/dll/import.hpp>
#include <boost/dll/alias.hpp>
#include <boost/dll/library_info.hpp>

#include <map>
#include <memory>
#include <unordered_map>

namespace mega::runtime
{

class ComponentManager
{
    using ComponentPath = boost::dll::fs::path;

    using FunctionPtr    = boost::shared_ptr< TypeErasedFunction* >;
    using FunctionPtrMap = std::unordered_map< mega::TypeID, FunctionPtr >;

    struct InterfaceComponent
    {
        using Ptr = std::shared_ptr< InterfaceComponent >;

        static ComponentPath makeTempComponent( const ComponentPath& path );
        InterfaceComponent( const ComponentPath& path, FunctionPtrMap& functions );

        const ComponentPath& getPath() const { return m_path; }

    private:
        ComponentPath              m_path;
        ComponentPath              m_tempPath;
        boost::dll::library_info   m_libraryInfo;
        boost::dll::shared_library m_library;
    };

    using InterfaceComponentMap = std::map< ComponentPath, InterfaceComponent::Ptr >;

public:
    using Ptr = std::unique_ptr< ComponentManager >;

    ComponentManager( const mega::network::Project& project, DatabaseInstance& database );

    TypeErasedFunction getOperationFunctionPtr( mega::TypeID concreteTypeID );

private:
    void loadComponent() {}

    const mega::network::Project& m_project;
    DatabaseInstance&             m_database;
    InterfaceComponentMap         m_interfaceComponents;
    FunctionPtrMap                m_functions;
};

} // namespace mega::runtime

#endif // COMPONENT_MANAGER_20_JUNE_2022
