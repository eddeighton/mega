



#ifndef NAME_RESOLUTION_8_JUNE_2022
#define NAME_RESOLUTION_8_JUNE_2022

#include "database/model/OperationsStage.hxx"

#include <stdexcept>

namespace mega
{
namespace invocation
{

class NameResolutionException : public std::runtime_error
{
public:
    NameResolutionException( const std::string& strMessage )
        : std::runtime_error( strMessage )
    {
    }
};

OperationsStage::Operations::NameResolution* resolve( OperationsStage::Database&                  database,
                                                      OperationsStage::Operations::Invocation*    pInvocation

);

} // namespace invocation
} // namespace mega

#endif // NAME_RESOLUTION_8_JUNE_2022
