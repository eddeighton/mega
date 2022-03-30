
#include "database/model/component.hpp"

namespace mega
{

Component::Component( const io::Object& object )
    : Base( object )
{
}

Component::Component( const io::Object&        object,
                      const io::ComponentInfo& componentInfo )
    : Base( object )
    , m_info( componentInfo )
{
}

void Component::load( io::Loader& loader )
{
    loader.load( m_info );
}

void Component::store( io::Storer& storer ) const
{
    storer.store( m_info );
}

} // namespace mega