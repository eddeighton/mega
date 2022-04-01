
#include "database/model/dependencies.hpp"

namespace mega
{

DependencyAnalysis::DependencyAnalysis( const io::ObjectInfo& object )
    : Base( object )
{
}

void DependencyAnalysis::load( io::Loader& loader )
{
    loader.loadObjectMap( m_dependencies );

}

void DependencyAnalysis::store( io::Storer& storer ) const
{
    storer.storeObjectMap( m_dependencies );
}

} // namespace mega