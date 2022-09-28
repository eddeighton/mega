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


#include "common.hpp"

#include "common/assert_verify.hpp"

#include <iostream>

int main( int argc, const char* argv[] )
{
    /*std::cout << "Page size: " << boost::interprocess::mapped_region::get_page_size() << std::endl;

    mega::runtime::ManagedSharedMemory segment( boost::interprocess::open_only, SHARED_MEMORY_NAME );

    ExampleBufferIndex* pBufferIndex = segment.find< ExampleBufferIndex >( "ExampleBufferIndex" ).first;
    std::cout << "Got buffer with size: " << pBufferIndex->size() << std::endl;

    ExampleBufferIndex::SharedPtr pBuffer1 = pBufferIndex->getShared( 0 );

    for ( int i = 0; i < 1000; ++i )
    {
        pBuffer1->Plug->link_5.push_back( mega::reference{} );
    }*/

    return 0;
}
