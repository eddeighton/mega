
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
