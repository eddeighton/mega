
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

#ifndef GUARD_2023_September_21_hyper_graph
#define GUARD_2023_September_21_hyper_graph

#include "mega/values/compilation/cardinality.hpp"

#include <ostream>

namespace mega
{

class EdgeType
{
public:
    enum Value
    {
        eParent,
        eChildSingular,
        eChildNonSingular,
        eDim,
        eLink,

        eMonoSingularMandatory,
        ePolySingularMandatory,
        eMonoNonSingularMandatory,
        ePolyNonSingularMandatory,

        eMonoSingularOptional,
        ePolySingularOptional,
        eMonoNonSingularOptional,
        ePolyNonSingularOptional,

        ePolyParent,
        TOTAL_EDGE_TYPES
    };

    EdgeType()
        : m_value( TOTAL_EDGE_TYPES )
    {
    }
    EdgeType( Value value )
        : m_value( value )
    {
    }

    const char*     str() const;
    static EdgeType fromStr( const char* psz );

    Value get() const { return m_value; }
    void  set( Value value ) { m_value = value; }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& m_value;
    }

private:
    Value m_value;
};

} // namespace mega

std::ostream& operator<<( std::ostream& os, mega::EdgeType edgeType );

namespace mega
{
inline EdgeType fromCardinality( bool bMonoMorphic, const CardinalityRange& cardinality )
{
    if( cardinality.isOptional() )
    {
        if( cardinality.isNonSingular() )
        {
            if( bMonoMorphic )
            {
                return EdgeType::eMonoNonSingularOptional;
            }
            else
            {
                return EdgeType::ePolyNonSingularOptional;
            }
        }
        else
        {
            if( bMonoMorphic )
            {
                return EdgeType::eMonoSingularOptional;
            }
            else
            {
                return EdgeType::ePolySingularOptional;
            }
        }
    }
    else
    {
        if( cardinality.isNonSingular() )
        {
            if( bMonoMorphic )
            {
                return EdgeType::eMonoNonSingularMandatory;
            }
            else
            {
                return EdgeType::ePolyNonSingularMandatory;
            }
        }
        else
        {
            if( bMonoMorphic )
            {
                return EdgeType::eMonoSingularMandatory;
            }
            else
            {
                return EdgeType::ePolySingularMandatory;
            }
        }
    }
}

} // namespace mega

#endif // GUARD_2023_September_21_hyper_graph
