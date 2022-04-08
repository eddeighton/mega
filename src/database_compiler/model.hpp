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
        using Counter = std::size_t;

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
            bool operator()( T left, T right ) const
            {
                return left->getCounter() < right->getCounter();
            }
        };

        ///////////////////////////////////////////////////
        ///////////////////////////////////////////////////
        // base classes
        class Type : public CountedObject
        {
        public:
            using Ptr = std::shared_ptr< Type >;

            Type( std::size_t& szCounter )
                : CountedObject( szCounter )
            {
            }

            virtual std::string getViewType() const = 0;
            virtual std::string getDataType() const = 0;
            virtual bool        isCtorParam() const = 0;
            virtual bool        isGet() const = 0;
            virtual bool        isSet() const = 0;
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

            Property( std::size_t& szCounter )
                : CountedObject( szCounter )
            {
            }

            bool isCtorParam() const { return m_type->isCtorParam(); }
            bool isGet() const { return m_type->isGet(); }
            bool isSet() const { return m_type->isSet(); }

            std::weak_ptr< ObjectPart > m_objectPart;

            std::string m_strName;
            Type::Ptr   m_type;
        };

        class ObjectPart : public CountedObject
        {
        public:
            using Ptr = std::shared_ptr< ObjectPart >;

            ObjectPart( std::size_t& szCounter )
                : CountedObject( szCounter )
            {
            }

            std::weak_ptr< Object > m_object;
            std::weak_ptr< File >   m_file;

            std::vector< Property::Ptr > m_properties;
            std::size_t                  m_typeID;

            std::string getDataType( const std::string& strDelimiter ) const;
        };

        class PrimaryObjectPart : public ObjectPart
        {
        public:
            PrimaryObjectPart( std::size_t& szCounter )
                : ObjectPart( szCounter )
            {
            }
            using Ptr = std::shared_ptr< PrimaryObjectPart >;
        };

        class SecondaryObjectPart : public ObjectPart
        {
        public:
            SecondaryObjectPart( std::size_t& szCounter )
                : ObjectPart( szCounter )
            {
            }
            using Ptr = std::shared_ptr< SecondaryObjectPart >;
        };

        class Object : public CountedObject
        {
        public:
            Object( std::size_t& szCounter )
                : CountedObject( szCounter )
            {
            }
            using Ptr = std::shared_ptr< Object >;

            // std::string m_strInheritance;
            std::string                m_strName;
            std::weak_ptr< Namespace > m_namespace;
            std::weak_ptr< File >      m_primaryFile;

            std::weak_ptr< Stage > m_stage;
            // primary object part for the object in its stage
            PrimaryObjectPart::Ptr m_primaryObjectPart;

            // secondary object parts that belong to the stage of the object
            std::vector< SecondaryObjectPart::Ptr > m_secondaryParts;

            Ptr                                    m_base;
            std::vector< std::weak_ptr< Object > > m_deriving;
        };

        class Namespace : public CountedObject
        {
        public:
            Namespace( std::size_t& szCounter )
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
            File( std::size_t& szCounter )
                : CountedObject( szCounter )
            {
            }
            using Ptr = std::shared_ptr< File >;

            std::weak_ptr< Stage > m_stage;

            std::string                    m_strName;
            std::vector< ObjectPart::Ptr > m_parts;
        };

        class Function : public CountedObject
        {
        public:
            using Ptr = std::shared_ptr< Function >;
            Function( std::size_t& szCounter )
                : CountedObject( szCounter )
            {
            }
            Property::Ptr m_property;
        };
        class FunctionGetter : public Function
        {
        public:
            using Ptr = std::shared_ptr< FunctionGetter >;
            FunctionGetter( std::size_t& szCounter )
                : Function( szCounter )
            {
            }
        };
        class FunctionSetter : public Function
        {
        public:
            using Ptr = std::shared_ptr< FunctionSetter >;
            FunctionSetter( std::size_t& szCounter )
                : Function( szCounter )
            {
            }
        };

        class SuperInterface;
        class Interface : public CountedObject
        {
        public:
            using Ptr = std::shared_ptr< Interface >;
            Interface( std::size_t& szCounter )
                : CountedObject( szCounter )
            {
            }
            std::weak_ptr< SuperInterface > m_superInterface;
            std::weak_ptr< Object >         m_object;
            std::vector< Function::Ptr >    m_functions;
            Interface::Ptr                  m_base;
            std::vector< ObjectPart::Ptr >  m_readOnlyObjectParts;
            std::vector< ObjectPart::Ptr >  m_readWriteObjectParts;

            std::string delimitTypeName( const std::string& str ) const
            {
                Object::Ptr pObject = m_object.lock();

                std::vector< Namespace::Ptr > namespaces;
                {
                    Namespace::Ptr pIter = pObject->m_namespace.lock();
                    while ( pIter )
                    {
                        namespaces.push_back( pIter );
                        pIter = pIter->m_namespace.lock();
                    }
                    std::reverse( namespaces.begin(), namespaces.end() );
                }

                std::ostringstream os;
                {
                    for ( Namespace::Ptr pNamespace : namespaces )
                    {
                        os << pNamespace->m_strName << str;
                    }
                    os << pObject->m_strName;
                }

                return os.str();
            }
        };

        class SuperInterface : public CountedObject
        {
        public:
            SuperInterface( std::size_t& szCounter )
                : CountedObject( szCounter )
            {
            }
            using Ptr = std::shared_ptr< SuperInterface >;

            std::weak_ptr< Stage >        m_stage;
            std::vector< Interface::Ptr > m_interfaces;
        };

        class StageFunction : public CountedObject
        {
        public:
            StageFunction( std::size_t& szCounter )
                : CountedObject( szCounter )
            {
            }
            using Ptr = std::shared_ptr< StageFunction >;

            std::weak_ptr< Stage > m_stage;
        };

        class Accessor : public StageFunction
        {
        public:
            Accessor( std::size_t& szCounter )
                : StageFunction( szCounter )
            {
            }
            using Ptr = std::shared_ptr< Accessor >;
            bool                   m_bPerObject;
            std::weak_ptr< Stage > m_stage;
            Type::Ptr              m_type;
        };

        class Constructor : public StageFunction
        {
        public:
            Constructor( std::size_t& szCounter )
                : StageFunction( szCounter )
            {
            }
            using Ptr = std::shared_ptr< Constructor >;
            std::weak_ptr< Stage >                   m_stage;
            Interface::Ptr                           m_interface;
            std::vector< std::weak_ptr< Property > > m_parameters;
        };

        class Refinement : public StageFunction
        {
        public:
            Refinement( std::size_t& szCounter )
                : StageFunction( szCounter )
            {
            }
            using Ptr = std::shared_ptr< Refinement >;
            std::weak_ptr< Stage >                   m_stage;
            Interface::Ptr                           m_fromInterface;
            Interface::Ptr                           m_toInterface;
            std::vector< std::weak_ptr< Property > > m_parameters;
        };

        class Stage : public CountedObject
        {
        public:
            Stage( std::size_t& szCounter )
                : CountedObject( szCounter )
            {
            }
            using Ptr = std::shared_ptr< Stage >;
            bool                     m_bPerObject;
            std::string              m_strName;
            std::vector< File::Ptr > m_files;

            std::vector< Accessor::Ptr >    m_accessors;
            std::vector< Constructor::Ptr > m_constructors;
            std::vector< Refinement::Ptr >  m_refinements;

            std::vector< Interface::Ptr >      m_interfaceTopological;
            std::vector< Interface::Ptr >      m_readOnlyInterfaces;
            std::vector< Interface::Ptr >      m_readWriteInterfaces;
            std::vector< SuperInterface::Ptr > m_superInterfaces;
        };

        class Schema : public CountedObject
        {
        public:
            Schema( std::size_t& szCounter )
                : CountedObject( szCounter )
            {
            }
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
            ValueType( std::size_t& szCounter )
                : Type( szCounter )
            {
            }
            using Ptr = std::shared_ptr< ValueType >;
            std::string m_cppType;

            virtual std::string getViewType() const { return m_cppType; }
            virtual std::string getDataType() const { return m_cppType; }
            virtual bool        isCtorParam() const { return true; }
            virtual bool        isGet() const { return true; }
            virtual bool        isSet() const { return true; }
        };

        class ArrayType : public Type
        {
        public:
            ArrayType( std::size_t& szCounter )
                : Type( szCounter )
            {
            }
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
            virtual bool isCtorParam() const { return false; }
            virtual bool isGet() const { return true; }
            virtual bool isSet() const { return true; }
        };

        class RefType : public Type
        {
        public:
            RefType( std::size_t& szCounter )
                : Type( szCounter )
            {
            }
            using Ptr = std::shared_ptr< RefType >;
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
            virtual bool isCtorParam() const { return true; }
            virtual bool isGet() const { return true; }
            virtual bool isSet() const { return true; }
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
