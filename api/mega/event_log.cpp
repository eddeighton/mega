

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
    mega::U64 size;
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

void events::put( const char* type, mega::AddressStorage address, const void* value, mega::U64 size )
{
    g_eventLogServer->write( type, strlen( type ), address, value, size );
}
    
bool events::update()
{
    return g_eventLogServer->updateHead();
}