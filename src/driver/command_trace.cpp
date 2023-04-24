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

#include "service/terminal.hpp"

#include "log/log.hpp"

#include "service/network/log.hpp"

#include "compiler/build_report.hpp"
#include "compiler/base_task.hpp"
/*
#include "ftxui/component/component.hpp" // for Checkbox, Renderer, Horizontal, Vertical, Input, Menu, Radiobox, ResizableSplitLeft, Tab
#include "ftxui/component/component_base.hpp"     // for ComponentBase, Component
#include "ftxui/component/component_options.hpp"  // for MenuOption, InputOption
#include "ftxui/component/event.hpp"              // for Event, Event::Custom
#include "ftxui/component/screen_interactive.hpp" // for Component, ScreenInteractive
#include "ftxui/dom/elements.hpp"
// for text, color, operator|, bgcolor, filler, Element, vbox, size, hbox, separator, flex, window,
// graph, EQUAL, paragraph, WIDTH, hcenter, Elements, bold, vscroll_indicator, HEIGHT, flexbox,
// hflow, border, frame, flex_grow, gauge, paragraphAlignCenter, paragraphAlignJustify, paragraphAlignLeft,
// paragraphAlignRight, dim, spinner, LESS_THAN, center, yframe, GREATER_THAN
#include "ftxui/dom/flexbox_config.hpp" // for FlexboxConfig
#include "ftxui/screen/color.hpp"       // for Color, Color::BlueLight, Color::RedLight, Color::Black,
// Color::Blue, Color::Cyan, Color::CyanLight, Color::GrayDark, Color::GrayLight, Color::Green, Color::GreenLight,
// Color::Magenta, Color::MagentaLight, Color::Red, Color::White, Color::Yellow, Color::YellowLight, Color::Default,
// Color::Palette256, ftxui
#include "ftxui/screen/color_info.hpp" // for ColorInfo
#include "ftxui/screen/terminal.hpp"   // for Size, Dimensions
#include "ftxui/component/loop.hpp"    // for Loop
*/
#include "common/stl.hpp"
#include "common/compose.hpp"

#include <boost/program_options.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include <vector>
#include <string>
#include <iostream>

namespace driver::trace
{
/*
Component Inner( std::vector< Component > children )
{
    Component vlist = Container::Vertical( std::move( children ) );
    return Renderer( vlist,
                     [ vlist ]
                     {
                         return hbox( {
                             text( " " ),
                             vlist->Render(),
                         } );
                     } );
}

Component Empty()
{
    return std::make_shared< ComponentBase >();
}*/

void command( bool bHelp, const std::vector< std::string >& args )
{
    namespace po                    = boost::program_options;
    bool                    bShowUI = true;
    po::options_description commandOptions( " Simulation Commands" );
    {
        // clang-format off
        commandOptions.add_options()
            ( "ui",    po::bool_switch( &bShowUI ), "Show FTXUI user interface for progress report" )

            ;
        // clang-format on
    }

    po::positional_options_description p;
    p.add( "file", -1 );

    po::variables_map vm;
    po::store( po::command_line_parser( args ).options( commandOptions ).positional( p ).run(), vm );
    po::notify( vm );

    if( bHelp )
    {
        std::cout << commandOptions << "\n";
    }
    else
    {
        std::atomic< bool >         bContinue = true;
        std::mutex                  mut;
        mega::compiler::BuildStatus buildState;

        std::thread inputThread(
            [ &buildState, &mut, &bContinue, &bShowUI ]()
            {
                while( bContinue )
                {
                    std::string strLine;
                    if( std::getline( std::cin, strLine ) )
                    {
                        try
                        {
                            std::istringstream         is( strLine );
                            mega::compiler::TaskReport report;
                            is >> report;
                            {
                                std::lock_guard< std::mutex > lock( mut );
                                buildState.progress( report );
                                if( !bShowUI )
                                {
                                    std::cout << report.str();
                                }
                            }
                        }
                        catch( std::exception& )
                        {
                            if( !bShowUI )
                            {
                                std::cout << strLine;
                            }
                        }
                    }
                }
            } );

        // auto pRootComponent = Inner( { Collapsible( "Megastructure 1", Empty() ) } );
        /*     Inner( {
                 Collapsible( "Collapsible 1.1",
                              Inner( {
                                  Collapsible( "Collapsible 1.1.1", Empty() ),
                                  Collapsible( "Collapsible 1.1.2", Empty() ),
                                  Collapsible( "Collapsible 1.1.3", Empty() ),
                              } ) ),
             } ) );*/

        /*if( bShowUI )
        {
            using namespace ftxui;

            using RowMap  = std::map< mega::compiler::TaskName, Component >;
            using TaskMap = mega::compiler::BuildStatus::TaskStatusMap;
            RowMap rowMap;
            //  mega::compiler::BuildStatus::TaskStatusMap

            auto screen = ScreenInteractive::Fullscreen();

            // std::vector< std::string > entries;
            // int                        selection = 0;
            // MenuOption                 option;
            // option.on_enter = screen.ExitLoopClosure();

            // auto pRootComponent = Menu( &entries, &selection, &option );

            auto components = ftxui::Container::Vertical( {} );

            auto pRootComponent = Renderer(
                [ & ]()
                { 
                    return components->Render() | ftxui::vscroll_indicator | ftxui::frame | ftxui::borderLight; 
                } );
            // clang-format off
            // clang-format on

            // screen.Loop( pRootComponent );

            Loop loop( &screen, pRootComponent );
            while( !loop.HasQuitted() )
            {
                {
                    std::lock_guard< std::mutex > lock( mut );
                    const TaskMap&                taskMap = buildState.get();

                    RowMap  removals;
                    TaskMap additions;

                    generics::match( rowMap.begin(), rowMap.end(), taskMap.begin(), taskMap.end(),
                                     generics::lessthan( generics::first< RowMap::const_iterator >(),
                                                         generics::first< TaskMap::const_iterator >() ),
                                     generics::collect( removals, generics::deref< RowMap::const_iterator >() ),
                                     generics::collect( additions, generics::deref< TaskMap::const_iterator >() ) );

                    for( const auto& removal : removals )
                    {
                        rowMap.erase( removal.first );
                        removal.second->Detach();
                    }
                    for( const auto& addition : additions )
                    {
                        const mega::compiler::TaskStatus& taskStatus = addition.second;
                        auto ib = rowMap.insert( { addition.first, Button( taskStatus.getName().str(), []() {} ) } );
                        components->Add( ib.first->second );
                    }
                }

                loop.RunOnce();
                std::this_thread::sleep_for( std::chrono::milliseconds( 40 ) );
                screen.PostEvent( ftxui::Event::Custom );
            }
        }*/
        // bContinue = false;
        inputThread.join();
    }
}

} // namespace driver::trace

/*
std::lock_guard< std::mutex > lock( mut );

const TaskMap& taskMap = buildState.get();

entries.clear();
for( const auto& [ name, status ] : taskMap )
{
    entries.push_back( name.str() );
}
*/

/*RowMap removals;
TaskMap additions;

generics::match(
    rowMap.begin(), rowMap.end(),
    taskMap.begin(), taskMap.end(),
    generics::lessthan( generics::first< RowMap::const_iterator >(),
                        generics::first< TaskMap::const_iterator >() ),
    generics::collect( removals, generics::deref< RowMap::const_iterator >() ),
    generics::collect( additions, generics::deref< TaskMap::const_iterator >() ) );

for( const auto& removal : removals )
{
    rowMap.erase( removal.first );
}
for( const auto& addition : additions )
{
    const mega::compiler::TaskStatus& taskStatus = addition.second;
    rowMap.insert( { addition.first, text( taskStatus.getName().str() ) } );
}

entries.clear();
for( const auto& [ name, pElement ] : rowMap )
{
    entries.push_back( name.str() );
}*/
