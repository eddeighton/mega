
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
    TaskDescriptor( const Buffer& buffer );

    inline bool operator<( const TaskDescriptor& cmp ) const { return m_buffer < cmp.m_buffer; }
    inline bool operator==( const TaskDescriptor& cmp ) const { return m_buffer == cmp.m_buffer; }
    inline bool operator!=( const TaskDescriptor& cmp ) const { return m_buffer != cmp.m_buffer; }

    const Buffer& get() const { return m_buffer; }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& m_buffer;
    }

private:
    Buffer m_buffer;
};

} // namespace pipeline
} // namespace mega

#endif // TASK_30_MAY_2022
