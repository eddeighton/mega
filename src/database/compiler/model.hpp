#ifndef DATABASE_COMPILER_MODEL_4_APRIL_2022
#define DATABASE_COMPILER_MODEL_4_APRIL_2022

#include "grammar.hpp"

#include "common/assert_verify.hpp"

#include <memory>
#include <string>
#include <vector>
#include <map>

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
            bool operator()( T left, T right ) const { return left->getCounter() < right->getCounter(); }
        };

        ///////////////////////////////////////////////////
        ///////////////////////////////////////////////////
        // base classes
        class Type : public CountedObject
        {
        public:
            using Ptr = std::shared_ptr< Type >;

            bool m_bLate = false;

            Type( std::size_t& szCounter )
                : CountedObject( szCounter )
            {
            }

            void setLate() { m_bLate = true; }

            virtual std::string getViewType( bool bAsArg ) const = 0;
            virtual std::string getDataType( bool bAsArg ) const = 0;
            virtual bool        isCtorParam() const              = 0;
            virtual bool        isGet() const                    = 0;
            virtual bool        isSet() const                    = 0;
            virtual bool        isInsert() const                 = 0;
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
            bool isInsert() const { return m_type->isInsert(); }

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
            std::string getPointerName() const;
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
            std::vector< File::Ptr >       m_dependencies;
        };
        class Interface;
        class Function : public CountedObject
        {
        public:
            using Ptr = std::shared_ptr< Function >;
            Function( std::size_t& szCounter )
                : CountedObject( szCounter )
            {
            }
            Property::Ptr              m_property;
            std::weak_ptr< Interface > m_interface;

            virtual std::string getName() const       = 0;
            virtual std::string getReturnType() const = 0;
            virtual std::string getParams() const     = 0;

            inline std::string getMangledName() const
            {
                std::ostringstream os;
                os << getReturnType() << getName() << getParams();
                return os.str();
            }
        };
        class FunctionGetter : public Function
        {
        public:
            using Ptr = std::shared_ptr< FunctionGetter >;
            FunctionGetter( std::size_t& szCounter )
                : Function( szCounter )
            {
            }
            virtual std::string getName() const
            {
                std::ostringstream os;
                os << "get_" << m_property->m_strName;
                return os.str();
            }
            virtual std::string getReturnType() const
            {
                std::ostringstream os;
                os << m_property->m_type->getViewType( true );
                return os.str();
            }
            virtual std::string getParams() const
            {
                std::ostringstream os;
                return os.str();
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
            virtual std::string getName() const
            {
                std::ostringstream os;
                os << "set_" << m_property->m_strName;
                return os.str();
            }
            virtual std::string getReturnType() const { return "void"; }
            virtual std::string getParams() const
            {
                std::ostringstream os;
                os << m_property->m_type->getViewType( true ) << " value ";
                return os.str();
            }
        };
        class FunctionInserter : public Function
        {
        public:
            using Ptr = std::shared_ptr< FunctionInserter >;
            FunctionInserter( std::size_t& szCounter )
                : Function( szCounter )
            {
            }
            virtual std::string getName() const;
            virtual std::string getReturnType() const { return "void"; }
            virtual std::string getParams() const;
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
            std::vector< Property::Ptr >    m_args;
            Interface::Ptr                  m_base;
            std::vector< ObjectPart::Ptr >  m_readOnlyObjectParts;
            std::vector< ObjectPart::Ptr >  m_readWriteObjectParts;
            bool                            m_isReadWrite;

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

            inline PrimaryObjectPart::Ptr getPrimaryObjectPart() const
            {
                PrimaryObjectPart::Ptr pPart = m_object.lock()->m_primaryObjectPart;
                VERIFY_RTE( pPart );
                return pPart;
            }
            inline bool ownsPrimaryObjectPart() const
            {
                ObjectPart::Ptr pPrimaryObjectPart = getPrimaryObjectPart();
                for ( model::ObjectPart::Ptr pPart : m_readWriteObjectParts )
                {
                    if ( pPart == pPrimaryObjectPart )
                    {
                        return true;
                    }
                }
                return false;
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

            using FunctionMultiMap = std::multimap< std::string, Function::Ptr >;
            FunctionMultiMap m_functions;

            std::string getTypeName() const
            {
                std::ostringstream os;
                os << "super";
                for ( model::Interface::Ptr pInterface : m_interfaces )
                {
                    os << "_" << pInterface->delimitTypeName( "_" );
                }
                return os.str();
            }
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
            bool                   m_bPerSource;
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
            std::weak_ptr< Stage > m_stage;
            Interface::Ptr         m_interface;
        };

        class Stage;
        class Source : public CountedObject
        {
        public:
            Source( std::size_t& szCounter )
                : CountedObject( szCounter )
            {
            }
            using Ptr = std::shared_ptr< Source >;
            std::string m_strName;
            std::vector< std::weak_ptr< Stage > > m_stages;
        };

        class Stage : public CountedObject
        {
        public:
            Stage( std::size_t& szCounter )
                : CountedObject( szCounter )
            {
            }
            using Ptr = std::shared_ptr< Stage >;

            std::string              m_strName;
            Source::Ptr              m_source;
            std::vector< File::Ptr > m_files;

            std::vector< Accessor::Ptr >    m_accessors;
            std::vector< Constructor::Ptr > m_constructors;

            std::vector< Interface::Ptr >      m_interfaceTopological;
            std::vector< Interface::Ptr >      m_readOnlyInterfaces;
            std::vector< Interface::Ptr >      m_readWriteInterfaces;
            std::vector< SuperInterface::Ptr > m_superInterfaces;

            Interface::Ptr getInterface( Object::Ptr pObject ) const
            {
                for ( Interface::Ptr pInterface : m_interfaceTopological )
                {
                    if ( pInterface->m_object.lock() == pObject )
                        return pInterface;
                }
                THROW_RTE( "Failed to locate interface for object" );
            }
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
            using ConversionMap    = std::map< ObjectPartPair, ObjectPartVector, CountedObjectPairComparator< ObjectPartPair > >;

            ConversionMap m_conversions;
            ConversionMap m_upcasts;
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
            ValueType( std::size_t& szCounter )
                : Type( szCounter )
            {
            }
            using Ptr = std::shared_ptr< ValueType >;
            std::string m_cppType;

            virtual std::string getViewType( bool bAsArg ) const { return bAsArg ? toConstRef( m_cppType ) : m_cppType; }
            virtual std::string getDataType( bool bAsArg ) const
            {
                if ( m_bLate )
                    return toOptional( m_cppType );
                else if ( bAsArg )
                    return toConstRef( m_cppType );
                else
                    return m_cppType;
            }
            virtual bool isCtorParam() const { return true; }
            virtual bool isGet() const { return true; }
            virtual bool isSet() const { return true; }
            virtual bool isInsert() const { return false; }
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

            virtual std::string getViewType( bool bAsArg ) const
            {
                VERIFY_RTE( m_object );
                std::ostringstream os;
                os << m_object->m_namespace.lock()->m_strFullName << "::" << m_object->m_strName << "*";
                return os.str();
            }
            virtual std::string getDataType( bool bAsArg ) const
            {
                VERIFY_RTE( m_object );
                std::ostringstream os;
                os << "data::Ptr< data::" << m_object->m_primaryFile.lock()->m_strName << "::" << m_object->m_strName << " >";

                if ( m_bLate )
                    return toOptional( os.str() );
                else if ( bAsArg )
                    return toConstRef( os.str() );
                else
                    return os.str();
            }
            virtual bool isCtorParam() const { return true; }
            virtual bool isGet() const { return true; }
            virtual bool isSet() const { return true; }
            virtual bool isInsert() const { return false; }
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

            virtual std::string getViewType( bool bAsArg ) const
            {
                VERIFY_RTE( m_underlyingType );
                std::ostringstream os;
                os << "std::vector< " << m_underlyingType->getViewType( false ) << " >";

                if( std::dynamic_pointer_cast< RefType >( m_underlyingType ) )
                {
                    return os.str();
                }
                else
                {
                    return bAsArg ? toConstRef( os.str() ) : os.str();
                }
            }
            virtual std::string getDataType( bool bAsArg ) const
            {
                VERIFY_RTE( m_underlyingType );
                std::ostringstream os;
                os << "std::vector< " << m_underlyingType->getDataType( false ) << " >";

                if ( m_bLate )
                    return toOptional( os.str() );
                else if ( bAsArg )
                    return toConstRef( os.str() );
                else
                    return os.str();
            }
            virtual bool isCtorParam() const { return true; }
            virtual bool isGet() const { return true; }
            virtual bool isSet() const { return true; }
            virtual bool isInsert() const { return true; }
        };

        class MapType : public Type
        {
        public:
            MapType( std::size_t& szCounter )
                : Type( szCounter )
            {
            }
            using Ptr = std::shared_ptr< MapType >;
            Type::Ptr m_fromType;
            Type::Ptr m_toType;
            Type::Ptr m_predicate;

            virtual std::string getViewType( bool bAsArg ) const
            {
                VERIFY_RTE( m_fromType );
                VERIFY_RTE( m_toType );
                std::ostringstream os;
                os << "std::map< " << m_fromType->getViewType( false ) << ", " << m_toType->getViewType( false ) << " >";

                if( std::dynamic_pointer_cast< RefType >( m_fromType ) || std::dynamic_pointer_cast< RefType >( m_toType ) )
                {
                    return os.str();
                }
                else
                {
                    return bAsArg ? toConstRef( os.str() ) : os.str();
                }
            }
            virtual std::string getDataType( bool bAsArg ) const
            {
                VERIFY_RTE( m_fromType );
                VERIFY_RTE( m_toType );
                std::ostringstream os;
                os << "std::map< " << m_fromType->getDataType( false ) << ", " << m_toType->getDataType( false ) << " >";

                if ( m_bLate )
                    return toOptional( os.str() );
                else if ( bAsArg )
                    return toConstRef( os.str() );
                else
                    return os.str();
            }
            virtual bool isCtorParam() const { return true; }
            virtual bool isGet() const { return true; }
            virtual bool isSet() const { return true; }
            virtual bool isInsert() const { return true; }
        };

        Schema::Ptr from_ast( const ::db::schema::Schema& schema );

    } // namespace model
} // namespace db

#endif // DATABASE_COMPILER_MODEL_4_APRIL_2022
