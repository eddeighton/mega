
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
#include "reports/url.hpp"
#include "reports/colours.hxx"

#include "utilities/serialization_helpers.hpp"

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/variant.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>

#include <vector>
#include <ostream>
#include <optional>

namespace mega::reports
{

using Text       = std::variant< std::string, Value >;
using TextVector = std::vector< Text >;

class Line;
class Multiline;
class Branch;
class Table;
class Graph;
using Container       = std::variant< Line, Multiline, Branch, Table, Graph >;
using ContainerVector = std::vector< Container >;

class Line
{
    friend class boost::serialization::access;
    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& m_element;
    }

public:
    Text m_element;
};

class Multiline
{
    friend class boost::serialization::access;
    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& m_elements;
    }

public:
    TextVector m_elements;
};

class Branch
{
    friend class boost::serialization::access;
    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& m_label;
        archive& m_elements;
    }

public:
    TextVector      m_label;
    ContainerVector m_elements;
};

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
    std::vector< std::string >     m_headings;
    std::vector< ContainerVector > m_rows;
};

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

        std::vector< TextVector > m_rows;
        Colour                    m_colour = Colour::lightblue;
        std::optional< URL >      m_url;
        std::optional< Value >    m_bookmark;
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
