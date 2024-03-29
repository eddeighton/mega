
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

#ifndef GUARD_2023_December_21_interface_visitor
#define GUARD_2023_December_21_interface_visitor

namespace Interface
{
struct Visitor
{
    virtual ~Visitor() = default;
    virtual bool visit( UserDimension* ) const { return false; }
    virtual bool visit( UserAlias* ) const { return false; }
    virtual bool visit( UserUsing* ) const { return false; }
    virtual bool visit( NonOwningLink* ) const { return false; }
    virtual bool visit( OwningLink* ) const { return false; }
    virtual bool visit( UserLink* ) const { return false; }
    virtual bool visit( Aggregate* ) const { return false; }

    virtual bool visit( Namespace* ) const { return false; }
    virtual bool visit( Abstract* ) const { return false; }
    virtual bool visit( Event* ) const { return false; }
    virtual bool visit( Object* ) const { return false; }
    virtual bool visit( Interupt* ) const { return false; }
    virtual bool visit( Requirement* ) const { return false; }
    virtual bool visit( Decider* ) const { return false; }
    virtual bool visit( Function* ) const { return false; }
    virtual bool visit( Action* ) const { return false; }
    virtual bool visit( Component* ) const { return false; }
    virtual bool visit( State* ) const { return false; }
    virtual bool visit( InvocationContext* ) const { return false; }
    virtual bool visit( Unspecified* ) const { return false; }
    virtual bool visit( IContext* ) const { return false; }
};

inline bool visit( Visitor& visitor, Interface::Node* pINode )
{
    if( auto* pUserDimension = db_cast< UserDimension >( pINode ) )
    {
        if( !visitor.visit( pUserDimension ) )
        {
            if( !visitor.visit( static_cast< Aggregate* >( pUserDimension ) ) )
            {
                return false;
            }
        }
        return true;
    }
    else if( auto* pUserAlias = db_cast< UserAlias >( pINode ) )
    {
        if( !visitor.visit( pUserAlias ) )
        {
            if( !visitor.visit( static_cast< Aggregate* >( pUserAlias ) ) )
            {
                return false;
            }
        }
        return true;
    }
    else if( auto* pUserUsing = db_cast< UserUsing >( pINode ) )
    {
        if( !visitor.visit( pUserUsing ) )
        {
            if( !visitor.visit( static_cast< Aggregate* >( pUserUsing ) ) )
            {
                return false;
            }
        }
        return true;
    }
    else if( auto* pNonOwningLink = db_cast< NonOwningLink >( pINode ) )
    {
        if( !visitor.visit( pNonOwningLink ) )
        {
            if( !visitor.visit( static_cast< UserLink* >( pNonOwningLink ) ) )
            {
                if( !visitor.visit( static_cast< Aggregate* >( pNonOwningLink ) ) )
                {
                    return false;
                }
            }
        }
        return true;
    }
    else if( auto* pOwningLink = db_cast< OwningLink >( pINode ) )
    {
        if( !visitor.visit( pOwningLink ) )
        {
            if( !visitor.visit( static_cast< UserLink* >( pOwningLink ) ) )
            {
                if( !visitor.visit( static_cast< Aggregate* >( pOwningLink ) ) )
                {
                    return false;
                }
            }
        }
        return true;
    }

    else if( auto* pUserLink = db_cast< UserLink >( pINode ) )
    {
        if( !visitor.visit( pUserLink ) )
        {
            if( !visitor.visit( static_cast< Aggregate* >( pUserLink ) ) )
            {
                return false;
            }
        }
        return true;
    }
    else if( auto* pAggregate = db_cast< Aggregate >( pINode ) )
    {
        if( !visitor.visit( pAggregate ) )
        {
            return false;
        }
        return true;
    }
    else if( auto* pNamespace = db_cast< Namespace >( pINode ) )
    {
        if( !visitor.visit( pNamespace ) )
        {
            if( !visitor.visit( static_cast< IContext* >( pNamespace ) ) )
            {
                return false;
            }
        }
        return true;
    }
    else if( auto* pAbstract = db_cast< Abstract >( pINode ) )
    {
        if( !visitor.visit( pAbstract ) )
        {
            if( !visitor.visit( static_cast< IContext* >( pAbstract ) ) )
            {
                return false;
            }
        }
        return true;
    }
    else if( auto* pEvent = db_cast< Event >( pINode ) )
    {
        if( !visitor.visit( pEvent ) )
        {
            if( !visitor.visit( static_cast< IContext* >( pEvent ) ) )
            {
                return false;
            }
        }
        return true;
    }
    else if( auto* pObject = db_cast< Object >( pINode ) )
    {
        if( !visitor.visit( pObject ) )
        {
            if( !visitor.visit( static_cast< IContext* >( pObject ) ) )
            {
                return false;
            }
        }
        return true;
    }
    else if( auto* pInterupt = db_cast< Interupt >( pINode ) )
    {
        if( !visitor.visit( pInterupt ) )
        {
            if( !visitor.visit( static_cast< InvocationContext* >( pInterupt ) ) )
            {
                if( !visitor.visit( static_cast< IContext* >( pInterupt ) ) )
                {
                    return false;
                }
            }
        }
        return true;
    }
    else if( auto* pRequirement = db_cast< Requirement >( pINode ) )
    {
        if( !visitor.visit( pRequirement ) )
        {
            if( !visitor.visit( static_cast< InvocationContext* >( pRequirement ) ) )
            {
                if( !visitor.visit( static_cast< IContext* >( pRequirement ) ) )
                {
                    return false;
                }
            }
        }
        return true;
    }
    else if( auto* pDecider = db_cast< Decider >( pINode ) )
    {
        if( !visitor.visit( pDecider ) )
        {
            if( !visitor.visit( static_cast< InvocationContext* >( pDecider ) ) )
            {
                if( !visitor.visit( static_cast< IContext* >( pDecider ) ) )
                {
                    return false;
                }
            }
        }
        return true;
    }
    else if( auto* pFunction = db_cast< Function >( pINode ) )
    {
        if( !visitor.visit( pFunction ) )
        {
            if( !visitor.visit( static_cast< InvocationContext* >( pFunction ) ) )
            {
                if( !visitor.visit( static_cast< IContext* >( pFunction ) ) )
                {
                    return false;
                }
            }
        }
        return true;
    }
    else if( auto* pAction = db_cast< Action >( pINode ) )
    {
        if( !visitor.visit( pAction ) )
        {
            if( !visitor.visit( static_cast< State* >( pAction ) ) )
            {
                if( !visitor.visit( static_cast< InvocationContext* >( pAction ) ) )
                {
                    if( !visitor.visit( static_cast< IContext* >( pAction ) ) )
                    {
                        return false;
                    }
                }
            }
        }
        return true;
    }
    else if( auto* pComponent = db_cast< Component >( pINode ) )
    {
        if( !visitor.visit( pComponent ) )
        {
            if( !visitor.visit( static_cast< State* >( pComponent ) ) )
            {
                if( !visitor.visit( static_cast< InvocationContext* >( pComponent ) ) )
                {
                    if( !visitor.visit( static_cast< IContext* >( pComponent ) ) )
                    {
                        return false;
                    }
                }
            }
        }
        return true;
    }
    else if( auto* pState = db_cast< State >( pINode ) )
    {
        if( !visitor.visit( pState ) )
        {
            if( !visitor.visit( static_cast< InvocationContext* >( pState ) ) )
            {
                if( !visitor.visit( static_cast< IContext* >( pState ) ) )
                {
                    return false;
                }
            }
        }
        return true;
    }
    else if( auto* pInvocationContext = db_cast< InvocationContext >( pINode ) )
    {
        if( !visitor.visit( pInvocationContext ) )
        {
            if( !visitor.visit( static_cast< IContext* >( pState ) ) )
            {
                return false;
            }
        }
        return true;
    }
    else if( auto* pUnspecified = db_cast< Unspecified >( pINode ) )
    {
        if( !visitor.visit( pUnspecified ) )
        {
            if( !visitor.visit( static_cast< IContext* >( pState ) ) )
            {
                return false;
            }
        }
        return true;
    }
    else if( auto* pIContext = db_cast< IContext >( pINode ) )
    {
        if( !visitor.visit( pIContext ) )
        {
            return false;
        }
        return true;
    }
    else
    {
        THROW_RTE( "Unknown interface node type" );
    }
};
} // namespace Interface

#endif // GUARD_2023_December_21_interface_visitor
