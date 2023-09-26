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

#include "model.hpp"
#include "grammar.hpp"

#include "common/string.hpp"
#include <common/hash.hpp>

#include <memory>
#include <vector>
#include <utility>
#include <map>
#include <algorithm>
#include <set>

namespace db
{
namespace model
{

std::string Object::delimitTypeName( const std::string& str ) const
{
    std::ostringstream os;
    {
        std::vector< Namespace::Ptr > namespaces;
        {
            Namespace::Ptr pIter = m_namespace.lock();
            while( pIter )
            {
                namespaces.push_back( pIter );
                pIter = pIter->m_namespace.lock();
            }
            std::reverse( namespaces.begin(), namespaces.end() );
        }
        for( Namespace::Ptr pNamespace : namespaces )
        {
            os << pNamespace->m_strName << str;
        }
        os << m_strName;
    }
    return os.str();
}

std::string Object::getDataTypeName() const
{
    return delimitTypeName( "_" );
}

std::string ObjectPart::getDataType( const std::string& strDelimiter ) const
{
    std::ostringstream osObjectPartType;
    {
        osObjectPartType << m_file.lock()->m_strName << strDelimiter << m_object.lock()->getDataTypeName();
    }
    return osObjectPartType.str();
}

std::string ObjectPart::getPointerName() const
{
    std::ostringstream osObjectPartType;
    {
        osObjectPartType << "p_" << getDataType( "_" );
    }
    return osObjectPartType.str();
}

std::string FunctionGetter::getShortName() const
{
    std::ostringstream os;
    os << "get_" << m_property->m_strName;
    return os.str();
}
std::string FunctionSetter::getShortName() const
{
    std::ostringstream os;
    os << "set_" << m_property->m_strName;
    return os.str();
}
std::string FunctionInserter::getShortName() const
{
    std::ostringstream os;
    model::Type::Ptr   pType = m_property->m_type;
    if( model::ArrayType::Ptr pArray = std::dynamic_pointer_cast< model::ArrayType >( pType ) )
    {
        os << "push_back_" << m_property->m_strName;
    }
    else if( model::MapType::Ptr pMap = std::dynamic_pointer_cast< model::MapType >( pType ) )
    {
        os << "insert_" << m_property->m_strName;
    }
    else
    {
        THROW_RTE( "Unsupported inserter type" );
    }

    return os.str();
}

std::string FunctionGetter::getLongName() const
{
    std::ostringstream os;
    os << "get_" << m_interface.lock()->delimitTypeName( "", "_" ) << "_" << m_property->m_strName;
    return os.str();
}
std::string FunctionSetter::getLongName() const
{
    std::ostringstream os;
    os << "set_" << m_interface.lock()->delimitTypeName( "", "_" ) << "_" << m_property->m_strName;
    return os.str();
}
std::string FunctionInserter::getLongName() const
{
    std::ostringstream os;
    model::Type::Ptr   pType = m_property->m_type;
    if( model::ArrayType::Ptr pArray = std::dynamic_pointer_cast< model::ArrayType >( pType ) )
    {
        os << m_interface.lock()->delimitTypeName( "", "_" ) << "_push_back_" << m_property->m_strName;
    }
    else if( model::MapType::Ptr pMap = std::dynamic_pointer_cast< model::MapType >( pType ) )
    {
        os << m_interface.lock()->delimitTypeName( "", "_" ) << "_insert_" << m_property->m_strName;
    }
    else
    {
        THROW_RTE( "Unsupported inserter type" );
    }

    return os.str();
}

Function::ParamVector FunctionInserter::getParams( const std::string& strStageNamespace ) const
{
    using namespace std::string_literals;
    Function::ParamVector parameters;

    model::Type::Ptr pType = m_property->m_type;
    if( model::ArrayType::Ptr pArray = std::dynamic_pointer_cast< model::ArrayType >( pType ) )
    {
        parameters.push_back(
            Function::Param{ pArray->m_underlyingType->getViewType( strStageNamespace, true ), "value"s } );
    }
    else if( model::MapType::Ptr pMap = std::dynamic_pointer_cast< model::MapType >( pType ) )
    {
        parameters.push_back( Function::Param{ pMap->m_fromType->getViewType( strStageNamespace, true ), "key"s } );
        parameters.push_back( Function::Param{ pMap->m_toType->getViewType( strStageNamespace, true ), "value"s } );
    }
    else
    {
        THROW_RTE( "Unsupported inserter type" );
    }

    return parameters;
}

std::string Interface::delimitTypeName( const std::string& strStageNamespace, const std::string& str ) const
{
    Object::Ptr        pObject = m_object.lock();
    std::ostringstream os;
    {
        if( !strStageNamespace.empty() )
            os << strStageNamespace << str;
        os << pObject->delimitTypeName( str );
    }
    return os.str();
}

std::string Object::inheritanceGroupVariant( model::Stage::Ptr pStage ) const
{
    std::ostringstream os;
    os << "data::Variant";

    /*
    os << "data::Variant< ";
    bool bFirst = true;
    for ( WeakPtr pObjectWeak : *m_pInheritanceGroup )
    {
        model::Object::Ptr pObject = pObjectWeak.lock();
        // determine if this object is visible in this stage
        if ( !pStage || pStage->isInterface( pObject ) )
        {
            for ( auto pPart : pObject->m_primaryObjectParts )
            {
                if ( bFirst )
                    bFirst = false;
                else
                    os << ", ";
                os << "data::Ptr< data::" << pPart->getDataType( "::" ) << " >";
            }
        }
    }
    os << " >";*/
    return os.str();
}
PrimaryObjectPart::Ptr Object::getPrimaryObjectPart( std::shared_ptr< Stage > pStage )
{
    VERIFY_RTE( !m_primaryObjectParts.empty() );
    if( m_primaryObjectParts.size() == 1U )
    {
        return m_primaryObjectParts.front();
    }

    //

    // gather the primary object parts with their associated stages
    // IFF they are a dependency of the passed stage
    std::map< PrimaryObjectPart::Ptr, std::shared_ptr< Stage > > partStages;
    for( const auto& p : m_primaryObjectParts )
    {
        auto pPrimaryObjectPartFileStage = p->m_file.lock()->m_stage.lock();
        if( pStage->isDependency( pPrimaryObjectPartFileStage ) )
        {
            partStages.insert( { p, pPrimaryObjectPartFileStage } );
        }
    }

    // determine if one of the primary object part stages has all others as a dependency
    std::vector< PrimaryObjectPart::Ptr > parts;
    for( auto [ pPart, pStage ] : partStages )
    {
        bool bHasOthersAsDependency = true;
        for( auto [ pPartOther, pStageOther ] : partStages )
        {
            if( pPart != pPartOther )
            {
                if( pStage == pStageOther )
                {
                    THROW_RTE( "Unreachable - multiple primary object parts for object in same stage" );
                }
                else if( !pStage->isDependency( pStageOther ) )
                {
                    bHasOthersAsDependency = false;
                    break;
                }
            }
        }
        if( bHasOthersAsDependency )
        {
            parts.push_back( pPart );
        }
    }

    VERIFY_RTE_MSG( !parts.empty(), "Failed to locate primary object part for object: "
                                        << delimitTypeName( "::" ) << " in stage: " << pStage->m_strName );
    VERIFY_RTE_MSG( parts.size() == 1U, "Duplicate primary object parts found for object: "
                                            << delimitTypeName( "::" ) << " in stage: " << pStage->m_strName );
    return parts.front();
}

std::vector< PrimaryObjectPart::Ptr > Interface::getPrimaryObjectParts() const
{
    return m_object.lock()->m_primaryObjectParts;
}

PrimaryObjectPart::Ptr Interface::getPrimaryObjectPart( std::shared_ptr< Stage > pStage ) const
{
    return m_object.lock()->getPrimaryObjectPart( pStage );
}

bool Interface::ownsPrimaryObjectPart( std::shared_ptr< Stage > pStage ) const
{
    auto pPrimary = getPrimaryObjectPart( pStage );
    return ownsPrimaryObjectPart( pPrimary );
}

bool Interface::ownsPrimaryObjectPart( PrimaryObjectPart::Ptr pPrimaryObjectPart ) const
{
    for( model::ObjectPart::Ptr pPart : m_readWriteObjectParts )
    {
        if( PrimaryObjectPart::Ptr p = std::dynamic_pointer_cast< PrimaryObjectPart >( pPart ) )
        {
            if( p == pPrimaryObjectPart )
            {
                return true;
            }
        }
    }
    return false;
}

bool Interface::ownsInheritedSecondaryObjectPart() const
{
    for( model::ObjectPart::Ptr pPart : m_readWriteObjectParts )
    {
        if( std::dynamic_pointer_cast< InheritedObjectPart >( pPart ) )
        {
            return true;
        }
    }
    return false;
}
namespace
{

class NameResolution
{
public:
    Namespace::Ptr         m_namespace;
    schema::IdentifierList id;
};

using TypeNameResMap        = std::map< RefType::Ptr, NameResolution >;
using InheritanceNameResMap = std::map< Object::Ptr, NameResolution >;

// every object can have at most a single object part in each file
using ObjectFilePair = std::pair< Object::Ptr, schema::IdentifierList >;
class ObjectFilePairComparator
{
public:
    bool operator()( const ObjectFilePair& left, const ObjectFilePair& right ) const
    {
        return ( left.first->getCounter() != right.first->getCounter() )
                   ? ( left.first->getCounter() < right.first->getCounter() )
                   : ( left.second < right.second );
    }
};

using ObjectFilePairToObjectPartMap = std::map< ObjectFilePair, ObjectPart::Ptr, ObjectFilePairComparator >;
using FileMap                       = std::map< schema::IdentifierList, File::Ptr >;
using StageMap                      = std::map< schema::Identifier, Stage::Ptr >;
using NamespaceMap                  = std::map< schema::Identifier, Namespace::Ptr >;
using ObjectVector                  = std::vector< Object::Ptr >;
using MapTypeVector                 = std::vector< MapType::Ptr >;
using InheritanceMap                = std::map< schema::IdentifierList, Object::Ptr >;
using SourceMap                     = std::map< schema::Identifier, Source::Ptr >;

struct Mapping
{
    Counter                       counter = 0U;
    TypeNameResMap                typeNameResMap;
    InheritanceNameResMap         inheritanceNameResMap;
    ObjectFilePairToObjectPartMap objectPartMap;
    FileMap                       fileMap;
    StageMap                      stageMap;
    NamespaceMap                  namespaceMap;
    ObjectVector                  objects;
    MapTypeVector                 mapTypes;
    SourceMap                     sourceMap;
};

void expandCPPType( const schema::Type& type, std::ostream& os )
{
    common::delimit( type.m_idList.begin(), type.m_idList.end(), "::", os );
    if( !type.m_children.empty() )
    {
        bool bFirst = true;
        os << "< ";
        for( const schema::Type& nested : type.m_children )
        {
            if( bFirst )
                bFirst = false;
            else
                os << ", ";
            expandCPPType( nested, os );
        }
        os << " >";
    }
}

Type::Ptr getType( const schema::Type& type, Mapping& mapping, Namespace::Ptr pNamespace )
{
    if( type.m_idList.size() == 1U )
    {
        const std::string& strID = type.m_idList.front();

        if( strID == "value" )
        {
            // value< typename >
            if( type.m_children.size() == 1U )
            {
                const schema::Type& cppType = type.m_children.front();
                if( !cppType.m_idList.empty() )
                {
                    std::ostringstream osCPPTypeName;
                    expandCPPType( cppType, osCPPTypeName );
                    ValueType::Ptr pValueType = std::make_shared< ValueType >( mapping.counter );
                    pValueType->m_cppType     = osCPPTypeName.str();
                    return pValueType;
                }
            }
        }
        else if( strID == "opt" )
        {
            // opt< value< cpptype > >
            if( type.m_children.size() == 1U )
            {
                const schema::Type& underlyingType = type.m_children.front();
                if( !underlyingType.m_children.empty() && !underlyingType.m_idList.empty() )
                {
                    OptType::Ptr pArray      = std::make_shared< OptType >( mapping.counter );
                    pArray->m_underlyingType = getType( underlyingType, mapping, pNamespace );
                    return pArray;
                }
            }
        }
        else if( strID == "array" )
        {
            // array< value< cpptype > >
            if( type.m_children.size() == 1U )
            {
                const schema::Type& underlyingType = type.m_children.front();
                if( !underlyingType.m_children.empty() && !underlyingType.m_idList.empty() )
                {
                    ArrayType::Ptr pArray    = std::make_shared< ArrayType >( mapping.counter );
                    pArray->m_underlyingType = getType( underlyingType, mapping, pNamespace );
                    return pArray;
                }
            }
        }
        else if( strID == "ref" )
        {
            // ref< ObjectType >
            if( type.m_children.size() == 1U )
            {
                const schema::Type& cppType = type.m_children.front();
                if( cppType.m_children.empty() && !cppType.m_idList.empty() )
                {
                    RefType::Ptr pRefType = std::make_shared< RefType >( mapping.counter );
                    // record the type name for later resolution
                    mapping.typeNameResMap.insert(
                        std::make_pair( pRefType, NameResolution{ pNamespace, cppType.m_idList } ) );
                    return pRefType;
                }
            }
        }
        else if( strID == "map" )
        {
            // ref< ObjectType >
            if( type.m_children.size() == 2U )
            {
                const schema::Type& fromType = type.m_children.front();
                const schema::Type& toType   = type.m_children.back();
                if( !fromType.m_idList.empty() && !toType.m_idList.empty() )
                {
                    MapType::Ptr pMapType = std::make_shared< MapType >( mapping.counter, false );
                    mapping.mapTypes.push_back( pMapType );
                    pMapType->m_fromType = getType( fromType, mapping, pNamespace );
                    pMapType->m_toType   = getType( toType, mapping, pNamespace );
                    return pMapType;
                }
            }
        }
        else if( strID == "multimap" )
        {
            // ref< ObjectType >
            if( type.m_children.size() == 2U )
            {
                const schema::Type& fromType = type.m_children.front();
                const schema::Type& toType   = type.m_children.back();
                if( !fromType.m_idList.empty() && !toType.m_idList.empty() )
                {
                    MapType::Ptr pMapType = std::make_shared< MapType >( mapping.counter, true );
                    mapping.mapTypes.push_back( pMapType );
                    pMapType->m_fromType = getType( fromType, mapping, pNamespace );
                    pMapType->m_toType   = getType( toType, mapping, pNamespace );
                    return pMapType;
                }
            }
        }
        else if( strID == "late" )
        {
            // late< ... >
            if( type.m_children.size() == 1U )
            {
                const schema::Type& nestedType = type.m_children.front();
                Type::Ptr           pType      = getType( nestedType, mapping, pNamespace );
                pType->setLate();
                return pType;
            }
        }
    }
    THROW_RTE( "Failed to resolve type for: " << type );
}

struct StageElementVariantVisitor : boost::static_visitor< void >
{
    Mapping&    mapping;
    Schema::Ptr pSchema;
    Stage::Ptr  pStage;
    StageElementVariantVisitor( Mapping& mapping, Schema::Ptr pSchema, Stage::Ptr pStage )
        : mapping( mapping )
        , pSchema( pSchema )
        , pStage( pStage )
    {
    }
    void operator()( const schema::File& file ) const
    {
        const schema::IdentifierList idlist = { pStage->m_strName, file.m_id };
        File::Ptr                    pFile  = std::make_shared< File >( mapping.counter );
        pFile->m_strName                    = file.m_id;
        pFile->m_stage                      = pStage;
        FileMap::iterator iFind             = mapping.fileMap.find( idlist );
        VERIFY_RTE_MSG( iFind == mapping.fileMap.end(), "Duplicate file name found: " << idlist );
        mapping.fileMap.insert( std::make_pair( idlist, pFile ) );
        pStage->m_files.push_back( pFile );
    }
    void operator()( const schema::Source& source ) const
    {
        Source::Ptr pSource;
        {
            SourceMap::iterator iFind = mapping.sourceMap.find( source.m_id );
            if( iFind != mapping.sourceMap.end() )
            {
                pSource = iFind->second;
            }
            else
            {
                pSource = std::make_shared< Source >( mapping.counter );
                mapping.sourceMap.insert( std::make_pair( source.m_id, pSource ) );
                pSchema->m_sources.push_back( pSource );
            }
        }

        pSource->m_strName = source.m_id;
        pSource->m_stages.push_back( pStage );

        pStage->m_sources.push_back( pSource );
    }
    void operator()( const schema::Dependency& dependency ) const
    {
        pStage->m_dependencyNames.push_back( dependency.m_id );
    }
    void operator()( const schema::GlobalAccessor& accessor ) const
    {
        Accessor::Ptr pAccessor = std::make_shared< Accessor >( mapping.counter );
        pAccessor->m_stage      = pStage;
        pAccessor->m_bPerSource = false;
        pAccessor->m_type       = getType( accessor.m_type, mapping, Namespace::Ptr() );
        pStage->m_accessors.push_back( pAccessor );
    }
    void operator()( const schema::PerSourceAccessor& accessor ) const
    {
        Accessor::Ptr pAccessor = std::make_shared< Accessor >( mapping.counter );
        pAccessor->m_stage      = pStage;
        pAccessor->m_bPerSource = true;
        pAccessor->m_type       = getType( accessor.m_type, mapping, Namespace::Ptr() );
        pStage->m_accessors.push_back( pAccessor );
    }
};

struct NamespaceVariantVisitor : boost::static_visitor< void >
{
    Namespace::Ptr pNamespace;
    Mapping&       mapping;
    NamespaceVariantVisitor( Mapping& mapping, Namespace::Ptr pNamespace )
        : mapping( mapping )
        , pNamespace( pNamespace )
    {
    }
    void operator()( const schema::Namespace& namespace_ ) const
    {
        Namespace::Ptr pChildNamespace = std::make_shared< Namespace >( mapping.counter );
        pChildNamespace->m_strName     = namespace_.m_name;
        pChildNamespace->m_strFullName = pNamespace->m_strName + "::" + namespace_.m_name;
        pChildNamespace->m_namespace   = pNamespace;

        NamespaceMap::iterator iFind = mapping.namespaceMap.find( pChildNamespace->m_strFullName );
        VERIFY_RTE_MSG(
            iFind == mapping.namespaceMap.end(), "Duplicate namespace name found: " << pChildNamespace->m_strFullName );
        mapping.namespaceMap.insert( std::make_pair( pChildNamespace->m_strFullName, pChildNamespace ) );

        NamespaceVariantVisitor visitor( mapping, pChildNamespace );
        for( const schema::NamespaceVariant& element : namespace_.m_elements )
        {
            boost::apply_visitor( visitor, element );
        }

        pNamespace->m_namespaces.push_back( pChildNamespace );
    }

    ObjectPart::Ptr getOrCreatePrimaryObjectPart( const ObjectFilePair& objectFilePair, File::Ptr pFile ) const
    {
        auto iFind = mapping.objectPartMap.find( objectFilePair );
        VERIFY_RTE( iFind == mapping.objectPartMap.end() );

        Object::Ptr pObject = objectFilePair.first;

        PrimaryObjectPart::Ptr pPrimaryObjectPart = std::make_shared< PrimaryObjectPart >( mapping.counter );
        pPrimaryObjectPart->m_object              = pObject;
        pPrimaryObjectPart->m_file                = pFile;
        pPrimaryObjectPart->m_typeID              = mapping.objectPartMap.size();

        pFile->m_parts.push_back( pPrimaryObjectPart );

        // VERIFY_RTE_MSG( pObject->m_primaryObjectParts.empty(), "Multiple primary object parts for: " <<
        // objectFilePair.second );
        pObject->m_primaryObjectParts.push_back( pPrimaryObjectPart );

        mapping.objectPartMap.insert( std::make_pair( objectFilePair, pPrimaryObjectPart ) );

        return pPrimaryObjectPart;
    }

    // returns existing primary or secondary OR creates secondary
    ObjectPart::Ptr getOrCreateSecondaryObjectPart( const ObjectFilePair& objectFilePair, File::Ptr pFile ) const
    {
        auto iFind = mapping.objectPartMap.find( objectFilePair );

        Object::Ptr pObject = objectFilePair.first;

        if( iFind == mapping.objectPartMap.end() )
        {
            auto pPrimaryObjectPart = pObject->getPrimaryObjectPart( pFile->m_stage.lock() );
            VERIFY_RTE( pPrimaryObjectPart );

            SecondaryObjectPart::Ptr pSecondaryObjectPart;

            if( pPrimaryObjectPart->m_file.lock()->m_stage.lock() != pFile->m_stage.lock() )
            {
                pSecondaryObjectPart = std::make_shared< InheritedObjectPart >( mapping.counter );
            }
            else
            {
                pSecondaryObjectPart = std::make_shared< AggregatedObjectPart >( mapping.counter );
            }

            pSecondaryObjectPart->m_object = pObject;
            pSecondaryObjectPart->m_file   = pFile;
            pSecondaryObjectPart->m_typeID = mapping.objectPartMap.size();
            pFile->m_parts.push_back( pSecondaryObjectPart );
            pObject->m_secondaryParts.push_back( pSecondaryObjectPart );
            mapping.objectPartMap.insert( std::make_pair( objectFilePair, pSecondaryObjectPart ) );

            return pSecondaryObjectPart;
        }
        else
        {
            return iFind->second;
        }
    }

    void operator()( const schema::Object& object ) const
    {
        Object::Ptr pObject = std::make_shared< Object >( mapping.counter, object.m_name );
        mapping.objects.push_back( pObject );

        pObject->m_namespace = pNamespace;
        if( object.m_optInheritance.has_value() )
        {
            mapping.inheritanceNameResMap.insert(
                std::make_pair( pObject, NameResolution{ pNamespace, object.m_optInheritance.value() } ) );
        }

        for( const schema::IdentifierList& fileIDList : object.m_files )
        {
            auto iFind = mapping.fileMap.find( fileIDList );
            VERIFY_RTE_MSG( iFind != mapping.fileMap.end(),
                            "Could not find fileIDList: " << fileIDList << " for object: " << object.m_name );

            File::Ptr pFile = iFind->second;

            ObjectFilePair objectFilePair( pObject, fileIDList );

            // create first object part if not one already
            ObjectPart::Ptr pObjectPart = getOrCreatePrimaryObjectPart( objectFilePair, pFile );

            for( const schema::Property& property : object.m_properties )
            {
                Property::Ptr pProperty = std::make_shared< Property >( mapping.counter );
                pProperty->m_strName    = property.m_name;
                if( property.m_optFile.has_value() )
                {
                    auto      i               = mapping.fileMap.find( property.m_optFile.value() );
                    File::Ptr pObjectPartFile = i->second;

                    VERIFY_RTE_MSG(
                        i != mapping.fileMap.end(),
                        "Could not find file: " << property.m_optFile.value() << " for object: " << object.m_name );

                    Stage::Ptr pObjectStage     = pFile->m_stage.lock();
                    Stage::Ptr pObjectPartStage = pObjectPartFile->m_stage.lock();

                    // ensure the object part is from current or later stage
                    VERIFY_RTE_MSG( pObjectStage->getCounter() <= pObjectPartStage->getCounter(),
                                    "Property: " << property.m_name << " specifies file which has earlier stage: "
                                                 << pObjectPartStage->m_strName << " than containing object: "
                                                 << object.m_name << " which is: " << pObjectStage->m_strName );

                    pObjectPart = getOrCreateSecondaryObjectPart(
                        ObjectFilePair( pObject, property.m_optFile.value() ), pObjectPartFile );
                }
                pProperty->m_objectPart = pObjectPart;
                pObjectPart->m_properties.push_back( pProperty );

                pProperty->m_type = getType( property.m_type, mapping, pNamespace );

                VERIFY_RTE_MSG( pProperty->m_type, "Failed to resolve type for property: " << property.m_name );
            }
        }

        pNamespace->m_objects.push_back( pObject );
    }
};

struct SchemaVariantVisitor : boost::static_visitor< void >
{
    Mapping&    mapping;
    Schema::Ptr pSchema;

    SchemaVariantVisitor( Mapping& mapping, Schema::Ptr pSchema )
        : mapping( mapping )
        , pSchema( pSchema )
    {
    }

    void operator()( const schema::Stage& stage ) const
    {
        Stage::Ptr pStage = std::make_shared< Stage >( mapping.counter );
        pStage->m_strName = stage.m_name;

        StageMap::iterator iFind = mapping.stageMap.find( stage.m_name );
        VERIFY_RTE_MSG( iFind == mapping.stageMap.end(), "Duplicate stage name found: " << stage.m_name );
        mapping.stageMap.insert( std::make_pair( stage.m_name, pStage ) );

        StageElementVariantVisitor visitor( mapping, pSchema, pStage );
        for( const schema::StageElementVariant& file : stage.m_elements )
        {
            boost::apply_visitor( visitor, file );
        }
        VERIFY_RTE_MSG( !pStage->m_sources.empty(), "Stage: " << pStage->m_strName << " is missing a source" );

        pSchema->m_stages.push_back( pStage );
    }

    void operator()( const schema::Namespace& namespace_ ) const
    {
        Namespace::Ptr pNamespace = std::make_shared< Namespace >( mapping.counter );
        pNamespace->m_strName     = namespace_.m_name;
        pNamespace->m_strFullName = namespace_.m_name;

        NamespaceMap::iterator iFind = mapping.namespaceMap.find( pNamespace->m_strFullName );
        VERIFY_RTE_MSG(
            iFind == mapping.namespaceMap.end(), "Duplicate namespace name found: " << pNamespace->m_strFullName );
        mapping.namespaceMap.insert( std::make_pair( pNamespace->m_strFullName, pNamespace ) );

        NamespaceVariantVisitor visitor( mapping, pNamespace );
        for( const schema::NamespaceVariant& element : namespace_.m_elements )
        {
            boost::apply_visitor( visitor, element );
        }

        pSchema->m_namespaces.push_back( pNamespace );
    }
};
} // namespace

void checkAmbiguity( Object::Ptr pResult, const NameResolution& nameRes )
{
    if( nameRes.m_namespace )
    {
        VERIFY_RTE_MSG( !pResult, "Ambiguous name resolution for: " << nameRes.id << " from namespace: "
                                                                    << nameRes.m_namespace->m_strFullName );
    }
    else
    {
        VERIFY_RTE_MSG( !pResult, "Ambiguous name resolution for: " << nameRes.id );
    }
}

Object::Ptr findType( const NameResolution& nameRes, Namespace::Ptr pNamespace,
                      schema::IdentifierList::const_iterator i, schema::IdentifierList::const_iterator iEnd )
{
    Object::Ptr pResult;

    const mega::U64 szDist = std::distance( i, iEnd );
    if( szDist == 1U )
    {
        for( Object::Ptr pObject : pNamespace->m_objects )
        {
            if( pObject->getIdentifier() == *i )
            {
                checkAmbiguity( pResult, nameRes );
                pResult = pObject;
            }
        }
    }
    else if( szDist > 1U )
    {
        for( Namespace::Ptr pNestedNamespace : pNamespace->m_namespaces )
        {
            if( pNestedNamespace->m_strName == *i )
            {
                if( Object::Ptr pObject = findType( nameRes, pNestedNamespace, i + 1, iEnd ) )
                {
                    checkAmbiguity( pResult, nameRes );
                    pResult = pObject;
                }
            }
        }
    }
    return pResult;
}

Object::Ptr findType( Schema::Ptr pSchema, const NameResolution& nameRes )
{
    Object::Ptr pResult;

    // attempt to search from the namespace outwards
    {
        Namespace::Ptr pNamespace = nameRes.m_namespace;
        while( pNamespace )
        {
            if( Object::Ptr pObject = findType( nameRes, pNamespace, nameRes.id.begin(), nameRes.id.end() ) )
            {
                checkAmbiguity( pResult, nameRes );
                pResult = pObject;
            }
            pNamespace = pNamespace->m_namespace.lock();
        }
    }

    // search all global namespaces
    if( !pResult )
    {
        const mega::U64 szDist = std::distance( nameRes.id.begin(), nameRes.id.end() );
        if( szDist > 1U )
        {
            for( Namespace::Ptr pNamespace : pSchema->m_namespaces )
            {
                if( pNamespace->m_strName == *nameRes.id.begin() )
                {
                    if( Object::Ptr pObject
                        = findType( nameRes, pNamespace, nameRes.id.begin() + 1, nameRes.id.end() ) )
                    {
                        checkAmbiguity( pResult, nameRes );
                        pResult = pObject;
                    }
                }
            }
        }
    }

    if( nameRes.m_namespace )
    {
        VERIFY_RTE_MSG( pResult, "Failed to resolve name resolution for: " << nameRes.id << " from namespace: "
                                                                           << nameRes.m_namespace->m_strFullName );
    }
    else
    {
        VERIFY_RTE_MSG( pResult, "Failed to resolve name resolution for: " << nameRes.id );
    }

    return pResult;
}

void getObjects( Namespace::Ptr pNamespace, std::vector< Object::Ptr >& objects )
{
    for( Object::Ptr pObject : pNamespace->m_objects )
    {
        objects.push_back( pObject );
    }
    for( Namespace::Ptr pIter : pNamespace->m_namespaces )
    {
        getObjects( pIter, objects );
    }
}

void stageInterfaces( Mapping& mapping, Schema::Ptr pSchema )
{
    using ObjectPtrVector = std::vector< ObjectPart::Ptr >;
    using ObjectPartMap   = std::map< Object::Ptr, ObjectPtrVector, CountedObjectComparator< Object::Ptr > >;

    // once an object exists it WILL ALWAYS exist in every stage from then on
    // so we only need to know the objects in the previous and current stage
    // each interface must correspond to an object.

    std::set< Stage::Ptr, CountedObjectComparator< Stage::Ptr > > open_stages;
    for( Stage::Ptr pStage : pSchema->m_stages )
    {
        open_stages.insert( pStage );
    }

    while( !open_stages.empty() )
    {
        // attempt to find stage with no open dependencies
        Stage::Ptr pStage;
        {
            for( Stage::Ptr pStageIter : open_stages )
            {
                bool bFoundOpenDependency = false;
                for( Stage::WeakPtr pDependency : pStageIter->m_dependencies )
                {
                    if( open_stages.count( pDependency.lock() ) )
                    {
                        bFoundOpenDependency = true;
                        break;
                    }
                }
                if( !bFoundOpenDependency )
                {
                    pStage = pStageIter;
                    open_stages.erase( pStage );
                    break;
                }
            }
            VERIFY_RTE_MSG( pStage, "Cyclic dependency in stage dependencies" );
        }

        // collect the new accumulation parts
        ObjectPartMap objectParts;
        for( File::Ptr pFile : pStage->m_files )
        {
            for( ObjectPart::Ptr pPart : pFile->m_parts )
            {
                Object::Ptr             pObject = pPart->m_object.lock();
                ObjectPartMap::iterator iFind   = objectParts.find( pObject );
                if( iFind != objectParts.end() )
                {
                    iFind->second.push_back( pPart );
                }
                else
                {
                    objectParts.insert( std::make_pair( pObject, ObjectPtrVector{ pPart } ) );
                }
            }
        }

        using InterfaceMap = std::map< Object::Ptr, Interface::Ptr >;
        InterfaceMap interfaceMap; // only finds on this so no comparator

        // add the read-write interfaces
        for( ObjectPartMap::iterator i = objectParts.begin(), iEnd = objectParts.end(); i != iEnd; ++i )
        {
            Interface::Ptr pInterface = std::make_shared< Interface >( mapping.counter );
            pInterface->m_isReadWrite = true;
            std::copy( i->second.begin(), i->second.end(), std::back_inserter( pInterface->m_readWriteObjectParts ) );
            pStage->m_readWriteInterfaces.push_back( pInterface );
            pInterface->m_object = i->first;
            interfaceMap.insert( std::make_pair( i->first, pInterface ) );
        }

        // add the read only interfaces
        {
            std::vector< Interface::Ptr > previousInterfaces;
            {
                std::set< Interface::Ptr, CountedObjectComparator< Interface::Ptr > > uniqueSet;
                for( Stage::WeakPtr pDependency : pStage->m_dependencies )
                {
                    Stage::Ptr pPreviousStage = pDependency.lock();

                    for( Interface::Ptr p : pPreviousStage->m_readWriteInterfaces )
                    {
                        if( !uniqueSet.count( p ) )
                        {
                            uniqueSet.insert( p );
                            previousInterfaces.push_back( p );
                        }
                    }
                    for( Interface::Ptr p : pPreviousStage->m_readOnlyInterfaces )
                    {
                        if( !uniqueSet.count( p ) )
                        {
                            uniqueSet.insert( p );
                            previousInterfaces.push_back( p );
                        }
                    }
                }
            }

            for( Interface::Ptr pPreviousInterface : previousInterfaces )
            {
                Object::Ptr            pObject = pPreviousInterface->m_object.lock();
                InterfaceMap::iterator iFind   = interfaceMap.find( pObject );
                if( iFind == interfaceMap.end() )
                {
                    Interface::Ptr pInterface = std::make_shared< Interface >( mapping.counter );
                    pInterface->m_isReadWrite = false;
                    interfaceMap.insert( std::make_pair( pObject, pInterface ) );
                    pInterface->m_readOnlyObjectParts = pPreviousInterface->m_readOnlyObjectParts;
                    std::copy( pPreviousInterface->m_readWriteObjectParts.begin(),
                               pPreviousInterface->m_readWriteObjectParts.end(),
                               std::back_inserter( pInterface->m_readOnlyObjectParts ) );
                    pInterface->m_object = pPreviousInterface->m_object;
                    pStage->m_readOnlyInterfaces.push_back( pInterface );
                }
                else
                {
                    Interface::Ptr pNewInterface = iFind->second;
                    std::copy( pPreviousInterface->m_readOnlyObjectParts.begin(),
                               pPreviousInterface->m_readOnlyObjectParts.end(),
                               std::back_inserter( pNewInterface->m_readOnlyObjectParts ) );
                    std::copy( pPreviousInterface->m_readWriteObjectParts.begin(),
                               pPreviousInterface->m_readWriteObjectParts.end(),
                               std::back_inserter( pNewInterface->m_readOnlyObjectParts ) );
                }
            }
        }

        // create interface functions and set base interfaces
        for( Interface::Ptr pInterface : pStage->m_readOnlyInterfaces )
        {
            Object::Ptr pObject = pInterface->m_object.lock();
            if( pObject->m_base )
            {
                InterfaceMap::iterator iFind = interfaceMap.find( pObject->m_base );
                VERIFY_RTE( iFind != interfaceMap.end() );
                pInterface->m_base = iFind->second;
            }
            VERIFY_RTE( pInterface->m_readWriteObjectParts.empty() );
            for( ObjectPart::Ptr pObjectPart : pInterface->m_readOnlyObjectParts )
            {
                for( Property::Ptr pProperty : pObjectPart->m_properties )
                {
                    if( pProperty->isGet() )
                    {
                        FunctionGetter::Ptr pGetter = std::make_shared< FunctionGetter >( mapping.counter );
                        pGetter->m_interface        = pInterface;
                        pGetter->m_property         = pProperty;
                        pInterface->m_functions.push_back( pGetter );
                    }
                }
            }
        }
        for( Interface::Ptr pInterface : pStage->m_readWriteInterfaces )
        {
            Object::Ptr pObject = pInterface->m_object.lock();
            if( pObject->m_base )
            {
                InterfaceMap::iterator iFind = interfaceMap.find( pObject->m_base );
                VERIFY_RTE( iFind != interfaceMap.end() );
                pInterface->m_base = iFind->second;
            }
            for( ObjectPart::Ptr pObjectPart : pInterface->m_readWriteObjectParts )
            {
                for( Property::Ptr pProperty : pObjectPart->m_properties )
                {
                    if( pProperty->isGet() )
                    {
                        FunctionGetter::Ptr pGetter = std::make_shared< FunctionGetter >( mapping.counter );
                        pGetter->m_interface        = pInterface;
                        pGetter->m_property         = pProperty;
                        pInterface->m_functions.push_back( pGetter );
                    }
                    if( pProperty->isSet() )
                    {
                        FunctionSetter::Ptr pSetter = std::make_shared< FunctionSetter >( mapping.counter );
                        pSetter->m_interface        = pInterface;
                        pSetter->m_property         = pProperty;
                        pInterface->m_functions.push_back( pSetter );
                    }
                    if( pProperty->isInsert() )
                    {
                        FunctionInserter::Ptr pInserter = std::make_shared< FunctionInserter >( mapping.counter );
                        pInserter->m_interface          = pInterface;
                        pInserter->m_property           = pProperty;
                        pInterface->m_functions.push_back( pInserter );
                    }
                    if( pProperty->isCtorParam() )
                    {
                        pInterface->m_args.push_back( pProperty );
                    }
                }
            }
            for( ObjectPart::Ptr pObjectPart : pInterface->m_readOnlyObjectParts )
            {
                for( Property::Ptr pProperty : pObjectPart->m_properties )
                {
                    if( pProperty->isGet() )
                    {
                        FunctionGetter::Ptr pGetter = std::make_shared< FunctionGetter >( mapping.counter );
                        pGetter->m_interface        = pInterface;
                        pGetter->m_property         = pProperty;
                        pInterface->m_functions.push_back( pGetter );
                    }
                }
            }
        }

        // create stage functions
        for( Interface::Ptr pInterface : pStage->m_readWriteInterfaces )
        {
            if( pInterface->ownsPrimaryObjectPart( pStage ) || pInterface->ownsInheritedSecondaryObjectPart() )
            {
                Constructor::Ptr pConstructor = std::make_shared< Constructor >( mapping.counter );
                pConstructor->m_interface     = pInterface;
                pConstructor->m_stage         = pStage;
                pStage->m_constructors.push_back( pConstructor );
            }
        }
    }
}

void objectGroups( Mapping& mapping, Schema::Ptr pSchema )
{
    std::set< Object::Ptr, CountedObjectComparator< Object::Ptr > > open_objects;
    {
        std::vector< Object::Ptr > objects;
        {
            for( Namespace::Ptr pNamespace : pSchema->m_namespaces )
            {
                getObjects( pNamespace, objects );
            }
        }
        for( Object::Ptr pObject : objects )
        {
            open_objects.insert( pObject );
        }
    }
    std::vector< std::set< Object::Ptr > > groups;
    while( !open_objects.empty() )
    {
        for( Object::Ptr pObject : open_objects )
        {
            bool bFound = false;
            for( std::set< Object::Ptr >& group : groups )
            {
                if( pObject->m_base )
                {
                    for( Object::Ptr pGroupMember : group )
                    {
                        if( pObject->m_base == pGroupMember )
                        {
                            bFound = true;
                            group.insert( pObject );
                            open_objects.erase( pObject );
                            break;
                        }
                    }
                }
                if( bFound )
                    break;
                for( Object::Ptr pGroupMember : group )
                {
                    if( pGroupMember->m_base == pObject )
                    {
                        bFound = true;
                        group.insert( pObject );
                        open_objects.erase( pObject );
                        break;
                    }
                }
                if( bFound )
                    break;
            }
            if( !bFound )
            {
                groups.push_back( std::set< Object::Ptr >{ pObject } );
                open_objects.erase( pObject );
            }
            break;
        }
    }

    for( std::set< Object::Ptr >& group : groups )
    {
        Object::WeakObjectPtrSetPtr pGroup = std::make_shared< Object::WeakObjectPtrSet >();
        for( Object::Ptr pObject : group )
        {
            pGroup->insert( pObject );
            pObject->m_pInheritanceGroup = pGroup;
        }
    }
}

void superTypes( Mapping& mapping, Schema::Ptr pSchema )
{
    // calculate the stage super interfaces
    for( Stage::Ptr pStage : pSchema->m_stages )
    {
        std::vector< Interface::Ptr > interfaces = pStage->m_readOnlyInterfaces;
        std::copy( pStage->m_readWriteInterfaces.begin(), pStage->m_readWriteInterfaces.end(),
                   std::back_inserter( interfaces ) );

        std::sort( interfaces.begin(), interfaces.end(), CountedObjectComparator< Interface::Ptr >() );

        {
            std::set< Interface::Ptr, CountedObjectComparator< Interface::Ptr > > uniqueInterfaces;
            for( Interface::Ptr pInterface : interfaces )
            {
                uniqueInterfaces.insert( pInterface );
            }
            VERIFY_RTE( uniqueInterfaces.size() == interfaces.size() );
        }

        std::vector< std::vector< Interface::Ptr > > disjointInheritanceSets;
        {
            std::set< Interface::Ptr, CountedObjectComparator< Interface::Ptr > > remaining(
                interfaces.begin(), interfaces.end() );
            while( !remaining.empty() )
            {
                Interface::Ptr pInterface = *remaining.begin();

                std::vector< Interface::Ptr > group;
                {
                    while( pInterface )
                    {
                        remaining.erase( pInterface );
                        group.push_back( pInterface );
                        pInterface = pInterface->m_base;
                    }
                    std::sort( group.begin(), group.end(), CountedObjectComparator< Interface::Ptr >() );

                    group.resize( std::distance(
                        group.begin(),
                        std::unique( group.begin(), group.end(), CountedObjectEquality< Interface::Ptr >() ) ) );
                }

                bool bFound = false;
                for( std::vector< Interface::Ptr >& existingGroup : disjointInheritanceSets )
                {
                    // is the object in the group?
                    std::vector< Interface::Ptr > intersection;
                    std::set_intersection( group.begin(), group.end(), existingGroup.begin(), existingGroup.end(),
                                           std::back_inserter( intersection ),
                                           CountedObjectComparator< Interface::Ptr >() );
                    if( !intersection.empty() )
                    {
                        std::copy( group.begin(), group.end(), std::back_inserter( existingGroup ) );
                        std::sort(
                            existingGroup.begin(), existingGroup.end(), CountedObjectComparator< Interface::Ptr >() );

                        auto uniqueEndIter = std::unique(
                            existingGroup.begin(), existingGroup.end(), CountedObjectEquality< Interface::Ptr >() );
                        existingGroup.resize( std::distance( existingGroup.begin(), uniqueEndIter ) );

                        {
                            std::set< Interface::Ptr, CountedObjectComparator< Interface::Ptr > > uniqueInterfaces;
                            for( Interface::Ptr pInterface : existingGroup )
                            {
                                uniqueInterfaces.insert( pInterface );
                            }
                            VERIFY_RTE( uniqueInterfaces.size() == existingGroup.size() );
                        }

                        bFound = true;
                        break;
                    }
                }

                if( !bFound )
                {
                    // add a new group
                    disjointInheritanceSets.push_back( group );

                    {
                        std::set< Interface::Ptr, CountedObjectComparator< Interface::Ptr > > uniqueInterfaces;
                        for( Interface::Ptr pInterface : group )
                        {
                            uniqueInterfaces.insert( pInterface );
                        }
                        VERIFY_RTE( uniqueInterfaces.size() == group.size() );
                    }
                }
            }
        }

        std::set< Interface::Ptr, CountedObjectComparator< Interface::Ptr > > uniqueInterfaces;

        for( const std::vector< Interface::Ptr >& group : disjointInheritanceSets )
        {
            // check unique
            common::Hash szNameHash;
            {
                for( Interface::Ptr pInterface : group )
                {
                    VERIFY_RTE( uniqueInterfaces.count( pInterface ) == 0 );
                    uniqueInterfaces.insert( pInterface );
                    szNameHash ^= pInterface->delimitTypeName( pStage->m_strName, "" );
                }
            }

            std::ostringstream osSuperTypeName;
            {
                // osSuperTypeName << "__super_" << mapping.counter;
                osSuperTypeName << "__super_" << szNameHash.toHexString();
            }

            SuperType::Ptr pSuperType = std::make_shared< SuperType >( mapping.counter, osSuperTypeName.str() );
            pSuperType->m_stage       = pStage;
            pSuperType->m_interfaces  = group;
            pStage->m_superTypes.push_back( pSuperType );

            for( Interface::Ptr pInterface : pSuperType->m_interfaces )
            {
                pInterface->m_superInterface = pSuperType;

                if( !pInterface->m_base )
                {
                    Object::Ptr pObject = pInterface->m_object.lock();
                    VERIFY_RTE( !pSuperType->m_base_object );
                    pSuperType->m_base_object = pObject;
                }

                // work out the function groups
                for( Function::Ptr pFunction : pInterface->m_functions )
                {
                    pSuperType->m_functions.insert(
                        std::make_pair( pFunction->getMangledName( pStage->m_strName ), pFunction ) );
                }
            }

            // calculate topological sort of the inheritance tree
            std::vector< Interface::Ptr > topological;
            {
                std::set< Interface::Ptr, CountedObjectComparator< Interface::Ptr > > open;
                {
                    for( Interface::Ptr pInterface : group )
                        open.insert( pInterface );
                }

                while( !open.empty() )
                {
                    bool bFound = false;
                    for( Interface::Ptr pIter : open )
                    {
                        if( pIter->m_base )
                        {
                            if( !open.count( pIter->m_base ) )
                            {
                                bFound = true;
                                topological.push_back( pIter );
                                open.erase( pIter );
                                break;
                            }
                        }
                        else
                        {
                            bFound = true;
                            topological.push_back( pIter );
                            open.erase( pIter );
                            break;
                        }
                    }
                    if( !bFound )
                    {
                        THROW_RTE( "Failed to solve topological sort of group" );
                    }
                }
            }

            for( Interface::Ptr pInterface : topological )
            {
                if( pStage->m_interfaceTopologicalSet.count( pInterface ) == 0 )
                {
                    pStage->m_interfaceTopologicalSet.insert( pInterface );
                    pStage->m_interfaceTopological.push_back( pInterface );
                }
            }
        }
    }
}

void objectPartConversions( Mapping& mapping, Schema::Ptr pSchema )
{
    // calculate the object part conversions
    std::vector< Object::Ptr >                                      objects;
    std::set< Object::Ptr, CountedObjectComparator< Object::Ptr > > objectsUnique;
    {
        for( Namespace::Ptr pNamespace : pSchema->m_namespaces )
        {
            getObjects( pNamespace, objects );
        }
    }

    for( Object::Ptr pObject : objects )
    {
        for( auto pPrimaryObjectPart : pObject->m_primaryObjectParts )
        {
            // to self case
            pSchema->m_conversions.insert( std::make_pair(
                Schema::ObjectPartPair{ pPrimaryObjectPart, pPrimaryObjectPart }, Schema::ObjectPartVector{} ) );

            if( !pObject->m_base )
            {
                pSchema->m_base_conversions.insert( std::make_pair(
                    Schema::ObjectPartPair{ pPrimaryObjectPart, pPrimaryObjectPart }, Schema::ObjectPartVector{} ) );
            }

            for( ObjectPart::Ptr pSecondary : pObject->m_secondaryParts )
            {
                pSchema->m_conversions.insert( std::make_pair( Schema::ObjectPartPair{ pPrimaryObjectPart, pSecondary },
                                                               Schema::ObjectPartVector{ pSecondary } ) );
            }
            {
                Object::Ptr              pBase = pObject->m_base;
                Schema::ObjectPartVector baseList;
                while( pBase )
                {
                    for( auto pBasePrimaryObjectPart : pBase->m_primaryObjectParts )
                    {
                        baseList.push_back( pBasePrimaryObjectPart );

                        pSchema->m_conversions.insert( std::make_pair(
                            Schema::ObjectPartPair{ pPrimaryObjectPart, pBasePrimaryObjectPart }, baseList ) );

                        if( !pBase->m_base )
                        {
                            pSchema->m_base_conversions.insert( std::make_pair(
                                Schema::ObjectPartPair{ pPrimaryObjectPart, pBasePrimaryObjectPart }, baseList ) );
                        }
                    }

                    for( ObjectPart::Ptr pSecondary : pBase->m_secondaryParts )
                    {
                        Schema::ObjectPartVector baseListPlusSecondary = baseList;
                        baseListPlusSecondary.push_back( pSecondary );
                        pSchema->m_conversions.insert( std::make_pair(
                            Schema::ObjectPartPair{ pPrimaryObjectPart, pSecondary }, baseListPlusSecondary ) );
                    }

                    pBase = pBase->m_base;
                }
            }
        }
    }
}

void fileDependencies( Mapping& mapping, Schema::Ptr pSchema )
{
    // calculate file dependencies
    for( Stage::Ptr pStage : pSchema->m_stages )
    {
        for( File::Ptr pFile : pStage->m_files )
        {
            std::set< File::Ptr, CountedObjectComparator< File::Ptr > > dependencies;
            for( ObjectPart::Ptr pPart : pFile->m_parts )
            {
                if( PrimaryObjectPart::Ptr pPrimaryPart = std::dynamic_pointer_cast< PrimaryObjectPart >( pPart ) )
                {
                    Object::Ptr pObject = pPrimaryPart->m_object.lock();
                    while( pObject )
                    {
                        pObject = pObject->m_base;
                        if( pObject )
                        {
                            File::Ptr pDependencyFile = pObject->getPrimaryObjectPart( pStage )->m_file.lock();
                            if( pDependencyFile != pFile )
                                dependencies.insert( pDependencyFile );
                        }
                    }
                }
                else if( InheritedObjectPart::Ptr pInheritedSecondaryObjectPart
                         = std::dynamic_pointer_cast< InheritedObjectPart >( pPart ) )
                {
                    Object::Ptr pObject         = pInheritedSecondaryObjectPart->m_object.lock();
                    File::Ptr   pDependencyFile = pObject->getPrimaryObjectPart( pStage )->m_file.lock();
                    if( pDependencyFile != pFile )
                        dependencies.insert( pDependencyFile );

                    if( std::find(
                            pDependencyFile->m_dependencies.begin(), pDependencyFile->m_dependencies.end(), pFile )
                        == pDependencyFile->m_dependencies.end() )
                    {
                        pDependencyFile->m_dependencies.push_back( pFile );
                    }
                }
            }
            std::copy( dependencies.begin(), dependencies.end(), std::back_inserter( pFile->m_dependencies ) );
        }
    }
}

Schema::Ptr from_ast( const ::db::schema::Schema& schema )
{
    Mapping mapping;

    common::Hash schemaHash;
    {
        std::ostringstream os;
        os << schema;
        schemaHash = common::Hash( os.str() );
    }

    Schema::Ptr pSchema = std::make_shared< Schema >( mapping.counter, schemaHash );

    SchemaVariantVisitor visitor( mapping, pSchema );

    for( const schema::SchemaVariant& element : schema.m_elements )
    {
        boost::apply_visitor( visitor, element );
    }

    // resolve stage dependencies
    for( Stage::Ptr pStage : pSchema->m_stages )
    {
        for( const std::string& strDependency : pStage->m_dependencyNames )
        {
            StageMap::iterator iFind = mapping.stageMap.find( strDependency );
            VERIFY_RTE_MSG(
                iFind != mapping.stageMap.end(),
                "Could not locate stage: " << pStage->m_strName << " dependency on stage: " << strDependency );
            pStage->m_dependencies.push_back( iFind->second );
        }
    }

    // resolve RefType type names
    for( TypeNameResMap::iterator i = mapping.typeNameResMap.begin(), iEnd = mapping.typeNameResMap.end(); i != iEnd;
         ++i )
    {
        i->first->m_object = findType( pSchema, i->second );
    }

    // resolve object inheritance type names
    for( InheritanceNameResMap::iterator i    = mapping.inheritanceNameResMap.begin(),
                                         iEnd = mapping.inheritanceNameResMap.end();
         i != iEnd;
         ++i )
    {
        Object::Ptr pObject = findType( pSchema, i->second );
        i->first->m_base    = pObject;
        pObject->m_deriving.push_back( i->first );
    }

    stageInterfaces( mapping, pSchema );

    objectGroups( mapping, pSchema );

    superTypes( mapping, pSchema );

    objectPartConversions( mapping, pSchema );

    fileDependencies( mapping, pSchema );

    return pSchema;
}
} // namespace model
} // namespace db
