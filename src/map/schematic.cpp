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

#include "map/schematic.hpp"
#include "map/file.hpp"
#include "map/space.hpp"
#include "map/wall.hpp"
#include "map/object.hpp"
#include "map/connection.hpp"
#include "map/factory.hpp"

namespace map
{

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
Schematic::Schematic( const std::string& strName )
    : BaseType( strName )
{
}

Schematic::Schematic( Schematic::PtrCst pOriginal, Node::Ptr pNewParent, const std::string& strName )
    : BaseType( pOriginal, pNewParent, strName )
{
}

void Schematic::init()
{
    File::init();

    if( !m_pCompilationMarkup.get() )
    {
        m_pCompilationMarkup.reset( new MultiPathMarkup( *this, nullptr, Schematic::eStage_Compilation ) );
    }
}

void Schematic::load( const format::File::Schematic& schematic )
{
    {
        // ensure the sites are restored to their original order using
        // the map index
        using NewSite = std::pair< Site::Ptr, const format::Site* >;
        std::vector< NewSite > newSites( schematic.children.size() );

        for( const auto& child : schematic.children )
        {
            int                 szIndex   = child.first;
            const format::Site& childSite = child.second;
            newSites[ szIndex ]           = std::make_pair( map::construct( getPtr(), childSite ), &childSite );
        }

        for( const NewSite& newSite : newSites )
        {
            add( newSite.first );
            newSite.first->init();
            newSite.first->load( *newSite.second );
        }
    }
}

void Schematic::save( format::File::Schematic& schematic ) const
{
    int szIndex = 0U;
    for( Site::Ptr pSite : getSites() )
    {
        format::Site childSite;
        pSite->save( childSite );
        schematic.children.insert( { szIndex, childSite } );
        ++szIndex;
    }
}

void Schematic::task_contours()
{
    for( Site::Ptr pSite : getSites() )
    {
        pSite->task_contour();
    }
}

void Schematic::task_extrusions()
{
    for( Site::Ptr pSite : getSites() )
    {
        if( Space::Ptr pSpace = boost::dynamic_pointer_cast< Space >( pSite ) )
        {
            pSpace->task_extrusions();
        }
    }
}
void Schematic::task_compilation()
{
    std::vector< Segment > edges;
    try
    {
        Schematic::Ptr pThis = boost::dynamic_pointer_cast< Schematic >( getPtr() );

        m_pCompilation.reset();
        m_pCompilation.reset( new Compilation( pThis ) );

        m_pCompilation->getEdges( edges );
    }
    catch( std::exception& )
    {
        m_pCompilation.reset();
        edges.clear();
    }

    m_pCompilationMarkup->set( edges );
}

} // namespace map