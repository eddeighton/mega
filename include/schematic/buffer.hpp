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

#ifndef BUFFER_14_09_2013
#define BUFFER_14_09_2013

#include "common/tick.hpp"
#include "common/assert_verify.hpp"

#include <vector>

namespace schematic
{

template < std::size_t PIXEL_SIZE >
class Buffer
{
    using BitmapType = std::vector< unsigned char >;

public:
    using Size = std::size_t;

    static constexpr inline Size bufferSize( Size uiWidth, Size uiHeight ) { return uiWidth * uiHeight * PIXEL_SIZE; }

    Buffer( Buffer& )            = delete;
    Buffer& operator=( Buffer& ) = delete;

    inline Buffer( Size uiWidth = 0U, Size uiHeight = 0U )
        : m_uiWidth( uiWidth )
        , m_uiHeight( uiHeight )
        , m_buffer( bufferSize( m_uiWidth, m_uiHeight ) )
    {
    }

    inline unsigned char*            get() { return m_buffer.data(); }
    inline const unsigned char*      get() const { return m_buffer.data(); }
    inline Size                      getWidth() const { return m_uiWidth; }
    inline Size                      getHeight() const { return m_uiHeight; }
    inline Size                      getStride() const { return m_uiWidth * PIXEL_SIZE; }
    inline Size                      getSize() const { return bufferSize( m_uiWidth, m_uiWidth ); }
    inline const Timing::UpdateTick& getLastUpdateTick() const { return m_lastUpdateTick; }
    inline void                      setModified() { m_lastUpdateTick.update(); }

    inline const unsigned char* getAt( Size uiX, Size uiY ) const
    {
        const unsigned char* pPixel = nullptr;
        ASSERT( uiX < m_uiWidth && uiY < m_uiHeight );
        if( uiX < m_uiWidth && uiY < m_uiHeight )
        {
            pPixel = m_buffer.data() + ( ( uiY * m_uiWidth * PIXEL_SIZE ) + uiX );
        }
        return pPixel;
    }

    inline void resize( Size uiWidth, Size uiHeight )
    {
        const Size uiRequired = bufferSize( uiWidth, uiHeight );
        if( m_buffer.size() < uiRequired )
            m_buffer.resize( uiRequired );
        if( uiWidth != m_uiWidth || uiHeight != m_uiHeight )
            m_lastUpdateTick.update();
        m_uiWidth  = uiWidth;
        m_uiHeight = uiHeight;
    }

    inline void reset()
    {
        for( auto& p : m_buffer )
        {
            p = 0U;
        }
    }

private:
    Size               m_uiWidth, m_uiHeight;
    BitmapType         m_buffer;
    Timing::UpdateTick m_lastUpdateTick;
};

using MonoBitmap = Buffer< 1U >;

} // namespace schematic

#endif // BUFFER_14_09_2013