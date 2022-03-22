
#include "database/model/allocator.hpp"
#include "database/model/concrete.hpp"

#include "database/stages/interface.hpp"

namespace eg
{
namespace concrete
{


void Allocator::load( Loader& loader )
{
    m_pContext_Allocated = loader.loadObjectRef< Action >();
    m_pContext_Allocating = loader.loadObjectRef< Action >();
}
void Allocator::store( Storer& storer ) const
{
    storer.storeObjectRef( m_pContext_Allocated );
    storer.storeObjectRef( m_pContext_Allocating );
}

void NothingAllocator::load( Loader& loader )
{
    Allocator::load( loader );
}
void NothingAllocator::store( Storer& storer ) const
{
    Allocator::store( storer );
}

void NothingAllocator::constructDimensions( Stages::Interface& stage )
{
    
}

void constructRuntimeDimensions( Stages::Interface& stage, Action* pContext )
{
    pContext->m_pStopCycle                     = stage.construct< concrete::Dimension_Generated >();
    pContext->m_pStopCycle->m_type             = concrete::Dimension_Generated::eActionStopCycle;
    pContext->m_pStopCycle->m_pContext         = pContext;
    pContext->m_pStopCycle->m_pParent          = pContext;
    pContext->m_children.push_back( pContext->m_pStopCycle );
    
    //NOTE - MUST always ensure the reference is constructed BEFORE the state as this ensures the
    //order of serialisation over the network.
    pContext->m_pReference                     = stage.construct< concrete::Dimension_Generated >();
    pContext->m_pReference->m_type             = concrete::Dimension_Generated::eActionReference;
    pContext->m_pReference->m_pContext         = pContext;
    pContext->m_pReference->m_pParent          = pContext;
    pContext->m_children.push_back( pContext->m_pReference );
    
    pContext->m_pState                         = stage.construct< concrete::Dimension_Generated >();
    pContext->m_pState->m_type                 = concrete::Dimension_Generated::eActionState;
    pContext->m_pState->m_pContext             = pContext;
    pContext->m_pState->m_pParent              = pContext;
    pContext->m_children.push_back( pContext->m_pState );
    
    if( dynamic_cast< const interface::Object* >( pContext->getContext() ) )
    {
        pContext->m_pLinkRefCount                  = stage.construct< concrete::Dimension_Generated >();
        pContext->m_pLinkRefCount->m_type          = concrete::Dimension_Generated::eLinkReferenceCount;
        pContext->m_pLinkRefCount->m_pContext      = pContext;
        pContext->m_pLinkRefCount->m_pParent       = pContext;
        pContext->m_children.push_back( pContext->m_pLinkRefCount );
    }
}

void SingletonAllocator::load( Loader& loader )
{
    Allocator::load( loader );
}
void SingletonAllocator::store( Storer& storer ) const
{
    Allocator::store( storer );
}

void SingletonAllocator::constructDimensions( Stages::Interface& stage )
{
    constructRuntimeDimensions( stage, m_pContext_Allocated );
}

void RangeAllocator::load( Loader& loader )
{
    Allocator::load( loader );
    m_pAllocatorData    = loader.loadObjectRef< Dimension_Generated >();
}
void RangeAllocator::store( Storer& storer ) const
{
    Allocator::store( storer );
    storer.storeObjectRef( m_pAllocatorData );
}

void RangeAllocator::constructDimensions( Stages::Interface& stage )
{
    constructRuntimeDimensions( stage, m_pContext_Allocated );
    
    m_pAllocatorData                 = stage.construct< concrete::Dimension_Generated >();
    m_pAllocatorData->m_type         = concrete::Dimension_Generated::eActionAllocator;
    m_pAllocatorData->m_pContext     = m_pContext_Allocated;
    m_pAllocatorData->m_pParent      = m_pContext_Allocated;
    m_pContext_Allocating->m_children.push_back( m_pAllocatorData );
}

std::string RangeAllocator::getAllocatorType() const
{
    const std::size_t szSize = m_pContext_Allocated->getLocalDomainSize();
    if( szSize <= 32U )
    {
        std::ostringstream os;
        os << "::eg::Bitmask32Allocator< " << szSize << " >";
        return os.str();
    }
    else if( szSize <= 64U )
    {
        std::ostringstream os;
        os << "::eg::Bitmask64Allocator< " << szSize << " >";
        return os.str();
    }
    else
    {
        std::ostringstream os;
        os << "::eg::RingAllocator< " << szSize << " >";
        return os.str();
    }
}
            
Allocator* chooseAllocator( Stages::Interface& stage, Action* pParent, Action* pChild )
{
    Allocator* pResult = nullptr;
    
    bool bGenerate = true;
    if( pParent == nullptr )
    {
        bGenerate = false;
    }
    else if( const interface::Root* pRoot = dynamic_cast< const interface::Root* >( pChild->getContext() ) )
    {
        switch( pRoot->getRootType() )
        {
            case eInterfaceRoot :
            case eFile          :
            case eFileRoot      :
            case eProjectName   :
                break;
            case eMegaRoot      :
            case eCoordinator   :
            case eHostName      :
            case eSubFolder     :
                bGenerate = false;
                break;
            default:
                THROW_RTE( "Unknown root type" );
        }
    }
    else if( const interface::Root* pRoot = dynamic_cast< const interface::Root* >( pParent->getContext() ) )
    {
        switch( pRoot->getRootType() )
        {
            case eInterfaceRoot :
            case eFile          :
            case eFileRoot      :
            case eProjectName   :
                break;
            case eMegaRoot      :
            case eCoordinator   :
            case eHostName      :
            case eSubFolder     :
                bGenerate = false;
                break;
            default:
                THROW_RTE( "Unknown root type" );
        }
    }
    
    if( !bGenerate )
    {
        concrete::NothingAllocator* pAllocator = stage.construct< concrete::NothingAllocator >();
        pAllocator->m_pContext_Allocating = pParent;
        pAllocator->m_pContext_Allocated = pChild;
        pResult = pAllocator;
    }
    else
    {
        if( pChild->getLocalDomainSize() == 1U )
        {
            SingletonAllocator* pAllocator = stage.construct< concrete::SingletonAllocator >();
            pAllocator->m_pContext_Allocating = pParent;
            pAllocator->m_pContext_Allocated = pChild;
            pResult = pAllocator;
        }
        else 
        {
            RangeAllocator* pAllocator = stage.construct< concrete::RangeAllocator >();
            pAllocator->m_pContext_Allocating = pParent;
            pAllocator->m_pContext_Allocated = pChild;
            pResult = pAllocator;
        }
    }
    
    return pResult;
}

}//namespace concrete
}//namespace eg