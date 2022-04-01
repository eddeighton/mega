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

#ifndef INPUT_TREE_18_04_2019
#define INPUT_TREE_18_04_2019

#include "objects.hpp"
#include "component.hpp"

#include "database/io/stages.hpp"
#include "database/io/object.hpp"
#include "database/io/loader.hpp"
#include "database/io/storer.hpp"

#include <boost/filesystem/path.hpp>

#include <string>
#include <vector>
#include <optional>

namespace mega
{
class Parser;

namespace Stages
{
    class Parser;
}

namespace input
{

    class Element : public io::FileObject< io::file::ObjectAST >
    {
        friend class io::File;
        using Base = io::FileObject< io::file::ObjectAST >;

    protected:
        Element( const io::ObjectInfo& object );

    public:
        virtual void print( std::ostream& os, std::string& strIndent,
                            const std::string& strAnnotation ) const {};
    };

    class Opaque : public Element
    {
        friend class io::Factory;
        friend class io::File;

    public:
        static const ObjectType Type = eInputOpaque;

    protected:
        Opaque( const io::ObjectInfo& object );
        Opaque( const io::ObjectInfo& object, const std::string& strOpaque );

    public:
        const std::string& getStr() const { return m_str; }
        virtual void       load( io::Loader& loader );
        virtual void       store( io::Storer& storer ) const;
        void               print( std::ostream& os, std::string& strIndent,
                                  const std::string& strAnnotation ) const;

    public:
        std::string m_str;
    };

    //////////////////////////////////////////////////////////////////////////////
    // Utility classes
    class HasIdentifier
    {
        friend class io::File;

    public:
        HasIdentifier();
        HasIdentifier( const std::string& strIdentifier );
        const std::string& getIdentifier() const { return m_strIdentifier; }

    protected:
        void load( io::Loader& loader );
        void store( io::Storer& storer ) const;

    public:
        std::string m_strIdentifier;
    };

    class HasConst
    {
        friend class io::File;

    public:
        bool isConst() const { return m_bIsConst; }

    protected:
        void load( io::Loader& loader );
        void store( io::Storer& storer ) const;

    public:
        bool m_bIsConst = false;
    };

    class HasChildren
    {
        friend class io::File;

    public:
        const std::vector< Element* >& getElements() const { return m_elements; }

    protected:
        void load( io::Loader& loader );
        void store( io::Storer& storer ) const;

    public:
        std::vector< Element* > m_elements;
    };

    class HasDomain
    {
        friend class io::File;

    public:
        const Opaque* getSize() const { return m_pSize; }

    protected:
        void load( io::Loader& loader );
        void store( io::Storer& storer ) const;

    public:
        Opaque* m_pSize = nullptr;
    };

    class HasParameters
    {
        friend class io::File;

    public:
        const Opaque* getReturnType() const { return m_pReturnType; }
        const Opaque* getParams() const { return m_pParams; }

    protected:
        void load( io::Loader& loader );
        void store( io::Storer& storer ) const;

    public:
        Opaque* m_pReturnType = nullptr;
        Opaque* m_pParams = nullptr;
    };
    /*
        class HasDefinition
        {
            friend class io::File;

        public:
            std::optional< boost::filesystem::path > getSourceFile() const { return m_sourceFile; }
            std::optional< boost::filesystem::path > getDefinitionFile() const
            {
                return m_definitionFile;
            }

        protected:
            void load( io::Loader& loader );
            void store( io::Storer& storer ) const;

        public:
            std::optional< boost::filesystem::path > m_sourceFile;
            std::optional< boost::filesystem::path > m_definitionFile;
        };
    */
    class HasInheritance
    {
        friend class io::File;

    public:
        const std::vector< Opaque* >& getInheritance() const { return m_inheritance; }

    protected:
        void load( io::Loader& loader );
        void store( io::Storer& storer ) const;

    public:
        std::vector< Opaque* > m_inheritance;
    };

    class HasVisibility
    {
        friend class io::File;

    public:
        VisibilityType getVisibility() const { return m_visibility; }

    protected:
        void load( io::Loader& loader );
        void store( io::Storer& storer ) const;

    public:
        VisibilityType m_visibility = TOTAL_VISIBILITY_TYPES;
    };

    void printDeclaration( std::ostream& os, std::string& strIndent,
                           const std::string& strInputType, const std::string& strIdentifier,
                           const Opaque* pReturnType, const Opaque* pParams, const Opaque* pSize,
                           const std::vector< Opaque* >& inheritance,
                           const std::string&            strAnnotation );

    //////////////////////////////////////////////////////////////////////////////
    // input tree
    class Dimension : public Element, public HasIdentifier, public HasConst
    {
        friend class io::Factory;
        friend class io::File;

    public:
        static const ObjectType Type = eInputDimension;

    protected:
        Dimension( const io::ObjectInfo& object );

    public:
        const Opaque* getType() const { return m_pType; }

        virtual void load( io::Loader& loader );
        virtual void store( io::Storer& storer ) const;
        void         print( std::ostream& os, std::string& strIndent,
                            const std::string& strAnnotation ) const;

    public:
        Opaque* m_pType;
    };

    class Include : public Element, public HasIdentifier
    {
        friend class io::Factory;
        friend class io::File;

    protected:
        Include( const io::ObjectInfo& object );
        Include( const io::ObjectInfo& object, const std::string& strIdenfier,
                 const std::string& strOpaque, const boost::filesystem::path& includeFilePath );

    public:
        const boost::filesystem::path& getIncludeFilePath() const { return m_path; }

        virtual void load( io::Loader& loader );
        virtual void store( io::Storer& storer ) const;
        void         print( std::ostream& os, std::string& strIndent,
                            const std::string& strAnnotation ) const;

    public:
        std::string             m_strOpaque;
        boost::filesystem::path m_path;
    };

    class Root;
    class MegaInclude : public Include
    {
        friend class io::Factory;
        friend class io::File;

    public:
        static const ObjectType Type = eInputMegaInclude;

    protected:
        MegaInclude( const io::ObjectInfo& object );
        MegaInclude( const io::ObjectInfo& object, const std::string& strIdenfier,
                     const std::string& strOpaque, const boost::filesystem::path& includeFilePath );

        virtual void load( io::Loader& loader );
        virtual void store( io::Storer& storer ) const;

    public:
        Root* m_pIncludeRoot = nullptr;
    };

    class CPPInclude : public Include
    {
        friend class io::Factory;
        friend class io::File;

    public:
        static const ObjectType Type = eInputCPPInclude;

    protected:
        CPPInclude( const io::ObjectInfo& object );
        CPPInclude( const io::ObjectInfo& object, const std::string& strIdenfier,
                    const std::string& strOpaque, const boost::filesystem::path& includeFilePath );

        virtual void load( io::Loader& loader );
        virtual void store( io::Storer& storer ) const;
    };

    class SystemInclude : public Include
    {
        friend class io::Factory;
        friend class io::File;

    public:
        static const ObjectType Type = eInputSystemInclude;

    protected:
        SystemInclude( const io::ObjectInfo& object );
        SystemInclude( const io::ObjectInfo& object, const std::string& strIdenfier,
                       const std::string&             strOpaque,
                       const boost::filesystem::path& includeFilePath );

        virtual void load( io::Loader& loader );
        virtual void store( io::Storer& storer ) const;
    };

    class Dependency : public Element, public HasIdentifier
    {
        friend class io::Factory;
        friend class io::File;

    public:
        static const ObjectType Type = eDependency;

    protected:
        Dependency( const io::ObjectInfo& object );
        Dependency( const io::ObjectInfo& object, const std::string& strIdenfier,
                    const Opaque* pImport );

    public:
        const Opaque* getOpaque() const { return m_pDependency; }

        virtual void load( io::Loader& loader );
        virtual void store( io::Storer& storer ) const;
        void         print( std::ostream& os, std::string& strIndent,
                            const std::string& strAnnotation ) const;

    public:
        const Opaque* m_pDependency;
    };

    class Using : public Element, public HasIdentifier
    {
        friend class io::Factory;
        friend class io::File;

    public:
        static const ObjectType Type = eInputUsing;

    protected:
        Using( const io::ObjectInfo& object );

    public:
        const Opaque* getType() const { return m_pType; }

        virtual void load( io::Loader& loader );
        virtual void store( io::Storer& storer ) const;
        void         print( std::ostream& os, std::string& strIndent,
                            const std::string& strAnnotation ) const;

    public:
        Opaque* m_pType;
    };

    class Export : public Element, public HasIdentifier
    {
        friend class io::Factory;
        friend class io::File;

    public:
        static const ObjectType Type = eInputExport;

    protected:
        Export( const io::ObjectInfo& object );

    public:
        const Opaque* getReturnType() const { return m_pReturnType; }
        const Opaque* getParameters() const { return m_pParameters; }

        virtual void load( io::Loader& loader );
        virtual void store( io::Storer& storer ) const;
        void         print( std::ostream& os, std::string& strIndent,
                            const std::string& strAnnotation ) const;

    public:
        Opaque* m_pReturnType;
        Opaque* m_pParameters;
    };

    class Visibility : public Element, public HasVisibility
    {
        friend class io::Factory;
        friend class io::File;

    public:
        static const ObjectType Type = eInputVisibility;

    protected:
        Visibility( const io::ObjectInfo& object );

    public:
        virtual void load( io::Loader& loader );
        virtual void store( io::Storer& storer ) const;
        void         print( std::ostream& os, std::string& strIndent,
                            const std::string& strAnnotation ) const;
    };

    class Context : public Element,
                    public HasIdentifier,
                    public HasChildren,
                    public HasDomain,
                    // public HasDefinition,
                    public HasParameters,
                    public HasInheritance
    {
        friend class io::Factory;
        friend class io::File;

    public:
        static const ObjectType Type = eInputContext;

    protected:
        Context( const io::ObjectInfo& object );

    public:
        virtual void load( io::Loader& loader );
        virtual void store( io::Storer& storer ) const;

        Context* findContext( const std::string& strIdentifier ) const;
        void     print( std::ostream& os, std::string& strIndent,
                        const std::string& strAnnotation ) const;

        const char* getContextType() const;

        enum ContextType
        {
            eUnknown,
            eAbstract,
            eEvent,
            eFunction,
            eAction,
            eLink,
            eObject
        };
        ContextType m_contextType = eUnknown;
    };

    class Root : public Context
    {
        friend class io::Factory;
        friend class io::File;

    public:
        static const ObjectType  Type = eInputRoot;
        static const std::string RootTypeName;

    protected:
        Root( const io::ObjectInfo& object );
        Root( const io::ObjectInfo& object, const mega::Component* pComponent,
              const boost::filesystem::path& filePath, RootType rootType );

    public:
        const boost::filesystem::path& getFilePath() const { return m_filePath; }

        RootType getRootType() const { return m_rootType; }

        virtual void load( io::Loader& loader );
        virtual void store( io::Storer& storer ) const;
        void         print( std::ostream& os, std::string& strIndent,
                            const std::string& strAnnotation ) const;

    public:
        const mega::Component*  m_pComponent;
        boost::filesystem::path m_filePath;
        RootType                m_rootType;
    };

    class Body : public io::FileObject< io::file::ObjectBody >
    {
        friend class io::Factory;
        friend class io::File;
        using Base = io::FileObject< io::file::ObjectBody >;

    protected:
        Body( const io::ObjectInfo& object );

    public:
        static const ObjectType Type = eInputBody;
        virtual void            load( io::Loader& loader );
        virtual void            store( io::Storer& storer ) const;

    public:
        std::string m_str;
        Element*    m_pContext;
    };

} // namespace input
} // namespace mega

#endif // INPUT_TREE_18_04_2019