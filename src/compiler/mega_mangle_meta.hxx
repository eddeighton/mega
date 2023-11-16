
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

#include "database/ComponentListing.hxx"

namespace mega::compiler
{

void constructMegaMangle( ComponentListing::Database& database )
{
    using namespace ComponentListing;
    database.construct< MegaMangle::Mangle >(
        MegaMangle::Mangle::Args
        {
            "unsigned char",
            "unsignedchar",
            "Simple",
            "System.Byte"
        }
    );
    database.construct< MegaMangle::Mangle >(
        MegaMangle::Mangle::Args
        {
            "signed char",
            "signedchar",
            "Simple",
            "System.SByte"
        }
    );
    database.construct< MegaMangle::Mangle >(
        MegaMangle::Mangle::Args
        {
            "unsigned short",
            "unsignedshort",
            "Simple",
            "System.UInt16"
        }
    );
    database.construct< MegaMangle::Mangle >(
        MegaMangle::Mangle::Args
        {
            "short",
            "short",
            "Simple",
            "System.Int16"
        }
    );
    database.construct< MegaMangle::Mangle >(
        MegaMangle::Mangle::Args
        {
            "unsigned int",
            "unsignedint",
            "Simple",
            "System.UInt16"
        }
    );
    database.construct< MegaMangle::Mangle >(
        MegaMangle::Mangle::Args
        {
            "int",
            "int",
            "Simple",
            "System.Int32"
        }
    );
    database.construct< MegaMangle::Mangle >(
        MegaMangle::Mangle::Args
        {
            "unsigned long",
            "unsignedlong",
            "Simple",
            "System.UInt64"
        }
    );
    database.construct< MegaMangle::Mangle >(
        MegaMangle::Mangle::Args
        {
            "unsigned long long",
            "unsignedlonglong",
            "Simple",
            "System.UInt64"
        }
    );
    database.construct< MegaMangle::Mangle >(
        MegaMangle::Mangle::Args
        {
            "long",
            "long",
            "Simple",
            "System.Int64"
        }
    );
    database.construct< MegaMangle::Mangle >(
        MegaMangle::Mangle::Args
        {
            "long long",
            "longlong",
            "Simple",
            "System.Int64"
        }
    );
    database.construct< MegaMangle::Mangle >(
        MegaMangle::Mangle::Args
        {
            "float",
            "float",
            "Simple",
            "System.Single"
        }
    );
    database.construct< MegaMangle::Mangle >(
        MegaMangle::Mangle::Args
        {
            "double",
            "double",
            "Simple",
            "System.Double"
        }
    );
    database.construct< MegaMangle::Mangle >(
        MegaMangle::Mangle::Args
        {
            "bool",
            "bool",
            "Simple",
            "System.Byte"
        }
    );
    database.construct< MegaMangle::Mangle >(
        MegaMangle::Mangle::Args
        {
            "class mega::TypeID",
            "classmega00TypeID",
            "Simple",
            "interop.TypeID"
        }
    );
    database.construct< MegaMangle::Mangle >(
        MegaMangle::Mangle::Args
        {
            "class mega::reference",
            "classmega00reference",
            "NonSimple",
            "interop.Reference"
        }
    );
    database.construct< MegaMangle::Mangle >(
        MegaMangle::Mangle::Args
        {
            "class std::vector<class mega::reference>",
            "classstd00vector3classmega00reference4",
            "NonSimple",
            ""
        }
    );
    database.construct< MegaMangle::Mangle >(
        MegaMangle::Mangle::Args
        {
            "class std::vector<class mega::TypeID>",
            "classstd00vector3classmega00TypeID4",
            "NonSimple",
            ""
        }
    );
    database.construct< MegaMangle::Mangle >(
        MegaMangle::Mangle::Args
        {
            "class std::basic_string<char>",
            "classstd00basic_string3char4",
            "NonSimple",
            ""
        }
    );
    database.construct< MegaMangle::Mangle >(
        MegaMangle::Mangle::Args
        {
            "class std::vector<int>",
            "classstd00vector3int4",
            "NonSimple",
            ""
        }
    );
    database.construct< MegaMangle::Mangle >(
        MegaMangle::Mangle::Args
        {
            "class boost::dynamic_bitset<unsigned long long>",
            "classboost00dynamic_bitset3unsignedlonglong4",
            "NonSimple",
            ""
        }
    );
    database.construct< MegaMangle::Mangle >(
        MegaMangle::Mangle::Args
        {
            "struct F2",
            "structF2",
            "Simple",
            "interop.F2"
        }
    );
    database.construct< MegaMangle::Mangle >(
        MegaMangle::Mangle::Args
        {
            "struct F3",
            "structF3",
            "Simple",
            "interop.F3"
        }
    );
    database.construct< MegaMangle::Mangle >(
        MegaMangle::Mangle::Args
        {
            "struct F4",
            "structF4",
            "Simple",
            "interop.F4"
        }
    );
    database.construct< MegaMangle::Mangle >(
        MegaMangle::Mangle::Args
        {
            "struct Quat",
            "structQuat",
            "Simple",
            "interop.Quat"
        }
    );
    database.construct< MegaMangle::Mangle >(
        MegaMangle::Mangle::Args
        {
            "struct F33",
            "structF33",
            "Simple",
            "interop.F33"
        }
    );
}

}
