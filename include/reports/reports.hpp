
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

#ifndef GUARD_2023_October_17_reports
#define GUARD_2023_October_17_reports

#include "reports/value.hpp"

#include <memory>
#include <vector>
#include <ostream>

namespace mega::reports
{
class Property
{
public:
    using Ptr       = std::shared_ptr< Property >;
    using PtrVector = std::vector< Ptr >;

    std::string m_identifier;
    Value       m_value;
};

class Element;

class Edge
{
public:
    using Ptr       = std::shared_ptr< Edge >;
    using PtrVector = std::vector< Ptr >;

    Property::PtrVector        m_properties;
    std::shared_ptr< Element > m_source;
    std::shared_ptr< Element > m_target;
};

class Element
{
public:
    virtual ~Element() = default;
    using Ptr          = std::shared_ptr< Element >;
    using PtrVector    = std::vector< Ptr >;

    Property::PtrVector m_properties;
};

class TreeNode : public Element
{
public:
    using Ptr       = std::shared_ptr< TreeNode >;
    using PtrVector = std::vector< Ptr >;

    Element::PtrVector m_children;
};

class Table : public Element
{
public:
    using Ptr       = std::shared_ptr< Table >;
    using PtrVector = std::vector< Ptr >;
    
    std::vector< Element::PtrVector > m_rows;
};

class SVG : public Element
{
public:
    using Ptr       = std::shared_ptr< SVG >;
    using PtrVector = std::vector< Ptr >;

    Element::PtrVector m_children;
    Edge::PtrVector    m_edges;
};

void renderHTML( Element::Ptr pElement, std::ostream& os );

} // namespace mega::reports

#endif // GUARD_2023_October_17_reports
