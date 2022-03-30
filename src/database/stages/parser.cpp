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

#include "database/stages/parser.hpp"
#include "database/io/manifest.hpp"
#include "database/io/database.hpp"
#include "database/io/component_info.hpp"
#include "parser/parser.hpp"

#include "database/model/eg.hpp"
#include "database/model/input.hpp"
#include "database/model/interface.hpp"
#include "database/model/identifiers.hpp"

#include <boost/dll/import.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem/operations.hpp>

namespace mega
{
namespace Stages
{
/*

    Parser::Parser( const boost::filesystem::path& parserDLL,
                    const boost::filesystem::path& sourceDir,
                    const boost::filesystem::path& buildDir )

        //: Creating( io::File::FileIDtoPathMap{}, io::ObjectInfo::NO_FILE )
        : m_parserDLL( parserDLL )
        , m_sourceDir( sourceDir )
        , m_buildDir( buildDir )
    {
    }

    void Parser::calculateManifest()
    {
        io::Manifest::PtrCst pManifest = std::make_shared< io::Manifest >( m_buildDir );

        io::Database< io::stage::Test > database( m_sourceDir, m_buildDir, pManifest );

        database.store();
    }
*/
    /*
    Parser::Parser( EG_PARSER_CALLBACK* pParserCallback,
                const boost::filesystem::path& parserDLLPath,
                const boost::filesystem::path& currentPath,
                std::ostream& os )
        :   Creating( io::File::FileIDtoPathMap{}, io::ObjectInfo::NO_FILE ),
            m_pParserCallback( pParserCallback ),
            m_parserDllPath( parserDLLPath ),
            m_currentPath( currentPath ),
            m_errorOS( os )
    {
        VERIFY_RTE( m_pParserCallback );
    }

    void Parser::parseEGSourceFile( const boost::filesystem::path& egSourceFile,
                Parser& stage, input::Root* pRoot )
    {
        static boost::shared_ptr< mega::EG_PARSER_INTERFACE > pParserInterface;

        if( !pParserInterface )
        {
            pParserInterface =
                boost::dll::import_symbol< mega::EG_PARSER_INTERFACE >(
                    m_parserDllPath, "g_parserSymbol" );
        }

        VERIFY_RTE_MSG( pParserInterface, "Failed to locate eg parser at: " << m_parserDllPath.string() );

        //record the source file parsed for this root
        pRoot->m_sourceFile = egSourceFile;

        pParserInterface->parseEGSourceFile( m_pParserCallback, egSourceFile, m_currentPath, m_errorOS, stage, pRoot );

    }

    input::Root* Parser::getMegaRoot(
        input::Root* pMegaStructureRoot,
        const SourceCodeTree::RootFolder& rootFolder,
        const SourceCodeTree::ProjectNameFolder& projectNameFolder,
        std::map< boost::filesystem::path, input::Root* >& rootTree )
    {
        boost::filesystem::path::const_iterator
            i = rootFolder.begin(), iEnd = rootFolder.end(),
            j = projectNameFolder.begin(), jEnd = projectNameFolder.end();

        //iterate until they become different
        for( ; i!=iEnd && j!=jEnd && *i == *j; ++i, ++j ){}

        VERIFY_RTE_MSG( i == iEnd,
            "Project name folder: " << projectNameFolder.string() <<
            " does NOT exist within mega structure root folder: " << rootFolder.string() );

        input::Root* pRoot = pMegaStructureRoot;
        {
            boost::filesystem::path treePath = rootFolder;
            int iFolderDepth = 0;
            for( ; j!=jEnd; ++j, ++iFolderDepth )
            {
                treePath = treePath / *j;

                input::Root* pNestedRoot = nullptr;

                std::map< boost::filesystem::path, input::Root* >::iterator
                    iFind = rootTree.find( treePath );
                if( iFind != rootTree.end() )
                {
                    pNestedRoot = iFind->second;
                }
                else
                {
                    pNestedRoot = construct< input::Root >();
                    {
                        pNestedRoot->m_strIdentifier = j->string();
                        switch( iFolderDepth )
                        {
                            case 0:
                                pNestedRoot->m_rootType = eCoordinator;
                                break;
                            case 1:
                                pNestedRoot->m_rootType = eHostName;
                                break;
                            case 2:
                                pNestedRoot->m_rootType = eProjectName;
                                break;
                            default:
                                THROW_RTE( "TODO" );
                                pNestedRoot->m_rootType = eSubFolder;
                                break;
                        }
                    }
                    pRoot->m_elements.push_back( pNestedRoot );

                    rootTree.insert( std::make_pair( treePath, pNestedRoot ) );
                }

                pRoot = pNestedRoot;
            }
        }

        return pRoot;
    }

    void Parser::parse( const SourceCodeTree& egSourceCodeFiles )
    {
        input::Root* pMegaStructureRoot = construct< input::Root >();
        {
            pMegaStructureRoot->m_rootType = eMegaRoot;
        }

        std::set< boost::filesystem::path > includePaths;
        std::map< boost::filesystem::path, input::Root* > rootTree;

        for( SourceCodeTree::FileMap::const_iterator
            i = egSourceCodeFiles.files.begin(),
            iEnd = egSourceCodeFiles.files.end();
            i != iEnd; ++i )
        {
            const SourceCodeTree::ProjectNameFolder& projectNameFolder = i->first;
            const SourceCodeTree::EGSourceFile& egSourceFile = i->second;

            boost::filesystem::path sourceFile = projectNameFolder / egSourceFile;

            input::Root* pRoot = getMegaRoot( pMegaStructureRoot, egSourceCodeFiles.root, projectNameFolder, rootTree );

            parseEGSourceFile( sourceFile, *this, pRoot ); //parse main root
            includePaths.insert( sourceFile );
        }

        handleInputIncludes( includePaths );
    }

    void Parser::parse( const std::vector< boost::filesystem::path >& egSourceCodeFiles )
    {
        input::Root* pRoot = construct< input::Root >();
        {
            pRoot->m_rootType = eFileRoot;
        }

        std::set< boost::filesystem::path > includePaths;
        for( const boost::filesystem::path& filePath : egSourceCodeFiles )
        {
            parseEGSourceFile( filePath, *this, pRoot ); //parse main root
            includePaths.insert( filePath );
        }

        handleInputIncludes( includePaths );
    }

    void Parser::handleInputIncludes( std::set< boost::filesystem::path >& includePaths )
    {
        std::set< boost::filesystem::path > newIncludePaths;

        //greedy parse all includes
        do
        {
            for( const boost::filesystem::path& includePath : newIncludePaths )
            {
                input::Root* pIncludeRoot = construct< input::Root >();
                {
                    pIncludeRoot->m_includePath = includePath;
                    pIncludeRoot->m_rootType = eFile;
                }

                parseEGSourceFile( includePath, *this, pIncludeRoot ); //parse include - non-main root
                includePaths.insert( includePath );
            }
            newIncludePaths.clear();

            std::vector< input::Include* > includes = many< input::Include >( getMaster() );
            for( input::Include* pInclude : includes )
            {
                if( pInclude->isEGInclude() )
                {
                    const boost::filesystem::path& filePath = pInclude->getIncludeFilePath();
                    if( includePaths.find( filePath ) == includePaths.end() )
                        newIncludePaths.insert( filePath );
                }
            }
        }while( !newIncludePaths.empty() );
    }

    interface::Element* addChild( Parser& stage, interface::Element* pParent, input::Element* pElement, VisibilityType visibility )
    {
        interface::Element* pNewNode = nullptr;
        switch( pElement->getType() )
        {
            case eInputOpaque:         pNewNode = stage.construct< interface::Opaque >(    pParent, pElement, visibility ); break;
            case eInputDimension:      pNewNode = stage.construct< interface::Dimension >( pParent, pElement, visibility ); break;
            case eInputInclude:        pNewNode = stage.construct< interface::Include >(   pParent, pElement, visibility ); break;
            case eInputUsing:          pNewNode = stage.construct< interface::Using >(     pParent, pElement, visibility ); break;
            case eInputExport:         pNewNode = stage.construct< interface::Export >(    pParent, pElement, visibility ); break;
            case eInputVisibility:     THROW_RTE( "Invalid attempt to construct interface visibility" );
            case eInputContext:
            {
                input::Context* pContext = dynamic_cast< input::Context* >( pElement );
                VERIFY_RTE( pContext );
                switch( pContext->m_contextType )
                {
                    case input::Context::eAbstract : pNewNode = stage.construct< interface::Abstract > ( pParent, pElement, visibility ); break;
                    case input::Context::eEvent    : pNewNode = stage.construct< interface::Event >    ( pParent, pElement, visibility ); break;
                    case input::Context::eFunction : pNewNode = stage.construct< interface::Function > ( pParent, pElement, visibility ); break;
                    case input::Context::eAction   : pNewNode = stage.construct< interface::Action >   ( pParent, pElement, visibility ); break;
                    case input::Context::eLink     : pNewNode = stage.construct< interface::Link >     ( pParent, pElement, visibility ); break;
                    case input::Context::eObject   : pNewNode = stage.construct< interface::Object >   ( pParent, pElement, visibility ); break;
                    case input::Context::eUnknown  :
                    default:
                    {
                        THROW_RTE( "Undefined context type for: " << pParent->getFriendlyName() << "::" << pContext->getIdentifier() );
                    }
                }
                break;
            }
            case eInputRoot:
                pNewNode = stage.construct< interface::Root >( pParent, pElement, visibility ); break;
            default:
                THROW_RTE( "Unsupported type" );
                break;
        }
        VERIFY_RTE( pNewNode );
        return pNewNode;
    }

    void Parser::buildTree( const FileElementMap& fileMap, interface::Element* pParentNode,
        input::Element* pElement,
        std::optional< boost::filesystem::path > includeDefinitionFile,
        bool bInIncludeTree,
        VisibilityType visibility )
    {
        switch( pElement->getType() )
        {
            case eInputOpaque    :
                {
                    //do nothing
                }
                break;
            case eInputDimension :
                {
                    //do nothing
                }
                break;
            case eInputInclude   :
                THROW_RTE( "unreachable" );
                break;
            case eInputUsing     :
                {
                    //do nothing
                }
                break;
            case eInputExport    :
                {
                    //do nothing
                }
                break;
            case eInputVisibility:
                THROW_RTE( "unreachable" );
                break;
            case eInputContext   :
            case eInputRoot      :
                {
                    input::Context* pContext = dynamic_cast< input::Context* >( pElement );
                    VERIFY_RTE( pContext );

                    for( input::Element* pChildElement : pContext->getElements() )
                    {
                        if( input::Include* pInclude = dynamic_cast< input::Include* >( pChildElement ) )
                        {
                            if( pInclude->isEGInclude() )
                            {
                                FileElementMap::const_iterator iFind =
                                    fileMap.find( pInclude->getIncludeFilePath() );
                                VERIFY_RTE_MSG( iFind != fileMap.end(), "Failed to find include file: " <<
                                    pInclude->getIncludeFilePath().string() <<
                                    " NOTE: you may have put include .eg file in root folder" );
                                input::Root* pIncludedRoot = iFind->second;

                                if( pInclude->getIdentifier().empty() )
                                {
                                    //if the include has no identifier then insert the included root elements
                                    buildTree( fileMap, pParentNode, pIncludedRoot, includeDefinitionFile, true, visibility );
                                }
                                else
                                {
                                    //otherwise insert the root with the include identifier
                                    interface::Element* pChild = addChild( *this, pParentNode, pIncludedRoot, visibility );
                                    pChild->pIncludeIdentifier = pInclude;
                                    buildTree( fileMap, pChild, pIncludedRoot, includeDefinitionFile, true, visibility );
                                }
                            }
                            else
                            {
                                addChild( *this, pParentNode, pChildElement, visibility );
                            }
                        }
                        else if( input::Context* pElementAction = dynamic_cast< input::Context* >( pChildElement ) )
                        {
                            interface::Element* pChild = addChild( *this, pParentNode, pChildElement, visibility );

                            if( bInIncludeTree )
                            {
                                //if the action is defined then set the definition file to the include definition file
                                if( includeDefinitionFile )
                                {
                                    //if the action has no definition of an operation then DO NOT set the definition file
                                    //this is ESSENTIAL for actions or objects to be treated as objects i.e. with lifetimes irrespective of execution of code
                                    if( pElementAction->getDefinitionFile() )
                                    {
                                        ( (interface::Context*)pChild )->setDefinitionFile( includeDefinitionFile );
                                    }
                                }
                                else
                                {
                                    THROW_RTE( "Include file problem" );
                                }
                                buildTree( fileMap, pChild, pChildElement, includeDefinitionFile, true, visibility );
                            }
                            else
                            {
                                //if the action is defined then use its definition file for any include sub trees
                                if( pElementAction->getDefinitionFile() )
                                {
                                    ( (interface::Context*)pChild )->setDefinitionFile( pElementAction->getDefinitionFile().value() );
                                }
                                buildTree( fileMap, pChild, pChildElement, pElementAction->getSourceFile(), false, visibility );
                            }
                        }
                        else if( input::Export* pElementExport = dynamic_cast< input::Export* >( pChildElement ) )
                        {
                            interface::Element* pChild = addChild( *this, pParentNode, pChildElement, visibility );
                            buildTree( fileMap, pChild, pChildElement, includeDefinitionFile, bInIncludeTree, visibility );
                        }
                        else if( input::Visibility* pVisibility = dynamic_cast< input::Visibility* >( pChildElement ) )
                        {
                            visibility = pVisibility->m_visibility;
                        }
                        else
                        {
                            interface::Element* pChild = addChild( *this, pParentNode, pChildElement, visibility );
                            buildTree( fileMap, pChild, pChildElement, includeDefinitionFile, bInIncludeTree, visibility );
                        }
                    }
                }
                break;
            default:
                THROW_RTE( "Unsupported type" );
                break;
        }
    }

    void Parser::buildAbstractTree()
    {
        interface::Root* pMasterRoot = construct< interface::Root >(); //eTreeRoot

        std::vector< input::Root* > roots = many< input::Root >( getMaster() );
        VERIFY_RTE( !roots.empty() );

        input::Root* pInputMainRoot = nullptr;

        FileElementMap fileMap;
        for( input::Root* pRootElement : roots )
        {
            std::optional< boost::filesystem::path > includePathOpt =
                pRootElement->getIncludePath();

            if( eMegaRoot == pRootElement->getRootType() ||
                eFileRoot == pRootElement->getRootType() )
            {
                VERIFY_RTE( !includePathOpt );
                VERIFY_RTE( !pInputMainRoot );
                pInputMainRoot = pRootElement;
            }
            else if( includePathOpt )
            {
                fileMap.insert( std::make_pair( includePathOpt.value(), pRootElement ) );
            }
        }

        VERIFY_RTE( pInputMainRoot );
        interface::Element* pInterfaceRoot = addChild( *this, pMasterRoot, pInputMainRoot, eVisPublic );
        ( (interface::Context*)pInterfaceRoot )->setDefinitionFile( pInputMainRoot->getDefinitionFile() );

        buildTree( fileMap, pInterfaceRoot, pInputMainRoot, pInputMainRoot->getSourceFile(), false, eVisPublic );

        //create the identifiers object
        Identifiers* pIdentifiers = construct< Identifiers >();
        pIdentifiers->populate( getMaster() );
    }
*/

} // namespace Stages
} // namespace mega
