
#include "common.hpp"

#include "common/assert_verify.hpp"

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>

#include <iostream>
#include <new>
#include <stdexcept>

int main( int argc, const char* argv[] )
{
    std::cout << "Page size: " << boost::interprocess::mapped_region::get_page_size() << std::endl;

    struct shm_remove
    {
        shm_remove() { boost::interprocess::shared_memory_object::remove( SHARED_MEMORY_NAME ); }
        ~shm_remove() { boost::interprocess::shared_memory_object::remove( SHARED_MEMORY_NAME ); }
    } remover;

    try
    {
        try
        {
            ManagedSharedMemory segment( boost::interprocess::create_only, SHARED_MEMORY_NAME, SHARED_MEMORY_SIZE );

            // segment.get_segment_manager()->get_free_memory()

            ExampleSharedBufferIndex* pBufferIndex = segment.construct< ExampleSharedBufferIndex >(
                "ExampleSharedBufferIndex" )( segment.get_segment_manager() );

            for ( int i = 0; i < 1000; ++i )
            {
                ExampleSharedBufferIndex::Index index   = pBufferIndex->allocate();
                ExampleSharedBufferIndex::Ptr   pBuffer = pBufferIndex->get( index );
                for ( int j = 0; j < 100; ++j )
                {
                    pBuffer->m_vector.push_back( j );
                }
            }

            std::cout << "Created: " << pBufferIndex->size() << " free: " << segment.get_free_memory() << std::endl;
        }
        catch ( boost::interprocess::bad_alloc& ex )
        {
            ManagedSharedMemory       segment( boost::interprocess::open_only, SHARED_MEMORY_NAME );
            ExampleSharedBufferIndex* pBufferIndex
                = segment.find< ExampleSharedBufferIndex >( "ExampleSharedBufferIndex" ).first;
            std::cout << "Exception: Created: " << pBufferIndex->size() << " free: " << segment.get_free_memory()
                      << std::endl;
        }

        char c = std::cin.get();

        /*try
        {
            //ManagedSharedMemory::shrink_to_fit( SHARED_MEMORY_NAME );
            //ManagedSharedMemory::grow( SHARED_MEMORY_NAME, SHARED_MEMORY_SIZE * 2 );

            ManagedSharedMemory segment( boost::interprocess::open_only, SHARED_MEMORY_NAME );

            ExampleSharedBufferIndex* pBufferIndex = segment.find< ExampleSharedBufferIndex >(
        "ExampleSharedBufferIndex" ).first;
            //pBufferIndex->attach( segment.get_segment_manager() );

            // ExampleSharedBufferIndex::Ptr pBuffer = pBufferIndex->get( 0 );

            for ( int i = 0; i < 1000; ++i )
            {
                ExampleSharedBufferIndex::Index index   = pBufferIndex->allocate();
                ExampleSharedBufferIndex::Ptr   pBuffer = pBufferIndex->get( index );
                pBuffer->m_vector.push_back( 123 );
            }
            std::cout << "Created: " << pBufferIndex->size() << " free: " << segment.get_free_memory() << std::endl;
        }
        catch ( boost::interprocess::bad_alloc& ex )
        {
            ManagedSharedMemory segment( boost::interprocess::open_only, SHARED_MEMORY_NAME );
            ExampleSharedBufferIndex* pBufferIndex = segment.find< ExampleSharedBufferIndex >(
        "ExampleSharedBufferIndex" ).first; std::cout << "Exception: Created: " << pBufferIndex->size() << " free: " <<
        segment.get_free_memory()
                      << std::endl;
        }
        */

        std::cout << "It worked" << std::endl;
    }
    catch ( std::bad_alloc& ex )
    {
        std::cout << "Bad Alloc: " << ex.what() << std::endl;
    }
    catch ( std::runtime_error& ex )
    {
        std::cout << "Runtime error: " << ex.what() << std::endl;
    }
    catch ( boost::interprocess::bad_alloc& ex )
    {
        std::cout << "bad allocation: " << ex.what() << std::endl;
    }

    return 0;
}
