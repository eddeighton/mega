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

            virtual std::string getViewType() const = 0;
            virtual std::string getDataType() const = 0;
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
            std::size_t m_typeID;
        };

        class Object
        {
        public:
            using Ptr = std::shared_ptr< Object >;

            std::weak_ptr< Namespace > m_namespace;
            std::weak_ptr< File >      m_primaryFile;

            std::string                    m_strInheritance;
            std::string                    m_strName;
            std::vector< ObjectPart::Ptr > m_parts;
            Ptr                            m_base;
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

        class Accessor
        {
        public:
            using Ptr = std::shared_ptr< Accessor >;
            std::weak_ptr< Stage > m_stage;
            bool                   m_bPerObject;
            Type::Ptr              m_type;
        };

        class Stage
        {
        public:
            using Ptr = std::shared_ptr< Stage >;
            bool                         m_bPerObject;
            std::string                  m_strName;
            std::vector< File::Ptr >     m_files;
            std::vector< Accessor::Ptr > m_accessors;
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

            virtual std::string getViewType() const { return m_cppType; }
            virtual std::string getDataType() const { return m_cppType; }
        };

        class ArrayType : public Type
        {
        public:
            using Ptr = std::shared_ptr< ArrayType >;
            Type::Ptr m_underlyingType;

            virtual std::string getViewType() const
            {
                VERIFY_RTE( m_underlyingType );
                std::ostringstream os;
                os << "std::vector< " << m_underlyingType->getViewType() << " >";
                return os.str();
            }
            virtual std::string getDataType() const
            {
                VERIFY_RTE( m_underlyingType );
                std::ostringstream os;
                os << "std::vector< " << m_underlyingType->getDataType() << " >";
                return os.str();
            }
        };

        class RefType : public Type
        {
        public:
            using Ptr = std::shared_ptr< RefType >;
            std::string m_strObjectType;
            Object::Ptr m_object;

            virtual std::string getViewType() const
            {
                VERIFY_RTE( m_object );
                std::ostringstream os;
                os << m_object->m_namespace.lock()->m_strFullName << "::" << m_object->m_strName
                   << "*";
                return os.str();
            }
            virtual std::string getDataType() const
            {
                VERIFY_RTE( m_object );
                std::ostringstream os;
                os << m_object->m_primaryFile.lock()->m_strName << "::" << m_object->m_strName
                   << "*";
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

        Schema::Ptr from_ast( const ::db::schema::Schema& schema );

    } // namespace model
} // namespace db

#endif // DATABASE_COMPILER_MODEL_4_APRIL_2022
