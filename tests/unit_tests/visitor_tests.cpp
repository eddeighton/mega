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

#include "mega/reference.hpp"
#include "mega/iterator.hpp"

#include "jit/jit_exception.hpp"

#include <gtest/gtest.h>

#include <vector>

namespace mega::mangle
{
mega::TypeID iterator_state( void* );
void         iterator_object_start( void*, mega::TypeID );
void         iterator_object_end( void* );
void         iterator_action_start( void*, mega::TypeID );
void         iterator_action_end( void*, mega::TypeID );
void         iterator_event_start( void*, mega::TypeID );
void         iterator_event_end( void*, mega::TypeID );
void         iterator_link_start( void*, mega::TypeID );
void         iterator_link_end( void*, mega::TypeID );
void         iterator_interupt_start( void*, mega::TypeID );
void         iterator_interupt_end( void*, mega::TypeID );
void         iterator_function_start( void*, mega::TypeID );
void         iterator_function_end( void*, mega::TypeID );
void         iterator_namespace_start( void*, mega::TypeID );
void         iterator_namespace_end( void*, mega::TypeID );
void         iterator_dimension( void*, mega::TypeID );
} // namespace mega::mangle

void traverse_0x10000( void* pIterator )
{
    switch( mega::mangle::iterator_state( pIterator ) )
    {
        case 0x10000: // Root START
            mega::mangle::iterator_object_start( pIterator, mega::TypeID{ 0x10016 } );
            break;
        case -2147418090: // Parent_ZeroToMany_OneToOne_Root END
            mega::mangle::iterator_link_end( pIterator, mega::TypeID{ 0x10015 } );
            break;
        case 0x10016: // Parent_ZeroToMany_OneToOne_Root START
            mega::mangle::iterator_link_start( pIterator, mega::TypeID{ -2147418090 } );
            break;
        case -2147418091: // Parent_ZeroToOne_OneToOne_Root END
            mega::mangle::iterator_link_end( pIterator, mega::TypeID{ 0x10017 } );
            break;
        case 0x10015: // Parent_ZeroToOne_OneToOne_Root START
            mega::mangle::iterator_link_start( pIterator, mega::TypeID{ -2147418091 } );
            break;
        case -2147418089: // Parent_Toaster_Root END
            mega::mangle::iterator_link_end( pIterator, mega::TypeID{ 0x10019 } );
            break;
        case 0x10017: // Parent_Toaster_Root START
            mega::mangle::iterator_link_start( pIterator, mega::TypeID{ -2147418089 } );
            break;
        case -2147418087: // Math_Basic_Root END
            mega::mangle::iterator_function_end( pIterator, mega::TypeID{ 0x10018 } );
            break;
        case 0x10019: // Math_Basic_Root START
            mega::mangle::iterator_function_start( pIterator, mega::TypeID{ -2147418087 } );
            break;
        case -2147418088: // Math_Basic2_Root END
            mega::mangle::iterator_function_end( pIterator, mega::TypeID{ 0x1001a } );
            break;
        case 0x10018: // Math_Basic2_Root START
            mega::mangle::iterator_function_start( pIterator, mega::TypeID{ -2147418088 } );
            break;
        case -2147418086: // AllTypesAreRef_Root END
            mega::mangle::iterator_function_end( pIterator, mega::TypeID{ 0x10013 } );
            break;
        case 0x1001a: // AllTypesAreRef_Root START
            mega::mangle::iterator_function_start( pIterator, mega::TypeID{ -2147418086 } );
            break;
        case -2147418093: // TestCallee_Root END
            mega::mangle::iterator_function_end( pIterator, mega::TypeID{ 0x10014 } );
            break;
        case 0x10013: // TestCallee_Root START
            mega::mangle::iterator_function_start( pIterator, mega::TypeID{ -2147418093 } );
            break;
        case -2147418092: // TestCall_Root END
            mega::mangle::iterator_function_end( pIterator, mega::TypeID{ 0x10007 } );
            break;
        case 0x10014: // TestCall_Root START
            mega::mangle::iterator_function_start( pIterator, mega::TypeID{ -2147418092 } );
            break;
        case 0x10007: // Root_m_U16 START
            mega::mangle::iterator_dimension( pIterator, mega::TypeID{ 0x1000c } );
            break;
        case 0x1000c: // Root_m_I16 START
            mega::mangle::iterator_dimension( pIterator, mega::TypeID{ 0x10006 } );
            break;
        case 0x10006: // Root_m_U32 START
            mega::mangle::iterator_dimension( pIterator, mega::TypeID{ 0x1000b } );
            break;
        case 0x1000b: // Root_m_I32 START
            mega::mangle::iterator_dimension( pIterator, mega::TypeID{ 0x10005 } );
            break;
        case 0x10005: // Root_m_U64 START
            mega::mangle::iterator_dimension( pIterator, mega::TypeID{ 0x1000a } );
            break;
        case 0x1000a: // Root_m_I64 START
            mega::mangle::iterator_dimension( pIterator, mega::TypeID{ 0x10010 } );
            break;
        case 0x10010: // Root_m_F32 START
            mega::mangle::iterator_dimension( pIterator, mega::TypeID{ 0x1000d } );
            break;
        case 0x1000d: // Root_m_F64 START
            mega::mangle::iterator_dimension( pIterator, mega::TypeID{ 0x10008 } );
            break;
        case 0x10008: // Root_m_TimeStamp START
            mega::mangle::iterator_dimension( pIterator, mega::TypeID{ 0x10004 } );
            break;
        case 0x10004: // Root_m_reference START
            mega::mangle::iterator_dimension( pIterator, mega::TypeID{ 0x10001 } );
            break;
        case 0x10001: // Root_m_vector_reference START
            mega::mangle::iterator_dimension( pIterator, mega::TypeID{ 0x10003 } );
            break;
        case 0x10003: // Root_m_string START
            mega::mangle::iterator_dimension( pIterator, mega::TypeID{ 0x10002 } );
            break;
        case 0x10002: // Root_m_vector_I32 START
            mega::mangle::iterator_dimension( pIterator, mega::TypeID{ 0x10012 } );
            break;
        case 0x10012: // Root_m_F2 START
            mega::mangle::iterator_dimension( pIterator, mega::TypeID{ 0x10011 } );
            break;
        case 0x10011: // Root_m_F3 START
            mega::mangle::iterator_dimension( pIterator, mega::TypeID{ 0x1000e } );
            break;
        case 0x1000e: // Root_m_F4 START
            mega::mangle::iterator_dimension( pIterator, mega::TypeID{ 0x10009 } );
            break;
        case 0x10009: // Root_m_Quat START
            mega::mangle::iterator_dimension( pIterator, mega::TypeID{ 0x1000f } );
            break;
        case 0x1000f: // Root_m_F33 START
            mega::mangle::iterator_dimension( pIterator, mega::TypeID{ -2147418112 } );
            break;
        case -2147418112: // Root END
            mega::mangle::iterator_object_end( pIterator );
            break;
        default:
            throw mega::runtime::JITException{ "Unknown concrete type id" };
            break;
    }
}

namespace mega
{

struct TestLogicalInstantiation
{
    LogicalObject start()
    {
        //
        return LogicalObject{ 0, TypeID{ 0x10000 } };
    }

    void* read( const LogicalReference& ref )
    {
        //
        return nullptr;
    }

    U64 linkSize( const LogicalReference& ref )
    {
        //
        return 0;
    }

    LogicalObject linkObject( const LogicalReference& link, U64 index )
    {
        //
        return {};
    }
};

struct TestLogicalTreeVisitor
{
    int objects    = 0;
    int actions    = 0;
    int events     = 0;
    int links      = 0;
    int dimensions = 0;
    int functions  = 0;

    void on_object_start( const LogicalReference& ref )
    {
        //
        ++objects;
    }
    void on_object_end( const LogicalReference& ref )
    {
        //
    }
    void on_action_start( const LogicalReference& ref )
    {
        //
        ++actions;
    }
    void on_action_end( const LogicalReference& ref )
    {
        //
    }
    void on_event_start( const LogicalReference& ref )
    {
        //
        ++events;
    }
    void on_event_end( const LogicalReference& ref )
    {
        //
    }
    void on_link_start( const LogicalReference& ref )
    {
        //
        ++links;
    }
    void on_link_end( const LogicalReference& ref )
    {
        //
    }
    void on_interupt( const LogicalReference& ref )
    {
        //
    }
    void on_function( const LogicalReference& ref )
    {
        //
        ++functions;
    }
    void on_namespace( const LogicalReference& ref )
    {
        //
    }
    void on_dimension( const LogicalReference& ref, void* pData )
    {
        //
        ++dimensions;
    }
};
} // namespace mega

TEST( Visitor, Basic )
{
    using namespace mega;

    TestLogicalInstantiation logicalInstantiation;
    TestLogicalTreeVisitor   visitor;

    LogicalTreeTraversal objectTraversal( logicalInstantiation, visitor );

    Iterator iterator( traverse_0x10000, objectTraversal );
    while( iterator )
    {
        ++iterator;
    }

    ASSERT_EQ( visitor.objects, 1 );
    ASSERT_EQ( visitor.links, 3 );
    ASSERT_EQ( visitor.functions, 5 );
    ASSERT_EQ( visitor.events, 0 );
    ASSERT_EQ( visitor.actions, 0 );
    ASSERT_EQ( visitor.dimensions, 18 );
}