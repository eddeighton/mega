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

#include "database/io/object.hpp"
#include "database/io/loader.hpp"
#include "database/io/storer.hpp"

#include <boost/filesystem/path.hpp>

#include <string>
#include <vector>
#include <optional>

namespace mega
{
class ObjectFactoryImpl;
class Parser;

namespace Stages
{
    class Parser;
}

namespace input
{
    class Element : public io::Object
    {
        friend class mega::ObjectFactoryImpl;
        friend class mega::Parser;
        friend io::Object* mega::ObjectFactoryImpl::create( const io::Object& );

    protected:
        Element( const io::Object& object );

    public:
        virtual void print( std::ostream& os, std::string& strIndent, const std::string& strAnnotation ) const {};
    };

    class Opaque : public Element
    {
        friend io::Object* mega::ObjectFactoryImpl::create( const io::Object& );
        friend class mega::Parser;
        friend class mega::Stages::Parser;

    public:
        static const ObjectType Type = eInputOpaque;

        static bool equalNullablePtrs( const Opaque* pLeft, const Opaque* pRight )
        {
            if ( pLeft && pRight )
            {
                return pLeft->getStr() == pRight->getStr();
            }
            else
            {
                return ( pLeft == nullptr ) && ( pRight == nullptr );
            }
        }

    protected:
        Opaque( const io::Object& object );

    public:
        const std::string& getStr() const { return m_str; }
        bool               isSemantic() const { return m_bSemantic; }
        virtual void       load( io::Loader& loader );
        virtual void       store( io::Storer& storer ) const;
        void               print( std::ostream& os, std::string& strIndent, const std::string& strAnnotation ) const;

        void modify( const std::string& strNew ) { m_str = strNew; }

    private:
        std::string m_str;
        bool        m_bSemantic;
    };

    //////////////////////////////////////////////////////////////////////////////
    // Utility classes
    class HasIdentifier
    {
        friend class mega::Parser;

    public:
        const std::string& getIdentifier() const { return m_strIdentifier; }

    protected:
        void load( io::Loader& loader );
        void store( io::Storer& storer ) const;

        std::string m_strIdentifier;
    };

    class HasConst
    {
        friend class mega::Parser;

    public:
        bool isConst() const { return m_bIsConst; }

    protected:
        void load( io::Loader& loader );
        void store( io::Storer& storer ) const;

        bool m_bIsConst = false;
    };

    class HasChildren
    {
        friend class mega::Parser;

    public:
        const std::vector< Element* >& getElements() const { return m_elements; }

    protected:
        void load( io::Loader& loader );
        void store( io::Storer& storer ) const;

        std::vector< Element* > m_elements;
    };

    class HasDomain
    {
        friend class mega::Parser;

    public:
        const Opaque* getSize() const { return m_pSize; }

    protected:
        void load( io::Loader& loader );
        void store( io::Storer& storer ) const;

        Opaque* m_pSize = nullptr;
    };

    class HasParameters
    {
        friend class mega::Parser;

    public:
        const Opaque* getReturnType() const { return m_pReturnType; }
        const Opaque* getParams() const { return m_pParams; }

    protected:
        void load( io::Loader& loader );
        void store( io::Storer& storer ) const;

        Opaque* m_pReturnType = nullptr;
        Opaque* m_pParams = nullptr;
    };

    class HasDefinition
    {
        friend class mega::Parser;

    public:
        std::optional< boost::filesystem::path > getSourceFile() const { return m_sourceFile; }
        std::optional< boost::filesystem::path > getDefinitionFile() const { return m_definitionFile; }

    protected:
        void load( io::Loader& loader );
        void store( io::Storer& storer ) const;

        std::optional< boost::filesystem::path > m_sourceFile;
        std::optional< boost::filesystem::path > m_definitionFile;
    };

    class HasInheritance
    {
        friend class mega::Parser;

    public:
        const std::vector< Opaque* >& getInheritance() const { return m_inheritance; }

    protected:
        void load( io::Loader& loader );
        void store( io::Storer& storer ) const;

        std::vector< Opaque* > m_inheritance;
    };

    class HasVisibility
    {
        friend class mega::Parser;

    public:
        VisibilityType getVisibility() const { return m_visibility; }

    protected:
        void load( io::Loader& loader );
        void store( io::Storer& storer ) const;

        VisibilityType m_visibility = TOTAL_VISIBILITY_TYPES;
    };

    void printDeclaration( std::ostream& os, std::string& strIndent,
                           const std::string&            strInputType,
                           const std::string&            strIdentifier,
                           const Opaque*                 pReturnType,
                           const Opaque*                 pParams,
                           const Opaque*                 pSize,
                           const std::vector< Opaque* >& inheritance,
                           const std::string&            strAnnotation );

    //////////////////////////////////////////////////////////////////////////////
    // input tree
    class Dimension : public Element, public HasIdentifier, public HasConst
    {
        friend io::Object* mega::ObjectFactoryImpl::create( const io::Object& );
        friend class mega::Parser;
        friend class mega::Stages::Parser;

    public:
        static const ObjectType Type = eInputDimension;

    protected:
        Dimension( const io::Object& object );

    public:
        const Opaque* getType() const { return m_pType; }

        virtual void load( io::Loader& loader );
        virtual void store( io::Storer& storer ) const;
        void         print( std::ostream& os, std::string& strIndent, const std::string& strAnnotation ) const;

    private:
        Opaque* m_pType;
    };

    class Include : public Element, public HasIdentifier
    {
        friend io::Object* mega::ObjectFactoryImpl::create( const io::Object& );
        friend class mega::Parser;
        friend class mega::Stages::Parser;

    public:
        static const ObjectType Type = eInputInclude;

    protected:
        Include( const io::Object& object );

    public:
        const boost::filesystem::path& getIncludeFilePath() const { return m_path; }
        bool                           isEGInclude() const { return m_bIsEGInclude; }
        bool                           isSystemInclude() const { return m_bIsSystemInclude; }

        virtual void load( io::Loader& loader );
        virtual void store( io::Storer& storer ) const;
        void         print( std::ostream& os, std::string& strIndent, const std::string& strAnnotation ) const;
        void         setIncludeFilePath( const std::string& strIncludeFile );

    private:
        boost::filesystem::path m_path;
        bool                    m_bIsEGInclude;
        bool                    m_bIsSystemInclude;
    };

    class Using : public Element, public HasIdentifier
    {
        friend io::Object* mega::ObjectFactoryImpl::create( const io::Object& );
        friend class mega::Parser;
        friend class mega::Stages::Parser;

    public:
        static const ObjectType Type = eInputUsing;

    protected:
        Using( const io::Object& object );

    public:
        const Opaque* getType() const { return m_pType; }

        virtual void load( io::Loader& loader );
        virtual void store( io::Storer& storer ) const;
        void         print( std::ostream& os, std::string& strIndent, const std::string& strAnnotation ) const;

    private:
        Opaque* m_pType;
    };

    class Export : public Element, public HasIdentifier
    {
        friend io::Object* mega::ObjectFactoryImpl::create( const io::Object& );
        friend class mega::Parser;
        friend class mega::Stages::Parser;

    public:
        static const ObjectType Type = eInputExport;

    protected:
        Export( const io::Object& object );

    public:
        const Opaque* getReturnType() const { return m_pReturnType; }
        const Opaque* getParameters() const { return m_pParameters; }

        virtual void load( io::Loader& loader );
        virtual void store( io::Storer& storer ) const;
        void         print( std::ostream& os, std::string& strIndent, const std::string& strAnnotation ) const;

    private:
        Opaque* m_pReturnType;
        Opaque* m_pParameters;
    };

    class Visibility : public Element, public HasVisibility
    {
        friend io::Object* mega::ObjectFactoryImpl::create( const io::Object& );
        friend class mega::Parser;
        friend class mega::Stages::Parser;

    public:
        static const ObjectType Type = eInputVisibility;

    protected:
        Visibility( const io::Object& object );

    public:
        virtual void load( io::Loader& loader );
        virtual void store( io::Storer& storer ) const;
        void         print( std::ostream& os, std::string& strIndent, const std::string& strAnnotation ) const;
    };

    class Context : public Element,
                    public HasIdentifier,
                    public HasChildren,
                    public HasDomain,
                    public HasDefinition,
                    public HasParameters,
                    public HasInheritance
    {
        friend io::Object* mega::ObjectFactoryImpl::create( const io::Object& );
        friend class mega::Parser;
        friend class mega::Stages::Parser;

    public:
        static const ObjectType Type = eInputContext;

    protected:
        Context( const io::Object& object );

    public:
        virtual void load( io::Loader& loader );
        virtual void store( io::Storer& storer ) const;

        Context* findContext( const std::string& strIdentifier ) const;
        void     print( std::ostream& os, std::string& strIndent, const std::string& strAnnotation ) const;

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
        friend io::Object* mega::ObjectFactoryImpl::create( const io::Object& );
        friend class mega::Parser;
        friend class mega::Stages::Parser;

    public:
        static const ObjectType  Type = eInputRoot;
        static const std::string RootTypeName;

    protected:
        Root( const io::Object& object );

    public:
        std::optional< boost::filesystem::path > getIncludePath() const { return m_includePath; }

        RootType getRootType() const { return m_rootType; }

        virtual void load( io::Loader& loader );
        virtual void store( io::Storer& storer ) const;
        void         print( std::ostream& os, std::string& strIndent, const std::string& strAnnotation ) const;

    private:
        std::optional< boost::filesystem::path > m_includePath; // null if main root
        RootType                                 m_rootType;
    };

} // namespace input
} // namespace mega

#endif // INPUT_TREE_18_04_2019