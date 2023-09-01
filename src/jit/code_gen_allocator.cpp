
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
        std::vector< FinalStage::Interface::IContext* > contexts;
        for( ; pIContext; pIContext = db_cast< FinalStage::Interface::IContext >( pIContext->get_parent() ) )
        {
            contexts.push_back( pIContext );
        }
        std::reverse( contexts.begin(), contexts.end() );
        bool bFirst = true;
        for( auto p : contexts )
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

std::string fullInterfaceTypeName( FinalStage::Interface::DimensionTrait* pDim )
{
    std::ostringstream osFullTypeName;
    osFullTypeName << fullInterfaceTypeName( pDim->get_parent() ) << '_' << pDim->get_id()->get_str();
    return osFullTypeName.str();
}

mega::U64 concreteLocalDomainSize( const FinalStage::Concrete::Context* pContext )
{
    using namespace FinalStage;
    using namespace FinalStage::Concrete;
    if( auto pEvent = db_cast< const Event >( pContext ) )
    {
        return pEvent->get_local_size();
    }
    else if( auto pAction = db_cast< const Action >( pContext ) )
    {
        return pAction->get_local_size();
    }
    else
    {
        return 1;
    }
}

std::string getContextTypeClass( const FinalStage::Concrete::Context* pContext )
{
    using namespace FinalStage;
    using namespace FinalStage::Concrete;
    if( db_cast< const Object >( pContext ) )
        return "object";
    else if( db_cast< const Action >( pContext ) )
        return "action";
    else if( db_cast< const Event >( pContext ) )
        return "event";
    else if( db_cast< const Link >( pContext ) )
        return "link";
    else if( db_cast< const Function >( pContext ) )
        return "function";
    else if( db_cast< const Namespace >( pContext ) )
        return "namespace";
    else if( db_cast< const Interupt >( pContext ) )
        return "interupt";
    else
    {
        THROW_RTE( "Unknown context type class" );
    }
}

/*
std::string makeIterName( const FinalStage::Concrete::Context* pContext )
{
    using namespace FinalStage;
    using namespace FinalStage::Concrete;
    std::ostringstream os;
    os << "i_" << pContext->get_interface()->get_identifier() << "_" << pContext->get_concrete_id();
    return os.str();
}*/
/*
struct ParentSizes
{
    std::vector< const FinalStage::Concrete::Context* > parents;
    std::vector< U64 >                                  sizes;

    ParentSizes( const JITDatabase& database, const FinalStage::Concrete::Context* pContext )
    {
        using namespace FinalStage;
        using namespace FinalStage::Concrete;

        U64 size = 1;
        while( pContext )
        {
            parents.push_back( pContext );
            sizes.push_back( size );
            size *= database.getLocalDomainSize( pContext->get_concrete_id() );
            if( !!db_cast< const Concrete::Object >( pContext ) )
                break;
            pContext = db_cast< const Concrete::Context >( pContext->get_parent() );
        }
        std::reverse( parents.begin(), parents.end() );
        std::reverse( sizes.begin(), sizes.end() );
    }

    std::string calculateInstance() const
    {
        using namespace FinalStage;
        using namespace FinalStage::Concrete;

        std::ostringstream instance;
        bool               bFirst = true;
        for( U64 i = 0; i != parents.size(); ++i )
        {
            if( bFirst )
                bFirst = false;
            else
                instance << " + ";
            instance << sizes[ i ] << " * " << makeIterName( parents[ i ] ) << " ";
        }
        return instance.str();
    }
};*/

/*
template < typename TDimensionType >
std::string calculateElementOffset( const JITDatabase& database, TDimensionType* pUserDim, std::string& strInstance )
{
    using namespace FinalStage;
    using namespace FinalStage::Concrete;

    std::ostringstream offset;
    {
        ParentSizes parentSizes( database, pUserDim->get_parent() );
        strInstance = parentSizes.calculateInstance();

        MemoryLayout::Part* pPart   = pUserDim->get_part();
        auto                pBuffer = pPart->get_buffer();

        offset << pPart->get_offset();
        offset << " + " << pPart->get_size() << " * (";
        offset << strInstance << ") + " << pUserDim->get_offset();
    }

    return offset.str();
}*/

/*
void generateAllocatorDimensions( const JITDatabase& database, FinalStage::Concrete::Dimensions::User* pUserDim,
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
                              { "owning", false },
                              { "owned", false }

    } );
    data[ "elements" ].push_back( element );
}

std::optional< nlohmann::json > allocatorLink( const JITDatabase& database, FinalStage::Concrete::Link* pLink )
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
    bool bOwned  = false;
    {
        if( bSource )
        {
            if( pRelation->get_ownership().get() == mega::Ownership::eOwnTarget )
                bOwning = true;
            if( pRelation->get_ownership().get() == mega::Ownership::eOwnSource )
                bOwned = true;
        }
        else
        {
            if( pRelation->get_ownership().get() == mega::Ownership::eOwnTarget )
                bOwned = true;
            if( pRelation->get_ownership().get() == mega::Ownership::eOwnSource )
                bOwning = true;
        }
    }

    // if( bOwning )
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
                                  { "owned", bOwned },
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
    //else
    //{
    //    // for now do not add non-owning links to xml format.
    //    return {};
    //}
}

void recurseAllocatorElements( const JITDatabase& database, FinalStage::Concrete::Context* pContext,
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

    std::string strInstance;
    {
        ParentSizes parentSizes( database, pContext );
        strInstance = parentSizes.calculateInstance();
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
                                  { "owning", false },
                                  { "owned", false },
                                  { "instance", strInstance }

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
                                  { "owning", false },
                                  { "owned", false },
                                  { "instance", strInstance }

        } );
        data[ "elements" ].push_back( element );
    }
}
*/

std::string makeStartState( const mega::TypeID& typeID )
{
    return printTypeID( typeID );
}

std::string makeEndState( const mega::TypeID& typeID )
{
    std::ostringstream os;
    os << static_cast< mega::TypeID::ValueType >( TypeID::make_end_state( typeID ) );
    return os.str();
}

void recurseTraversalStates( const JITDatabase& database, nlohmann::json& data,
                             const FinalStage::Concrete::Context* pContext )
{
    using namespace FinalStage;
    using namespace FinalStage::Concrete;

    // collate elements
    using ContextElementVariant       = std::variant< const Dimensions::User*, const Context* >;
    using ContextElementVariantVector = std::vector< ContextElementVariant >;
    ContextElementVariantVector elements;
    {
        {
            auto children = pContext->get_children();
            std::copy( children.begin(), children.end(), std::back_inserter( elements ) );
        }

        if( auto pUserDimensionContext = db_cast< const UserDimensionContext >( pContext ) )
        {
            auto dimensions = pUserDimensionContext->get_dimensions();
            std::copy( dimensions.begin(), dimensions.end(), std::back_inserter( elements ) );
        }
    }

    mega::TypeID typeIDSuccessor = pContext->get_concrete_id();
    for( ContextElementVariantVector::iterator i = elements.begin(), iPrev, iEnd = elements.end(); i != iEnd; ++i )
    {
        {
            if( auto ppDim = std::get_if< const Dimensions::User* >( &*i ) )
            {
                const Dimensions::User* pDim = *ppDim;
                typeIDSuccessor              = pDim->get_concrete_id();
            }
            else if( auto ppContext = std::get_if< const Context* >( &*i ) )
            {
                const Context* pChildContext = *ppContext;
                typeIDSuccessor              = pChildContext->get_concrete_id();
            }
            else
            {
                THROW_RTE( "Unknown type" );
            }
        }

        if( i == elements.begin() )
        {
            nlohmann::json state( { { "value", makeStartState( pContext->get_concrete_id() ) },
                                    { "start", true },
                                    { "local_domain_size", concreteLocalDomainSize( pContext ) },
                                    { "type", getContextTypeClass( pContext ) },
                                    { "successor", makeStartState( typeIDSuccessor ) },
                                    { "name", fullInterfaceTypeName( pContext->get_interface() ) }

            } );
            data[ "states" ].push_back( state );
        }
        else
        {
            if( auto ppDim = std::get_if< const Dimensions::User* >( &*iPrev ) )
            {
                const Dimensions::User* pDim = *ppDim;

                nlohmann::json state( { { "value", makeStartState( pDim->get_concrete_id() ) },
                                        { "start", true },
                                        { "local_domain_size", 1 },
                                        { "type", "dimension" },
                                        { "successor", makeStartState( typeIDSuccessor ) },
                                        { "name", fullInterfaceTypeName( pDim->get_interface_dimension() ) }

                } );
                data[ "states" ].push_back( state );
            }
            else if( auto ppContext = std::get_if< const Context* >( &*iPrev ) )
            {
                const Context* pChildContext = *ppContext;

                // NOTE: this is the END state for the context
                nlohmann::json state( { { "value", makeEndState( pChildContext->get_concrete_id() ) },
                                        { "start", false },
                                        { "local_domain_size", concreteLocalDomainSize( pChildContext ) },
                                        { "type", getContextTypeClass( pChildContext ) },
                                        { "successor", makeStartState( typeIDSuccessor ) },
                                        { "name", fullInterfaceTypeName( pChildContext->get_interface() ) }

                } );
                data[ "states" ].push_back( state );

                // recursively generate the START state for the context
                recurseTraversalStates( database, data, pChildContext );
            }
            else
            {
                THROW_RTE( "Unknown type" );
            }
        }
        iPrev = i;
    }

    // generate transition to context end state from final element OR context start state
    if( elements.empty() )
    {
        // transition from start to end state for same type
        nlohmann::json state( { { "value", makeStartState( pContext->get_concrete_id() ) },
                                { "start", true },
                                { "local_domain_size", concreteLocalDomainSize( pContext ) },
                                { "type", getContextTypeClass( pContext ) },
                                { "successor", makeEndState( pContext->get_concrete_id() ) },
                                { "name", fullInterfaceTypeName( pContext->get_interface() ) }

        } );
        data[ "states" ].push_back( state );
    }
    else
    {
        if( auto ppDim = std::get_if< const Dimensions::User* >( &elements.back() ) )
        {
            const Dimensions::User* pDim = *ppDim;

            nlohmann::json state( { { "value", makeStartState( pDim->get_concrete_id() ) },
                                    { "start", true },
                                    { "local_domain_size", 1 },
                                    { "type", "dimension" },
                                    { "successor", makeEndState( pContext->get_concrete_id() ) },
                                    { "name", fullInterfaceTypeName( pDim->get_interface_dimension() ) }

            } );
            data[ "states" ].push_back( state );
        }
        else if( auto ppContext = std::get_if< const Context* >( &elements.back() ) )
        {
            const Context* pChildContext = *ppContext;

            // NOTE: this is the END state for the context
            nlohmann::json state( { { "value", makeEndState( pChildContext->get_concrete_id() ) },
                                    { "start", false },
                                    { "local_domain_size", concreteLocalDomainSize( pChildContext ) },
                                    { "type", getContextTypeClass( pChildContext ) },
                                    { "successor", makeEndState( pContext->get_concrete_id() ) },
                                    { "name", fullInterfaceTypeName( pChildContext->get_interface() ) }

            } );
            data[ "states" ].push_back( state );

            // recursively generate the START state for the context
            recurseTraversalStates( database, data, pChildContext );
        }
        else
        {
            THROW_RTE( "Unknown type" );
        }
    }
}

void CodeGenerator::generate_alllocator( const LLVMCompiler& compiler, const JITDatabase& database,
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
                           { "relations", nlohmann::json::array() },
                           { "mangled_data_types", nlohmann::json::array() },
                           { "state", nlohmann::json::array() }

    } );

    recurseTraversalStates( database, data, pObject );
    // special case for Object END where will pop stack
    {
        nlohmann::json state( { { "value", makeEndState( pObject->get_concrete_id() ) },
                                { "start", false },
                                { "type", getContextTypeClass( pObject ) },
                                { "successor", makeEndState( pObject->get_concrete_id() ) },
                                { "name", fullInterfaceTypeName( pObject->get_interface() ) }

        } );
        data[ "states" ].push_back( state );
    }
    // std::vector< FinalStage::HyperGraph::Relation* > owningRelations;

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
                    bool bOwned  = false;
                    {
                        if( bSource )
                        {
                            if( pRelation->get_ownership().get() == mega::Ownership::eOwnTarget )
                                bOwning = true;
                            if( pRelation->get_ownership().get() == mega::Ownership::eOwnSource )
                                bOwned = true;
                        }
                        else
                        {
                            if( pRelation->get_ownership().get() == mega::Ownership::eOwnTarget )
                                bOwned = true;
                            if( pRelation->get_ownership().get() == mega::Ownership::eOwnSource )
                                bOwning = true;
                        }
                    }

                    std::string    strMangle;
                    RelationID     relationID = pRelation->get_id();
                    nlohmann::json link( { { "type", mega::psz_mega_reference_vector },
                                           { "type_id", printTypeID( pLinkDim->get_concrete_id() ) },
                                           { "link_type_id", printTypeID( pLinkDim->get_link()->get_concrete_id() ) },
                                           { "mangle", "" },
                                           { "name", pLinkDim->get_link()->get_link()->get_identifier() },
                                           { "offset", pLinkDim->get_offset() },
                                           { "singular", false },
                                           { "types", nlohmann::json::array() },
                                           { "owning", bOwning },
                                           { "owned", bOwned },
                                           { "relation_id_lower", printTypeID( relationID.getLower() ) },
                                           { "relation_id_upper", printTypeID( relationID.getUpper() ) }

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
