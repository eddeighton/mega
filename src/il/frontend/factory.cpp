
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

#include "il/frontend/factory.hpp"
#include "il/elements/elements.hpp"
#include "il/elements/types.hpp"
#include "il/elements/mangle.hpp"
#include "il/elements/types.hxx"
#include "il/elements/functions.hxx"
#include "il/elements/materialisers.hxx"

// #include "environment/jit_database.hpp"

namespace mega::il
{

using namespace std::string_literals;

namespace
{

ValueType voidPtr = Ptr{ e_void };
Mutable   intType{ e_int };
ValueType charPtr = Ptr{ e_char };
ValueType u64     = Mutable{ e_unsignedlong };

auto expAdd( auto expression, U64 size )
{
    return Operator{ { expression, ReadLiteral{ Literal{ Const{ e_int }, std::to_string( size ) } } }, Operator::eAdd };
}
/*
void collectStartStates( FinalStage::Automata::Enum* pEnum, std::vector< mega::U32 >& startBits )
{
    if( auto pTest = FinalStage::db_cast< FinalStage::Automata::Test >( pEnum ) )
    {
        if( pTest->get_start_state() )
        {
            startBits.push_back( pTest->get_bitset_index() );
        }
    }

    for( auto pChildEnum : pEnum->get_children() )
    {
        collectStartStates( pChildEnum, startBits );
    }
}

void collectStartStates( FinalStage::Concrete::Object* pObject, FinalStage::Concrete::Dimensions::Bitset* pBitSet,
                         std::vector< std::string >& blocks )
{
    // determine the correct initialisation for the bitset size
    std::vector< mega::U32 > startBits;
    {
        for( auto pEnum : pObject->get_automata_enums() )
        {
            collectStartStates( pEnum, startBits );
        }
        std::sort( startBits.begin(), startBits.end() );
    }

    const auto numberOfBits   = pBitSet->get_number_of_bits();
    mega::U32  numberOfBlocks = numberOfBits / 64;
    if( numberOfBits % 64 )
    {
        ++numberOfBlocks;
    }

    auto iBit = startBits.begin(), iBitEnd = startBits.end();
    for( auto iBlock = 0; iBlock != numberOfBlocks; ++iBlock )
    {
        static const std::string blockBinaryLiteral
            = "0000000000000000000000000000000000000000000000000000000000000000";
        std::string block = blockBinaryLiteral;

        for( int i = 0; i != 64; ++i )
        {
            if( ( iBit != iBitEnd ) && ( *iBit == ( i + ( iBlock * 64 ) ) ) )
            {
                block[ 63 - i ] = '1';
                ++iBit;
            }
        }
        blocks.push_back( block );
    }
}*/
} // namespace

FunctionDefinition Factory::generate_Object_constructor( runtime::JITDatabase& db, const runtime::FunctorID& functorID )
{
    THROW_TODO;
    /*auto pObject = db.getObject( functorID.m_Object.typeID );

    Variable< ValueType > pMemory{ voidPtr, "pMemory"s };
    Variable< ValueType > iterator{ charPtr, "p"s };

    std::vector< Statement > statements;
    for( auto pBuffer : pObject->get_buffers() )
    {
        for( auto pPart : pBuffer->get_parts() )
        {
            std::vector< Statement > initialisations;
            for( auto pUserDim : pPart->get_user_dimensions() )
            {
                initialisations.emplace_back( ExpressionStatement{
                    Call{ MangleCTor{ pUserDim->get_interface_dimension()->get_mangle()->get_mangle() },
                          { expAdd( Read{ iterator }, pUserDim->get_offset() ) } } } );
            }

            for( auto pLink : pPart->get_link_dimensions() )
            {
                if( pLink->get_singular() )
                {
                    initialisations.emplace_back( ExpressionStatement{ Call{
                        MangleCTor{ "classmega00reference" }, { expAdd( Read{ iterator }, pLink->get_offset() ) } } } );

                    initialisations.emplace_back( ExpressionStatement{
                        Call{ MangleCTor{ "classmega00TypeID" },
                              { expAdd( Read{ iterator }, pLink->get_link_type()->get_offset() ) } } } );
                }
                else
                {
                    initialisations.emplace_back(
                        ExpressionStatement{ Call{ MangleCTor{ "classstd00vector3classmega00reference4" },
                                                   { expAdd( Read{ iterator }, pLink->get_offset() ) } } } );

                    initialisations.emplace_back( ExpressionStatement{
                        Call{ MangleCTor{ "classstd00vector3classmega00TypeID4" },
                              { expAdd( Read{ iterator }, pLink->get_link_type()->get_offset() ) } } } );
                }
            }

            for( auto pBitset : pPart->get_bitset_dimensions() )
            {
                std::vector< std::string > blocks;
                collectStartStates( pObject, pBitset, blocks );

                std::vector< Expression > initExpressions;
                for( const auto& block : blocks )
                {
                    std::ostringstream osBlock;
                    osBlock << "0b" << block;
                    initExpressions.emplace_back( ReadLiteral{ Literal{ Const{ e_unsignedlong }, osBlock.str() } } );
                }

                Variable startState{ u64, "start_state" };

                initialisations.emplace_back( Scope{
                    { ArrayVariableDeclaration{ startState, initExpressions },
                      ExpressionStatement{ Call{
                          new_bitset_{},
                          {
                              expAdd( Read{ iterator }, pBitset->get_offset() ),
                              Operator{ { Operator{ {
                                                        Read{ startState },                                    //
                                                        ReadLiteral{ Literal{ Const{ e_unsignedlong }, "0" } } //
                                                    },
                                                    Operator::eIndex } },
                                        Operator::eAddressOf },
                              Operator{
                                  { Operator{ {
                                                  Read{ startState }, //
                                                  ReadLiteral{ Literal{ Const{ e_unsignedlong },
                                                                        std::to_string( initExpressions.size() ) } } //
                                              },
                                              Operator::eIndex } },
                                  Operator::eAddressOf },
                          } } } } } );
            }

            auto start = expAdd( Cast{ { Read{ pMemory } }, charPtr }, pPart->get_offset() );
            auto end   = expAdd( start, pPart->get_total_domain_size() * pPart->get_size() );

            statements.emplace_back( ForLoop{

                iterator,
                start,
                Operator{ { Read{ iterator }, end }, Operator::eNotEqual },
                Operator::makeIncrement( iterator, std::to_string( pPart->get_size() ) ),
                { initialisations } } );
        }
    }

    return { functorID.name(), Mutable{ e_void }, { pMemory }, statements };*/
}
FunctionDefinition Factory::generate_Object_destructor( runtime::JITDatabase& db, const runtime::FunctorID& functorID )
{
    THROW_TODO;
   /* auto pObject = db.getObject( functorID.m_Object.typeID );

    Variable< ValueType > pReference{ ConstRef{ e_reference }, "ref"s };
    Variable< ValueType > pMemory{ voidPtr, "pMemory"s };
    Variable< ValueType > bLinkReset{ Mutable{ e_bool }, "bLinkReset"s };

    Variable< ValueType > iterator{ charPtr, "p"s };

    std::vector< Statement > statements;
    for( auto pBuffer : pObject->get_buffers() )
    {
        for( auto pPart : pBuffer->get_parts() )
        {
            std::vector< Statement > destructions;
            for( auto pUserDim : pPart->get_user_dimensions() )
            {
                destructions.emplace_back( ExpressionStatement{
                    Call{ MangleDTor{ pUserDim->get_interface_dimension()->get_mangle()->get_mangle() },
                          { expAdd( Read{ iterator }, pUserDim->get_offset() ) } } } );
            }

            for( auto pLink : pPart->get_link_dimensions() )
            {
                if( pLink->get_singular() )
                {
                    destructions.emplace_back( ExpressionStatement{ Call{
                        MangleDTor{ "classmega00reference" }, { expAdd( Read{ iterator }, pLink->get_offset() ) } } } );

                    destructions.emplace_back( ExpressionStatement{
                        Call{ MangleDTor{ "classmega00TypeID" },
                              { expAdd( Read{ iterator }, pLink->get_link_type()->get_offset() ) } } } );
                }
                else
                {
                    destructions.emplace_back(
                        ExpressionStatement{ Call{ MangleDTor{ "classstd00vector3classmega00reference4" },
                                                   { expAdd( Read{ iterator }, pLink->get_offset() ) } } } );

                    destructions.emplace_back( ExpressionStatement{
                        Call{ MangleDTor{ "classstd00vector3classmega00TypeID4" },
                              { expAdd( Read{ iterator }, pLink->get_link_type()->get_offset() ) } } } );
                }


                Relation                 relationMat;
                Variable< Materialiser > relationMatVar{ relationMat, "linkReset"s };
                const FunctionTemplate&  linkReset = relationMat.getFunctionTemplate( Relation::Reset );

                Variable< ValueType > linkRelationID{ Const{ e_RelationID }, "linkRef" };
                Variable< ValueType > linkRef{ Const{ e_reference }, "linkRef" };

                // link reset
                destructions.emplace_back(
                    If{ { If::Case{ Read{ bLinkReset },
                                    {

                                        { MaterialiserStatement //
                                        { relationMatVar, { Read{ linkRelationID } }, linkReset.function },
                                        ExpressionStatement //
                                        { CallFunctor{ relationMatVar, linkReset.function, { Read{ linkRef } } } } }

                                    } } } } );

            // const mega::Pointer linkRef = mega::Pointer::make( ref,
            //         mega::TypeInstance{ {{ link.link_type_id }}, static_cast< mega::Instance >( instance ) } );
            // static thread_local mega::runtime::relation::LinkReset function(
            //     g_pszModuleName, mega::RelationID{ {{ link.relation_id_lower }}, {{ link.relation_id_upper }} } );
            // function( linkRef );

            }

            for( auto pBitset : pPart->get_bitset_dimensions() )
            {
                destructions.emplace_back( ExpressionStatement{
                    Call{ MangleDTor{ pBitset->get_interface_compiler_dimension()->get_mangle()->get_mangle() },
                          { expAdd( Read{ iterator }, pBitset->get_offset() ) } } } );
            }


            auto start = expAdd( Cast{ { Read{ pMemory } }, charPtr }, pPart->get_offset() );
            auto end   = expAdd( start, pPart->get_total_domain_size() * pPart->get_size() );

            statements.emplace_back( ForLoop{

                iterator,
                start,
                Operator{ { Read{ iterator }, end }, Operator::eNotEqual },
                Operator::makeIncrement( iterator, std::to_string( pPart->get_size() ) ),
                { destructions } } );
        }
    }

    return { functorID.name(), Mutable{ e_void }, { pReference, pMemory, bLinkReset }, statements };*/
}
FunctionDefinition Factory::generate_Object_binaryLoad( runtime::JITDatabase& db, const runtime::FunctorID& functorID )
{
    THROW_TODO;
}
FunctionDefinition Factory::generate_Object_binarySave( runtime::JITDatabase& db, const runtime::FunctorID& functorID )
{
    THROW_TODO;
}
FunctionDefinition Factory::generate_Object_getFunction( runtime::JITDatabase& db, const runtime::FunctorID& functorID )
{
    THROW_TODO;
}
FunctionDefinition Factory::generate_Object_unParent( runtime::JITDatabase& db, const runtime::FunctorID& functorID )
{
    THROW_TODO;
}
FunctionDefinition Factory::generate_Object_traverse( runtime::JITDatabase& db, const runtime::FunctorID& functorID )
{
    THROW_TODO;
}
FunctionDefinition Factory::generate_Object_linkSize( runtime::JITDatabase& db, const runtime::FunctorID& functorID )
{
    THROW_TODO;
}
FunctionDefinition Factory::generate_Object_linkGet( runtime::JITDatabase& db, const runtime::FunctorID& functorID )
{
    THROW_TODO;
}
FunctionDefinition Factory::generate_Object_anyRead( runtime::JITDatabase& db, const runtime::FunctorID& functorID )
{
    THROW_TODO;
}
FunctionDefinition Factory::generate_Object_anyWrite( runtime::JITDatabase& db, const runtime::FunctorID& functorID )
{
    THROW_TODO;
}
FunctionDefinition Factory::generate_Object_enumerate( runtime::JITDatabase& db, const runtime::FunctorID& functorID )
{
    THROW_TODO;
}

FunctionDefinition Factory::generate_Relation_Make( runtime::JITDatabase& db, const runtime::FunctorID& functorID )
{
    THROW_TODO;
}
FunctionDefinition Factory::generate_Relation_Break( runtime::JITDatabase& db, const runtime::FunctorID& functorID )
{
    THROW_TODO;
}
FunctionDefinition Factory::generate_Relation_Reset( runtime::JITDatabase& db, const runtime::FunctorID& functorID )
{
    THROW_TODO;
}
FunctionDefinition Factory::generate_Relation_Size( runtime::JITDatabase& db, const runtime::FunctorID& functorID )
{
    THROW_TODO;
}
FunctionDefinition Factory::generate_Relation_Get( runtime::JITDatabase& db, const runtime::FunctorID& functorID )
{
    THROW_TODO;
}
} // namespace mega::il
