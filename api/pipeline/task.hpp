
//  Copyright (c) Deighton Systems Limited. 2022. All Rights Reserved.
//  Author: Edward Deighton
//  License: Please see license.txt in the project root folder.

//  Use and copying of this software and preparation of derivative works
//  based upon this software are permitted. Any copy of this software or
//  of any derivative work must include the above copyright notice, this
//  paragraph and the one after it.  Any distribution of this software or
//  derivative works must comply with all applicable laws.

//  This software is made available AS IS, and COPYRIGHT OWNERS DISCLAIMS
//  ALL WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
//  PURPOSE, AND NOTWITHSTANDING ANY OTHER PROVISION CONTAINED HEREIN, ANY
//  LIABILITY FOR DAMAGES RESULTING FROM THE SOFTWARE OR ITS USE IS
//  EXPRESSLY DISCLAIMED, WHETHER ARISING IN CONTRACT, TORT (INCLUDING
//  NEGLIGENCE) OR STRICT LIABILITY, EVEN IF COPYRIGHT OWNERS ARE ADVISED
//  OF THE POSSIBILITY OF SUCH DAMAGES.


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
    TaskDescriptor( const std::string& strName, const std::string& strSourceFileName, const Buffer& buffer );

    inline bool operator<( const TaskDescriptor& cmp ) const
    {
        return ( m_strName != cmp.m_strName )               ? ( m_strName < cmp.m_strName )
               : ( m_strSourceFile != cmp.m_strSourceFile ) ? ( m_strSourceFile < cmp.m_strSourceFile )
                                                            : ( m_buffer < cmp.m_buffer );
    }
    inline bool operator==( const TaskDescriptor& cmp ) const
    {
        return ( m_strName == cmp.m_strName ) && ( m_strSourceFile == cmp.m_strSourceFile )
               && ( m_buffer == cmp.m_buffer );
    }
    inline bool operator!=( const TaskDescriptor& cmp ) const { return !this->operator==( cmp ); }

    const std::string& getName() const { return m_strName; }
    const std::string  getSourceFile() const { return m_strSourceFile; }
    const Buffer&      getBuffer() const { return m_buffer; }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& m_strName;
        archive& m_strSourceFile;
        archive& m_buffer;
    }

private:
    std::string m_strName, m_strSourceFile;
    Buffer      m_buffer;
};

} // namespace pipeline
} // namespace mega

#endif // TASK_30_MAY_2022
