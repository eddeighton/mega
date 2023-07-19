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

#ifndef MPO_CONTEXT_SEPT_18_2022
#define MPO_CONTEXT_SEPT_18_2022

#include "jit_base.hpp"

#include "mega/reference.hpp"

#include <string>
#include <vector>

namespace mega
{
namespace log
{
class Storage;
}

class Context
{
public:
    using MachineIDVector        = std::vector< MachineID >;
    using MachineProcessIDVector = std::vector< MP >;
    using MPOVector              = std::vector< MPO >;

    // queries
    virtual MachineIDVector        getMachines()                       = 0;
    virtual MachineProcessIDVector getProcesses( MachineID machineID ) = 0;
    virtual MPOVector              getMPO( MP machineProcess )         = 0;

    virtual MPO             getThisMPO()                                  = 0;
    virtual mega::reference getThisRoot()                                 = 0;
    virtual mega::reference getRoot( MPO mpo )                            = 0;
    virtual MPO             constructMPO( MP machineProcess )             = 0;
    virtual MP              constructExector( MachineID daemonMachineID ) = 0;

    virtual void jit( runtime::JITFunctor func ) = 0;
    virtual void yield()                         = 0;

    // clock
    virtual TimeStamp cycle() = 0;
    virtual F32       ct()    = 0;
    virtual F32       dt()    = 0;

    // log
    virtual log::Storage& getLog() = 0;

    static Context* get();
};

class MPOContext;
MPOContext* getMPOContext();
void        resetMPOContext();
void        setMPOContext( MPOContext* pMPOContext );

struct _MPOContextStack
{
    MPOContext* _pMPOContext_;
    inline _MPOContextStack()
    {
        _pMPOContext_ = getMPOContext();
        resetMPOContext();
    }
    inline ~_MPOContextStack() { setMPOContext( _pMPOContext_ ); }
};

} // namespace mega

#endif // MPO_CONTEXT_SEPT_18_2022
