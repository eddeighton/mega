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


#ifndef DERIVATION_18_04_2019
#define DERIVATION_18_04_2019

#include "eg.hpp"
#include "objects.hpp"
#include "concrete.hpp"
#include "database/stages/stage.hpp"

#include "mega/common.hpp"

#include "common/file.hpp"

#include <map>
#include <set>
#include <vector>
#include <tuple>

namespace eg
{
    template< class T >
    inline std::vector< T > uniquify_without_reorder( const std::vector< T >& ids )
    {
        /*
        not this...
        std::sort( ids.begin(), ids.end() );
        auto last = std::unique( ids.begin(), ids.end() );
        ids.erase( last, ids.end() );
        */
        
        std::vector< T > result;
        std::set< T > uniq;
        for( const T& value : ids )
        {
            if( uniq.count( value ) == 0 )
            {
                result.push_back( value );
                uniq.insert( value );
            }
        }
        return result;
    }

    class LinkGroup;
    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////
    class DerivationAnalysis : public IndexedObject
    {
        friend class ObjectFactoryImpl;
    public:
        static const ObjectType Type = eDerivationAnalysis;
    protected:
        DerivationAnalysis( const IndexedObject& object )
            :   IndexedObject( object )
        {
    
        }
        
    public:
        struct Compatibility
        {
            using StaticCompatibilitySet = std::set< const interface::Context*, CompareIndexedObjects >;
            using DynamicCompatibilitySet = std::set< const concrete::Action*, CompareIndexedObjects >;
            StaticCompatibilitySet staticCompatibleTypes;
            StaticCompatibilitySet staticLinkCompatibleTypes;
            DynamicCompatibilitySet dynamicCompatibleTypes;
            DynamicCompatibilitySet dynamicCompatibleFromLinkTypes;
            DynamicCompatibilitySet dynamicCompatibleToLinkTypes;
        };
        
        using CompatibilityMap = std::map< const interface::Context*, Compatibility, CompareIndexedObjects >;
        CompatibilityMap m_compatibility;
        
        void analyseCompatibility( 
            const std::vector< const interface::Context* >& interfaceActions,
            const std::vector< const concrete::Inheritance_Node* >& iNodes );
            
        void analyseLinkCompatibility( 
            const std::vector< const interface::Context* >& interfaceActions,
            const std::vector< LinkGroup* >& links );
            
        const Compatibility& getCompatibility( const interface::Context* pAction ) const;
    
        using InstanceMap = std::multimap< const interface::Element*, const concrete::Element*, CompareIndexedObjects >;
        InstanceMap m_instanceMap;
        
        using InheritanceNodeMap = std::multimap< const interface::Context*, const concrete::Inheritance_Node*, CompareIndexedObjects >;
        InheritanceNodeMap m_inheritanceMap;
        
        void getInstances( const interface::Element* pElement, std::vector< const concrete::Element* >& instances, bool bDeriving ) const;
                
    public:
        virtual void load( Loader& loader );
        virtual void store( Storer& storer ) const;
    };
    
}

#endif //DERIVATION_18_04_2019