#ifndef DATABASE_COMPILER_MODEL_4_APRIL_2022
#define DATABASE_COMPILER_MODEL_4_APRIL_2022

#include "grammar.hpp"

#include <memory>
#include <string>
#include <vector>

namespace dbcomp
{
namespace model
{

    ///////////////////////////////////////////////////
    ///////////////////////////////////////////////////
    // base classes
    class Type
    {
    public:
        using Ptr = std::shared_ptr< Type >;
    };

    class Property
    {
    public:
        using Ptr = std::shared_ptr< Property >;

        std::string m_strName;
        Type::Ptr   m_type;
    };

    class ObjectPart
    {
    public:
        using Ptr = std::shared_ptr< ObjectPart >;
        std::vector< Property::Ptr > m_properties;
    };

    class Object
    {
    public:
        using Ptr = std::shared_ptr< Object >;

        std::string                    m_strName;
        std::vector< ObjectPart::Ptr > m_parts;
    };

    class Namespace
    {
    public:
        using Ptr = std::shared_ptr< Namespace >;

        std::string                   m_strName;
        std::vector< Object::Ptr >    m_objects;
        std::vector< Namespace::Ptr > m_namespaces;
    };

    class File
    {
    public:
        using Ptr = std::shared_ptr< File >;

        std::string                    m_strName;
        std::vector< ObjectPart::Ptr > m_parts;
    };

    class Stage
    {
    public:
        using Ptr = std::shared_ptr< Stage >;

        std::string              m_strName;
        std::vector< File::Ptr > m_files;
    };

    class Schema
    {
    public:
        using Ptr = std::shared_ptr< Stage >;

        std::vector< Namespace::Ptr > m_namespaces;
        std::vector< Stage::Ptr > m_stages;
    };

    ///////////////////////////////////////////////////
    ///////////////////////////////////////////////////
    // types
    class ValueType : public Type
    {
    public:
        std::string m_cppType;
    };

    class ReferenceType : public Type
    {
    public:
        Object::Ptr m_objectType;
    };

    class OptionalType : public Type
    {
    public:
        Type::Ptr m_underlyingType;
    };

    class PredicateType : public Type
    {
    public:
        std::string m_cppType;
    };

    class ArrayType : public Type
    {
    public:
        Type::Ptr m_underlyingType;
    };

    class SetType : public Type
    {
    public:
        Type::Ptr m_underlyingType;
        Type::Ptr m_predicate;
    };

    class MapType : public Type
    {
    public:
        Type::Ptr m_fromType;
        Type::Ptr m_toType;
        Type::Ptr m_predicate;
    };

    class MultiMapType : public Type
    {
    public:
        Type::Ptr m_fromType;
        Type::Ptr m_toType;
        Type::Ptr m_predicate;
    };

    ///////////////////////////////////////////////////
    ///////////////////////////////////////////////////
    // files
    class PerObjectFile : public File
    {
    public:
    };

    class PerProgramFile : public File
    {
    public:
    };


    Schema::Ptr from_ast( const ::dbcomp::Schema& schema );

} // namespace model
} // namespace dbcomp

#endif // DATABASE_COMPILER_MODEL_4_APRIL_2022
