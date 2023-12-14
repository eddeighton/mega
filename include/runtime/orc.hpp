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

#include "runtime/functor_id.hxx"

#include "mega/values/compilation/megastructure_installation.hpp"

#include <memory>
#include <set>
#include <string>

namespace mega::runtime
{

class Orc
{
public:
    Orc( const mega::MegastructureInstallation& megastructureInstallation );

    class Module
    {
    protected:
        friend class Orc;
        virtual ~Module() = 0;

        virtual void* getRawFunctionPtr( const std::string& strSymbol ) = 0;
    public:
        using Ptr = std::shared_ptr< Module >;

        inline void* get( const FunctorID& functionID )
        {
            return getRawFunctionPtr( functionID.getSymbol() );
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
