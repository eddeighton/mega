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

#include "schematic/file.hpp"
#include "schematic/site.hpp"
#include "schematic/container.hpp"
#include "schematic/compilation.hpp"
#include "schematic/markup.hpp"

#include "common/task.hpp"
#include "common/scheduler.hpp"

namespace schematic
{

class Schematic : public Container< Site, File >
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

    virtual void init();

    void load( const format::File::Schematic& schematic );
    void save( format::File::Schematic& schematic ) const;

    const Site::PtrVector& getSites() const { return BaseType::getElements(); }

    enum CompilationStage
    {
        eStage_Site,
        eStage_SiteContour,
        eStage_Extrusion,
        eStage_Compilation
    };

    void task_contours();
    void task_extrusions();
    void task_compilation();

    // GlyphSpecProducer
    virtual void getMarkupPolygonGroups( MarkupPolygonGroup::List& polyGroups )
    {
        if( m_pCompilationMarkup.get() )
            polyGroups.push_back( m_pCompilationMarkup.get() );
    }

    Compilation::Ptr getCompilation() const { return m_pCompilation; }

private:
    Compilation::Ptr                   m_pCompilation;
    std::unique_ptr< MultiPathMarkup > m_pCompilationMarkup;
};

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
/*
class SiteContourTask : public task::Task
{
    Schematic::WeakPtr m_pSchematic;

public:
    SiteContourTask( Schematic::Ptr pSchematic, const task::Task::RawPtrSet& dependencies );
    virtual void run( task::Progress& taskProgress );
};

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
class SiteExtrusionTask : public task::Task
{
    Schematic::WeakPtr m_pSchematic;

public:
    SiteExtrusionTask( Schematic::Ptr pSchematic, const task::Task::RawPtrSet& dependencies );
    virtual void run( task::Progress& taskProgress );
};

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
class CompilationTask : public task::Task
{
    Schematic::WeakPtr m_pSchematic;

public:
    CompilationTask( Schematic::Ptr pSchematic, const task::Task::RawPtrSet& dependencies );
    virtual void run( task::Progress& taskProgress );
};
*/
} // namespace schematic

#endif // SCHEMATIC_API_28_JAN_2021
