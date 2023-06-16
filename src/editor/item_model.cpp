
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

#include "item_model.hpp"

#include <QDebug>
#include <QColor>
#include <QBrush>

#ifndef Q_MOC_RUN

#include "common/assert_verify.hpp"
#include "common/stl.hpp"
#include "common/compose.hpp"

#include <algorithm>

#endif

namespace editor
{

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void doTreeItemUpdate( ItemModel::TreeItem* pRootItem, ItemModel& model )
{
    // LOG_PROFILE_BEGIN( tree_update );
    ASSERT( pRootItem );
    pRootItem->update( model );
    // LOG_PROFILE_END( tree_update );
}
ItemModel::TreeItem* createRootTreeItem( schematic::IEditContext*& pEditContext, schematic::Node::PtrCst pNode )
{
    return new ItemModel::TreeItem( pEditContext, pNode );
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
ItemModel::ItemModel( QObject* parent )
    : QAbstractItemModel( parent )
{
}

QModelIndex ItemModel::getNodeIndex( schematic::Node::PtrCst pNode ) const
{
    QModelIndex result;
    if( pNode )
    {
        if( m_pRoot.get() )
        {
            if( schematic::Node::PtrCst pParent = pNode->getParent() )
                result = index( pNode->getIndex(), 0, getNodeIndex( pParent ) );
            else
                result = QModelIndex();
        }
    }
    return result;
}

schematic::Node::PtrCst ItemModel::getIndexNode( const QModelIndex& index ) const
{
    schematic::Node::PtrCst pNode;
    if( TreeItem* pItem = index.isValid() ? static_cast< TreeItem* >( index.internalPointer() ) : m_pRoot.get() )
        pNode = pItem->getNode();
    return pNode;
}

void ItemModel::OnSchematicUpdate( schematic::IEditContext* pNewContext )
{
    m_pEditContext = pNewContext;
    if( m_pRoot.get() )
    {
        doTreeItemUpdate( m_pRoot.get(), *this );
    }
}

void ItemModel::OnSchematicFocussed( schematic::Schematic::Ptr pSchematic )
{
    if( m_pRoot.get() )
    {
        beginRemoveRows( QModelIndex(), 0, m_pRoot->rowCount() );
        m_pRoot.reset();
        endRemoveRows();
    }

    m_pNode = pSchematic;

    if( pSchematic )
    {
        m_pRoot.reset( createRootTreeItem( m_pEditContext, pSchematic ) );
        doTreeItemUpdate( m_pRoot.get(), *this );
    }
}

Qt::ItemFlags ItemModel::flags( const QModelIndex& index ) const
{
    // qDebug() << "ItemModel::flags " << index;
    if( !index.isValid() )
        return {};

    if( schematic::Property::PtrCst pProperty
        = boost::dynamic_pointer_cast< const schematic::Property >( getIndexNode( index ) ) )
    {
        return Qt::ItemIsEditable | QAbstractItemModel::flags( index );
    }

    return QAbstractItemModel::flags( index );
}
bool ItemModel::setData( const QModelIndex& modelIndex, const QVariant& value, int role )
{
    if( modelIndex.isValid() )
    {
        if( role == Qt::EditRole )
        {
            if( schematic::Property::PtrCst pPropertyCst
                = boost::dynamic_pointer_cast< const schematic::Property >( getIndexNode( modelIndex ) ) )
            {
                if( m_pEditContext )
                {
                    m_pEditContext->setProperty( pPropertyCst, value.toString().toStdString() );
                    emit dataChanged( index( modelIndex.row(), 0, modelIndex ), index( modelIndex.row(), 2, modelIndex ) );
                    return true;
                }
            }
        }
    }
    return false;
}

bool ItemModel::setItemData( const QModelIndex& modelIndex, const QMap< int, QVariant >& roles )
{
    bool bSet = false;
    if( modelIndex.isValid() )
    {
        auto iFind = roles.find( Qt::EditRole );
        if( iFind != roles.end() )
        {
            const auto& value = *iFind;
            if( schematic::Property::PtrCst pPropertyCst
                = boost::dynamic_pointer_cast< const schematic::Property >( getIndexNode( modelIndex ) ) )
            {
                if( m_pEditContext )
                {
                    m_pEditContext->setProperty( pPropertyCst, value.toString().toStdString() );
                    emit dataChanged( index( modelIndex.row(), 0, modelIndex ), index( modelIndex.row(), 2, modelIndex ) );
                    return true;
                }
            }
        }
    }
    return bSet;
}

QModelIndex ItemModel::index( int row, int column, const QModelIndex& parentIndex ) const
{
    if( TreeItem* pParentItem
        = parentIndex.isValid() ? static_cast< TreeItem* >( parentIndex.internalPointer() ) : m_pRoot.get() )
    {
        if( TreeItem* pChildItem = pParentItem->getChildAt( row ) )
            return createIndex( row, column, pChildItem );
    }
    return {};
}

QModelIndex ItemModel::parent( const QModelIndex& childIndex ) const
{
    // qDebug() << "ItemModel::parent " << childIndex;
    if( !childIndex.isValid() )
        return {};

    if( TreeItem* pChildItem = static_cast< TreeItem* >( childIndex.internalPointer() ) )
    {
        if( TreeItem* pParentItem = pChildItem->getParent() )
        {
            if( pParentItem == m_pRoot.get() )
                return {};
            return createIndex( pParentItem->getRowNumber(), 0, pParentItem );
        }
    }
    return {};
}

int ItemModel::rowCount( const QModelIndex& parent ) const
{
    // qDebug() << "ItemModel::rowCount " << parent;
    if( parent.column() > 0 )
        return {};

    if( TreeItem* parentItem = parent.isValid() ? static_cast< TreeItem* >( parent.internalPointer() ) : m_pRoot.get() )
        return parentItem->rowCount();
    else
        return {};
}

int ItemModel::columnCount( const QModelIndex& parent ) const
{
    // qDebug() << "ItemModel::columnCount " << parent;
    if( parent.isValid() )
    {
        if( TreeItem* pItem = static_cast< TreeItem* >( parent.internalPointer() ) )
            return pItem->columnCount();
    }
    return m_pRoot.get() ? m_pRoot->columnCount() : 0;
}

QVariant ItemModel::data( const QModelIndex& index, int role ) const
{
    // qDebug() << "ItemModel::data " << index << " " << role;
    if( !index.isValid() )
        return {};

    if( ( role != Qt::DisplayRole ) ) //&& ( role != Qt::ForegroundRole ) )
        return {};

    if( TreeItem* pItem = static_cast< TreeItem* >( index.internalPointer() ) )
    {
        if( role == Qt::DisplayRole )
        {
            return pItem->getData( index.column() );
        }
        // THIS does not work??
        /*else if( role == Qt::ForegroundRole )
        {
            if( m_pEditContext && m_pEditContext->isNodeContext( m_pNode.lock() ) )
            {
                return QBrush( QColor( 0, 0, 0, 255 ) );
            }
            else
            {
                return QBrush( QColor( 0, 0, 0, 255 ) );
            }
        }*/
    }
    else
    {
        return {};
    }
}

QVariant ItemModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
    // qDebug() << "ItemModel::headerData " << section << " " << orientation << " " << role;
    if( orientation == Qt::Horizontal && role == Qt::DisplayRole )
    {
        switch( section )
        {
            case 0:
                return QVariant( "Item" );
            case 1:
                return QVariant( "Value" );
            default:
                return {};
        }
    }

    return {};
}

void ItemModel::TreeItem::update( ItemModel& model, const QModelIndex& parentModelIndex )
{
    struct CompareIters
    {
        bool operator()( PtrTreeItemMap::const_iterator i1, schematic::Node::PtrVector::const_iterator i2 ) const
        {
            return i1->first.lock() < *i2;
        }
        bool opposite( PtrTreeItemMap::const_iterator i1, schematic::Node::PtrVector::const_iterator i2 ) const
        {
            return *i2 < i1->first.lock();
        }
    };
    struct CompareItersUpdate
    {
        bool operator()( PtrTreeItemMap::const_iterator i1, schematic::Node::PtrVector::const_iterator i2 ) const
        {
            ASSERT( i1->first.lock() == *i2 );
            return i1->second->getTick() < ( *i2 )->getLastModifiedTick();
        }
    };

    schematic::Node::PtrCst pNode = m_pNode.lock();
    ASSERT( pNode );
    if( !pNode )
        return;
    m_lastUpdateTick.update();

    const QModelIndex modelIndex = m_pParent ? model.index( getRowNumber(), 0, parentModelIndex ) : QModelIndex();

    // calculate the required additions, removals and updates
    PtrTreeItemMap          removals;
    schematic::Node::PtrSet additions;
    Set                     updates;
    {
        schematic::Node::PtrVector nodes = pNode->getChildren();
        std::sort( nodes.begin(), nodes.end() );
        generics::matchGetUpdates(
            m_nodeMap.begin(), m_nodeMap.end(), nodes.begin(), nodes.end(), CompareIters(), CompareItersUpdate(),
            generics::collect( removals, generics::deref< PtrTreeItemMap::const_iterator >() ),
            generics::collect( additions, generics::deref< schematic::Node::PtrVector::const_iterator >() ),
            generics::collect( updates, generics::second< PtrTreeItemMap::const_iterator >() ) );
    }

    // remove all the removals
    for( PtrTreeItemMap::iterator i = removals.begin(), iEnd = removals.end(); i != iEnd; ++i )
    {
        // remove the treeitem
        TreeItem* pItem = i->second;
        m_nodeMap.erase( i->first );
        const int iRowNumber = pItem->getRowNumber();
        {
            model.beginRemoveRows( modelIndex, iRowNumber, iRowNumber );
            m_children.erase( m_children.begin() + iRowNumber );
            delete pItem;
            model.endRemoveRows();
        }
    }

    // process the additions, updates and moves
    int iRow = 0;
    for( schematic::Node::PtrVector::const_iterator j = pNode->getChildren().begin(), jEnd = pNode->getChildren().end();
         j != jEnd; ++j, ++iRow )
    {
        schematic::Node::Ptr pIter = *j;
        if( additions.find( pIter ) != additions.end() )
        {
            // node needs to be added at this position
            TreeItem* pNewTreeItem = new TreeItem( m_pEditContext, pIter, this );
            m_nodeMap.insert( std::make_pair( schematic::Node::PtrCstWeak( pIter ), pNewTreeItem ) );
            {
                model.beginInsertRows( modelIndex, iRow, iRow );
                m_children.insert( m_children.begin() + iRow, pNewTreeItem );
                model.endInsertRows();
            }
        }
        else
        {
            ASSERT( iRow < static_cast< int >( m_children.size() ) );
            TreeItem* pChildItem = m_children[ iRow ];
            // if the nodes match at this position
            if( pChildItem->m_pNode.lock() == pIter )
            {
                // then just test if need to update
                if( updates.find( pChildItem ) != updates.end() )
                {
                    // do the update
                    pChildItem->m_qName  = QVariant( pIter->getName().c_str() );
                    pChildItem->m_qValue = QVariant( pIter->getStatement().c_str() );
                    model.dataChanged( model.index( iRow, 0, modelIndex ), model.index( iRow, 2, modelIndex ) );
                }
            }
            else
            {
                // move the treeItem to this position
                PtrTreeItemMap::iterator iFind = m_nodeMap.find( pIter );
                ASSERT( iFind != m_nodeMap.end() );
                TreeItem* pSourceItem = iFind->second;
                ASSERT( pSourceItem->m_pNode.lock() == pIter );
                int iSource = pSourceItem->getRowNumber();
                ASSERT( iSource > iRow );
                {
                    model.beginMoveRows( modelIndex, iSource, iSource, modelIndex, iRow );
                    std::swap( *( m_children.begin() + iSource ), *( m_children.begin() + iRow ) );
                    pSourceItem->m_qName  = QVariant( pIter->getName().c_str() );
                    pSourceItem->m_qValue = QVariant( pIter->getStatement().c_str() );
                    ASSERT( m_children[ iRow ] == pSourceItem );
                    model.endMoveRows();
                }
            }
        }
    }

    ASSERT( m_children.size() == pNode->getChildren().size() );
    Vector::iterator c = m_children.begin();
    for( schematic::Node::PtrVector::const_iterator i = pNode->getChildren().begin(), iEnd = pNode->getChildren().end();
         i != iEnd; ++i, ++c )
    {
        ( *c )->update( model, modelIndex );
    }
}

} // namespace editor
