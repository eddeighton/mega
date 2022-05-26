
#ifndef DECODER_25_MAY_2022
#define DECODER_25_MAY_2022

#include "service/protocol/common/header.hpp"
#include "service/protocol/common/serialisation.hpp"

#include <memory>

namespace mega
{
namespace network
{
class Activity;

class Decoder
{
public:
    virtual bool decode( boost::archive::binary_iarchive& archive, const Header& header,
                         std::shared_ptr< Activity > pActivity )
        = 0;
};
} // namespace network
} // namespace mega

#endif // DECODER_25_MAY_2022
