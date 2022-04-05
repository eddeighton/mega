#ifndef DATABASE_COMPILER_MODEL_4_APRIL_2022
#define DATABASE_COMPILER_MODEL_4_APRIL_2022

#include "grammar.hpp"

#include "common/assert_verify.hpp"

#include <memory>
#include <string>
#include <vector>

namespace db
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

        virtual std::string getReturnType() const = 0;
    };

    class File;
    class ObjectPart;
    class Object;
    class Namespace;
    class Stage;

    class Property
    {
    public:
        using Ptr = std::shared_ptr< Property >;

        std::weak_ptr< ObjectPart > m_objectPart;

        std::string m_strName;
        Type::Ptr   m_type;
    };

    class ObjectPart
    {
    public:
        using Ptr = std::shared_ptr< ObjectPart >;

        std::weak_ptr< Object > m_object;
        std::weak_ptr< File >   m_file;

        std::vector< Property::Ptr > m_properties;
    };

    class Object
    {
    public:
        using Ptr = std::shared_ptr< Object >;

        std::weak_ptr< Namespace > m_namespace;
        std::weak_ptr< File >      m_primaryFile;

        std::string                    m_strName;
        std::vector< ObjectPart::Ptr > m_parts;
    };

    class Namespace
    {
    public:
        using Ptr = std::shared_ptr< Namespace >;

        std::weak_ptr< Namespace > m_namespace;

        std::string                   m_strName;
        std::string                   m_strFullName;
        std::vector< Object::Ptr >    m_objects;
        std::vector< Namespace::Ptr > m_namespaces;
    };

    class File
    {
    public:
        using Ptr = std::shared_ptr< File >;

        std::weak_ptr< Stage > m_stage;

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
        using Ptr = std::shared_ptr< Schema >;

        std::vector< Namespace::Ptr > m_namespaces;
        std::vector< Stage::Ptr >     m_stages;
    };

    ///////////////////////////////////////////////////
    ///////////////////////////////////////////////////
    // types
    class ValueType : public Type
    {
    public:
        using Ptr = std::shared_ptr< ValueType >;
        std::string m_cppType;

        virtual std::string getReturnType() const { return m_cppType; }
    };

    class ArrayType : public Type
    {
    public:
        using Ptr = std::shared_ptr< ArrayType >;
        Type::Ptr m_underlyingType;

        virtual std::string getReturnType() const 
        { 
            VERIFY_RTE( m_underlyingType );
            std::ostringstream os;
            os << "std::vector< " << m_underlyingType->getReturnType() << " >";
            return os.str();
        }
    };
/*
    class ReferenceType : public Type
    {
    public:
        using Ptr = std::shared_ptr< ReferenceType >;
        Object::Ptr m_objectType;
    };

    class OptionalType : public Type
    {
    public:
        using Ptr = std::shared_ptr< OptionalType >;
        Type::Ptr m_underlyingType;
    };

    class PredicateType : public Type
    {
    public:
        using Ptr = std::shared_ptr< PredicateType >;
        std::string m_cppType;
    };

    class ArrayType : public Type
    {
    public:
        using Ptr = std::shared_ptr< ArrayType >;
        Type::Ptr m_underlyingType;
    };

    class SetType : public Type
    {
    public:
        using Ptr = std::shared_ptr< SetType >;
        Type::Ptr m_underlyingType;
        Type::Ptr m_predicate;
    };

    class MapType : public Type
    {
    public:
        using Ptr = std::shared_ptr< MapType >;
        Type::Ptr m_fromType;
        Type::Ptr m_toType;
        Type::Ptr m_predicate;
    };

    class MultiMapType : public Type
    {
    public:
        using Ptr = std::shared_ptr< MultiMapType >;

        Type::Ptr m_fromType;
        Type::Ptr m_toType;
        Type::Ptr m_predicate;
    };*/

    ///////////////////////////////////////////////////
    ///////////////////////////////////////////////////
    // files
    class PerObjectFile : public File
    {
    public:
        using Ptr = std::shared_ptr< PerObjectFile >;
    };

    class PerProgramFile : public File
    {
    public:
        using Ptr = std::shared_ptr< PerProgramFile >;
    };

    Schema::Ptr from_ast( const ::db::schema::Schema& schema );

} // namespace model
} // namespace db

#endif // DATABASE_COMPILER_MODEL_4_APRIL_2022
