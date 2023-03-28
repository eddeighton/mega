
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
WORKSPACE_ROOT_PATH = os.environ['WORKSPACE_ROOT_PATH']
BUILD_PATH = os.environ['BUILD_PATH']
CFG_LINK = os.environ['CFG_LINK']
CFG_TYPE = os.environ['CFG_TYPE']
CFG_TUPLE = os.environ['CFG_TUPLE']

MEGA_BIN =  BUILD_PATH + '/' + CFG_TUPLE + "/mega/install/bin" 

sys.path.append( MEGA_BIN )

os.chdir( "/home/foobar/test_{}".format( CFG_TYPE ) )

import megastructure
import asyncio

mega = megastructure

_continue = True

async def run_megastructure():
    global _continue
    while _continue:
        megastructure.run_one()
        await asyncio.sleep(0.1)


asyncio.create_task( run_megastructure() )

def getAllMPOs():
    for machine in megastructure.getRoot().getMachines():
        for process in machine.getProcesses():
            for mpo in process.getMPOs():
                yield mpo

def getAllMPOsList():
    return [mpo for mpo in getAllMPOs()]

def getFirstMPO():
    return getAllMPOsList()[0]

def getFirstRoot():
    return getFirstMPO().getRoot()

mega = megastructure

def example():
    p = mega.getProcess( "0.0")
    mpo = p.createMPO()
    r = mpo.getRoot()
    w1 = r.SocketSurface.WallSocket()
    r2 = w1.SocketInstall()

    print( "Got root back from created WallSocket: {} {}".format( w1, r2 ) )
        
    ws = r.SocketSurface()
    w2 = ws[0]
    r3 = w2.SocketInstall()
    r4 = r2.Root.Get()

    print( "Got root back: {} {}".format( r3, r4 ) )

def getFirstSimRoot():
    mpo = mega.getMPO( "0.0.0" )
    return mpo.getRoot()
