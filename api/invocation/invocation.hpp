
#ifndef INVOCATION_6_JUNE_2022
#define INVOCATION_6_JUNE_2022

#include "database/model/environment.hxx"
#include "database/types/invocation_id.hpp"

#include "database/model/OperationsStage.hxx"

#include <stdexcept>
#include <vector>
#include <set>

namespace mega
{
namespace invocation
{

class Exception : public std::runtime_error
{
public:
    Exception( const std::string& strMessage )
        : std::runtime_error( strMessage )
    {
    }
};

#define THROW_INVOCATION_EXCEPTION( msg ) \
    DO_STUFF_AND_REQUIRE_SEMI_COLON( std::ostringstream _os; _os << msg; throw Exception( _os.str() ); )


template< class T >
inline std::vector< T > uniquify_without_reorder( const std::vector< T >& ids )
{
    /*
    not this...
    std::sort( ids.begin(), ids.end() );
    auto last = std::unique( ids.begin(), ids.end() );
    ids.erase( last, ids.end() );
    */
    
    std::vector< T > result;
    std::set< T > uniq;
    for( const T& value : ids )
    {
        if( uniq.count( value ) == 0 )
        {
            result.push_back( value );
            uniq.insert( value );
        }
    }
    return result;
}

OperationsStage::Operations::Invocation* construct( io::Environment& environment, const mega::invocation::ID& id,
                                                    OperationsStage::Database&    database,
                                                    const mega::io::megaFilePath& sourceFile );

} // namespace invocation
} // namespace mega

#endif // INVOCATION_6_JUNE_2022
