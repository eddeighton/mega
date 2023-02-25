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

#ifndef CONTAINER_29_JAN_2021
#define CONTAINER_29_JAN_2021

#include "schematic/node.hpp"

#include "common/compose.hpp"
#include "common/assert_verify.hpp"

#include <vector>

namespace schematic
{
template < typename T, typename TBase >
class Container : public TBase
{
public:
    using TPtr    = boost::shared_ptr< T >;
    using TVector = std::vector< TPtr >;

    template < typename TArg1 >
    Container( TArg1 param )
        : TBase( param )
    {
    }

    template < typename TArg1, typename TArg2 >
    Container( TArg1 pParent, TArg2 param )
        : TBase( pParent, param )
    {
    }

    template < typename TArg1, typename TArg2 >
    Container( TArg1 pParent, TArg2 param, const std::string& strName )
        : TBase( pParent, param, strName )
    {
    }

    virtual void init()
    {
        m_elements.clear();
        this->template for_each(
            generics::collectIfConvert( m_elements, Node::ConvertPtrType< T >(), Node::ConvertPtrType< T >() ) );
        TBase::init();
    }

    virtual bool add( Node::Ptr pNewNode )
    {
        const bool bAdded = Node::add( pNewNode );
        if( bAdded )
        {
            if( TPtr pNewSite = boost::dynamic_pointer_cast< T >( pNewNode ) )
                m_elements.push_back( pNewSite );
        }
        return bAdded;
    }

    virtual void remove( Node::Ptr pNode )
    {
        Node::remove( pNode );
        if( TPtr pOldSite = boost::dynamic_pointer_cast< T >( pNode ) )
        {
            auto iFind = std::find( m_elements.begin(), m_elements.end(), pOldSite );
            VERIFY_RTE( iFind != m_elements.end() );
            if( iFind != m_elements.end() )
                m_elements.erase( iFind );
        }
    }

    const TVector& getElements() const { return m_elements; }

private:
    TVector m_elements;
};

} // namespace schematic

#endif // CONTAINER_29_JAN_2021