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

#ifndef IDENTIFIERS_19_04_2019
#define IDENTIFIERS_19_04_2019

#include "interface.hpp"

#include "database/io/object.hpp"

#include <map>

namespace mega
{

class Identifiers : public io::Object
{
    friend class ObjectFactoryImpl;

public:
    static const ObjectType Type = eIdentifiers;

protected:
    Identifiers( const io::Object& object );

    virtual void load( io::Loader& loader );
    virtual void store( io::Storer& storer ) const;

public:
    void populate( const io::Object::Array& objects );

    using IdentifierMap = std::map< std::string, const interface::Element* >;
    const IdentifierMap& getIdentifiersMap() const { return m_identifierMap; }

    const interface::Element*                isElement( const std::string& strIdentifier ) const;
    std::vector< const interface::Element* > getGroup( const interface::Element* pElement ) const;
    std::vector< const interface::Element* > getGroupBack( const interface::Element* pElement ) const;

private:
    struct CompareIdentifiers
    {
        bool operator()( const interface::Element* pLeft, const interface::Element* pRight ) const
        {
            return pLeft->getIdentifier() < pRight->getIdentifier();
        }
    };
    std::map< std::string, const interface::Element* > m_identifierMap;
    using GroupMap = std::multimap< const interface::Element*, const interface::Element*, CompareIdentifiers >;
    GroupMap m_identifierGroups;

    using GroupBackMap = std::map< const interface::Element*, const interface::Element*, CompareIdentifiers >;
    GroupBackMap m_identifierGroupsBack;
};

} // namespace mega

#endif // IDENTIFIERS_19_04_2019