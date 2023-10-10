#  Copyright (c) Deighton Systems Limited. 2022. All Rights Reserved.
#  Author: Edward Deighton
#  License: Please see license.txt in the project root folder.
#
#  Use and copying of this software and preparation of derivative works
#  based upon this software are permitted. Any copy of this software or
#  of any derivative work must include the above copyright notice, this
#  paragraph and the one after it.  Any distribution of this software or
#  derivative works must comply with all applicable laws.
#
#  This software is made available AS IS, and COPYRIGHT OWNERS DISCLAIMS
#  ALL WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE
#  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
#  PURPOSE, AND NOTWITHSTANDING ANY OTHER PROVISION CONTAINED HEREIN, ANY
#  LIABILITY FOR DAMAGES RESULTING FROM THE SOFTWARE OR ITS USE IS
#  EXPRESSLY DISCLAIMED, WHETHER ARISING IN CONTRACT, TORT (INCLUDING
#  NEGLIGENCE) OR STRICT LIABILITY, EVEN IF COPYRIGHT OWNERS ARE ADVISED
#  OF THE POSSIBILITY OF SUCH DAMAGES.


import sys
import os

# grab the mega structure environment
WORKSPACE_ROOT_PATH = os.environ["WORKSPACE_ROOT_PATH"]
BUILD_PATH = os.environ["BUILD_PATH"]
CFG_LINK = os.environ["CFG_LINK"]
CFG_TYPE = os.environ["CFG_TYPE"]
CFG_TUPLE = os.environ["CFG_TUPLE"]

MEGA_BIN = BUILD_PATH + "/" + CFG_TUPLE + "/mega/install/bin"

sys.path.append(MEGA_BIN)

os.chdir("/home/foobar/test_{}".format(CFG_TYPE))

import megastructure
import asyncio

mega = megastructure
F2 = mega.F2
F3 = mega.F3
F4 = mega.F4
Quat = mega.Quat

Type = mega.Type

_continue = True

async def run_megastructure():
    global _continue
    while _continue:
        megastructure.run_one()
        await asyncio.sleep(0.1)

def dump( ref ):
    print( ref.dump() )


asyncio.create_task( run_megastructure() )

print( "CFG_TUPLE: {}".format( CFG_TUPLE ) )
print( "cwd: {}".format( os.getcwd() ) )

def createMPOOnProcess( process ):
    p = mega.getProcess( process )
    m = p.createMPO()
    return m

def test():
    
    r = mega.getMPO().getRoot()
    t = mega.new( Type.Toaster )
    dump( r )
    dump( t )