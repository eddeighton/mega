
#ifndef TASK_30_MAY_2022
#define TASK_30_MAY_2022

#include <vector>
#include <string>

namespace mega
{
namespace pipeline
{

class TaskDescriptor
{
public:
    using Buffer = std::string;
    using Vector = std::vector< TaskDescriptor >;

    TaskDescriptor();
    TaskDescriptor( const std::string& strName, const Buffer& buffer );

    inline bool operator<( const TaskDescriptor& cmp ) const
    {
        return ( m_strName != cmp.m_strName ) ? ( m_strName < cmp.m_strName ) : ( m_buffer < cmp.m_buffer );
    }
    inline bool operator==( const TaskDescriptor& cmp ) const
    {
        return ( m_strName == cmp.m_strName ) && ( m_buffer == cmp.m_buffer );
    }
    inline bool operator!=( const TaskDescriptor& cmp ) const { return !this->operator==( cmp ); }

    const std::string& getName() const { return m_strName; }
    const Buffer&      getBuffer() const { return m_buffer; }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& m_strName;
        archive& m_buffer;
    }

private:
    std::string m_strName;
    Buffer      m_buffer;
};

} // namespace pipeline
} // namespace mega

#endif // TASK_30_MAY_2022
