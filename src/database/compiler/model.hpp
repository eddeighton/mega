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

#ifndef DATABASE_COMPILER_MODEL_4_APRIL_2022
#define DATABASE_COMPILER_MODEL_4_APRIL_2022

#include "grammar.hpp"

#include "mega/native_types.hpp"

#include "common/assert_verify.hpp"
#include "common/hash.hpp"

#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <map>
#include <set>

namespace db::model
{
using Counter = mega::U64;

class CountedObject
{
public:
    CountedObject( Counter& szCounter )
        : m_szIndex( szCounter++ )
    {
    }
    virtual ~CountedObject() {}
    inline Counter getCounter() const { return m_szIndex; }

private:
    Counter m_szIndex;
};

template < typename T >
class CountedObjectComparator
{
public:
    bool operator()( T left, T right ) const { return left->getCounter() < right->getCounter(); }
};

template < typename T >
class CountedObjectEquality
{
public:
    bool operator()( T left, T right ) const { return left->getCounter() == right->getCounter(); }
};

template < typename T >
class CountedObjectComparatorWeak
{
public:
    bool operator()( T left, T right ) const { return left.lock()->getCounter() < right.lock()->getCounter(); }
};

///////////////////////////////////////////////////
///////////////////////////////////////////////////
// base classes
class Type : public CountedObject
{
public:
    using Ptr = std::shared_ptr< Type >;

    bool m_bLate = false;

    Type( Counter& szCounter )
        : CountedObject( szCounter )
    {
    }

    void setLate() { m_bLate = true; }

    virtual std::string getViewType( const std::string& strStageNamespace, bool bAsArg ) const = 0;

    enum DatabaseTypeFormat
    {
        eNormal,
        eNormal_NoLate,
        eAsArgument
    };
    virtual std::string getDatabaseType( DatabaseTypeFormat ) const = 0;
    virtual bool        isCtorParam() const                         = 0;
    virtual bool        isGet() const                               = 0;
    virtual bool        isSet() const                               = 0;
    virtual bool        isInsert() const                            = 0;
};

class File;
class ObjectPart;
class Object;
class Namespace;
class Stage;

class Property : public CountedObject
{
public:
    using Ptr = std::shared_ptr< Property >;

    Property( Counter& szCounter )
        : CountedObject( szCounter )
    {
    }

    bool isCtorParam() const { return !( m_type->m_bLate || !m_type->isCtorParam() ); }
    bool isGet() const { return m_type->isGet(); }
    bool isSet() const { return m_type->isSet(); }
    bool isInsert() const { return m_type->isInsert(); }

    std::weak_ptr< ObjectPart > m_objectPart;

    std::string m_strName;
    Type::Ptr   m_type;
};

class ObjectPart : public CountedObject
{
public:
    using Ptr = std::shared_ptr< ObjectPart >;

    ObjectPart( Counter& szCounter )
        : CountedObject( szCounter )
    {
    }

    std::weak_ptr< Object >      m_object;
    std::weak_ptr< File >        m_file;
    std::vector< Property::Ptr > m_properties;
    mega::U64                    m_typeID;

    std::string getDataType( const std::string& strDelimiter ) const;
    std::string getPointerName() const;
};

class PrimaryObjectPart : public ObjectPart
{
public:
    PrimaryObjectPart( Counter& szCounter )
        : ObjectPart( szCounter )
    {
    }
    using Ptr = std::shared_ptr< PrimaryObjectPart >;
};

class SecondaryObjectPart : public ObjectPart
{
public:
    SecondaryObjectPart( Counter& szCounter )
        : ObjectPart( szCounter )
    {
    }
    using Ptr = std::shared_ptr< SecondaryObjectPart >;
};

class InheritedObjectPart : public SecondaryObjectPart
{
public:
    InheritedObjectPart( Counter& szCounter )
        : SecondaryObjectPart( szCounter )
    {
    }
    using Ptr = std::shared_ptr< InheritedObjectPart >;
};

class AggregatedObjectPart : public SecondaryObjectPart
{
public:
    AggregatedObjectPart( Counter& szCounter )
        : SecondaryObjectPart( szCounter )
    {
    }
    using Ptr = std::shared_ptr< AggregatedObjectPart >;
};

class Object : public CountedObject
{
public:
    Object( Counter& szCounter, std::string strIdentifier )
        : CountedObject( szCounter )
        , m_strName( std::move( strIdentifier ) )
    {
    }
    using Ptr     = std::shared_ptr< Object >;
    using WeakPtr = std::weak_ptr< Object >;

    using WeakObjectPtrSet    = std::set< WeakPtr, CountedObjectComparatorWeak< Object::WeakPtr > >;
    using WeakObjectPtrSetPtr = std::shared_ptr< WeakObjectPtrSet >;
    WeakObjectPtrSetPtr m_pInheritanceGroup;

    std::string inheritanceGroupVariant( std::shared_ptr< Stage > pStage ) const;
    std::string delimitTypeName( const std::string& str ) const;

    std::weak_ptr< Namespace > m_namespace;

    // primary object part for the object in its stage
    std::vector< PrimaryObjectPart::Ptr >   m_primaryObjectParts;
    std::vector< SecondaryObjectPart::Ptr > m_secondaryParts;

    PrimaryObjectPart::Ptr getPrimaryObjectPart( std::shared_ptr< Stage > pStage );

    Ptr                                    m_base;
    std::vector< std::weak_ptr< Object > > m_deriving;

    const std::string& getIdentifier() const { return m_strName; }
    std::string        getDataTypeName() const;

private:
    std::string m_strName;
};

class Namespace : public CountedObject
{
public:
    Namespace( Counter& szCounter )
        : CountedObject( szCounter )
    {
    }
    using Ptr = std::shared_ptr< Namespace >;

    std::weak_ptr< Namespace > m_namespace;

    std::string                   m_strName;
    std::string                   m_strFullName;
    std::vector< Object::Ptr >    m_objects;
    std::vector< Namespace::Ptr > m_namespaces;
};

class File : public CountedObject
{
public:
    File( Counter& szCounter )
        : CountedObject( szCounter )
    {
    }
    using Ptr = std::shared_ptr< File >;

    std::weak_ptr< Stage > m_stage;

    std::string                    m_strName;
    std::vector< ObjectPart::Ptr > m_parts;
    std::vector< File::Ptr >       m_dependencies;
};
class Interface;
class Function : public CountedObject
{
public:
    using Ptr = std::shared_ptr< Function >;
    Function( Counter& szCounter )
        : CountedObject( szCounter )
    {
    }
    Property::Ptr              m_property;
    std::weak_ptr< Interface > m_interface;

    struct Param
    {
        std::string type, name;
    };
    using ParamVector = std::vector< Param >;

    virtual std::string getShortName() const                                        = 0;
    virtual std::string getLongName() const                                         = 0;
    virtual std::string getReturnType( const std::string& strStageNamespace ) const = 0;
    virtual ParamVector getParams( const std::string& strStageNamespace ) const     = 0;

    inline std::string getMangledName( const std::string& strStageNamespace ) const
    {
        std::ostringstream os;
        os << getReturnType( strStageNamespace ) << getShortName();
        for( const auto& param : getParams( strStageNamespace ) )
            os << param.type << param.name;
        return os.str();
    }
};
class FunctionGetter : public Function
{
public:
    using Ptr = std::shared_ptr< FunctionGetter >;
    FunctionGetter( Counter& szCounter )
        : Function( szCounter )
    {
    }
    virtual std::string getShortName() const;
    virtual std::string getLongName() const;
    virtual std::string getReturnType( const std::string& strStageNamespace ) const
    {
        std::ostringstream os;
        os << m_property->m_type->getViewType( strStageNamespace, true );
        return os.str();
    }
    virtual ParamVector getParams( const std::string& ) const { return ParamVector{}; }
};
class FunctionSetter : public Function
{
public:
    using Ptr = std::shared_ptr< FunctionSetter >;
    FunctionSetter( Counter& szCounter )
        : Function( szCounter )
    {
    }
    virtual std::string getShortName() const;
    virtual std::string getLongName() const;
    virtual std::string getReturnType( const std::string& ) const { return "void"; }
    virtual ParamVector getParams( const std::string& strStageNamespace ) const
    {
        using namespace std::string_literals;
        return ParamVector{ Param{ m_property->m_type->getViewType( strStageNamespace, true ), "value"s } };
    }
};
class FunctionInserter : public Function
{
public:
    using Ptr = std::shared_ptr< FunctionInserter >;
    FunctionInserter( Counter& szCounter )
        : Function( szCounter )
    {
    }
    virtual std::string getShortName() const;
    virtual std::string getLongName() const;
    virtual std::string getReturnType( const std::string& ) const { return "void"; }
    virtual ParamVector getParams( const std::string& strStageNamespace ) const;
};

class SuperType;
class Interface : public CountedObject
{
public:
    using Ptr = std::shared_ptr< Interface >;
    Interface( Counter& szCounter )
        : CountedObject( szCounter )
    {
    }
    std::weak_ptr< SuperType >     m_superInterface;
    std::weak_ptr< Object >        m_object;
    std::vector< Function::Ptr >   m_functions;
    std::vector< Property::Ptr >   m_args;
    Interface::Ptr                 m_base;
    std::vector< ObjectPart::Ptr > m_readOnlyObjectParts;
    std::vector< ObjectPart::Ptr > m_readWriteObjectParts;
    bool                           m_isReadWrite;

    std::string delimitTypeName( const std::string& strStageNamespace, const std::string& str ) const;

    std::vector< PrimaryObjectPart::Ptr > getPrimaryObjectParts() const;
    PrimaryObjectPart::Ptr                getPrimaryObjectPart( std::shared_ptr< Stage > pStage ) const;
    bool                                  ownsPrimaryObjectPart( std::shared_ptr< Stage > pStage ) const;
    bool                                  ownsPrimaryObjectPart( PrimaryObjectPart::Ptr pPrimaryObjectPart ) const;
    bool                                  ownsInheritedSecondaryObjectPart() const;
};

class SuperType : public CountedObject
{
public:
    SuperType( Counter& szCounter, std::string strTypeName )
        : CountedObject( szCounter )
        , m_strTypeName( std::move( strTypeName ) )
    {
    }
    using Ptr = std::shared_ptr< SuperType >;

    Object::Ptr m_base_object;

    std::weak_ptr< Stage >        m_stage;
    std::vector< Interface::Ptr > m_interfaces;

    using FunctionMultiMap = std::multimap< std::string, Function::Ptr >;
    FunctionMultiMap m_functions;

    const std::string& getTypeName() const { return m_strTypeName; }

private:
    std::string m_strTypeName;
};

class StageFunction : public CountedObject
{
public:
    StageFunction( Counter& szCounter )
        : CountedObject( szCounter )
    {
    }
    using Ptr = std::shared_ptr< StageFunction >;

    std::weak_ptr< Stage > m_stage;
};

class Accessor : public StageFunction
{
public:
    Accessor( Counter& szCounter )
        : StageFunction( szCounter )
    {
    }
    using Ptr = std::shared_ptr< Accessor >;
    bool                   m_bPerSource;
    std::weak_ptr< Stage > m_stage;
    Type::Ptr              m_type;
};

class Constructor : public StageFunction
{
public:
    Constructor( Counter& szCounter )
        : StageFunction( szCounter )
    {
    }
    using Ptr = std::shared_ptr< Constructor >;
    std::weak_ptr< Stage > m_stage;
    Interface::Ptr         m_interface;
};

class Stage;
class Source : public CountedObject
{
public:
    Source( Counter& szCounter )
        : CountedObject( szCounter )
    {
    }
    using Ptr = std::shared_ptr< Source >;
    std::string                           m_strName;
    std::vector< std::weak_ptr< Stage > > m_stages;
};

class Stage : public CountedObject, public std::enable_shared_from_this< Stage >
{
public:
    Stage( Counter& szCounter )
        : CountedObject( szCounter )
    {
    }
    using Ptr     = std::shared_ptr< Stage >;
    using WeakPtr = std::weak_ptr< Stage >;

    std::vector< std::string > m_dependencyNames;
    std::vector< WeakPtr >     m_dependencies;

    std::string                m_strName;
    std::vector< Source::Ptr > m_sources;
    std::vector< File::Ptr >   m_files;

    std::vector< Accessor::Ptr >    m_accessors;
    std::vector< Constructor::Ptr > m_constructors;

    std::vector< Interface::Ptr >                                         m_interfaceTopological;
    std::set< Interface::Ptr, CountedObjectComparator< Interface::Ptr > > m_interfaceTopologicalSet;
    std::vector< Interface::Ptr >                                         m_readOnlyInterfaces;
    std::vector< Interface::Ptr >                                         m_readWriteInterfaces;
    std::vector< SuperType::Ptr >                                         m_superTypes;

    void getDependencies( std::vector< Ptr >& dependencies )
    {
        for( WeakPtr p : m_dependencies )
        {
            p.lock()->getDependencies( dependencies );
        }
        Ptr pThis = shared_from_this();
        if( std::find( dependencies.begin(), dependencies.end(), pThis ) == dependencies.end() )
            dependencies.push_back( pThis );
    }

    // determine if pStage IS a dependency of this
    bool isDependency( Stage::Ptr pStage ) const
    {
        if( shared_from_this() == pStage )
            return true;
        for( WeakPtr p : m_dependencies )
        {
            if( p.lock()->isDependency( pStage ) )
                return true;
        }
        return false;
    }

    Interface::Ptr isInterface( Object::Ptr pObject ) const
    {
        for( Interface::Ptr pInterface : m_interfaceTopological )
        {
            if( pInterface->m_object.lock() == pObject )
                return pInterface;
        }
        return {};
    }
    Interface::Ptr getInterface( Object::Ptr pObject ) const
    {
        if( Interface::Ptr pInterface = isInterface( pObject ) )
            return pInterface;
        THROW_RTE( "Failed to locate interface for object: " << pObject->delimitTypeName( "::" )
                                                             << " in stage: " << m_strName );
    }
};

class Schema : public CountedObject
{
public:
    Schema( Counter& szCounter, common::Hash schemaHash )
        : CountedObject( szCounter )
        , m_schemaHash( schemaHash )
    {
    }
    using Ptr = std::shared_ptr< Schema >;

    common::Hash m_schemaHash;

    std::vector< Namespace::Ptr > m_namespaces;
    std::vector< Stage::Ptr >     m_stages;
    std::vector< Source::Ptr >    m_sources;

    template < typename T >
    class CountedObjectPairComparator
    {
    public:
        bool operator()( T left, T right ) const
        {
            return ( left.first->getCounter() != right.first->getCounter() )
                       ? ( left.first->getCounter() < right.first->getCounter() )
                       : ( left.second->getCounter() < right.second->getCounter() );
        }
    };

    using ObjectPartPair   = std::pair< ObjectPart::Ptr, ObjectPart::Ptr >;
    using ObjectPartVector = std::vector< ObjectPart::Ptr >;
    using ConversionMap = std::map< ObjectPartPair, ObjectPartVector, CountedObjectPairComparator< ObjectPartPair > >;

    ConversionMap m_conversions;
    ConversionMap m_base_conversions;
};

inline std::string toConstRef( const std::string& strType )
{
    std::ostringstream os;
    os << "const " << strType << "&";
    return os.str();
}

inline std::string toOptional( const std::string& strType )
{
    std::ostringstream os;
    os << "std::optional< " << strType << " >";
    return os.str();
}

///////////////////////////////////////////////////
///////////////////////////////////////////////////
// types
class ValueType : public Type
{
public:
    ValueType( Counter& szCounter )
        : Type( szCounter )
    {
    }
    using Ptr = std::shared_ptr< ValueType >;
    std::string m_cppType;

    virtual std::string getViewType( const std::string& /*strStageNamespace*/, bool bAsArg ) const
    {
        return bAsArg ? toConstRef( m_cppType ) : m_cppType;
    }
    virtual std::string getDatabaseType( DatabaseTypeFormat formatType ) const
    {
        switch( formatType )
        {
            case eNormal:
                return m_bLate ? toOptional( m_cppType ) : m_cppType;
            case eNormal_NoLate:
                return m_cppType;
            case eAsArgument:
                return toConstRef( m_cppType );
        }
        THROW_RTE( "Unknown format type" );
        return "";
    }
    virtual bool isCtorParam() const { return true; }
    virtual bool isGet() const { return true; }
    virtual bool isSet() const { return true; }
    virtual bool isInsert() const { return false; }
};

class RefType : public Type
{
public:
    RefType( Counter& szCounter )
        : Type( szCounter )
    {
    }
    using Ptr = std::shared_ptr< RefType >;
    Object::Ptr m_object;

    virtual std::string getViewType( const std::string& strStageNamespace, bool /*bAsArg*/ ) const
    {
        VERIFY_RTE( m_object );
        std::ostringstream os;
        os << "::" << strStageNamespace << "::" << m_object->m_namespace.lock()->m_strFullName
           << "::" << m_object->getIdentifier() << "*";
        return os.str();
    }
    virtual std::string getDatabaseType( DatabaseTypeFormat formatType ) const
    {
        VERIFY_RTE( m_object );

        VERIFY_RTE_MSG( m_object->m_primaryObjectParts.size() == 1, "Ambiguous primary object part" );
        auto pFile = m_object->m_primaryObjectParts.back()->m_file.lock();

        std::ostringstream os;
        os << "data::Ptr< data::" << pFile->m_strName << "::" << m_object->getDataTypeName() << " >";

        switch( formatType )
        {
            case eNormal:
                return m_bLate ? toOptional( os.str() ) : os.str();
            case eNormal_NoLate:
                return os.str();
            case eAsArgument:
                return toConstRef( os.str() );
        }
        THROW_RTE( "Unknown format type" );
        return "";
    }
    virtual bool isCtorParam() const { return true; }
    virtual bool isGet() const { return true; }
    virtual bool isSet() const { return true; }
    virtual bool isInsert() const { return false; }
};

class OptType : public Type
{
public:
    OptType( Counter& szCounter )
        : Type( szCounter )
    {
    }
    using Ptr = std::shared_ptr< OptType >;
    Type::Ptr m_underlyingType;

    virtual std::string getViewType( const std::string& strStageNamespace, bool bAsArg ) const
    {
        VERIFY_RTE( m_underlyingType );
        std::ostringstream os;
        os << "std::optional< " << m_underlyingType->getViewType( strStageNamespace, false ) << " >";

        if( std::dynamic_pointer_cast< RefType >( m_underlyingType ) )
        {
            return os.str();
        }
        else
        {
            return bAsArg ? toConstRef( os.str() ) : os.str();
        }
    }
    virtual std::string getDatabaseType( DatabaseTypeFormat formatType ) const
    {
        VERIFY_RTE( m_underlyingType );
        std::ostringstream os;
        os << "std::optional< " << m_underlyingType->getDatabaseType( eNormal ) << " >";

        switch( formatType )
        {
            case eNormal:
                return m_bLate ? toOptional( os.str() ) : os.str();
            case eNormal_NoLate:
                return os.str();
            case eAsArgument:
                return toConstRef( os.str() );
        }
        THROW_RTE( "Unknown format type" );
        return "";
    }
    virtual bool isCtorParam() const { return true; }
    virtual bool isGet() const { return true; }
    virtual bool isSet() const { return true; }
    virtual bool isInsert() const { return false; }
};

class ArrayType : public Type
{
public:
    ArrayType( Counter& szCounter )
        : Type( szCounter )
    {
    }
    using Ptr = std::shared_ptr< ArrayType >;
    Type::Ptr m_underlyingType;

    virtual std::string getViewType( const std::string& strStageNamespace, bool bAsArg ) const
    {
        VERIFY_RTE( m_underlyingType );
        std::ostringstream os;
        os << "std::vector< " << m_underlyingType->getViewType( strStageNamespace, false ) << " >";

        if( std::dynamic_pointer_cast< RefType >( m_underlyingType ) )
        {
            return os.str();
        }
        else
        {
            return bAsArg ? toConstRef( os.str() ) : os.str();
        }
    }
    virtual std::string getDatabaseType( DatabaseTypeFormat formatType ) const
    {
        VERIFY_RTE( m_underlyingType );
        std::ostringstream os;
        os << "std::vector< " << m_underlyingType->getDatabaseType( eNormal ) << " >";

        switch( formatType )
        {
            case eNormal:
                return m_bLate ? toOptional( os.str() ) : os.str();
            case eNormal_NoLate:
                return os.str();
            case eAsArgument:
                return toConstRef( os.str() );
        }
        THROW_RTE( "Unknown format type" );
        return "";
    }
    virtual bool isCtorParam() const { return true; }
    virtual bool isGet() const { return true; }
    virtual bool isSet() const { return true; }
    virtual bool isInsert() const { return true; }
};

class MapType : public Type
{
    bool m_bIsMultiMap = false;

public:
    MapType( Counter& szCounter, bool bIsMultiMap )
        : Type( szCounter )
        , m_bIsMultiMap( bIsMultiMap )
    {
    }
    using Ptr = std::shared_ptr< MapType >;
    Type::Ptr m_fromType;
    Type::Ptr m_toType;
    Type::Ptr m_predicate;

    virtual std::string getViewType( const std::string& strStageNamespace, bool bAsArg ) const
    {
        VERIFY_RTE( m_fromType );
        VERIFY_RTE( m_toType );
        std::ostringstream os;

        if( m_bIsMultiMap )
        {
            os << "std::multimap< " << m_fromType->getViewType( strStageNamespace, false ) << ", "
               << m_toType->getViewType( strStageNamespace, false ) << " >";
        }
        else
        {
            os << "std::map< " << m_fromType->getViewType( strStageNamespace, false ) << ", "
               << m_toType->getViewType( strStageNamespace, false ) << " >";
        }

        if( std::dynamic_pointer_cast< RefType >( m_fromType ) || std::dynamic_pointer_cast< RefType >( m_toType ) )
        {
            return os.str();
        }
        else
        {
            return bAsArg ? toConstRef( os.str() ) : os.str();
        }
    }
    virtual std::string getDatabaseType( DatabaseTypeFormat formatType ) const
    {
        VERIFY_RTE( m_fromType );
        VERIFY_RTE( m_toType );
        std::ostringstream os;

        if( m_bIsMultiMap )
        {
            os << "std::multimap< " << m_fromType->getDatabaseType( eNormal ) << ", "
               << m_toType->getDatabaseType( eNormal ) << " >";
        }
        else
        {
            os << "std::map< " << m_fromType->getDatabaseType( eNormal ) << ", " << m_toType->getDatabaseType( eNormal )
               << " >";
        }

        switch( formatType )
        {
            case eNormal:
                return m_bLate ? toOptional( os.str() ) : os.str();
            case eNormal_NoLate:
                return os.str();
            case eAsArgument:
                return toConstRef( os.str() );
        }
        THROW_RTE( "Unknown format type" );
        return "";
    }
    virtual bool isCtorParam() const { return true; }
    virtual bool isGet() const { return true; }
    virtual bool isSet() const { return true; }
    virtual bool isInsert() const { return true; }
};

Schema::Ptr from_ast( const ::db::schema::Schema& schema );

} // namespace db::model

#endif // DATABASE_COMPILER_MODEL_4_APRIL_2022
