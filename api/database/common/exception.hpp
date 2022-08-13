#ifndef EXCEPTION_11_MAY_2022
#define EXCEPTION_11_MAY_2022

#include <stdexcept>
#include <string>

namespace mega
{
namespace io
{

class DatabaseVersionException : public std::exception
{
public:
    DatabaseVersionException( const std::string& str )
        : m_string( str )
    {
    }

    virtual const char* what() const noexcept { return m_string.c_str(); }

private:
    std::string m_string;
};

} // namespace io
} // namespace mega

#endif // EXCEPTION_11_MAY_2022