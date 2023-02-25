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

#include <boost/shared_ptr.hpp>

#include <vector>

namespace schematic
{

template < unsigned int PIXEL_SIZE >
class Buffer
{
public:
    using BitmapType = std::vector< unsigned char >;
    using Ptr        = boost::shared_ptr< Buffer >;

    Buffer( unsigned int uiWidth, unsigned int uiHeight )
        : m_uiWidth( uiWidth )
        , m_uiHeight( uiHeight )
        , m_buffer( uiWidth * uiHeight * PIXEL_SIZE )
    {
    }

    unsigned char*            get() { return m_buffer.data(); }
    const unsigned char*      get() const { return m_buffer.data(); }
    unsigned int              getWidth() const { return m_uiWidth; }
    unsigned int              getHeight() const { return m_uiHeight; }
    unsigned int              getStride() const { return m_uiWidth * PIXEL_SIZE; }
    unsigned int              getSize() const { return m_uiWidth * m_uiWidth * PIXEL_SIZE; }
    const Timing::UpdateTick& getLastUpdateTick() const { return m_lastUpdateTick; }
    void                      setModified() { m_lastUpdateTick.update(); }

    const unsigned char* getAt( unsigned int uiX, unsigned int uiY ) const
    {
        const unsigned char* pPixel = 0u;

        ASSERT( uiX < m_uiWidth && uiY < m_uiHeight );
        if( uiX < m_uiWidth && uiY < m_uiHeight )
            pPixel = m_buffer.data() + ( uiY * m_uiWidth * PIXEL_SIZE + uiX );
        return pPixel;
    }

    void resize( unsigned int uiWidth, unsigned int uiHeight )
    {
        const unsigned int uiRequired = uiWidth * uiHeight * PIXEL_SIZE;
        if( m_buffer.size() < uiRequired )
            m_buffer.resize( uiRequired );
        if( uiWidth != m_uiWidth || uiHeight != m_uiHeight )
            m_lastUpdateTick.update();
        m_uiWidth  = uiWidth;
        m_uiHeight = uiHeight;
    }

private:
    unsigned int       m_uiWidth, m_uiHeight;
    BitmapType         m_buffer;
    Timing::UpdateTick m_lastUpdateTick;
};

using NavBitmap = Buffer< 1U >;

} // namespace schematic

#endif // BUFFER_14_09_2013