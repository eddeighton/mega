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

#include "invocation/name_resolution.hpp"
#include "invocation/invocation.hpp"

#include "database/model/OperationsStage.hxx"

namespace mega::invocation
{

using namespace OperationsStage;

namespace
{

void expandLinks( OperationsStage::Database& database, std::vector< Operations::Name* >& names )
{
    using namespace OperationsStage::Operations;

    // keep going until ALL leaf nodes that are links are expanded to linked to types
    bool bContinue = true;
    while( bContinue )
    {
        bContinue = false;
        for( Name* pName : names )
        {
            // find leaf nodes
            if( pName->get_children().empty() )
            {
                ConcreteVariant* pConVar = pName->get_element()->get_concrete();

                if( pConVar->get_context().has_value() )
                {
                    THROW_TODO;
                   /*if( auto pLink = db_cast< Concrete::Link >( pConVar->get_context().value() ) )
                    {
                        bContinue = true;
                        pName->set_is_reference( true );

                        InterfaceVariantVectorVector interfaceVariantVectorVector
                            = linkToInterfaceVariantVector( database, pLink );

                        std::vector< Operations::ElementVector* > elementVector
                            = toElementVector( database, interfaceVariantVectorVector );

                        for( Operations::ElementVector* pElementVector : elementVector )
                        {
                            for( Element* pElement : pElementVector->get_elements() )
                            {
                                Name* pChildName = database.construct< Name >(
                                    Name::Args{ NameRoot::Args{ {} }, pElement, false, false } );
                                names.push_back( pChildName );
                                pName->push_back_children( pChildName );
                            }
                        }
                    }*/
                }
            }
        }
    }
}

void addType( OperationsStage::Database&        database,
              std::vector< Operations::Name* >& names,
              Operations::ElementVector*        pElementVector )
{
    using namespace OperationsStage::Operations;

    std::vector< Operations::Name* > namesCopy = names;
    for( Name* pName : namesCopy )
    {
        // find leaf nodes in the names tree
        if( pName->get_children().empty() )
        {
            ConcreteVariant* pConVar = pName->get_element()->get_concrete();
            if( !pConVar->get_context().has_value() )
            {
                THROW_RTE( "Cannot resolve non-context element in invocation" );
            }
            Concrete::Context* pContext = pConVar->get_context().value();

            // initially attempt to find any direct members of the context that
            // correspond to ANY of the concrete elements
            std::vector< Element* > memberElements;
            {
                for( Element* pElement : pElementVector->get_elements() )
                {
                    if( auto opt = pElement->get_concrete()->get_context() )
                    {
                        for( Concrete::Context* pChildContext : pContext->get_children() )
                        {
                            if( pChildContext == opt.value() )
                            {
                                memberElements.push_back( pElement );
                                break;
                            }
                        }
                    }
                    else if( auto opt = pElement->get_concrete()->get_dimension() )
                    {
                        if( auto pUserDimensionContext = db_cast< Concrete::UserDimensionContext >( pContext ) )
                        {
                            for( Concrete::Dimensions::User* pDimension : pUserDimensionContext->get_dimensions() )
                            {
                                if( pDimension == opt.value() )
                                {
                                    memberElements.push_back( pElement );
                                    break;
                                }
                            }
                        }
                    }
                }
            }

            // if ANY match is found then prefer it over EVERYTHING else
            if( !memberElements.empty() )
            {
                for( Element* pElement : memberElements )
                {
                    Name* pMemberName
                        = database.construct< Name >( Name::Args{ NameRoot::Args{ {} }, pElement, true, false } );
                    names.push_back( pMemberName );
                    pName->push_back_children( pMemberName );
                }
            }
            else
            {
                // otherwise simply add everything in the element vector
                for( Element* pElement : pElementVector->get_elements() )
                {
                    Name* pChildName
                        = database.construct< Name >( Name::Args{ NameRoot::Args{ {} }, pElement, false, false } );
                    names.push_back( pChildName );
                    pName->push_back_children( pChildName );
                }
            }
        }
    }
}

void pruneBranches( OperationsStage::Operations::Name* pName )
{
    using namespace OperationsStage::Operations;

    auto children = pName->get_children();

    if( !children.empty() )
    {
        if( pName->get_is_reference() )
        {
            if( !pName->get_is_member() )
            {
                for( Name* pChild : children )
                {
                    pruneBranches( pChild );
                    if( pChild->get_is_member() )
                    {
                        pName->set_is_member( true );
                    }
                }
            }
        }
        else
        {
            std::vector< Name* > best;
            bool                 bRemovedChild = false;
            for( Name* pChild : children )
            {
                pruneBranches( pChild );
                if( pChild->get_is_member() )
                    best.push_back( pChild );
                else
                    bRemovedChild = true;
            }
            if( !best.empty() )
            {
                pName->set_is_member( true );
                if( bRemovedChild )
                    pName->set_children( best );
            }
        }
    }
}

} // namespace

OperationsStage::Operations::NameResolution* resolve( OperationsStage::Database&               database,
                                                      OperationsStage::Operations::Invocation* pInvocation )
{
    // bool bExpandFinalReferences = false;
    switch( pInvocation->get_operation() )
    {
        case id_Imp_NoParams:
        case id_Imp_Params:
        case id_Start:
        case id_Stop:
        case id_Move:
        case id_Get:
        case id_Range:
        case id_Raw:
            break;
        case HIGHEST_OPERATION_TYPE:
        default:
            THROW_RTE( "Invalid operation type" );
    }

    using namespace OperationsStage::Operations;

    Operations::Context*  pContext  = pInvocation->get_context();
    Operations::TypePath* pTypePath = pInvocation->get_type_path();

    std::vector< Name* > names;
    NameRoot*            pNameRoot = database.construct< NameRoot >( NameRoot::Args{ {} } );

    for( ElementVector* pElementVector : pContext->get_vectors() )
    {
        for( Element* pElement : pElementVector->get_elements() )
        {
            Name* pName = database.construct< Name >( Name::Args{ NameRoot::Args{ {} }, pElement, false, false } );
            names.push_back( pName );
            pNameRoot->push_back_children( pName );
        }
    }

    for( ElementVector* pElementVector : pTypePath->get_vectors() )
    {
        //expandLinks( database, names );
        addType( database, names, pElementVector );

        for( Name* pName : pNameRoot->get_children() )
        {
            pruneBranches( pName );
        }
    }

    /*if( bExpandFinalReferences )
    {
        expandLinks( database, names );
        for( Name* pName : pNameRoot->get_children() )
        {
            pruneBranches( pName );
        }
    }*/

    return database.construct< NameResolution >( NameResolution::Args{ pNameRoot } );
}

} // namespace mega::invocation
