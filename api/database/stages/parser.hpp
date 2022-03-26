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

#ifndef EG_PARSER_18_04_2019
#define EG_PARSER_18_04_2019

#include "database/io/database.hpp"
#include "stage.hpp"

#include <boost/filesystem.hpp>

#include <vector>
#include <memory>
#include <map>
#include <set>
#include <ostream>
#include <optional>

namespace clang
{
class DiagnosticsEngine;
class FileManager;
} // namespace clang

namespace mega
{
class Identifiers;
class Manifest;
class Component;

namespace interface
{
    class Element;
    class Root;
} // namespace interface

namespace input
{
    class Element;
    class Root;
} // namespace input

// class EG_PARSER_CALLBACK;

namespace Stages
{
    class Parser //: public Creating
    {
    public:
        Parser( const boost::filesystem::path& parserDLL,
                const boost::filesystem::path& sourceDir,
                const boost::filesystem::path& buildDir );

        //void calculateManifest();


    private:
        boost::filesystem::path m_parserDLL;
        boost::filesystem::path m_sourceDir;
        boost::filesystem::path m_buildDir;

        /*
    public:
        Parser(  EG_PARSER_CALLBACK* pParserCallback,
                        const boost::filesystem::path& parserDLLPath,
                        const boost::filesystem::path& currentPath,
                        std::ostream& os );

        struct SourceCodeTree
        {
            using RootFolder = boost::filesystem::path;
            using ProjectNameFolder = boost::filesystem::path;
            using EGSourceFile = boost::filesystem::path;
            using FileMap = std::multimap< ProjectNameFolder, EGSourceFile >;
            RootFolder root;
            FileMap files;
        };

        void parse( const SourceCodeTree& egSourceCodeFiles );

        void parse( const std::vector< boost::filesystem::path >& egSourceCodeFiles );

        void buildAbstractTree();

        const interface::Root* getTreeRoot() const { return io::root_cst< mega::interface::Root >( getMaster() ); }

        const Identifiers* getIdentifiers() const { return io::one_cst< mega::Identifiers >( getMaster() ); }
    private:

        input::Root* getMegaRoot(
            input::Root* pMegaStructureRoot,
            const SourceCodeTree::RootFolder& rootFolder,
            const SourceCodeTree::ProjectNameFolder& projectNameFolder,
            std::map< boost::filesystem::path, input::Root* >& rootTree );

        void handleInputIncludes( std::set< boost::filesystem::path >& includePaths );

        using FileElementMap = std::map< boost::filesystem::path, input::Root* >;
        void buildTree( const FileElementMap& fileMap, interface::Element*, input::Element*,
            std::optional< boost::filesystem::path > , bool bInIncludeTree, VisibilityType visibility );

        void parseEGSourceFile( const boost::filesystem::path& egSourceFile,
                    Parser& stage, input::Root* pRoot );
    private:
        EG_PARSER_CALLBACK* m_pParserCallback;
        const boost::filesystem::path m_parserDllPath;
        const boost::filesystem::path m_currentPath;
        std::ostream& m_errorOS;*/
    };
} // namespace Stages
} // namespace mega

#endif // EG_PARSER_18_04_2019