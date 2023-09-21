
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

#ifndef GUARD_2023_September_21_derivation
#define GUARD_2023_September_21_derivation

namespace Derivation
{
using Vertex              = Concrete::Graph::Vertex;
using VertexVariant       = std::vector< Vertex* >;
using VertexVariantVector = std::vector< VertexVariant >;

struct Spec
{
    VertexVariant       context;
    VertexVariantVector path;
};

struct Step
{
    Concrete::Graph::Vertex* pVertex = nullptr;
    Concrete::Graph::Edge*   pEdge   = nullptr;
    std::vector< Step >      steps;
};

struct Solution
{
    Spec                spec;
    std::vector< Step > steps;
};

Solution solve( const Spec& derivation )
{
    Solution result{ derivation };

    return result;
}
} // namespace Derivation

#endif // GUARD_2023_September_21_derivation
