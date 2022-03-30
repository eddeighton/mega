//  Copyright (c) Deighton Systems Limited. 2019. All Rights Reserved.
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

#ifndef LAYOUT_18_02_2019
#define LAYOUT_18_02_2019

#include "concrete.hpp"

namespace mega
{
class AnalysisStage;
class Buffer;
class Layout;

namespace Stages
{
    class Implementation;
}

class DataMember : public io::Object
{
    friend class io::Factory;
    friend class Stages::Implementation;

public:
    static const ObjectType Type = eDataMember;

protected:
    DataMember( const io::ObjectInfo& object )
        : io::Object( object )
    {
    }

public:
    virtual void load( io::Loader& loader );
    virtual void store( io::Storer& storer ) const;

    const concrete::Dimension* getInstanceDimension() const { return m_pDimension; }

    const std::string& getName() const { return m_name; }
    const Buffer*      getBuffer() const { return m_pBuffer; }

private:
    const concrete::Dimension* m_pDimension;
    std::string                m_name;
    Buffer*                    m_pBuffer;
};

class Buffer : public io::Object
{
    friend class io::Factory;
    friend class Stages::Implementation;

public:
    static const ObjectType Type = eBuffer;

protected:
    Buffer( const io::ObjectInfo& object )
        : io::Object( object )
    {
    }

public:
    virtual void load( io::Loader& loader );
    virtual void store( io::Storer& storer ) const;

    std::size_t             getSize() const { return m_size; }
    const std::string&      getTypeName() const { return m_name; }
    const std::string&      getVariableName() const { return m_variable; }
    bool                    isSimple() const { return m_simple; }
    const concrete::Action* getAction() const { return m_pContext; }

    const std::vector< const DataMember* >& getDataMembers() const { return m_dataMembers; }

private:
    std::size_t             m_size;
    std::string             m_name, m_variable;
    bool                    m_simple;
    const concrete::Action* m_pContext = nullptr;

    std::vector< const DataMember* > m_dataMembers;
};

class Layout : public io::Object
{
    friend class io::Factory;
    friend class Stages::Implementation;

public:
    static const ObjectType Type = eLayout;

protected:
    Layout( const io::ObjectInfo& object )
        : io::Object( object )
    {
    }

private:
    using DimensionMap = std::map< const concrete::Dimension*, DataMember*, io::CompareIndexedObjects >;
    using DimensionMapCst = std::map< const concrete::Dimension*, const DataMember*, io::CompareIndexedObjects >;

public:
    virtual void load( io::Loader& loader );
    virtual void store( io::Storer& storer ) const;

    const std::vector< Buffer* > getBuffers() const { return m_buffers; }

    inline const DataMember* getDataMember( const concrete::Dimension* pDimension ) const
    {
        DimensionMapCst::const_iterator iFind = m_dimensionMap.find( pDimension );
        VERIFY_RTE( iFind != m_dimensionMap.end() );
        return iFind->second;
    }

private:
    std::vector< Buffer* > m_buffers;
    DimensionMapCst        m_dimensionMap;
};

} // namespace mega

#endif // LAYOUT_18_02_2019