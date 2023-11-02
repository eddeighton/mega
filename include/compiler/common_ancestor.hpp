
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

#ifndef GUARD_2023_November_01_common_ancestor
#define GUARD_2023_November_01_common_ancestor

struct CommonAncestor
{
    using GraphVertex             = Concrete::Graph::Vertex;
    using GraphEdge               = Concrete::Graph::Edge;
    using GraphEdgeVector         = std::vector< GraphEdge* >;
private:
    static inline GraphVertex* pathToObjectRoot( GraphVertex* pVertex, GraphEdgeVector& path )
    {
        while( !db_cast< Concrete::Object >( pVertex ) )
        {
            bool bFound = false;
            for( auto pEdge : pVertex->get_out_edges() )
            {
                if( pEdge->get_type().get() == mega::EdgeType::eParent )
                {
                    path.push_back( pEdge );
                    pVertex = pEdge->get_target();
                    VERIFY_RTE( !bFound );
                    bFound = true;
                }
            }
            if( !bFound )
            {
                THROW_RTE( "Failed to find path to object root from vertex" );
                return nullptr;
            }
        }
        return pVertex;
    }

    static inline bool invertObjectRootPath( const GraphEdgeVector& path, GraphEdgeVector& descendingPath )
    {
        for( auto pEdge : path )
        {
            bool bFound = false;
            for( auto pInverseEdge : pEdge->get_target()->get_out_edges() )
            {
                VERIFY_RTE( pEdge->get_target() == pInverseEdge->get_source() );
                if( pEdge->get_source() == pInverseEdge->get_target() )
                {
                    switch( pInverseEdge->get_type().get() )
                    {
                        case mega::EdgeType::eChildSingular:
                        case mega::EdgeType::eLink:
                        case mega::EdgeType::eDim:
                        {
                            descendingPath.push_back( pInverseEdge );
                            VERIFY_RTE( !bFound );
                            bFound = true;
                        }
                        break;
                        case mega::EdgeType::eChildNonSingular:
                            // do no allow non-singular
                            break;
                        default:
                            break;
                    }
                }
            }
            if( !bFound )
            {
                return false;
            }
        }
        std::reverse( descendingPath.begin(), descendingPath.end() );
        return true;
    }
public:
    static inline GraphVertex* commonRootDerivation( GraphVertex* pSource, GraphVertex* pTarget, GraphEdgeVector& edges )
    {
        if( pSource == pTarget )
        {
            return pSource;
        }

        GraphEdgeVector sourcePath, targetPath;
        auto            pSourceObject = pathToObjectRoot( pSource, sourcePath );
        auto            pTargetObject = pathToObjectRoot( pTarget, targetPath );

        // if not the same object then fail
        if( pSourceObject != pTargetObject )
        {
            return nullptr;
        }

        // while both paths contain edges then if the edge is the same there is a lower common root
        GraphVertex* pCommonAncestor = pSourceObject;

        while( !sourcePath.empty() && !targetPath.empty() && ( sourcePath.back() == targetPath.back() ) )
        {
            pCommonAncestor = sourcePath.back()->get_source();
            sourcePath.pop_back();
            targetPath.pop_back();
        }

        GraphEdgeVector descendingPath;
        if( invertObjectRootPath( targetPath, descendingPath ) )
        {
            if( !sourcePath.empty() && !descendingPath.empty() )
            {
                VERIFY_RTE( descendingPath.front()->get_source() == sourcePath.back()->get_target() );
            }

            std::copy( sourcePath.begin(), sourcePath.end(), std::back_inserter( edges ) );
            std::copy( descendingPath.begin(), descendingPath.end(), std::back_inserter( edges ) );

            return pCommonAncestor;
        }
        else
        {
            return nullptr;
        }
    }

};

#endif //GUARD_2023_November_01_common_ancestor
