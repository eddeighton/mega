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

#ifndef INVOCATION_ID_12_AUG_2022
#define INVOCATION_ID_12_AUG_2022

#include "mega/common.hpp"

#include <utility>
#include <vector>
#include <array>

namespace mega
{

class InvocationID
{
public:
    using SymbolIDVector = std::vector< mega::SymbolID >;

    SymbolIDVector    m_context;
    SymbolIDVector    m_type_path;
    mega::OperationID m_operation;

    inline bool operator==( const InvocationID& cmp ) const
    {
        return ( m_context == cmp.m_context ) && ( m_type_path == cmp.m_type_path )
               && ( m_operation == cmp.m_operation );
    }

    inline bool operator<( const InvocationID& cmp ) const
    {
        return ( m_context != cmp.m_context )       ? ( m_context < cmp.m_context )
               : ( m_type_path != cmp.m_type_path ) ? ( m_type_path < cmp.m_type_path )
                                                    : ( m_operation < cmp.m_operation );
    }

    InvocationID() = default;

    InvocationID( SymbolIDVector context, SymbolIDVector typePath, mega::OperationID operationID )
        : m_context( std::move( context ) )
        , m_type_path( std::move( typePath ) )
        , m_operation( operationID )
    {
    }

    template < mega::U64 Size >
    constexpr InvocationID( mega::TypeID context, const std::array< mega::TypeID, Size >& typePath,
                            mega::TypeID operation )
        : m_context{ context }
        , m_type_path( typePath.begin(), typePath.end() )
        , m_operation( static_cast< mega::OperationID >( operation ) )
    {
    }

    template < mega::U64 ContextSize, mega::U64 TypePathSize >
    constexpr InvocationID( const std::array< mega::TypeID, ContextSize >&  context,
                            const std::array< mega::TypeID, TypePathSize >& typePath, mega::TypeID operation )
        : m_context( context.begin(), context.end() )
        , m_type_path( typePath.begin(), typePath.end() )
        , m_operation( static_cast< mega::OperationID >( operation ) )
    {
    }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& m_context;
        archive& m_type_path;
        archive& m_operation;
    }
};

} // namespace mega

#endif // INVOCATION_ID_12_AUG_2022
