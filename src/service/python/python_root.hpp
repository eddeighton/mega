
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

#ifndef GUARD_2023_March_12_root
#define GUARD_2023_March_12_root

#include "python_machine.hpp"
#include "python_mpo.hpp"

#include "mega/values/runtime/mpo.hpp"

#include <vector>
#include <string>

namespace mega::service::python
{

class PythonModule;

class PythonRoot
{
public:
    PythonRoot( PythonModule& module );
    PythonRoot( PythonRoot& )  = default;
    PythonRoot( PythonRoot&& ) = default;

    std::vector< PythonMachine > getMachines() const;

private:
    PythonModule& m_module;
};
} // namespace mega::service::python

#endif // GUARD_2023_March_12_root
