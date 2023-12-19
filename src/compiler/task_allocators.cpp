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

#include "base_task.hpp"

#include "database/MemoryStage.hxx"
#include "database/sources.hpp"

#include "mega/values/compilation/operation_id.hpp"
#include "mega/mangle/traits.hpp"
#include "mega/memory.hpp"

#include "common/file.hpp"
#include <common/stash.hpp>

#include <optional>
#include <vector>
#include <string>

namespace MemoryStage
{
#include "compiler/interface.hpp"
}

namespace
{

// pad the offset UP to the nearest multiple of alignment
static U64 padToAlignment( U64 alignment, U64 offset )
{
    const U64 diff = offset % alignment;
    if( diff )
    {
        return offset + ( alignment - diff );
    }
    else
    {
        return offset;
    }
}
} // namespace

namespace mega::compiler
{

using namespace MemoryStage;

class Task_Allocators : public BaseTask
{
    const mega::io::megaFilePath& m_sourceFilePath;

public:
    Task_Allocators( const TaskArguments& taskArguments, const mega::io::megaFilePath& sourceFilePath )
        : BaseTask( taskArguments )
        , m_sourceFilePath( sourceFilePath )
    {
    }

    struct Parts
    {
        using PartVector = std::vector< MemoryLayout::Part* >;
        PartVector parts;
    };

    using Element = std::variant<        //
        Concrete::Dimensions::User*,     //
        Concrete::Dimensions::Link*,     //
        Concrete::Dimensions::LinkType*, //
        Concrete::Dimensions::Bitset* >;

    using DomainMap = std::multimap< U64, Element >;

    void collateElementsAndSetDomainSizes( Database& database, Concrete::Context* pContext, DomainMap& elements,
                                          U64 localDomain, U64 totalDomain )
    {
        using namespace MemoryStage;

        Concrete::Context* pReconstruct = database.construct< Concrete::Context >( Concrete::Context::Args{
            pContext, static_cast< Instance >( localDomain ), static_cast< Instance >( totalDomain ) } );

        if( auto pDimensionContext = db_cast< Concrete::UserDimensionContext >( pReconstruct ) )
        {
            for( auto pDim : pDimensionContext->get_dimensions() )
            {
                elements.insert( { totalDomain, pDim } );
            }
            for( auto pLink : pDimensionContext->get_links() )
            {
                elements.insert( { totalDomain, pLink } );
                elements.insert( { totalDomain, pLink->get_link_type() } );
            }
            for( auto pBitset : pDimensionContext->get_bitsets() )
            {
                elements.insert( { totalDomain, pBitset } );
            }
        }

        for( Concrete::ContextGroup* pContextGroup : pContext->get_children() )
        {
            if( auto pChildContext = db_cast< Concrete::Context >( pContextGroup ) )
            {
                const U64 localDomainSize = getLocalDomainSize( pChildContext );

                collateElementsAndSetDomainSizes(
                    database, pChildContext, elements, localDomainSize, totalDomain * localDomainSize );
            }
        }
    }

    void calculateMemoryLayout( Database& database, Concrete::Object* pObject )
    {
        DomainMap elements;
        collateElementsAndSetDomainSizes( database, pObject, elements, 1, 1 );

        std::vector< MemoryLayout::Part* > parts;
        for( auto i = elements.begin(), iEnd = elements.end(); i != iEnd; )
        {
            MemoryLayout::Part* pContextPart
                = database.construct< MemoryLayout::Part >( MemoryLayout::Part::Args{ i->first } );

            struct Visitor
            {
                Database&           database;
                MemoryLayout::Part* pContextPart;

                mega::SizeAlignment                            sizeAlignment;
                std::vector< Concrete::Dimensions::User* >     userDimensions;
                std::vector< Concrete::Dimensions::Link* >     links;
                std::vector< Concrete::Dimensions::LinkType* > linkTypes;
                std::vector< Concrete::Dimensions::Bitset* >   bitsets;

                void operator()( Concrete::Dimensions::User* pUserDimension )
                {
                    const U64 szAlign       = pUserDimension->get_interface_dimension()->get_alignment();
                    const U64 szSize        = pUserDimension->get_interface_dimension()->get_size();
                    sizeAlignment.alignment = std::max( sizeAlignment.alignment, szAlign );
                    sizeAlignment.size      = padToAlignment( szAlign, sizeAlignment.size );
                    auto pDim               = database.construct< Concrete::Dimensions::User >(
                        Concrete::Dimensions::User::Args{ pUserDimension, pContextPart, sizeAlignment.size } );
                    sizeAlignment.size += szSize;
                    userDimensions.push_back( pDim );
                }

                void operator()( Concrete::Dimensions::Link* pLink )
                {
                    if( pLink->get_singular() )
                    {
                        const U64 szAlign       = alignof( mega::Pointer );
                        const U64 szSize        = sizeof( mega::Pointer );
                        sizeAlignment.alignment = std::max( sizeAlignment.alignment, szAlign );
                        sizeAlignment.size      = padToAlignment( szAlign, sizeAlignment.size );
                        auto pLink2             = database.construct< Concrete::Dimensions::Link >(
                            Concrete::Dimensions::Link::Args{ pLink, pContextPart, sizeAlignment.size } );
                        sizeAlignment.size += szSize;
                        links.push_back( pLink2 );
                    }
                    else
                    {
                        const U64 szAlign       = mega::DimensionTraits< mega::ReferenceVector >::Alignment;
                        const U64 szSize        = mega::DimensionTraits< mega::ReferenceVector >::Size;
                        sizeAlignment.alignment = std::max( sizeAlignment.alignment, szAlign );
                        sizeAlignment.size      = padToAlignment( szAlign, sizeAlignment.size );
                        auto pLink2             = database.construct< Concrete::Dimensions::Link >(
                            Concrete::Dimensions::Link::Args{ pLink, pContextPart, sizeAlignment.size } );
                        sizeAlignment.size += szSize;
                        links.push_back( pLink2 );
                    }
                }

                void operator()( Concrete::Dimensions::LinkType* pLinkType )
                {
                    if( pLinkType->get_link()->get_singular() )
                    {
                        const U64 szAlign       = alignof( mega::TypeID );
                        const U64 szSize        = sizeof( mega::TypeID );
                        sizeAlignment.alignment = std::max( sizeAlignment.alignment, szAlign );
                        sizeAlignment.size      = padToAlignment( szAlign, sizeAlignment.size );
                        auto pLinkType2         = database.construct< Concrete::Dimensions::LinkType >(
                            Concrete::Dimensions::LinkType::Args{ pLinkType, pContextPart, sizeAlignment.size } );
                        sizeAlignment.size += szSize;
                        linkTypes.push_back( pLinkType2 );
                    }
                    else
                    {
                        const U64 szAlign       = alignof( mega::LinkTypeVector );
                        const U64 szSize        = sizeof( mega::LinkTypeVector );
                        sizeAlignment.alignment = std::max( sizeAlignment.alignment, szAlign );
                        sizeAlignment.size      = padToAlignment( szAlign, sizeAlignment.size );
                        auto pLinkType2         = database.construct< Concrete::Dimensions::LinkType >(
                            Concrete::Dimensions::LinkType::Args{ pLinkType, pContextPart, sizeAlignment.size } );
                        sizeAlignment.size += szSize;
                        linkTypes.push_back( pLinkType2 );
                    }
                }

                void operator()( Concrete::Dimensions::Bitset* pBitset )
                {
                    // determine size of bitset
                    const U64 szAlign = alignof( BitSet );
                    const U64 szSize  = sizeof( BitSet );

                    sizeAlignment.alignment = std::max( sizeAlignment.alignment, szAlign );
                    sizeAlignment.size      = padToAlignment( szAlign, sizeAlignment.size );
                    auto pBitset2           = database.construct< Concrete::Dimensions::Bitset >(
                        Concrete::Dimensions::Bitset::Args{ pBitset, pContextPart, sizeAlignment.size } );
                    sizeAlignment.size += szSize;
                    bitsets.push_back( pBitset2 );
                }

            } visitor( database, pContextPart );

            auto iUpper = elements.upper_bound( i->first );
            for( ; i != iUpper; ++i )
            {
                std::visit( visitor, i->second );
            }

            pContextPart->set_size( visitor.sizeAlignment.size );
            pContextPart->set_alignment( visitor.sizeAlignment.alignment );
            pContextPart->set_user_dimensions( visitor.userDimensions );
            pContextPart->set_link_dimensions( visitor.links );
            pContextPart->set_bitset_dimensions( visitor.bitsets );

            parts.push_back( pContextPart );
        }

        std::vector< MemoryLayout::Buffer* > buffers;
        if( !parts.empty() )
        {
            // time for some genius - cannot include mega::runtime::ObjectHeader due to dependencies
            // so redefine
            struct FakeObjectHeader : public mega::ObjectHeaderBase
            {
                std::shared_ptr< void > m_pAllocator;
            };

            U64 szOffset    = sizeof( FakeObjectHeader );
            U64 szAlignment = alignof( FakeObjectHeader );
            for( auto p : parts )
            {
                szOffset = padToAlignment( p->get_alignment(), szOffset );
                p->set_offset( szOffset );
                szOffset += ( p->get_size() * p->get_total_domain_size() );
                szAlignment = std::max( szAlignment, p->get_alignment() );
            }

            MemoryLayout::SimpleBuffer* pSimpleBuffer = database.construct< MemoryLayout::SimpleBuffer >(
                MemoryLayout::SimpleBuffer::Args{ MemoryLayout::Buffer::Args{ parts, szOffset, szAlignment } } );
            buffers.push_back( pSimpleBuffer );

            for( auto p : parts )
            {
                p->set_buffer( pSimpleBuffer );
            }
        }

        pObject = database.construct< Concrete::Object >( Concrete::Object::Args{ pObject, buffers } );
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        const mega::io::CompilationFilePath concrete = m_environment.ConcreteStage_Concrete( m_sourceFilePath );
        const mega::io::CompilationFilePath compilationFile
            = m_environment.MemoryStage_MemoryLayout( m_sourceFilePath );

        start( taskProgress, "Task_Allocators", m_sourceFilePath.path(), compilationFile.path() );

        const task::DeterminantHash determinant(
            { m_toolChain.toolChainHash,
              m_environment.getBuildHashCode( m_environment.ParserStage_AST( m_sourceFilePath ) ),
              m_environment.getBuildHashCode( m_environment.InterfaceStage_Tree( m_sourceFilePath ) ),
              m_environment.getBuildHashCode( concrete ) } );

        if( m_environment.restore( compilationFile, determinant ) )
        {
            m_environment.setBuildHashCode( compilationFile );
            cached( taskProgress );
            return;
        }

        using namespace MemoryStage;

        Database database( m_environment, m_sourceFilePath );

        for( auto pObject : database.many< Concrete::Object >( m_sourceFilePath ) )
        {
            calculateMemoryLayout( database, pObject );
        }

        const task::FileHash fileHashCode = database.save_MemoryLayout_to_temp();
        m_environment.setBuildHashCode( compilationFile, fileHashCode );
        m_environment.temp_to_real( compilationFile );
        m_environment.stash( compilationFile, determinant );

        succeeded( taskProgress );
    }
};

BaseTask::Ptr create_Task_Allocators( const TaskArguments& taskArguments, const mega::io::megaFilePath& sourceFilePath )
{
    return std::make_unique< Task_Allocators >( taskArguments, sourceFilePath );
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
// createParts
/*

using namespace MemoryStage;
using namespace MemoryStage::Concrete;

struct PartDimensions
{
    std::vector< Concrete::Dimensions::User* >   user;
    std::vector< Concrete::Dimensions::Link* >   link;
    std::vector< Concrete::Dimensions::Bitset* > bitset;

    mega::SizeAlignment calculatePartLayout( MemoryStage::Database& database, MemoryLayout::Part* pPart )
    {
        mega::SizeAlignment result;
        {
            for( auto p : user )
            {
                const U64 szAlign = p->get_interface_dimension()->get_alignment();
                const U64 szSize  = p->get_interface_dimension()->get_size();
                result.alignment  = std::max( result.alignment, szAlign );
                result.size       = padToAlignment( szAlign, result.size );
                database.construct< Concrete::Dimensions::User >(
                    Concrete::Dimensions::User::Args{ p, result.size, pPart } );
                result.size += szSize;
            }
        }
        {
            for( auto pLink : link )
            {
                Concrete::Dimensions::LinkType* pLinkType = nullptr;
                {
                    if( pLink->get_singular() )
                    {
                        const U64 szAlign = alignof( mega::TypeID );
                        const U64 szSize  = sizeof( mega::TypeID );
                        result.alignment  = std::max( result.alignment, szAlign );
                        result.size       = padToAlignment( szAlign, result.size );

                        pLinkType = database.construct< Concrete::Dimensions::LinkType >(
                            Concrete::Dimensions::LinkType::Args{ pLink->get_parent_context(), result.size, pPart } );
                        result.size += szSize;
                    }
                    else
                    {
                        const U64 szAlign = alignof( mega::LinkTypeVector );
                        const U64 szSize  = sizeof( mega::LinkTypeVector );
                        result.alignment  = std::max( result.alignment, szAlign );
                        result.size       = padToAlignment( szAlign, result.size );

                        pLinkType = database.construct< Concrete::Dimensions::LinkType >(
                            Concrete::Dimensions::LinkType::Args{ pLink->get_parent_context(), result.size, pPart } );
                        result.size += szSize;
                    }
                }

                if( pLink->get_singular() )
                {
                    const U64 szAlign = alignof( mega::Pointer );
                    const U64 szSize  = sizeof( mega::Pointer );
                    result.alignment  = std::max( result.alignment, szAlign );
                    result.size       = padToAlignment( szAlign, result.size );
                    auto pLink2       = database.construct< Concrete::Dimensions::Link >(
                        Concrete::Dimensions::Link::Args{ pLink, result.size, pPart, pLinkType } );
                    pLinkType->set_link( pLink2 );
                    result.size += szSize;
                }
                else
                {
                    const U64 szAlign = mega::DimensionTraits< mega::ReferenceVector >::Alignment;
                    const U64 szSize  = mega::DimensionTraits< mega::ReferenceVector >::Size;
                    result.alignment  = std::max( result.alignment, szAlign );
                    result.size       = padToAlignment( szAlign, result.size );
                    auto pLink2       = database.construct< Concrete::Dimensions::Link >(
                        Concrete::Dimensions::Link::Args{ pLink, result.size, pPart, pLinkType } );
                    pLinkType->set_link( pLink2 );
                    result.size += szSize;
                }
            }
        }
        // ensure the entire part is padded to its own alignment to allow array of the part
        result.size = padToAlignment( result.alignment, result.size );
        return result;
    }

    MemoryLayout::Part* createPart( MemoryStage::Database& database, MemoryStage::Concrete::Context* pContext,
                                    U64 szTotalDomainSize )
    {
        MemoryLayout::Part* pContextPart = database.construct< MemoryLayout::Part >(
            MemoryLayout::Part::Args{ szTotalDomainSize, pContext, user, link } );
        const mega::SizeAlignment sizeAlignment = calculatePartLayout( database, pContextPart );
        pContextPart->set_size( sizeAlignment.size );
        pContextPart->set_alignment( sizeAlignment.alignment );
        return pContextPart;
    }
};

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
// createBuffers
void Task_Allocators::createBuffers( MemoryStage::Database& database, MemoryStage::Concrete::Context* pParentContext,
                                     MemoryStage::Concrete::Context* pContext, Parts* pParts )
{
    using namespace MemoryStage;
    using namespace MemoryStage::Concrete;

    if( Object* pObject = db_cast< Object >( pContext ) )
    {
        Parts parts;
        {
            createParts( database, pObject, &parts );
            for( Concrete::ContextGroup* pContextGroup : pContext->get_children() )
            {
                if( Concrete::Context* pChildContext = db_cast< Concrete::Context >( pContextGroup ) )
                {
                    createBuffers( database, pContext, pChildContext, &parts );
                }
            }
        }

        std::vector< MemoryLayout::Buffer* > objectBuffers;
        if( !parts.parts.empty() )
        {
            // time for some genius - cannot include mega::runtime::ObjectHeader due to dependencies
            // so redefine
            struct FakeObjectHeader : public mega::ObjectHeaderBase
            {
                std::shared_ptr< void > m_pAllocator;
            };

            U64 szOffset    = sizeof( FakeObjectHeader );
            U64 szAlignment = alignof( FakeObjectHeader );
            for( auto p : parts.parts )
            {
                szOffset = padToAlignment( p->get_alignment(), szOffset );
                p->set_offset( szOffset );
                szOffset += ( p->get_size() * p->get_total_domain_size() );
                szAlignment = std::max( szAlignment, p->get_alignment() );
            }

            MemoryLayout::SimpleBuffer* pSimpleBuffer = database.construct< MemoryLayout::SimpleBuffer >(
                MemoryLayout::SimpleBuffer::Args{ MemoryLayout::Buffer::Args{ parts.parts, szOffset, szAlignment } } );
            objectBuffers.push_back( pSimpleBuffer );

            for( auto p : parts.parts )
            {
                p->set_buffer( pSimpleBuffer );
            }
        }

        pObject = database.construct< Object >( Object::Args{ pObject, objectBuffers } );
    }
    else
    {
        if( pParts )
        {
            if( auto pNamespace = db_cast< Namespace >( pContext ) )
            {
            }
            else if( auto pState = db_cast< State >( pContext ) )
            {
                createParts( database, pState, pParts );
            }
            else if( auto pEvent = db_cast< Event >( pContext ) )
            {
                createParts( database, pEvent, pParts );
            }
            else if( auto pFunction = db_cast< Function >( pContext ) )
            {
            }
            else if( auto pInterupt = db_cast< Interupt >( pContext ) )
            {
            }
            else
            {
                THROW_RTE( "Unknown context type" );
            }
        }

        for( Concrete::ContextGroup* pContextGroup : pContext->get_children() )
        {
            if( auto pChildContext = db_cast< Concrete::Context >( pContextGroup ) )
            {
                createBuffers( database, pContext, pChildContext, pParts );
            }
        }
    }
}
*/
} // namespace mega::compiler
