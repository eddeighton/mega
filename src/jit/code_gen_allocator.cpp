
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

#include "code_generator.hpp"
#include "symbol_utils.hpp"

#include "mega/types/traits.hpp"

namespace mega::runtime
{

std::string fullInterfaceTypeName( FinalStage::Interface::IContext* pIContext )
{
    std::ostringstream osFullTypeName;
    {
        bool bFirst = true;
        for( ; pIContext; pIContext = db_cast< FinalStage::Interface::IContext >( pIContext->get_parent() ) )
        {
            if( bFirst )
                bFirst = false;
            else
                osFullTypeName << '_';
            osFullTypeName << pIContext->get_identifier();
        }
    }
    return osFullTypeName.str();
}

std::string makeIterName( FinalStage::Concrete::Context* pContext )
{
    using namespace FinalStage;
    using namespace FinalStage::Concrete;
    std::ostringstream os;
    os << "i_" << pContext->get_interface()->get_identifier() << "_" << pContext->get_concrete_id();
    return os.str();
}

template < typename TDimensionType >
std::string calculateElementOffset( const DatabaseInstance& database, TDimensionType* pUserDim,
                                    std::string& strInstance )
{
    using namespace FinalStage;
    using namespace FinalStage::Concrete;

    MemoryLayout::Part* pPart   = pUserDim->get_part();
    auto                pBuffer = pPart->get_buffer();

    std::vector< Concrete::Context* > parents;
    std::vector< U64 >                sizes;
    {
        Concrete::Context* pContext = pUserDim->get_parent();
        U64                size     = 1;
        while( pContext )
        {
            parents.push_back( pContext );
            sizes.push_back( size );
            size *= database.getLocalDomainSize( pContext->get_concrete_id() );
            if( !!db_cast< Concrete::Object >( pContext ) )
                break;
            pContext = db_cast< Concrete::Context >( pContext->get_parent() );
        }
        std::reverse( parents.begin(), parents.end() );
        std::reverse( sizes.begin(), sizes.end() );
    }

    std::ostringstream offset;
    offset << pPart->get_offset();
    offset << " + " << pPart->get_size() << " * (";
    bool bFirst = true;

    {
        std::ostringstream instance;
        for( U64 i = 0; i != parents.size(); ++i )
        {
            if( bFirst )
                bFirst = false;
            else
                instance << " + ";
            instance << sizes[ i ] << " * " << makeIterName( parents[ i ] ) << " ";
        }
        strInstance = instance.str();
    }

    offset << strInstance << ") + " << pUserDim->get_offset();

    return offset.str();
}

void generateAllocatorDimensions( const DatabaseInstance& database, FinalStage::Concrete::Dimensions::User* pUserDim,
                                  nlohmann::json& data )
{
    std::string    strInstance;
    nlohmann::json element( { { "concrete_id", printTypeID( pUserDim->get_concrete_id() ) },
                              { "is_object", false },
                              { "name", pUserDim->get_interface_dimension()->get_id()->get_str() },
                              { "type", pUserDim->get_interface_dimension()->get_canonical_type() },
                              { "mangle", megaMangle( pUserDim->get_interface_dimension()->get_erased_type() ) },
                              { "offset", calculateElementOffset< FinalStage::Concrete::Dimensions::User >(
                                              database, pUserDim, strInstance ) },
                              { "instance", strInstance },
                              { "is_part_begin", false },
                              { "is_part_end", false },
                              { "owning", false }

    } );
    data[ "elements" ].push_back( element );
}

std::optional< nlohmann::json > allocatorLink( const DatabaseInstance& database, FinalStage::Concrete::Link* pLink )
{
    using namespace FinalStage;
    using namespace FinalStage::Concrete;

    HyperGraph::Relation*      pRelation      = pLink->get_link()->get_relation();
    Interface::LinkInterface*  pLinkInterface = pLink->get_link_interface();
    Dimensions::LinkReference* pLinkRef       = pLink->get_link_reference();

    bool bSource = false;
    if( pRelation->get_source_interface() == pLinkInterface )
    {
        bSource = true;
    }
    else if( pRelation->get_target_interface() == pLinkInterface )
    {
        bSource = false;
    }
    else
    {
        THROW_RTE( "Invalid link" );
    }

    bool bOwning = false;
    {
        if( bSource )
        {
            if( pRelation->get_ownership().get() == mega::Ownership::eOwnTarget )
                bOwning = true;
        }
        else
        {
            if( pRelation->get_ownership().get() == mega::Ownership::eOwnSource )
                bOwning = true;
        }
    }

    if( bOwning )
    {
        std::string strRelationID;
        {
            std::ostringstream osRelationID;
            osRelationID << pRelation->get_id().getLower() << ',' << pRelation->get_id().getUpper();
            strRelationID = osRelationID.str();
        }

        std::string strCanonicalType;
        bool        bSingular = true;
        {
            if( auto pRange = db_cast< Dimensions::LinkMany >( pLinkRef ) )
            {
                // range
                strCanonicalType = mega::psz_mega_reference_vector;
                bSingular        = false;
            }
            else if( auto pSingle = db_cast< Dimensions::LinkSingle >( pLinkRef ) )
            {
                // singular
                strCanonicalType = mega::psz_mega_reference;
                bSingular        = true;
            }
            else
            {
                THROW_RTE( "Unknown link type" );
            }
        }

        std::string    strInstance;
        nlohmann::json element( { { "concrete_id", printTypeID( pLink->get_concrete_id() ) },
                                  { "is_object", false },
                                  { "name", pLink->get_link()->get_identifier() },
                                  { "type", strCanonicalType },
                                  { "mangle", megaMangle( strCanonicalType ) },
                                  { "offset", calculateElementOffset< FinalStage::Concrete::Dimensions::LinkReference >(
                                                  database, pLink->get_link_reference(), strInstance ) },
                                  { "instance", strInstance },
                                  { "is_part_begin", false },
                                  { "is_part_end", false },
                                  { "singular", bSingular },
                                  { "owning", bOwning },
                                  { "source", bSource },
                                  { "relationID", strRelationID },
                                  { "types", nlohmann::json::array() }

        } );

        if( bSource )
        {
            for( auto pTarget : pRelation->get_targets() )
            {
                for( auto pConcreteLink : pTarget->get_concrete() )
                {
                    auto pObjectOpt = pConcreteLink->get_concrete_object();
                    VERIFY_RTE( pObjectOpt.has_value() );
                    nlohmann::json type(
                        { { "link_type_id", printTypeID( pConcreteLink->get_concrete_id() ) },
                          { "object_type_id", printTypeID( pObjectOpt.value()->get_concrete_id() ) },
                          { "full_name", fullInterfaceTypeName( pObjectOpt.value()->get_interface_object() ) }

                        } );
                    element[ "types" ].push_back( type );
                }
            }
        }
        else
        {
            for( auto pTarget : pRelation->get_sources() )
            {
                for( auto pConcreteLink : pTarget->get_concrete() )
                {
                    auto pObjectOpt = pConcreteLink->get_concrete_object();
                    VERIFY_RTE( pObjectOpt.has_value() );
                    nlohmann::json type(
                        { { "link_type_id", printTypeID( pConcreteLink->get_concrete_id() ) },
                          { "object_type_id", printTypeID( pObjectOpt.value()->get_concrete_id() ) },
                          { "full_name", fullInterfaceTypeName( pObjectOpt.value()->get_interface_object() ) }

                        } );
                    element[ "types" ].push_back( type );
                }
            }
        }

        return element;
    }
    else
    {
        // for now do not add non-owning links to xml format.
        return {};
    }
}

void recurseAllocatorElements( const DatabaseInstance& database, FinalStage::Concrete::Context* pContext,
                               nlohmann::json& data )
{
    using namespace FinalStage;
    using namespace FinalStage::Concrete;

    std::optional< nlohmann::json > linkData;
    {
        if( auto pLink = db_cast< Link >( pContext ) )
        {
            linkData = allocatorLink( database, pLink );
            if( !linkData )
                return;
        }
    }

    if( auto pFunction = db_cast< Function >( pContext ) )
    {
        return;
    }

    {
        nlohmann::json element( { { "concrete_id", printTypeID( pContext->get_concrete_id() ) },
                                  { "is_object", !!db_cast< Concrete::Object >( pContext ) },
                                  { "name", pContext->get_interface()->get_identifier() },
                                  { "iter", makeIterName( pContext ) },
                                  { "start", 0 },
                                  { "end", database.getLocalDomainSize( pContext->get_concrete_id() ) },
                                  { "is_part_begin", true },
                                  { "is_part_end", false },
                                  { "owning", false }

        } );
        data[ "elements" ].push_back( element );
    }

    if( auto pNamespace = db_cast< Namespace >( pContext ) )
    {
        for( auto pUserDim : pNamespace->get_dimensions() )
        {
            generateAllocatorDimensions( database, pUserDim, data );
        }
    }
    else if( auto pAction = db_cast< Action >( pContext ) )
    {
        for( auto pUserDim : pAction->get_dimensions() )
        {
            generateAllocatorDimensions( database, pUserDim, data );
        }
    }
    else if( auto pEvent = db_cast< FinalStage::Concrete::Event >( pContext ) )
    {
        for( auto pUserDim : pEvent->get_dimensions() )
        {
            generateAllocatorDimensions( database, pUserDim, data );
        }
    }
    else if( auto pObject = db_cast< Object >( pContext ) )
    {
        for( auto pUserDim : pObject->get_dimensions() )
        {
            generateAllocatorDimensions( database, pUserDim, data );
        }
    }
    else if( auto pLink = db_cast< Link >( pContext ) )
    {
        data[ "elements" ].push_back( linkData.value() );
    }
    else if( auto pBuffer = db_cast< Buffer >( pContext ) )
    {
    }
    else
    {
        THROW_RTE( "Unknown context type" );
    }

    {
        for( auto pChildContext : pContext->get_children() )
        {
            recurseAllocatorElements( database, pChildContext, data );
        }
    }

    {
        nlohmann::json element( { { "concrete_id", printTypeID( pContext->get_concrete_id() ) },
                                  { "is_object", !!db_cast< Concrete::Object >( pContext ) },
                                  { "name", pContext->get_interface()->get_identifier() },
                                  { "is_part_begin", false },
                                  { "is_part_end", true },
                                  { "owning", false }

        } );
        data[ "elements" ].push_back( element );
    }
}

void CodeGenerator::generate_alllocator( const LLVMCompiler& compiler, const DatabaseInstance& database,
                                         mega::TypeID objectTypeID, std::ostream& os )
{
    SPDLOG_TRACE( "RUNTIME: generate_alllocator: {}", objectTypeID );

    FinalStage::Concrete::Object*            pObject    = database.getObject( objectTypeID );
    const FinalStage::Components::Component* pComponent = pObject->get_component();

    const std::string strFullTypeName = fullInterfaceTypeName( pObject->get_interface_object() );

    std::ostringstream osObjectTypeID;
    osObjectTypeID << printTypeID( objectTypeID );
    nlohmann::json data( { { "objectTypeID", osObjectTypeID.str() },
                           { "objectName", strFullTypeName },
                           { "parts", nlohmann::json::array() },
                           { "mangled_data_types", nlohmann::json::array() },
                           { "elements", nlohmann::json::array() } } );

    recurseAllocatorElements( database, pObject, data );

    std::set< std::string > mangledDataTypes;
    {
        using namespace FinalStage;
        for( auto pBuffer : pObject->get_buffers() )
        {
            bool bBufferIsSimple = false;
            if( db_cast< MemoryLayout::SimpleBuffer >( pBuffer ) )
                bBufferIsSimple = true;

            for( auto pPart : pBuffer->get_parts() )
            {
                std::ostringstream osPartType;
                std::ostringstream osPartName;

                osPartType << pPart->get_context()->get_interface()->get_identifier();
                osPartName << pPart->get_context()->get_interface()->get_identifier();

                nlohmann::json part( { { "type", osPartType.str() },
                                       { "name", osPartName.str() },
                                       { "size", pPart->get_size() },
                                       { "offset", pPart->get_offset() },
                                       { "total_domain", pPart->get_total_domain_size() },
                                       { "members", nlohmann::json::array() },
                                       { "links", nlohmann::json::array() }

                } );

                for( auto pUserDim : pPart->get_user_dimensions() )
                {
                    const std::string strMangle = megaMangle( pUserDim->get_interface_dimension()->get_erased_type() );
                    nlohmann::json    member( { { "type", pUserDim->get_interface_dimension()->get_canonical_type() },
                                                { "type_id", printTypeID( pUserDim->get_concrete_id() ) },
                                                { "mangle", strMangle },
                                                { "name", pUserDim->get_interface_dimension()->get_id()->get_str() },
                                                { "offset", pUserDim->get_offset() } } );
                    part[ "members" ].push_back( member );
                    mangledDataTypes.insert( strMangle );
                }

                for( auto pLinkDim : pPart->get_link_dimensions() )
                {
                    Concrete::Link*           pConcreteLink  = pLinkDim->get_link();
                    Interface::LinkInterface* pLinkInterface = pConcreteLink->get_link_interface();
                    HyperGraph::Relation*     pRelation      = pLinkInterface->get_relation();

                    bool bSource = false;
                    if( pRelation->get_source_interface() == pLinkInterface )
                    {
                        bSource = true;
                    }
                    else if( pRelation->get_target_interface() == pLinkInterface )
                    {
                        bSource = false;
                    }
                    else
                    {
                        THROW_RTE( "Invalid link" );
                    }

                    bool bOwning = false;
                    {
                        if( bSource )
                        {
                            if( pRelation->get_ownership().get() == mega::Ownership::eOwnTarget )
                                bOwning = true;
                        }
                        else
                        {
                            if( pRelation->get_ownership().get() == mega::Ownership::eOwnSource )
                                bOwning = true;
                        }
                    }

                    std::string    strMangle;
                    nlohmann::json link( { { "type", mega::psz_mega_reference_vector },
                                           { "type_id", printTypeID( pLinkDim->get_concrete_id() ) },
                                           { "mangle", "" },
                                           { "name", pLinkDim->get_link()->get_link()->get_identifier() },
                                           { "offset", pLinkDim->get_offset() },
                                           { "singular", false },
                                           { "types", nlohmann::json::array() },
                                           { "owning", bOwning }

                    } );

                    if( auto pLinkMany = db_cast< Concrete::Dimensions::LinkMany >( pLinkDim ) )
                    {
                        strMangle          = megaMangle( mega::psz_mega_reference_vector );
                        link[ "singular" ] = false;
                    }
                    else if( auto pLinkSingle = db_cast< Concrete::Dimensions::LinkSingle >( pLinkDim ) )
                    {
                        strMangle          = megaMangle( mega::psz_mega_reference );
                        link[ "singular" ] = true;
                    }
                    else
                    {
                        THROW_RTE( "Unknown link type" );
                    }

                    if( bSource )
                    {
                        for( auto pTarget : pRelation->get_targets() )
                        {
                            for( auto pConcreteLink : pTarget->get_concrete() )
                            {
                                auto pObjectOpt = pConcreteLink->get_concrete_object();
                                VERIFY_RTE( pObjectOpt.has_value() );
                                nlohmann::json type(
                                    { { "link_type_id", printTypeID( pConcreteLink->get_concrete_id() ) },
                                      { "object_type_id", printTypeID( pObjectOpt.value()->get_concrete_id() ) }

                                    } );
                                link[ "types" ].push_back( type );
                            }
                        }
                    }
                    else
                    {
                        for( auto pTarget : pRelation->get_sources() )
                        {
                            for( auto pConcreteLink : pTarget->get_concrete() )
                            {
                                auto pObjectOpt = pConcreteLink->get_concrete_object();
                                VERIFY_RTE( pObjectOpt.has_value() );
                                nlohmann::json type(
                                    { { "link_type_id", printTypeID( pConcreteLink->get_concrete_id() ) },
                                      { "object_type_id", printTypeID( pObjectOpt.value()->get_concrete_id() ) }

                                    } );
                                link[ "types" ].push_back( type );
                            }
                        }
                    }

                    link[ "mangle" ] = strMangle;
                    part[ "links" ].push_back( link );
                    mangledDataTypes.insert( strMangle );
                }
                for( auto pAllocDim : pPart->get_allocation_dimensions() )
                {
                    if( auto pAllocator = db_cast< Concrete::Dimensions::Allocator >( pAllocDim ) )
                    {
                        const std::string strAllocatorTypeName = allocatorTypeName( database, pAllocator );
                        const std::string strMangle            = megaMangle( strAllocatorTypeName );

                        std::ostringstream osAllocName;
                        osAllocName << "alloc_"
                                    << printTypeID(
                                           pAllocator->get_allocator()->get_allocated_context()->get_concrete_id() );

                        mangledDataTypes.insert( strMangle );

                        nlohmann::json member( { { "type", strAllocatorTypeName },
                                                 { "type_id", printTypeID( pAllocDim->get_concrete_id() ) },
                                                 { "mangle", strMangle },
                                                 { "name", osAllocName.str() },
                                                 { "offset", pAllocator->get_offset() } } );
                        part[ "members" ].push_back( member );
                    }
                    else
                    {
                        THROW_RTE( "Unknown allocation dimension type" );
                    }
                }

                data[ "parts" ].push_back( part );
            }
        }
    }

    for( const auto& str : mangledDataTypes )
    {
        data[ "mangled_data_types" ].push_back( str );
    }

    std::ostringstream osCPPCode;
    m_pInja->render_allocator( data, osCPPCode );
    compiler.compileToLLVMIR( osObjectTypeID.str(), osCPPCode.str(), os, pComponent );
}

} // namespace mega::runtime
