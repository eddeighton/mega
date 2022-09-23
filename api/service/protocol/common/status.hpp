#ifndef STATUS_23_SEPT_2022
#define STATUS_23_SEPT_2022

#include <vector>
#include <ostream>

namespace mega
{
namespace network
{

class Status
{
public:
    using StatusVector = std::vector< Status >;

    Status() {}
    Status( const StatusVector& children )
        : m_childStatus( children )
    {
    }

    const std::string&  getDescription() const { return m_description; }
    const StatusVector& getChildren() const { return m_childStatus; }

    void setDescription( const std::string& strDescription ) { m_description = strDescription; }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& m_description;
        archive& m_childStatus;
    }

private:
    std::string m_description;

    StatusVector m_childStatus;
};

std::ostream& operator<<( std::ostream& os, const Status& conversationID );

} // namespace network
} // namespace mega

#endif // STATUS_23_SEPT_2022
