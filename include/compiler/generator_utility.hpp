
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

#ifndef GUARD_2023_July_28_generator_utility
#define GUARD_2023_July_28_generator_utility

inline std::string toHex( mega::TypeID typeID )
{
    std::ostringstream os;
    os << "0x" << std::hex << std::setw( 8 ) << std::setfill( '0' ) << 
        static_cast< mega::U32 >( typeID.getSymbolID() );
    return os.str();
}

struct CleverUtility
{
    using IDList = std::vector< std::string >;
    IDList& theList;
    CleverUtility( IDList& theList, const std::string& strID )
        : theList( theList )
    {
        theList.push_back( strID );
    }
    ~CleverUtility() { theList.pop_back(); }
};


#endif //GUARD_2023_July_28_generator_utility
