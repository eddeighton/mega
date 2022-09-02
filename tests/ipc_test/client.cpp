
#include "common.hpp"

#include "common/assert_verify.hpp"

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>

#include <iostream>

int main( int argc, const char* argv[] )
{
    std::cout << "Page size: " << boost::interprocess::mapped_region::get_page_size() << std::endl;

    ManagedSharedMemory segment( boost::interprocess::open_only, SHARED_MEMORY_NAME );

    ExampleSharedBufferIndex* pBufferIndex
        = segment.find< ExampleSharedBufferIndex >( "ExampleSharedBufferIndex" ).first;
    std::cout << "Got buffer with size: " << pBufferIndex->size() << std::endl;

    ExampleSharedBufferIndex::Ptr pBuffer1 = pBufferIndex->get( 0 );

    for ( int i = 0; i < 1000; ++i )
    {
        pBuffer1->m_vector.push_back( i * i );
    }
    
    return 0;
}
