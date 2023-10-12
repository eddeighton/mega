
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


struct TypeIDSequenceGen
{
    using NewSymbolNames = std::map< std::string, Symbols::SymbolTypeID* >;
    using CastFunctor    = std::function< Interface::IContext*( Interface::ContextGroup* ) >;
    
    const NewSymbolNames& symbolNames;

    TypeIDSequenceGen( const NewSymbolNames& symbolNames )
        : symbolNames( symbolNames )
    {
    }

    mega::TypeID getTypeID( const std::string& strID ) const
    {
        auto iFind = symbolNames.find( strID );
        VERIFY_RTE( iFind != symbolNames.end() );
        const Symbols::SymbolTypeID* pSymbolTypeID = iFind->second;
        return pSymbolTypeID->get_id();
    }

    void recurse( Interface::ContextGroup* pContextGroup, mega::TypeIDSequence& sequence ) const
    {
        if( auto pContext = db_cast< Interface::IContext >( pContextGroup ) )
        {
            sequence.push_back( getTypeID( Interface::getIdentifier( pContext ) ) );
            recurse( pContext->get_parent(), sequence );
        }
    }

    mega::TypeIDSequence operator()( Interface::IContext* pContext ) const
    {
        mega::TypeIDSequence sequence{ getTypeID( Interface::getIdentifier( pContext ) ) };
        recurse( pContext->get_parent(), sequence );
        std::reverse( sequence.begin(), sequence.end() );
        return sequence;
    }
    mega::TypeIDSequence operator()( Interface::DimensionTrait* pDimension ) const
    {
        mega::TypeIDSequence sequence{ getTypeID( Interface::getIdentifier( pDimension ) ) };
        recurse( pDimension->get_parent(), sequence );
        std::reverse( sequence.begin(), sequence.end() );
        return sequence;
    }
    mega::TypeIDSequence operator()( Interface::LinkTrait* pLink ) const
    {
        mega::TypeIDSequence sequence{ getTypeID( Interface::getIdentifier( pLink ) ) };
        recurse( pLink->get_parent(), sequence );
        std::reverse( sequence.begin(), sequence.end() );
        return sequence;
    }
};
