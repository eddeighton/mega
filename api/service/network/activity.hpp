#ifndef ACTIVITY_24_MAY_2022
#define ACTIVITY_24_MAY_2022

#include <memory>
#include <set>

namespace mega
{
namespace network
{

class Activity
{
public:
    using Ptr = std::shared_ptr< Activity >;
    using PtrSet = std::set< Ptr >;


};

class ActivityManager
{
public:

    Activity::Ptr start();

private:
    Activity::PtrSet m_active, m_pending;
};

}
} // namespace mega

#endif // ACTIVITY_24_MAY_2022
