
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

#ifndef GUARD_2023_March_12_MACHINE
#define GUARD_2023_March_12_MACHINE

#include "process.hpp"

#include "mega/values/runtime/mpo.hpp"

#include <vector>

namespace mega::service::python
{

class PythonModule;

class PythonMachine
{
public:
    PythonMachine( PythonModule& module, mega::MachineID machineID );

    std::vector< PythonProcess > getProcesses() const;
    PythonProcess createExecutor() const;
private:
    PythonModule&   m_module;
    mega::MachineID m_machineID;
};
} // namespace mega::service::python

#endif // GUARD_2023_March_12_MACHINE
