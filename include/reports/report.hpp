
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
#include "reports/colours.hxx"

#include "mega/values/service/url.hpp"

#include "common/serialisation.hpp"

#include <vector>
#include <ostream>
#include <optional>

namespace mega::reports
{

using ValueVector = std::vector< Value >;

class Link;
class Line;
class Multiline;
class Branch;
class Table;
class Graph;
using Container       = boost::variant< Line, Multiline, Branch, Table, Graph >;
using ContainerVector = std::vector< Container >;

/***
    Line{ Value, opt< URL >, opt< Bookmark > }
*/
class Line
{
    friend class boost::serialization::access;
    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& m_element;
        archive& m_url;
        archive& m_bookmark;
    }

public:
    Value                  m_element;
    std::optional< URL>    m_url;
    std::optional< Value > m_bookmark;
};

/***
    Multiline{ vec< Value >, opt< URL >, opt< Bookmark > }
*/
class Multiline
{
    friend class boost::serialization::access;
    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& m_elements;
        archive& m_url;
        archive& m_bookmark;
    }

public:
    ValueVector            m_elements;
    std::optional< URL>    m_url;
    std::optional< Value > m_bookmark;
};

/***
    Branch{ vec< Value >, vec< Container >, opt< Bookmark > }
*/
class Branch
{
    friend class boost::serialization::access;
    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& m_label;
        archive& m_elements;
        archive& m_bookmark;
    }

public:
    ValueVector            m_label;
    ContainerVector        m_elements;
    std::optional< Value > m_bookmark;
};

/***
    Table{ vec< Value >, vec< vec< Container > > }

    NOTE: If headings are empty then only containers row columns displayed
*/
class Table
{
    friend class boost::serialization::access;
    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& m_headings;
        archive& m_rows;
    }

public:
    std::vector< Value >           m_headings;
    std::vector< ContainerVector > m_rows;
};

/***
    Graph{ vec< Node >, vec< Edge > }

    Node{ vec< vec< Value > >, colour( lightblue ), opt< URL >, opt< Bookmark > }
    Edge{ sourceIndex, targetIndex, style }

    Edge Styles: dashed, dotted, solid, invis, bold
*/
class Graph
{
    friend class boost::serialization::access;
    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& m_nodes;
        archive& m_edges;
    }

public:
    class Node
    {
        friend class boost::serialization::access;
        template < class Archive >
        inline void serialize( Archive& archive, const unsigned int version )
        {
            archive& m_rows;
            archive& m_colour;
            archive& m_url;
            archive& m_bookmark;
        }

    public:
        using Vector = std::vector< Node >;

        std::vector< ValueVector > m_rows;
        Colour                     m_colour = Colour::lightblue;
        std::optional< URL >       m_url;
        std::optional< Value >     m_bookmark;
    };

    class Edge
    {
        friend class boost::serialization::access;
        template < class Archive >
        inline void serialize( Archive& archive, const unsigned int version )
        {
            archive& m_source;
            archive& m_target;
            archive& m_colour;
            archive& m_style;
        }

    public:
        using Vector = std::vector< Edge >;

        class Style
        {
        public:
            enum Type
            {
                dashed,
                dotted,
                solid,
                invis,
                bold,
                TOTAL_STYLES
            };

            Style() = default;

            inline constexpr Style( Type type )
                : m_style( type )
            {
            }

            inline const char* str() const
            {
                static const std::array< const char*, TOTAL_STYLES > g_styles
                    = { "dashed", "dotted", "solid", "invis", "bold" };
                return g_styles[ m_style ];
            }

            template < class Archive >
            inline void serialize( Archive& archive, const unsigned int version )
            {
                archive& m_style;
            }

        private:
            Type m_style = solid;
        };

        int    m_source, m_target;
        Colour m_colour = Colour::black;
        Style  m_style  = Style::solid;
    };

    Node::Vector m_nodes;
    Edge::Vector m_edges;
};

} // namespace mega::reports

#endif // GUARD_2023_October_17_reports
