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

#include "mega/values/runtime/reference.hpp"
#include "mega/iterator.hpp"
#include "mega/logical_tree.hpp"

#include "jit/jit_exception.hpp"

#include <gtest/gtest.h>

#include <vector>
/*
namespace mega::mangle
{
mega::TypeID iterator_state( void* );
void         iterator_object_start( void*, mega::TypeID );
void         iterator_object_end( void* );
void         iterator_component_start( void*, mega::TypeID, mega::Instance );
void         iterator_component_end( void*, mega::TypeID );
void         iterator_action_start( void*, mega::TypeID, mega::Instance );
void         iterator_action_end( void*, mega::TypeID );
void         iterator_state_start( void*, mega::TypeID, mega::Instance );
void         iterator_state_end( void*, mega::TypeID );
void         iterator_event_start( void*, mega::TypeID, mega::Instance );
void         iterator_event_end( void*, mega::TypeID );
void         iterator_link_start( void*, mega::TypeID, bool bOwning, bool bOwned );
void         iterator_link_end( void*, mega::TypeID, bool bOwning, bool bOwned );
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
            mega::mangle::iterator_link_end( pIterator, mega::TypeID{ 0x10015 }, false, false );
            break;
        case 0x10016: // Parent_ZeroToMany_OneToOne_Root START
            mega::mangle::iterator_link_start( pIterator, mega::TypeID{ -2147418090 }, false, false );
            break;
        case -2147418091: // Parent_ZeroToOne_OneToOne_Root END
            mega::mangle::iterator_link_end( pIterator, mega::TypeID{ 0x10017 }, false, false );
            break;
        case 0x10015: // Parent_ZeroToOne_OneToOne_Root START
            mega::mangle::iterator_link_start( pIterator, mega::TypeID{ -2147418091 }, false, false );
            break;
        case -2147418089: // Parent_Toaster_Root END
            mega::mangle::iterator_link_end( pIterator, mega::TypeID{ 0x1001e }, false, false );
            break;
        case 0x10017: // Parent_Toaster_Root START
            mega::mangle::iterator_link_start( pIterator, mega::TypeID{ -2147418089 }, false, false );
            break;
        case -2147418082: // TestAction_Root END
            mega::mangle::iterator_action_end( pIterator, mega::TypeID{ 0x10019 } );
            break;
        case 0x1001e: // TestAction_Root START
            mega::mangle::iterator_action_start( pIterator, mega::TypeID{ 0x1001f }, 4 );
            break;
        case -2147418081: // TestB_TestAction_Root END
            mega::mangle::iterator_action_end( pIterator, mega::TypeID{ -2147418082 } );
            break;
        case 0x1001f: // TestB_TestAction_Root START
            mega::mangle::iterator_action_start( pIterator, mega::TypeID{ 0x10020 }, 3 );
            break;
        case -2147418080: // TestC_TestB_TestAction_Root END
            mega::mangle::iterator_action_end( pIterator, mega::TypeID{ -2147418081 } );
            break;
        case 0x10020: // TestC_TestB_TestAction_Root START
            mega::mangle::iterator_action_start( pIterator, mega::TypeID{ 0x10021 }, 2 );
            break;
        case 0x10021: // TestC_TestB_TestAction_Root_m_inner_dimension START
            mega::mangle::iterator_dimension( pIterator, mega::TypeID{ -2147418080 } );
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

        case 0x40000: // ObjB START
            mega::mangle::iterator_object_start( pIterator, mega::TypeID{ 0x40003 } );
            break;
        case -2147221501: // Child_ZeroToOne_OneToOne_ObjB END
            mega::mangle::iterator_link_end( pIterator, mega::TypeID{ 0x40002 }, false, false );
            break;
        case 0x40003: // Child_ZeroToOne_OneToOne_ObjB START
            mega::mangle::iterator_link_start( pIterator, mega::TypeID{ -2147221501 }, false, false );
            break;
        case -2147221502: // Parent_ZeroToMany_OneToOne_Other_ObjB END
            mega::mangle::iterator_link_end( pIterator, mega::TypeID{ 0x40004 }, false, false );
            break;
        case 0x40002: // Parent_ZeroToMany_OneToOne_Other_ObjB START
            mega::mangle::iterator_link_start( pIterator, mega::TypeID{ -2147221502 }, false, false );
            break;
        case -2147221500: // Child_ZeroToMany_OneToOne_Other_ObjB END
            mega::mangle::iterator_link_end( pIterator, mega::TypeID{ 0x40001 }, false, false );
            break;
        case 0x40004: // Child_ZeroToMany_OneToOne_Other_ObjB START
            mega::mangle::iterator_link_start( pIterator, mega::TypeID{ -2147221500 }, false, false );
            break;
        case 0x40001: // ObjB_m_string START
            mega::mangle::iterator_dimension( pIterator, mega::TypeID{ -2147221504 } );
            break;
        case -2147221504: // ObjB END
            mega::mangle::iterator_object_end( pIterator );
            break;

        case 0x50000: // ObjA START
            mega::mangle::iterator_object_start( pIterator, mega::TypeID{ 0x50003 } );
            break;
        case -2147155965: // Child_ZeroToMany_OneToOne_ObjA END
            mega::mangle::iterator_link_end( pIterator, mega::TypeID{ 0x50002 }, false, false );
            break;
        case 0x50003: // Child_ZeroToMany_OneToOne_ObjA START
            mega::mangle::iterator_link_start( pIterator, mega::TypeID{ -2147155965 }, false, false );
            break;
        case -2147155966: // Parent_ZeroToMany_OneToMany_ObjA END
            mega::mangle::iterator_link_end( pIterator, mega::TypeID{ 0x50004 }, false, false );
            break;
        case 0x50002: // Parent_ZeroToMany_OneToMany_ObjA START
            mega::mangle::iterator_link_start( pIterator, mega::TypeID{ -2147155966 }, false, false );
            break;
        case -2147155964: // Child_ZeroToMany_OneToMany_ObjA END
            mega::mangle::iterator_link_end( pIterator, mega::TypeID{ 0x50001 }, false, false );
            break;
        case 0x50004: // Child_ZeroToMany_OneToMany_ObjA START
            mega::mangle::iterator_link_start( pIterator, mega::TypeID{ -2147155964 }, false, false );
            break;
        case 0x50001: // ObjA_m_string START
            mega::mangle::iterator_dimension( pIterator, mega::TypeID{ -2147155968 } );
            break;
        case -2147155968: // ObjA END
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

    U64 linkSize( const LogicalReference& ref, bool bOwning, bool bOwned )
    {
        if( ref.typeInstance.type == TypeID{ 0x10015 } )
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }

    LogicalObject linkGet( const LogicalReference& link, U64 index )
    {
        if( index == 0 )
        {
            if( link.typeInstance.type == TypeID{ 0x10015 } )
            {
                return LogicalObject{ 1, TypeID{ 0x40000 } };
            }
        }
        else
        {
            return {};
        }
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
    int component  = 0;
    int states     = 0;

    void on_object_start( const LogicalReference& ref )
    {
        //
        ++objects;
    }
    void on_object_end( const LogicalReference& ref )
    {
        //
    }
    void on_component_start( const LogicalReference& ref )
    {
        //
        ++component;
    }
    void on_component_end( const LogicalReference& ref )
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
    void on_state_start( const LogicalReference& ref )
    {
        //
        ++states;
    }
    void on_state_end( const LogicalReference& ref )
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
    void on_link_start( const LogicalReference& ref, bool bOwning, bool bOwned )
    {
        //
        ++links;
    }
    void on_link_end( const LogicalReference& ref, bool bOwning, bool bOwned )
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

    ASSERT_EQ( visitor.objects, 2 );
    ASSERT_EQ( visitor.links, 3 + 3 );
    ASSERT_EQ( visitor.functions, 5 );
    ASSERT_EQ( visitor.events, 0 );
    ASSERT_EQ( visitor.actions, 40 );
    ASSERT_EQ( visitor.dimensions, 42 + 1 );
}*/