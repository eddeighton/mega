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

namespace
{

std::unique_ptr< mega::EventLogServer > g_eventLogServer( mega::EventLogServer::create( "log" ) );

}

mega::event_iterator events::getIterator()
{
    return g_eventLogServer->head();
}

bool events::get( mega::event_iterator& iterator, Event& event )
{
    const char* type;
    mega::AddressStorage address;
    const void* value;
    std::size_t size;
    while( g_eventLogServer->read( iterator, type, address, value, size ) )
    {
        if( 0U == strcmp( type, "stop" ) )
        {
            event.data = *reinterpret_cast< const mega::reference* >( value );
            return true;
        }
    }  
    return false;
}

bool events::get( mega::event_iterator& iterator, RawEvent& event )
{
    return g_eventLogServer->read( iterator, event.type, event.address, event.value, event.size );
}

void events::put( const char* type, mega::AddressStorage address, const void* value, std::size_t size )
{
    g_eventLogServer->write( type, strlen( type ), address, value, size );
}
    
bool events::update()
{
    return g_eventLogServer->updateHead();
}