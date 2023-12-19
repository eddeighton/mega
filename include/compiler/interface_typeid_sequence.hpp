
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
    using NewSymbolNames = std::map< std::string, Symbols::SymbolID* >;
    using ObjectSubObjectIDSequence = mega::SymbolTraits::SymbolIDVectorPair;
    
    const NewSymbolNames& symbolNames;

    TypeIDSequenceGen( const NewSymbolNames& symbolNames )
        : symbolNames( symbolNames )
    {
    }

    mega::interface::SymbolID getSymbolID( const std::string& strID ) const
    {
        auto iFind = symbolNames.find( strID );
        VERIFY_RTE( iFind != symbolNames.end() );
        const Symbols::SymbolID* pSymbolID = iFind->second;
        return pSymbolID->get_id();
    }

    // recurse up the tree from the node to the root
    // record the initial path into second - then if encounter object
    // record remaining into first.
    template< typename T >
    void recurse( T* pInterface, ObjectSubObjectIDSequence& sequence, bool bFoundObject ) const
    {
        if( !bFoundObject )
        {
            if( db_cast< Interface::Object >( pInterface ) )
            {
                bFoundObject = true;
            }
        }
        else
        {
            VERIFY_RTE( !db_cast< Interface::Object >( pInterface ) );
        }
        if( bFoundObject )
        {
            sequence.first.push_back( getSymbolID( Interface::getIdentifier( pInterface ) ) );
        }
        else
        {
            sequence.second.push_back( getSymbolID( Interface::getIdentifier( pInterface ) ) );
        }

        if( auto pParent = db_cast< Interface::IContext >( pInterface->get_parent() ) )
        {
            recurse( pParent, sequence, bFoundObject );
        }
    }

    template< typename T >
    ObjectSubObjectIDSequence operator()( T* pContext ) const
    {
        ObjectSubObjectIDSequence sequence;
        recurse( pContext, sequence, false );
        if( !sequence.first.empty() )
        {
            std::reverse( sequence.first.begin(), sequence.first.end() );
        }
        std::reverse( sequence.second.begin(), sequence.second.end() );
        return sequence;
    }
};
