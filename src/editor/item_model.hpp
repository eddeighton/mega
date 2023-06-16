
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

#ifndef GUARD_2023_June_02_item_model
#define GUARD_2023_June_02_item_model

#include <QAbstractItemModel>
#include <QDebug>

#include <QAbstractItemModel>
#include <QDebug>

#ifndef Q_MOC_RUN

#include "schematic/node.hpp"
#include "schematic/schematic.hpp"
#include "schematic/editInteractions.hpp"

#include "common/tick.hpp"

#include <list>
#include <set>
#include <map>
#include <memory>

#endif

namespace editor
{

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
class ItemModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    //////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////
    struct TreeItem
    {
        friend void      doTreeItemUpdate( TreeItem*, ItemModel& );
        friend TreeItem* createRootTreeItem( schematic::IEditContext*& pEditContext, schematic::Node::PtrCst );

    private:
        using Vector         = std::vector< TreeItem* >;
        using Set            = std::set< TreeItem* >;
        using StringItemMap  = std::map< std::string, TreeItem* >;
        using PtrTreeItemMap = std::map< schematic::Node::PtrCstWeak, TreeItem* >;

        TreeItem( schematic::IEditContext*& pEditContext, schematic::Node::PtrCst pNode, TreeItem* pParent = nullptr )
            : m_pEditContext( pEditContext )
            , m_pNode( pNode )
            , m_pParent( pParent )
            , m_qName( QVariant( pNode->getName().c_str() ) )
            , m_qValue( QVariant( pNode->getStatement().c_str() ) )
        {
        }

    public:
        // public interface used by the ItemModel
        ~TreeItem() { generics::deleteAndClear( m_children ); }

        inline QVariant getData( int iColumn ) const
        {
            switch( iColumn )
            {
                case 0:
                    return m_qName;
                case 1:
                    return m_qValue;
            }
            return {};
        }
        inline TreeItem* getChildAt( int iRow ) const
        {
            return ( iRow < static_cast< int >( m_children.size() ) ) ? m_children[ iRow ] : nullptr;
        }
        inline TreeItem* getParent() const { return m_pParent; }
        inline int       columnCount() const { return 2; }
        inline int       rowCount() const { return m_children.size(); }
        inline int       getRowNumber() const
        {
            if( m_pParent )
            {
                const int iRowNum = std::find( m_pParent->m_children.begin(), m_pParent->m_children.end(), this )
                                    - m_pParent->m_children.begin();
                ASSERT( iRowNum < static_cast< int >( m_pParent->m_children.size() )
                        && m_pParent->m_children[ iRowNum ] == this );
                return iRowNum;
            }
            else
                return 0;
        }

        schematic::Node::PtrCst getNode() const { return m_pNode.lock(); }

    private:
        const Timing::UpdateTick& getTick() const { return m_lastUpdateTick; }

        friend class CompareItersUpdate;
        void update( ItemModel& model, const QModelIndex& parentModelIndex = QModelIndex() );

    private:
        schematic::IEditContext*& m_pEditContext;
        TreeItem*                 m_pParent;
        Vector                    m_children;
        QVariant                  m_qName, m_qValue;

        Timing::UpdateTick          m_lastUpdateTick;
        schematic::Node::PtrCstWeak m_pNode;
        PtrTreeItemMap              m_nodeMap;
    };

    //////////////////////////////////////////////////////////////////////////////
    explicit ItemModel( QObject* parent = nullptr );

    QModelIndex             getNodeIndex( schematic::Node::PtrCst pNode ) const;
    schematic::Node::PtrCst getIndexNode( const QModelIndex& index ) const;

    virtual Qt::ItemFlags flags( const QModelIndex& index ) const;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    virtual bool setItemData(const QModelIndex &index, const QMap<int, QVariant> &roles);
    virtual QModelIndex   index( int row, int column, const QModelIndex& parent = QModelIndex() ) const;
    virtual QModelIndex   parent( const QModelIndex& child ) const;
    virtual int           rowCount( const QModelIndex& parent = QModelIndex() ) const;
    virtual int           columnCount( const QModelIndex& parent = QModelIndex() ) const;
    virtual QVariant      data( const QModelIndex& index, int role = Qt::DisplayRole ) const;
    virtual QVariant      headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

    //////////////////////////////////////////////////////////////////////////////
    // Update routines
    void OnSchematicUpdate( schematic::IEditContext* pNewContext );
    void OnSchematicFocussed( schematic::Schematic::Ptr pSchematic );

private:
    schematic::IEditContext*    m_pEditContext = nullptr;
    schematic::Node::PtrCstWeak m_pNode;
    std::unique_ptr< TreeItem > m_pRoot;
};

} // namespace editor

#endif // GUARD_2023_June_02_item_model
