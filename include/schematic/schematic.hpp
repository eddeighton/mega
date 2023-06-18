//  Copyright (c) Deighton Systems Limited. 2022. All Rights Reserved.
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

#ifndef SCHEMATIC_API_28_JAN_2021
#define SCHEMATIC_API_28_JAN_2021

#include "schematic/compilation_stage.hpp"
#include "schematic/file.hpp"
#include "schematic/site.hpp"
#include "schematic/container.hpp"
#include "schematic/markup.hpp"
#include "schematic/analysis/analysis.hpp"

#include "boost/filesystem/path.hpp"

namespace schematic
{

class Schematic : public Container< Site, File >, public boost::enable_shared_from_this< Schematic >
{
public:
    using BaseType = Container< Site, File >;

    using Ptr     = boost::shared_ptr< Schematic >;
    using WeakPtr = boost::weak_ptr< Schematic >;
    using PtrCst  = boost::shared_ptr< const Schematic >;
    using PtrList = std::list< Ptr >;
    using PtrSet  = std::set< Ptr >;

    Schematic( const std::string& strName );
    Schematic( Schematic::PtrCst pOriginal, Node::Ptr pNewParent, const std::string& strName );

    virtual Node::PtrCst getPtr() const { return shared_from_this(); }
    virtual Node::Ptr    getPtr() { return shared_from_this(); }
    virtual void         init();
    virtual Node::Ptr    copy( Node::Ptr pParent, const std::string& strName ) const;
    virtual void         load( const format::Node& node );
    virtual void         save( format::Node& node ) const;

    virtual std::string getStatement() const { return getName(); }

    const Site::PtrVector& getSites() const { return BaseType::getElements(); }

    bool compile( CompilationStage stage, std::ostream& os );
    void compileMap( const boost::filesystem::path& filePath );

    // GlyphSpecProducer
    virtual void getMarkupPolygonGroups( MarkupPolygonGroup::List& polyGroups )
    {
        if( m_pAnalysisMarkup.get() )
            polyGroups.push_back( m_pAnalysisMarkup.get() );
        if( m_pPropertyMarkup.get() )
            polyGroups.push_back( m_pPropertyMarkup.get() );
    }

    exact::Analysis::Ptr getAnalysis() const { return m_pAnalysis; }

private:
    exact::Analysis::Ptr                  m_pAnalysis;
    std::unique_ptr< MultiPathMarkup >    m_pAnalysisMarkup;
    std::unique_ptr< MultiPolygonMarkup > m_pPropertyMarkup;
};

} // namespace schematic

#endif // SCHEMATIC_API_28_JAN_2021
