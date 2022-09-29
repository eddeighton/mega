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

#include "mega/common.hpp"

#include "service/protocol/common/header.hpp"

#include <string>
#include <vector>

namespace mega
{

class MPOContext
{
public:
    static void        resume( MPOContext* pMPOContext );
    static void        suspend();
    static MPOContext* get();

public:
    using SimID       = network::ConversationID;
    using SimIDVector = std::vector< SimID >;

    // native code interface
    virtual SimIDVector     getSimulationIDs()            = 0;
    virtual SimID           createSimulation()            = 0;
    virtual mega::reference getRoot( const SimID& simID ) = 0;
    virtual mega::reference getRoot()                     = 0;

public:
    // runtime internal interface
    virtual MPO            getThisMPO()                                                       = 0;
    virtual std::string    acquireMemory( MPO mpo )                                           = 0;
    virtual MPO            getNetworkAddressMPO( NetworkAddress networkAddress )              = 0;
    virtual NetworkAddress getRootNetworkAddress( MPO mpo )                                   = 0;
    virtual NetworkAddress allocateNetworkAddress( MPO mpo, TypeID objectTypeID )             = 0;
    virtual void           deAllocateNetworkAddress( MPO mpo, NetworkAddress networkAddress ) = 0;
    virtual void           stash( const std::string& filePath, mega::U64 determinant )        = 0;
    virtual bool           restore( const std::string& filePath, mega::U64 determinant )      = 0;
    virtual bool           readLock( MPO mpo )                                                = 0;
    virtual bool           writeLock( MPO mpo )                                               = 0;
    virtual void           cycleComplete()                                                    = 0;
};

class Cycle
{
public:
    ~Cycle()
    {
        mega::MPOContext::get()->cycleComplete();
    }
};

#define SUSPEND_MPO_CONTEXT()                      \
    MPOContext* _pMPOContext_ = MPOContext::get(); \
    MPOContext::suspend()

#define RESUME_MPO_CONTEXT() MPOContext::resume( _pMPOContext_ )

} // namespace mega

#endif // MPO_CONTEXT_SEPT_18_2022
