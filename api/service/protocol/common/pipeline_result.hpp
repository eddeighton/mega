
#ifndef PIPELINE_RESULT_2_JUNE_2022
#define PIPELINE_RESULT_2_JUNE_2022

#include <string>

namespace mega
{
namespace network
{

class PipelineResult
{
public:
    PipelineResult() {}

    PipelineResult( bool bSuccess, const std::string& strMsg )
        : m_bSuccess( bSuccess )
        , m_strMsg( strMsg )
    {
    }

    bool        getSuccess() const { return m_bSuccess; }
    std::string getMessage() const { return m_strMsg; }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& m_bSuccess;
        archive& m_strMsg;
    }

    bool        m_bSuccess;
    std::string m_strMsg;
};

} // namespace network
} // namespace mega

#endif // PIPELINE_RESULT_2_JUNE_2022