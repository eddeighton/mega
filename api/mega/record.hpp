
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

#ifndef GUARD_2022_October_01_record
#define GUARD_2022_October_01_record

#include "mega/native_types.hpp"
#include "reference.hpp"
#include "event.hpp"

#include <boost/interprocess/streams/vectorstream.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <vector>

namespace mega
{
    class Record
    {
        using Buffer = std::vector< char >;
        enum class Type : char
        {
            LogInfo,
            LogWarn,
            LogError,
            Write
        };

        Type             getType() const { return m_type; }
        TimeStamp        getTimestamp() const { return m_timestamp; }
        const reference& getReference() const { return m_reference; }
        const Buffer&    getBuffer() const { return m_buffer; }

        inline Record( Type type, TimeStamp timeStamp, reference ref )
            : m_type( type )
            , m_timestamp( timeStamp )
            , m_reference( ref )
        {
        }

        template < typename T >
        inline Record( Type type, TimeStamp timeStamp, reference ref, const T& data )
            : m_type( type )
            , m_timestamp( timeStamp )
            , m_reference( ref )
        {
            setBuffer( data );
        }

        template < typename T >
        inline T getBuffer() const
        {
            T                                              data;
            boost::interprocess::basic_vectorbuf< Buffer > is( m_buffer );
            boost::archive::binary_iarchive                ia( is );
            ia&                                            data;
            return data;
        }

        template < typename T >
        inline void setBuffer( const T& data )
        {
            boost::interprocess::basic_vectorbuf< Buffer > os;
            {
                boost::archive::binary_oarchive oa( os );
                oa&                             data;
            }
            m_buffer = os.vector();
        }

        template < class Archive >
        inline void serialize( Archive& archive, const unsigned int version )
        {
            archive& m_type;
            archive& m_timestamp;
            archive& m_reference;
            archive& m_buffer;
        }

        inline operator Event() const { return Event{ m_reference }; }

    private:
        Type            m_type;
        mega::TimeStamp m_timestamp;
        mega::reference m_reference;
        Buffer          m_buffer;
    };
}

#endif //GUARD_2022_October_01_record
