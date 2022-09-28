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

#include "mega/common.hpp"

#include "common/assert_verify.hpp"

#include <iostream>
#include <new>
#include <stdexcept>

int main( int argc, const char* argv[] )
{
    /*std::cout << "Page size: " << boost::interprocess::mapped_region::get_page_size() << std::endl;

    struct shm_remove
    {
        shm_remove() { boost::interprocess::shared_memory_object::remove( SHARED_MEMORY_NAME ); }
        ~shm_remove() { boost::interprocess::shared_memory_object::remove( SHARED_MEMORY_NAME ); }
    } remover;

    try
    {
        try
        {
            mega::runtime::ManagedSharedMemory segment(
                boost::interprocess::create_only, SHARED_MEMORY_NAME, SHARED_MEMORY_SIZE );

            ExampleBufferIndex* pBufferIndex
                = segment.construct< ExampleBufferIndex >( "ExampleBufferIndex" )( segment.get_segment_manager() );

            for ( int i = 0; i < 1000; ++i )
            {
                ExampleBufferIndex::Index     index   = pBufferIndex->allocate();
                ExampleBufferIndex::SharedPtr pBuffer = pBufferIndex->getShared( index );
                for ( int j = 0; j < 100; ++j )
                {
                    pBuffer->Plug->link_5.push_back( mega::reference{} );
                }
                HeapBuffer_3* pHeapBuffer = pBufferIndex->getHeap( index );
                pHeapBuffer->Root[ 0 ].m_testDimension2.push_back( 1 );
            }

            std::cout << "Created: " << pBufferIndex->size() << " free: " << segment.get_free_memory() << std::endl;
        }
        catch ( boost::interprocess::bad_alloc& ex )
        {
            mega::runtime::ManagedSharedMemory segment( boost::interprocess::open_only, SHARED_MEMORY_NAME );
            ExampleBufferIndex* pBufferIndex = segment.find< ExampleBufferIndex >( "ExampleBufferIndex" ).first;
            std::cout << "Exception: Created: " << pBufferIndex->size() << " free: " << segment.get_free_memory()
                      << std::endl;
        }

        char c = std::cin.get();

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
    }*/

    return 0;
}
