
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

#ifndef GUARD_2023_November_24_mangle
#define GUARD_2023_November_24_mangle

#include "il/elements/types.hpp"

namespace mega::il
{

struct MangleFunction : ExternFunction
{
    std::string mangleType;

    inline void setName( const std::string& str )
    {
        std::ostringstream os;
        os << "mega::mangle::" << str << mangleType;
        name = os.str();
    }

    MangleFunction( std::string _mangleType )
        : mangleType( std::move( _mangleType ) )
    {
        returnType = Mutable{ e_void };
    }
};

struct MangleCTor : MangleFunction
{
    MangleCTor( std::string _mangleType )
        : MangleFunction( std::move( _mangleType ) )
    {
        using namespace std::string_literals;
        setName( "new_"s );
        parameterTypes.emplace_back( Ptr{ e_void } );
    }
};

struct MangleDTor : MangleFunction
{
    MangleDTor( std::string _mangleType )
        : MangleFunction( std::move( _mangleType ) )
    {
        using namespace std::string_literals;
        setName( "delete_"s );
        parameterTypes.emplace_back( Ptr{ e_void } );
    }
};

struct MangleCopy : MangleFunction
{
    MangleCopy( std::string _mangleType )
        : MangleFunction( std::move( _mangleType ) )
    {
        using namespace std::string_literals;
        setName( "copy_"s );
        parameterTypes.emplace_back( Ptr{ e_void } );
        parameterTypes.emplace_back( Ptr{ e_void } );
    }
};

struct MangleSaveXML : MangleFunction
{
    MangleSaveXML( std::string _mangleType )
        : MangleFunction( std::move( _mangleType ) )
    {
        using namespace std::string_literals;
        setName( "save_xml_"s );
        parameterTypes.emplace_back( Ptr{ e_void } );
        parameterTypes.emplace_back( ConstPtr{ e_char } );
        parameterTypes.emplace_back( Ptr{ e_void } );
    }
};

struct MangleLoadXML : MangleFunction
{
    MangleLoadXML( std::string _mangleType )
        : MangleFunction( std::move( _mangleType ) )
    {
        using namespace std::string_literals;
        setName( "load_xml_"s );
        parameterTypes.emplace_back( Ptr{ e_void } );
        parameterTypes.emplace_back( ConstPtr{ e_char } );
        parameterTypes.emplace_back( Ptr{ e_void } );
    }
};

struct MangleSaveBin : MangleFunction
{
    MangleSaveBin( std::string _mangleType )
        : MangleFunction( std::move( _mangleType ) )
    {
        using namespace std::string_literals;
        setName( "save_bin_"s );
        parameterTypes.emplace_back( Ptr{ e_void } );
        parameterTypes.emplace_back( Ptr{ e_void } );
    }
};

struct MangleLoadBin : MangleFunction
{
    MangleLoadBin( std::string _mangleType )
        : MangleFunction( std::move( _mangleType ) )
    {
        using namespace std::string_literals;
        setName( "load_bin_"s );
        parameterTypes.emplace_back( Ptr{ e_void } );
        parameterTypes.emplace_back( Ptr{ e_void } );
    }
};
/*
struct MangleSaveRecord : MangleFunction
{
    MangleSaveRecord( std::string _mangleType )
        : MangleFunction( std::move( _mangleType ) )
    {
        using namespace std::string_literals;
        setName( "save_record_"s );
        parameterTypes.emplace_back( ConstRef{ e_reference } );
        parameterTypes.emplace_back( ConstPtr{ e_void } );
    }
};
*/
struct MangleLoadRecord : MangleFunction
{
    MangleLoadRecord( std::string _mangleType )
        : MangleFunction( std::move( _mangleType ) )
    {
        using namespace std::string_literals;
        setName( "load_record_"s );
        parameterTypes.emplace_back( Ptr{ e_void } );
        parameterTypes.emplace_back( Ptr{ e_void } );
        parameterTypes.emplace_back( Mutable{ e_unsignedlong } );
    }
};
/*
struct MangleReadAny : MangleFunction
{
    MangleReadAny( std::string _mangleType )
        : MangleFunction( std::move( _mangleType ) )
    {
        using namespace std::string_literals;
        setName( "read_any_"s );
        parameterTypes.emplace_back( Ptr{ e_void } );
    }
};

struct MangleWriteAny : MangleFunction
{
    MangleWriteAny( std::string _mangleType )
        : MangleFunction( std::move( _mangleType ) )
    {
        using namespace std::string_literals;
        setName( "write_any_"s );
        parameterTypes.emplace_back( Ptr{ e_void } );
        parameterTypes.emplace_back( Mutable{ e_Any } );
    }
};
*/
} // namespace mega::il

#endif // GUARD_2023_November_24_mangle
