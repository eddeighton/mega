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

#include "database/model/input.hpp"
#include "database/model/eg.hpp"

#include "common/file.hpp"

namespace mega
{
namespace input
{

    HasIdentifier::HasIdentifier()
    {
    }

    HasIdentifier::HasIdentifier( const std::string& strIdentifier )
        : m_strIdentifier( strIdentifier )
    {
    }

    void HasIdentifier::load( io::Loader& loader )
    {
        loader.load( m_strIdentifier );
    }

    void HasIdentifier::store( io::Storer& storer ) const
    {
        storer.store( m_strIdentifier );
    }

    void HasConst::load( io::Loader& loader )
    {
        loader.load( m_bIsConst );
    }

    void HasConst::store( io::Storer& storer ) const
    {
        storer.store( m_bIsConst );
    }

    void HasChildren::load( io::Loader& loader )
    {
        loader.loadObjectVector( m_elements );
    }

    void HasChildren::store( io::Storer& storer ) const
    {
        storer.storeObjectVector( m_elements );
    }

    void HasDomain::load( io::Loader& loader )
    {
        m_pSize = loader.loadOptionalObjectRef< Opaque >();
    }

    void HasDomain::store( io::Storer& storer ) const
    {
        storer.storeOptionalObjectRef( m_pSize );
    }

    void HasParameters::load( io::Loader& loader )
    {
        m_pReturnType = loader.loadOptionalObjectRef< Opaque >();
        m_pParams = loader.loadOptionalObjectRef< Opaque >();
    }

    void HasParameters::store( io::Storer& storer ) const
    {
        storer.storeOptionalObjectRef( m_pReturnType );
        storer.storeOptionalObjectRef( m_pParams );
    }

    void HasDefinition::load( io::Loader& loader )
    {
        loader.loadOptional( m_sourceFile );
        loader.loadOptional( m_definitionFile );
    }

    void HasDefinition::store( io::Storer& storer ) const
    {
        storer.storeOptional( m_sourceFile );
        storer.storeOptional( m_definitionFile );
    }

    void HasInheritance::load( io::Loader& loader )
    {
        loader.loadObjectVector( m_inheritance );
    }

    void HasInheritance::store( io::Storer& storer ) const
    {
        storer.storeObjectVector( m_inheritance );
    }

    void HasVisibility::load( io::Loader& loader )
    {
        loader.load( m_visibility );
    }

    void HasVisibility::store( io::Storer& storer ) const
    {
        storer.store( m_visibility );
    }

    void printDeclaration( std::ostream&                 os,
                           std::string&                  strIndent,
                           const std::string&            strInputType,
                           const std::string&            strIdentifier,
                           const Opaque*                 pReturnType,
                           const Opaque*                 pParams,
                           const Opaque*                 pSize,
                           const std::vector< Opaque* >& inheritance,
                           const std::string&            strAnnotation )
    {
        os << strIndent << strInputType << " " << strIdentifier;

        if ( pParams )
        {
            os << "( " << pParams->getStr() << " )";
        }

        if ( pSize )
        {
            os << "[ " << pSize->getStr() << " ]";
        }

        if ( pReturnType )
        {
            os << " : " << pReturnType->getStr();
        }

        for ( const Opaque* pInherited : inheritance )
        {
            if ( pInherited == inheritance.front() )
                os << " : " << pInherited->getStr();
            else
                os << ", " << pInherited->getStr();
        }

        os << " //" << strAnnotation;
    }

    Element::Element( const io::ObjectInfo& object )
        : Base( object )
    {
    }

    Opaque::Opaque( const io::ObjectInfo& object )
        : Element( object )
    {
    }

    Opaque::Opaque( const io::ObjectInfo& object, const std::string& strOpaque )
        : Element( object )
        , m_str( strOpaque )
    {
    }

    void Opaque::load( io::Loader& loader )
    {
        loader.load( m_str );
    }

    void Opaque::store( io::Storer& storer ) const
    {
        storer.store( m_str );
    }

    void Opaque::print( std::ostream& os, std::string& strIndent, const std::string& strAnnotation ) const
    {
        os << strIndent << m_str << "\n"; // << "//" << szIndex;
    }

    Dimension::Dimension( const io::ObjectInfo& object )
        : Element( object )
        , m_pType( nullptr )
    {
    }

    void Dimension::load( io::Loader& loader )
    {
        HasIdentifier::load( loader );
        HasConst::load( loader );
        m_pType = loader.loadObjectRef< Opaque >();
    }

    void Dimension::store( io::Storer& storer ) const
    {
        HasIdentifier::store( storer );
        HasConst::store( storer );
        storer.storeObjectRef( m_pType );
    }

    void Dimension::print( std::ostream& os, std::string& strIndent, const std::string& strAnnotation ) const
    {
        VERIFY_RTE( m_pType );
        os << strIndent;
        if ( m_bIsConst )
            os << "const ";
        os << "dim " << m_pType->getStr() << " " << m_strIdentifier << ";//" << strAnnotation << "\n";
    }

    Include::Include( const io::ObjectInfo& object )
        : Element( object )
    {
    }
    Include::Include( const io::ObjectInfo& object, const std::string& strIdenfier, const std::string& strOpaque, const boost::filesystem::path& includeFilePath )
        : Element( object )
        , HasIdentifier( strIdenfier )
        , m_strOpaque( strOpaque )
        , m_path( includeFilePath )
    {
    }

    void Include::load( io::Loader& loader )
    {
        HasIdentifier::load( loader );
        loader.load( m_strOpaque );
        loader.load( m_path );
    }

    void Include::store( io::Storer& storer ) const
    {
        HasIdentifier::store( storer );
        storer.store( m_strOpaque );
        storer.store( m_path );
    }

    void Include::print( std::ostream& os, std::string& strIndent, const std::string& strAnnotation ) const
    {
        if ( m_strIdentifier.empty() )
            os << strIndent << "include( " << m_strOpaque << " );//" << strAnnotation << "\n";
        else
            os << strIndent << "include " << m_strIdentifier << "( " << m_strOpaque << " );//" << strAnnotation << "\n";
    }

    MegaInclude::MegaInclude( const io::ObjectInfo& object )
        : Include( object )
    {
    }
    MegaInclude::MegaInclude( const io::ObjectInfo& object, const std::string& strIdenfier, const std::string& strOpaque, const boost::filesystem::path& includeFilePath )
        : Include( object, strIdenfier, strOpaque, includeFilePath )
    {
    }
    void MegaInclude::load( io::Loader& loader )
    {
        Include::load( loader );
    }

    void MegaInclude::store( io::Storer& storer ) const
    {
        Include::store( storer );
    }

    CPPInclude::CPPInclude( const io::ObjectInfo& object )
        : Include( object )
    {
    }
    CPPInclude::CPPInclude( const io::ObjectInfo& object, const std::string& strIdenfier, const std::string& strOpaque, const boost::filesystem::path& includeFilePath )
        : Include( object, strIdenfier, strOpaque, includeFilePath )
    {
    }
    void CPPInclude::load( io::Loader& loader )
    {
        Include::load( loader );
    }

    void CPPInclude::store( io::Storer& storer ) const
    {
        Include::store( storer );
    }

    SystemInclude::SystemInclude( const io::ObjectInfo& object )
        : Include( object )
    {
    }
    SystemInclude::SystemInclude( const io::ObjectInfo& object, const std::string& strIdenfier, const std::string& strOpaque, const boost::filesystem::path& includeFilePath )
        : Include( object, strIdenfier, strOpaque, includeFilePath )
    {
    }
    void SystemInclude::load( io::Loader& loader )
    {
        Include::load( loader );
    }

    void SystemInclude::store( io::Storer& storer ) const
    {
        Include::store( storer );
    }

    Import::Import( const io::ObjectInfo& object )
        : Element( object )
        , m_pImport( nullptr )
    {
    }

    Import::Import( const io::ObjectInfo& object, const std::string& strIdenfier, const Opaque* pImport )
        : Element( object )
        , HasIdentifier( strIdenfier )
        , m_pImport( pImport )
    {
    }

    void Import::load( io::Loader& loader )
    {
        HasIdentifier::load( loader );
        m_pImport = loader.loadObjectRef< Opaque >();
    }

    void Import::store( io::Storer& storer ) const
    {
        HasIdentifier::store( storer );
        storer.storeObjectRef( m_pImport );
    }

    void Import::print( std::ostream& os, std::string& strIndent, const std::string& strAnnotation ) const
    {
        VERIFY_RTE( m_pImport );
        if ( m_strIdentifier.empty() )
            os << strIndent << "import( " << m_pImport->getStr() << " );//" << strAnnotation << "\n";
        else
            os << strIndent << "import " << m_strIdentifier << "( " << m_pImport->getStr() << " );//" << strAnnotation << "\n";
    }

    Using::Using( const io::ObjectInfo& object )
        : Element( object )
        , m_pType( nullptr )
    {
    }

    void Using::load( io::Loader& loader )
    {
        HasIdentifier::load( loader );
        m_pType = loader.loadObjectRef< Opaque >();
    }

    void Using::store( io::Storer& storer ) const
    {
        HasIdentifier::store( storer );
        storer.storeObjectRef( m_pType );
    }

    void Using::print( std::ostream& os, std::string& strIndent, const std::string& strAnnotation ) const
    {
        VERIFY_RTE( m_pType );
        os << strIndent << "using " << m_strIdentifier << " = " << m_pType->getStr() << ";//" << strAnnotation << "\n";
    }

    Export::Export( const io::ObjectInfo& object )
        : Element( object )
        , m_pReturnType( nullptr )
        , m_pParameters( nullptr )
    {
    }

    void Export::load( io::Loader& loader )
    {
        HasIdentifier::load( loader );
        m_pReturnType = loader.loadObjectRef< Opaque >();
        m_pParameters = loader.loadObjectRef< Opaque >();
    }

    void Export::store( io::Storer& storer ) const
    {
        HasIdentifier::store( storer );
        storer.storeObjectRef( m_pReturnType );
        storer.storeObjectRef( m_pParameters );
    }

    void Export::print( std::ostream& os, std::string& strIndent, const std::string& strAnnotation ) const
    {
        VERIFY_RTE( m_pReturnType );
        VERIFY_RTE( m_pParameters );

        os << strIndent << "export " << m_pReturnType->getStr() << " " << m_strIdentifier << "( " << m_pParameters->getStr() << " )//" << strAnnotation << "\n";
    }

    Visibility::Visibility( const io::ObjectInfo& object )
        : Element( object )
    {
    }

    void Visibility::load( io::Loader& loader )
    {
        HasVisibility::load( loader );
    }

    void Visibility::store( io::Storer& storer ) const
    {
        HasVisibility::store( storer );
    }

    void Visibility::print( std::ostream& os, std::string& strIndent, const std::string& strAnnotation ) const
    {
        switch ( m_visibility )
        {
        case eVisPublic:
        case eVisPrivate:
            os << strIndent << g_VisibilityTypeStrings[ m_visibility ] << strAnnotation << "\n";
            break;
        default:
            THROW_RTE( "Unknown visibility type" );
        }
    }

    Context::Context( const io::ObjectInfo& object )
        : Element( object )
    {
    }

    void Context::load( io::Loader& loader )
    {
        HasIdentifier::load( loader );
        HasChildren::load( loader );
        HasDomain::load( loader );
        HasDefinition::load( loader );
        HasParameters::load( loader );
        HasInheritance::load( loader );

        loader.load( m_contextType );
    }

    void Context::store( io::Storer& storer ) const
    {
        HasIdentifier::store( storer );
        HasChildren::store( storer );
        HasDomain::store( storer );
        HasDefinition::store( storer );
        HasParameters::store( storer );
        HasInheritance::store( storer );

        storer.store( m_contextType );
    }

    Context* Context::findContext( const std::string& strIdentifier ) const
    {
        for ( Element* pObject : m_elements )
        {
            if ( Context* pContext = dynamic_cast< Context* >( pObject ) )
            {
                if ( pContext->m_strIdentifier == strIdentifier )
                    return pContext;
            }
        }
        return nullptr;
    }

    void Context::print( std::ostream& os, std::string& strIndent, const std::string& strAnnotation ) const
    {
        printDeclaration( os, strIndent, getContextType(), m_strIdentifier, m_pReturnType, m_pParams, m_pSize, m_inheritance, strAnnotation );
    }

    const char* Context::getContextType() const
    {
        switch ( m_contextType )
        {
        case eAbstract:
            return "abstract";
        case eEvent:
            return "event";
        case eFunction:
            return "function";
        case eAction:
            return "action";
        case eLink:
            return "link";
        case eObject:
            return "object";
        default:
            THROW_RTE( "Unknown context type" );
        }
    }

    const std::string Root::RootTypeName = "root";

    Root::Root( const io::ObjectInfo& object )
        : Context( object )
    {
        m_contextType = eObject;
        m_strIdentifier = RootTypeName;
        m_rootType = eFile;
    }
    Root::Root( const io::ObjectInfo& object, RootType rootType )
        : Context( object )
    {
        m_contextType = eObject;
        m_strIdentifier = RootTypeName;
        m_rootType = rootType;
    }

    void Root::load( io::Loader& loader )
    {
        Context::load( loader );
        loader.loadOptional( m_includePath );
        loader.load( m_rootType );
    }

    void Root::store( io::Storer& storer ) const
    {
        Context::store( storer );
        storer.storeOptional( m_includePath );
        storer.store( m_rootType );
    }

    void Root::print( std::ostream& os, std::string& strIndent, const std::string& strAnnotation ) const
    {
        printDeclaration( os, strIndent, "root", m_strIdentifier, nullptr, nullptr, m_pSize, m_inheritance, strAnnotation );
    }

    Body::Body( const io::ObjectInfo& object )
        : Base( object )
        , m_pContext( nullptr )
    {
    }
    void Body::load( io::Loader& loader )
    {
        loader.load( m_str );
        m_pContext = loader.loadObjectRef< Element >();
        VERIFY_RTE( m_pContext );
    }
    void Body::store( io::Storer& storer ) const
    {
        VERIFY_RTE( m_pContext );
        storer.store( m_str );
        storer.storeObjectRef( m_pContext );
    }
} // namespace input
} // namespace mega
