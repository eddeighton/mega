
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

#ifndef GUARD_2023_April_14_maths_types
#define GUARD_2023_April_14_maths_types

#include <boost/qvm/all.hpp>

//#include "mega/archive_traits.hpp"

#include <array>

namespace qvm = boost::qvm;

struct F2
{
    std::array< float, 2 > data;

    constexpr inline float& x() { return data[ 0 ]; }
    constexpr inline float& y() { return data[ 1 ]; }

    constexpr inline const float& x() const { return data[ 0 ]; }
    constexpr inline const float& y() const { return data[ 1 ]; }

    constexpr inline void x( float f ) { data[ 0 ] = f; }
    constexpr inline void y( float f ) { data[ 1 ] = f; }
};

namespace boost::qvm
{
template <>
struct vec_traits< F2 >
{
    static constexpr int const dim = 2;
    using scalar_type              = float;
    template < int I >
    static inline scalar_type& write_element( F2& v )
    {
        return v.data[ I ];
    }
    template < int I >
    static inline scalar_type read_element( F2 const& v )
    {
        return v.data[ I ];
    }
    static inline scalar_type& write_element_idx( int i, F2& v ) { return v.data[ i ]; }      // optional
    static inline scalar_type  read_element_idx( int i, F2 const& v ) { return v.data[ i ]; } // optional
};
} // namespace boost::qvm

struct F3
{
    std::array< float, 3 > data;

    constexpr inline float& x() { return data[ 0 ]; }
    constexpr inline float& y() { return data[ 1 ]; }
    constexpr inline float& z() { return data[ 2 ]; }

    constexpr inline const float& x() const { return data[ 0 ]; }
    constexpr inline const float& y() const { return data[ 1 ]; }
    constexpr inline const float& z() const { return data[ 2 ]; }

    constexpr inline void x( float f ) { data[ 0 ] = f; }
    constexpr inline void y( float f ) { data[ 1 ] = f; }
    constexpr inline void z( float f ) { data[ 2 ] = f; }
};

namespace boost::qvm
{
template <>
struct vec_traits< F3 >
{
    static constexpr int const dim = 3;
    using scalar_type              = float;
    template < int I >
    static inline scalar_type& write_element( F3& v )
    {
        return v.data[ I ];
    }
    template < int I >
    static inline scalar_type read_element( F3 const& v )
    {
        return v.data[ I ];
    }
    static inline scalar_type& write_element_idx( int i, F3& v ) { return v.data[ i ]; }      // optional
    static inline scalar_type  read_element_idx( int i, F3 const& v ) { return v.data[ i ]; } // optional
};
} // namespace boost::qvm

struct F4
{
    std::array< float, 4 > data;

    constexpr inline float& x() { return data[ 0 ]; }
    constexpr inline float& y() { return data[ 1 ]; }
    constexpr inline float& z() { return data[ 2 ]; }
    constexpr inline float& w() { return data[ 3 ]; }

    constexpr inline const float& x() const { return data[ 0 ]; }
    constexpr inline const float& y() const { return data[ 1 ]; }
    constexpr inline const float& z() const { return data[ 2 ]; }
    constexpr inline const float& w() const { return data[ 3 ]; }
    
    constexpr inline void x( float f ) { data[ 0 ] = f; }
    constexpr inline void y( float f ) { data[ 1 ] = f; }
    constexpr inline void z( float f ) { data[ 2 ] = f; }
    constexpr inline void w( float f ) { data[ 3 ] = f; }
};

namespace boost::qvm
{
template <>
struct vec_traits< F4 >
{
    static constexpr int const dim = 4;
    using scalar_type              = float;
    template < int I >
    static inline scalar_type& write_element( F4& v )
    {
        return v.data[ I ];
    }
    template < int I >
    static inline scalar_type read_element( F4 const& v )
    {
        return v.data[ I ];
    }
    static inline scalar_type& write_element_idx( int i, F4& v ) { return v.data[ i ]; }      // optional
    static inline scalar_type  read_element_idx( int i, F4 const& v ) { return v.data[ i ]; } // optional
};
} // namespace boost::qvm

struct Quat
{
    std::array< float, 4 > data;

    constexpr inline float& x() { return data[ 0 ]; }
    constexpr inline float& y() { return data[ 1 ]; }
    constexpr inline float& z() { return data[ 2 ]; }
    constexpr inline float& w() { return data[ 3 ]; }

    constexpr inline const float& x() const { return data[ 0 ]; }
    constexpr inline const float& y() const { return data[ 1 ]; }
    constexpr inline const float& z() const { return data[ 2 ]; }
    constexpr inline const float& w() const { return data[ 3 ]; }
    
    constexpr inline void x( float f ) { data[ 0 ] = f; }
    constexpr inline void y( float f ) { data[ 1 ] = f; }
    constexpr inline void z( float f ) { data[ 2 ] = f; }
    constexpr inline void w( float f ) { data[ 3 ] = f; }
};

namespace boost::qvm
{
template <>
struct quat_traits< Quat >
{
    static constexpr int const dim = 4;
    using scalar_type              = float;
    template < int I >
    static inline scalar_type& write_element( Quat& v )
    {
        return v.data[ I ];
    }
    template < int I >
    static inline scalar_type read_element( Quat const& v )
    {
        return v.data[ I ];
    }
    static inline scalar_type& write_element_idx( int i, Quat& v ) { return v.data[ i ]; }      // optional
    static inline scalar_type  read_element_idx( int i, Quat const& v ) { return v.data[ i ]; } // optional
};
} // namespace boost::qvm

struct F33
{
    std::array< std::array< float, 3 >, 3 > data;
};

namespace boost::qvm
{
template <>
struct mat_traits< F33 >
{
    static int const rows = 3;
    static int const cols = 3;
    typedef float    scalar_type;
    template < int R, int C >
    static inline scalar_type& write_element( F33& m )
    {
        return m.data[ R ][ C ];
    }
    template < int R, int C >
    static inline scalar_type read_element( F33 const& m )
    {
        return m.data[ R ][ C ];
    }
    static inline scalar_type& write_element_idx( int r, int c, F33& m ) { return m.data[ r ][ c ]; }
    static inline scalar_type  read_element_idx( int r, int c, F33 const& m ) { return m.data[ r ][ c ]; }
};
} // namespace boost::qvm

#endif // GUARD_2023_April_14_maths_types
