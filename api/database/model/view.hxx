#ifndef DATABASE_VIEW_GUARD_4_APRIL_2022
#define DATABASE_VIEW_GUARD_4_APRIL_2022

#include "database/io/object_info.hpp"
#include "database/io/environment.hpp"

#include <boost/filesystem/path.hpp>

#include <string>
#include <vector>
#include <optional>
#include <memory>

namespace ComponentListing
{
    //////////////////////////////////////////////////////////
    // forward declarations
    namespace Components
    {
    class Component;
    } // Components

    //////////////////////////////////////////////////////////
    // definitions
    class Database
    {
    public:
        struct Pimpl;
        Database( const mega::io::Environment& environment );

        template< typename T >
        inline T* construct();

        void store();

    private:
        Components::Component* construct_Components_Component();

    private:
        std::shared_ptr< Pimpl > m_pimpl;
    };

    template<>
    inline Components::Component* Database::construct< Components::Component >()
    {
        return construct_Components_Component();
    }

    //////////////////////////////////////////////////////////
    // definitions

    namespace Components
    {
        class Component
        {
            friend struct Database::Pimpl;
        private:
            Component( void* pData );

        public:
            // read-only properties

            // read-write properties
            const std::string& get_name() const;
            void set_name( const std::string& value ) const;
            const boost::filesystem::path& get_directory() const;
            void set_directory( const boost::filesystem::path& value ) const;
            const std::vector< boost::filesystem::path >& get_includeDirectories() const;
            void set_includeDirectories( const std::vector< boost::filesystem::path >& value ) const;
            const std::vector< boost::filesystem::path >& get_sourceFiles() const;
            void set_sourceFiles( const std::vector< boost::filesystem::path >& value ) const;
        private:
            void* m_pData;
        };
    } // Components
}
namespace InputParse
{
    //////////////////////////////////////////////////////////
    // forward declarations
    namespace Components
    { 
    class Component;
    } // Components
    namespace Parser
    {
    class Opaque;
    } // Parser
    namespace Parser
    {
    class Dimension;
    } // Parser
    namespace Parser
    {
    class Context;
    } // Parser
    namespace Parser
    {
    class Root;
    } // Parser

    //////////////////////////////////////////////////////////
    // definitions
    class Database
    {
    public:
        struct Pimpl;
        Database( const mega::io::Environment& environment, const boost::filesystem::path& objectFile );

        template< typename T >
        inline T* construct();

        void store();

    private:
        Parser::Opaque* construct_Parser_Opaque();
        Parser::Dimension* construct_Parser_Dimension();
        Parser::Context* construct_Parser_Context();
        Parser::Root* construct_Parser_Root();

    private:
        std::shared_ptr< Pimpl > m_pimpl;
    };

    template<>
    inline Parser::Opaque* Database::construct< Parser::Opaque >()
    {
        return construct_Parser_Opaque();
    }
    template<>
    inline Parser::Dimension* Database::construct< Parser::Dimension >()
    {
        return construct_Parser_Dimension();
    }
    template<>
    inline Parser::Context* Database::construct< Parser::Context >()
    {
        return construct_Parser_Context();
    }
    template<>
    inline Parser::Root* Database::construct< Parser::Root >()
    {
        return construct_Parser_Root();
    }

    //////////////////////////////////////////////////////////
    // definitions
    namespace Components
    {
        class Component
        {
            friend struct Database::Pimpl;
        private:
            Component( void* pData );
        public:
            // read-only properties
            const std::string& get_name() const;
            const boost::filesystem::path& get_directory() const;
            const std::vector< boost::filesystem::path >& get_includeDirectories() const;
            const std::vector< boost::filesystem::path >& get_sourceFiles() const;
        private:
            void* m_pData;
        };
    } // Components

    namespace Parser
    {
        class Opaque
        {
            friend struct Database::Pimpl;
        private:
            Opaque( void* pData );

        public:
            // read-only properties

            // read-write properties
            const std::string& get_str() const;
            void set_str( const std::string& value ) const;
        private:
            void* m_pData;
        };
    } // Parser
    namespace Parser
    {
        class Dimension
        {
            friend struct Database::Pimpl;
        private:
            Dimension( void* pData );

        public:
            // read-only properties

            // read-write properties
            const bool& get_isConst() const;
            void set_isConst( const bool& value ) const;
            const std::string& get_identifier() const;
            void set_identifier( const std::string& value ) const;
            const std::string& get_type() const;
            void set_type( const std::string& value ) const;
        private:
            void* m_pData;
        };
    } // Parser
    namespace Parser
    {
        class Context
        {
            friend struct Database::Pimpl;
        private:
            Context( void* pData );

        public:
            // read-only properties

            // read-write properties
            const std::string& get_identifier() const;
            void set_identifier( const std::string& value ) const;
            const std::string& get_body() const;
            void set_body( const std::string& value ) const;
        private:
            void* m_pData;
        };
    } // Parser
    namespace Parser
    {
        class Root
        {
            friend struct Database::Pimpl;
        private:
            Root( void* pData );

        public:
            // read-only properties

            // read-write properties
        private:
            void* m_pData;
        };
    } // Parser
}
namespace Interface
{
    //////////////////////////////////////////////////////////
    // forward declarations
    namespace Components
    { 
    class Component;
    } // Components
    namespace Parser
    { 
    class Opaque;
    } // Parser
    namespace Parser
    { 
    class Dimension;
    } // Parser
    namespace Parser
    { 
    class Context;
    } // Parser
    namespace Parser
    { 
    class Root;
    } // Parser
    namespace Interface
    {
    class Root;
    } // Interface

    //////////////////////////////////////////////////////////
    // definitions
    class Database
    {
    public:
        struct Pimpl;
        Database( const mega::io::Environment& environment, const boost::filesystem::path& objectFile );

        template< typename T >
        inline T* construct();

        void store();

    private:
        Interface::Root* construct_Interface_Root();

    private:
        std::shared_ptr< Pimpl > m_pimpl;
    };

    template<>
    inline Interface::Root* Database::construct< Interface::Root >()
    {
        return construct_Interface_Root();
    }

    //////////////////////////////////////////////////////////
    // definitions
    namespace Components
    {
        class Component
        {
            friend struct Database::Pimpl;
        private:
            Component( void* pData );
        public:
            // read-only properties
            const std::string& get_name() const;
            const boost::filesystem::path& get_directory() const;
            const std::vector< boost::filesystem::path >& get_includeDirectories() const;
            const std::vector< boost::filesystem::path >& get_sourceFiles() const;
        private:
            void* m_pData;
        };
    } // Components
    namespace Parser
    {
        class Opaque
        {
            friend struct Database::Pimpl;
        private:
            Opaque( void* pData );
        public:
            // read-only properties
            const std::string& get_str() const;
        private:
            void* m_pData;
        };
    } // Parser
    namespace Parser
    {
        class Dimension
        {
            friend struct Database::Pimpl;
        private:
            Dimension( void* pData );
        public:
            // read-only properties
            const bool& get_isConst() const;
            const std::string& get_identifier() const;
            const std::string& get_type() const;
        private:
            void* m_pData;
        };
    } // Parser
    namespace Parser
    {
        class Context
        {
            friend struct Database::Pimpl;
        private:
            Context( void* pData );
        public:
            // read-only properties
            const std::string& get_identifier() const;
            const std::string& get_body() const;
        private:
            void* m_pData;
        };
    } // Parser
    namespace Parser
    {
        class Root
        {
            friend struct Database::Pimpl;
        private:
            Root( void* pData );
        public:
            // read-only properties
        private:
            void* m_pData;
        };
    } // Parser

    namespace Interface
    {
        class Root
        {
            friend struct Database::Pimpl;
        private:
            Root( void* pData );

        public:
            // read-only properties

            // read-write properties
        private:
            void* m_pData;
        };
    } // Interface
}
namespace Dependencies
{
    //////////////////////////////////////////////////////////
    // forward declarations
    namespace Components
    { 
    class Component;
    } // Components
    namespace Parser
    { 
    class Opaque;
    } // Parser
    namespace Parser
    { 
    class Dimension;
    } // Parser
    namespace Parser
    { 
    class Context;
    } // Parser
    namespace Parser
    { 
    class Root;
    } // Parser
    namespace Interface
    { 
    class Root;
    } // Interface

    //////////////////////////////////////////////////////////
    // definitions
    class Database
    {
    public:
        struct Pimpl;
        Database( const mega::io::Environment& environment );

        template< typename T >
        inline T* construct();

        void store();

    private:

    private:
        std::shared_ptr< Pimpl > m_pimpl;
    };


    //////////////////////////////////////////////////////////
    // definitions
    namespace Components
    {
        class Component
        {
            friend struct Database::Pimpl;
        private:
            Component( void* pData );
        public:
            // read-only properties
            const std::string& get_name() const;
            const boost::filesystem::path& get_directory() const;
            const std::vector< boost::filesystem::path >& get_includeDirectories() const;
            const std::vector< boost::filesystem::path >& get_sourceFiles() const;
        private:
            void* m_pData;
        };
    } // Components
    namespace Parser
    {
        class Opaque
        {
            friend struct Database::Pimpl;
        private:
            Opaque( void* pData );
        public:
            // read-only properties
            const std::string& get_str() const;
        private:
            void* m_pData;
        };
    } // Parser
    namespace Parser
    {
        class Dimension
        {
            friend struct Database::Pimpl;
        private:
            Dimension( void* pData );
        public:
            // read-only properties
            const bool& get_isConst() const;
            const std::string& get_identifier() const;
            const std::string& get_type() const;
        private:
            void* m_pData;
        };
    } // Parser
    namespace Parser
    {
        class Context
        {
            friend struct Database::Pimpl;
        private:
            Context( void* pData );
        public:
            // read-only properties
            const std::string& get_identifier() const;
            const std::string& get_body() const;
        private:
            void* m_pData;
        };
    } // Parser
    namespace Parser
    {
        class Root
        {
            friend struct Database::Pimpl;
        private:
            Root( void* pData );
        public:
            // read-only properties
        private:
            void* m_pData;
        };
    } // Parser
    namespace Interface
    {
        class Root
        {
            friend struct Database::Pimpl;
        private:
            Root( void* pData );
        public:
            // read-only properties
        private:
            void* m_pData;
        };
    } // Interface

}
namespace Analysis
{
    //////////////////////////////////////////////////////////
    // forward declarations
    namespace Components
    { 
    class Component;
    } // Components
    namespace Parser
    { 
    class Opaque;
    } // Parser
    namespace Parser
    { 
    class Dimension;
    } // Parser
    namespace Parser
    { 
    class Context;
    } // Parser
    namespace Parser
    { 
    class Root;
    } // Parser
    namespace Interface
    { 
    class Root;
    } // Interface

    //////////////////////////////////////////////////////////
    // definitions
    class Database
    {
    public:
        struct Pimpl;
        Database( const mega::io::Environment& environment, const boost::filesystem::path& objectFile );

        template< typename T >
        inline T* construct();

        void store();

    private:

    private:
        std::shared_ptr< Pimpl > m_pimpl;
    };


    //////////////////////////////////////////////////////////
    // definitions
    namespace Components
    {
        class Component
        {
            friend struct Database::Pimpl;
        private:
            Component( void* pData );
        public:
            // read-only properties
            const std::string& get_name() const;
            const boost::filesystem::path& get_directory() const;
            const std::vector< boost::filesystem::path >& get_includeDirectories() const;
            const std::vector< boost::filesystem::path >& get_sourceFiles() const;
        private:
            void* m_pData;
        };
    } // Components
    namespace Parser
    {
        class Opaque
        {
            friend struct Database::Pimpl;
        private:
            Opaque( void* pData );
        public:
            // read-only properties
            const std::string& get_str() const;
        private:
            void* m_pData;
        };
    } // Parser
    namespace Parser
    {
        class Dimension
        {
            friend struct Database::Pimpl;
        private:
            Dimension( void* pData );
        public:
            // read-only properties
            const bool& get_isConst() const;
            const std::string& get_identifier() const;
            const std::string& get_type() const;
        private:
            void* m_pData;
        };
    } // Parser
    namespace Parser
    {
        class Context
        {
            friend struct Database::Pimpl;
        private:
            Context( void* pData );
        public:
            // read-only properties
            const std::string& get_identifier() const;
            const std::string& get_body() const;
        private:
            void* m_pData;
        };
    } // Parser
    namespace Parser
    {
        class Root
        {
            friend struct Database::Pimpl;
        private:
            Root( void* pData );
        public:
            // read-only properties
        private:
            void* m_pData;
        };
    } // Parser
    namespace Interface
    {
        class Root
        {
            friend struct Database::Pimpl;
        private:
            Root( void* pData );
        public:
            // read-only properties
        private:
            void* m_pData;
        };
    } // Interface

}
namespace Concrete
{
    //////////////////////////////////////////////////////////
    // forward declarations
    namespace Components
    { 
    class Component;
    } // Components
    namespace Parser
    { 
    class Opaque;
    } // Parser
    namespace Parser
    { 
    class Dimension;
    } // Parser
    namespace Parser
    { 
    class Context;
    } // Parser
    namespace Parser
    { 
    class Root;
    } // Parser
    namespace Interface
    { 
    class Root;
    } // Interface

    //////////////////////////////////////////////////////////
    // definitions
    class Database
    {
    public:
        struct Pimpl;
        Database( const mega::io::Environment& environment );

        template< typename T >
        inline T* construct();

        void store();

    private:

    private:
        std::shared_ptr< Pimpl > m_pimpl;
    };


    //////////////////////////////////////////////////////////
    // definitions
    namespace Components
    {
        class Component
        {
            friend struct Database::Pimpl;
        private:
            Component( void* pData );
        public:
            // read-only properties
            const std::string& get_name() const;
            const boost::filesystem::path& get_directory() const;
            const std::vector< boost::filesystem::path >& get_includeDirectories() const;
            const std::vector< boost::filesystem::path >& get_sourceFiles() const;
        private:
            void* m_pData;
        };
    } // Components
    namespace Parser
    {
        class Opaque
        {
            friend struct Database::Pimpl;
        private:
            Opaque( void* pData );
        public:
            // read-only properties
            const std::string& get_str() const;
        private:
            void* m_pData;
        };
    } // Parser
    namespace Parser
    {
        class Dimension
        {
            friend struct Database::Pimpl;
        private:
            Dimension( void* pData );
        public:
            // read-only properties
            const bool& get_isConst() const;
            const std::string& get_identifier() const;
            const std::string& get_type() const;
        private:
            void* m_pData;
        };
    } // Parser
    namespace Parser
    {
        class Context
        {
            friend struct Database::Pimpl;
        private:
            Context( void* pData );
        public:
            // read-only properties
            const std::string& get_identifier() const;
            const std::string& get_body() const;
        private:
            void* m_pData;
        };
    } // Parser
    namespace Parser
    {
        class Root
        {
            friend struct Database::Pimpl;
        private:
            Root( void* pData );
        public:
            // read-only properties
        private:
            void* m_pData;
        };
    } // Parser
    namespace Interface
    {
        class Root
        {
            friend struct Database::Pimpl;
        private:
            Root( void* pData );
        public:
            // read-only properties
        private:
            void* m_pData;
        };
    } // Interface

}
namespace Layout
{
    //////////////////////////////////////////////////////////
    // forward declarations
    namespace Components
    { 
    class Component;
    } // Components
    namespace Parser
    { 
    class Opaque;
    } // Parser
    namespace Parser
    { 
    class Dimension;
    } // Parser
    namespace Parser
    { 
    class Context;
    } // Parser
    namespace Parser
    { 
    class Root;
    } // Parser
    namespace Interface
    { 
    class Root;
    } // Interface

    //////////////////////////////////////////////////////////
    // definitions
    class Database
    {
    public:
        struct Pimpl;
        Database( const mega::io::Environment& environment );

        template< typename T >
        inline T* construct();

        void store();

    private:

    private:
        std::shared_ptr< Pimpl > m_pimpl;
    };


    //////////////////////////////////////////////////////////
    // definitions
    namespace Components
    {
        class Component
        {
            friend struct Database::Pimpl;
        private:
            Component( void* pData );
        public:
            // read-only properties
            const std::string& get_name() const;
            const boost::filesystem::path& get_directory() const;
            const std::vector< boost::filesystem::path >& get_includeDirectories() const;
            const std::vector< boost::filesystem::path >& get_sourceFiles() const;
        private:
            void* m_pData;
        };
    } // Components
    namespace Parser
    {
        class Opaque
        {
            friend struct Database::Pimpl;
        private:
            Opaque( void* pData );
        public:
            // read-only properties
            const std::string& get_str() const;
        private:
            void* m_pData;
        };
    } // Parser
    namespace Parser
    {
        class Dimension
        {
            friend struct Database::Pimpl;
        private:
            Dimension( void* pData );
        public:
            // read-only properties
            const bool& get_isConst() const;
            const std::string& get_identifier() const;
            const std::string& get_type() const;
        private:
            void* m_pData;
        };
    } // Parser
    namespace Parser
    {
        class Context
        {
            friend struct Database::Pimpl;
        private:
            Context( void* pData );
        public:
            // read-only properties
            const std::string& get_identifier() const;
            const std::string& get_body() const;
        private:
            void* m_pData;
        };
    } // Parser
    namespace Parser
    {
        class Root
        {
            friend struct Database::Pimpl;
        private:
            Root( void* pData );
        public:
            // read-only properties
        private:
            void* m_pData;
        };
    } // Parser
    namespace Interface
    {
        class Root
        {
            friend struct Database::Pimpl;
        private:
            Root( void* pData );
        public:
            // read-only properties
        private:
            void* m_pData;
        };
    } // Interface

}
namespace HyperGraph
{
    //////////////////////////////////////////////////////////
    // forward declarations
    namespace Components
    { 
    class Component;
    } // Components
    namespace Parser
    { 
    class Opaque;
    } // Parser
    namespace Parser
    { 
    class Dimension;
    } // Parser
    namespace Parser
    { 
    class Context;
    } // Parser
    namespace Parser
    { 
    class Root;
    } // Parser
    namespace Interface
    { 
    class Root;
    } // Interface

    //////////////////////////////////////////////////////////
    // definitions
    class Database
    {
    public:
        struct Pimpl;
        Database( const mega::io::Environment& environment );

        template< typename T >
        inline T* construct();

        void store();

    private:

    private:
        std::shared_ptr< Pimpl > m_pimpl;
    };


    //////////////////////////////////////////////////////////
    // definitions
    namespace Components
    {
        class Component
        {
            friend struct Database::Pimpl;
        private:
            Component( void* pData );
        public:
            // read-only properties
            const std::string& get_name() const;
            const boost::filesystem::path& get_directory() const;
            const std::vector< boost::filesystem::path >& get_includeDirectories() const;
            const std::vector< boost::filesystem::path >& get_sourceFiles() const;
        private:
            void* m_pData;
        };
    } // Components
    namespace Parser
    {
        class Opaque
        {
            friend struct Database::Pimpl;
        private:
            Opaque( void* pData );
        public:
            // read-only properties
            const std::string& get_str() const;
        private:
            void* m_pData;
        };
    } // Parser
    namespace Parser
    {
        class Dimension
        {
            friend struct Database::Pimpl;
        private:
            Dimension( void* pData );
        public:
            // read-only properties
            const bool& get_isConst() const;
            const std::string& get_identifier() const;
            const std::string& get_type() const;
        private:
            void* m_pData;
        };
    } // Parser
    namespace Parser
    {
        class Context
        {
            friend struct Database::Pimpl;
        private:
            Context( void* pData );
        public:
            // read-only properties
            const std::string& get_identifier() const;
            const std::string& get_body() const;
        private:
            void* m_pData;
        };
    } // Parser
    namespace Parser
    {
        class Root
        {
            friend struct Database::Pimpl;
        private:
            Root( void* pData );
        public:
            // read-only properties
        private:
            void* m_pData;
        };
    } // Parser
    namespace Interface
    {
        class Root
        {
            friend struct Database::Pimpl;
        private:
            Root( void* pData );
        public:
            // read-only properties
        private:
            void* m_pData;
        };
    } // Interface

}
namespace Derivations
{
    //////////////////////////////////////////////////////////
    // forward declarations
    namespace Components
    { 
    class Component;
    } // Components
    namespace Parser
    { 
    class Opaque;
    } // Parser
    namespace Parser
    { 
    class Dimension;
    } // Parser
    namespace Parser
    { 
    class Context;
    } // Parser
    namespace Parser
    { 
    class Root;
    } // Parser
    namespace Interface
    { 
    class Root;
    } // Interface

    //////////////////////////////////////////////////////////
    // definitions
    class Database
    {
    public:
        struct Pimpl;
        Database( const mega::io::Environment& environment );

        template< typename T >
        inline T* construct();

        void store();

    private:

    private:
        std::shared_ptr< Pimpl > m_pimpl;
    };


    //////////////////////////////////////////////////////////
    // definitions
    namespace Components
    {
        class Component
        {
            friend struct Database::Pimpl;
        private:
            Component( void* pData );
        public:
            // read-only properties
            const std::string& get_name() const;
            const boost::filesystem::path& get_directory() const;
            const std::vector< boost::filesystem::path >& get_includeDirectories() const;
            const std::vector< boost::filesystem::path >& get_sourceFiles() const;
        private:
            void* m_pData;
        };
    } // Components
    namespace Parser
    {
        class Opaque
        {
            friend struct Database::Pimpl;
        private:
            Opaque( void* pData );
        public:
            // read-only properties
            const std::string& get_str() const;
        private:
            void* m_pData;
        };
    } // Parser
    namespace Parser
    {
        class Dimension
        {
            friend struct Database::Pimpl;
        private:
            Dimension( void* pData );
        public:
            // read-only properties
            const bool& get_isConst() const;
            const std::string& get_identifier() const;
            const std::string& get_type() const;
        private:
            void* m_pData;
        };
    } // Parser
    namespace Parser
    {
        class Context
        {
            friend struct Database::Pimpl;
        private:
            Context( void* pData );
        public:
            // read-only properties
            const std::string& get_identifier() const;
            const std::string& get_body() const;
        private:
            void* m_pData;
        };
    } // Parser
    namespace Parser
    {
        class Root
        {
            friend struct Database::Pimpl;
        private:
            Root( void* pData );
        public:
            // read-only properties
        private:
            void* m_pData;
        };
    } // Parser
    namespace Interface
    {
        class Root
        {
            friend struct Database::Pimpl;
        private:
            Root( void* pData );
        public:
            // read-only properties
        private:
            void* m_pData;
        };
    } // Interface

}

#endif //DATABASE_VIEW_GUARD_4_APRIL_2022
