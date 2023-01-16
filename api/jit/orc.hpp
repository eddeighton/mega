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

#ifndef JIT_8_AUG_2022
#define JIT_8_AUG_2022

#if defined( _WIN32 )
#   ifdef MEGA_ORC_API_SHARED_MODULE
#       define ORC_EXPORT __declspec( dllexport )
#   else
#       define ORC_EXPORT __declspec( dllimport )
#   endif
#elif defined( __GNUC__ )
#   ifdef MEGA_ORC_API_SHARED_MODULE
#       define ORC_EXPORT __attribute__( ( visibility( "default" ) ) )
#   else
#       define ORC_EXPORT
#   endif
#endif

#include <memory>
#include <set>
#include <string>

namespace mega::runtime
{

class ORC_EXPORT JITCompiler
{
public:
    JITCompiler();

    class ORC_EXPORT Module
    {
    protected:
        friend class JITCompiler;
        virtual ~Module() = 0;

    public:
        using Ptr = std::shared_ptr< Module >;

        virtual void* getRawFunctionPtr( const std::string& strSymbol ) = 0;

        template< typename FunctionType >
        FunctionType get( const std::string& strSymbol )
        {
            return reinterpret_cast< FunctionType >( getRawFunctionPtr( strSymbol ) );
        }
    };
 
    Module::Ptr compile( const std::string& strModule );

private:
    void unload( Module* pModule );

    class Pimpl;
    std::shared_ptr< Pimpl > m_pPimpl;
};

} // namespace mega::runtime

#endif // JIT_8_AUG_2022
