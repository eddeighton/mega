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

#include "common/backtrace.hpp"

#include <boost/program_options.hpp>
#include <boost/stacktrace.hpp>

#include <iostream>
#include <memory>


void on_terminate()
{
    std::cout << "terminate called..." << std::endl;
    std::cout << boost::stacktrace::stacktrace();
    std::abort();
}

int inner_main(int argc, char* argv[])
{
    std::set_terminate( on_terminate );

    Common::disableDebugErrorPrompts();

    std::string strFilter;
    bool bWait = false, bBreak = false;

    namespace bpo = boost::program_options;
    boost::program_options::options_description desc("Allowed options");
    desc.add_options()
        ("help", "produce help message")
        ("filter",      bpo::value< std::string >( &strFilter ), "filter string to select subset of tests")
        ("wait",        bpo::value< bool >( &bWait )->implicit_value( true ),            "wait once complete to keep terminal open")
        ("break",       bpo::value< bool >( &bBreak )->implicit_value( true ),           "break at startup to enable attaching debugger" )
    ;

    boost::program_options::variables_map vm;
    boost::program_options::store( boost::program_options::parse_command_line( argc, argv, desc), vm);
    boost::program_options::notify( vm );

    if ( vm.count("help") ) {
        std::cout << desc << "\n";
        return 1;
    }

    if( bBreak )
    {
        char c;
        std::cin >> c;
    }

    int szResult = 0;

    

	return szResult;
}

int main(int argc, char* argv[])
{
    try
    {
        return inner_main( argc, argv );
    }
    catch( std::exception& ex )
    {
        std::cerr << "Exception calling main: " << ex.what() << std::endl;
        return 1;
    }
    catch( ... )
    {
        std::cerr << "Unknown exception calling main" << std::endl;
        return 1;
    }
}
