
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

#include <gtest/gtest.h>
#include <gtest/gtest-param-test.h>

#include "mega/values/native_types.hpp"
#include "mega/values/compilation/interface/relation_id.hpp"

#include "event/filename.hpp"
#include "event/records.hxx"
#include "event/file_log.hpp"
#include "event/memory_log.hpp"

#include <boost/filesystem/operations.hpp>

#include <string_view>

using Path = boost::filesystem::path;

struct LogFilenameTestData
{
    Path                   root;
    std::string            name;
    mega::event::BufferIndex index;
    Path                   expected;
};

class FileNamesAccept : public ::testing::TestWithParam< LogFilenameTestData >
{
protected:
    LogFilenameTestData data;
};

TEST_P( FileNamesAccept, AcceptedFileNames )
{
    const LogFilenameTestData data = GetParam();
    ASSERT_EQ( mega::event::toFilePath( data.root, data.name, data.index ), data.expected );
    std::string            strFileType;
    mega::event::BufferIndex index;
    ASSERT_TRUE( mega::event::fromFilePath( data.expected, strFileType, index ) );
    ASSERT_EQ( strFileType, data.name );
    ASSERT_EQ( data.index, index );
}

// clang-format off
INSTANTIATE_TEST_SUITE_P( TestFileNames, FileNamesAccept,
        ::testing::Values
        ( 
            LogFilenameTestData{ Path{ "/tmp" },        "test123", 0,           Path{    "/tmp/test123_0000000000.log"  } },
            LogFilenameTestData{ Path{ "/tmp" },        "ABC123",  123,         Path{     "/tmp/ABC123_0000000123.log"  } },
            LogFilenameTestData{ Path{ "/tmp" },        "test",    0,           Path{       "/tmp/test_0000000000.log"  } },
            LogFilenameTestData{ Path{ "/tmp/a/b/c" },  "TEST",    1234567890,  Path{ "/tmp/a/b/c/TEST_1234567890.log"  } } 
        ));
// clang-format on

class FileNamesReject : public ::testing::TestWithParam< Path >
{
};

TEST_P( FileNamesReject, RejectedFileNames )
{
    std::string            strFileType;
    mega::event::BufferIndex index;
    ASSERT_FALSE( mega::event::fromFilePath( GetParam(), strFileType, index ) );
}

// clang-format off
INSTANTIATE_TEST_SUITE_P( TestFileNames, FileNamesReject,
        ::testing::Values
        ( 
            Path{    "/tmp/test123_0000000000.lo"   },
            Path{     "/tmp/ABC123_0000000123.logg" },
            Path{       "_test_0000000000.log"      },
            Path{ "/tmp/a/b/c/@TEST_1234567890.log" },
            Path{ "/tmp/a/b/c/TEST*_1234567890.log" },
            Path{ "/tmp/a/b/c/TEST_1234567890" } 
        ));
// clang-format on

TEST( MemoryLogTests, Basic )
{
    using namespace mega::event;

    MemoryStorage log;

    // clang-format off
    std::vector< Log::Type > types = 
    {
        Log::eTrace,
        Log::eDebug,
        Log::eInfo, 
        Log::eWarn, 
        Log::eError,
        Log::eFatal
    };
    std::vector< std::string > msgs =
    {
        "Test Log Msg with Log::eTrace",
        "Test Log Msg with Log::eDebug",
        "Test Log Msg with Log::eInfo",
        "Test Log Msg with Log::eWarn",
        "Test Log Msg with Log::eError",
        "Test Log Msg with Log::eFatal"
    };
    // clang-format on
    const int iTests = types.size();
    for( int i = 0; i < iTests; ++i )
    {
        log.record( Log::Write( types[ i ], msgs[ i ] ) );
    }

    int index = 0;
    for( auto i = log.begin< Log::Read >(), iEnd = log.end< Log::Read >(); i != iEnd; ++i, ++index )
    {
        Log::Read r = *i;
        ASSERT_EQ( r.getType(), types[ index ] );
        ASSERT_EQ( r.getMessage(), msgs[ index ] );
    }
}

namespace bfs = boost::filesystem;

class BasicLogTest : public ::testing::Test
{
public:
    bfs::path m_folder;

protected:
    virtual void SetUp() override { m_folder = bfs::temp_directory_path() / "log_test"; }
    virtual void TearDown() override { bfs::remove_all( m_folder ); }
};

TEST_F( BasicLogTest, Cycles )
{
    const boost::filesystem::path logPath = m_folder / "basic";
    mega::event::FileStorage        log( logPath, false );

    using namespace mega::event;

    // fill first log file
    for( int i = 0; i < mega::event::FileStorage::IndexType::RecordsPerFile * 3; ++i )
    {
        ASSERT_EQ( log.getTimeStamp(), i );
        ASSERT_EQ( log.get( TrackID::eLog ), Offset{} );
        log.cycle();
    }
}

TEST_F( BasicLogTest, LogMsg )
{
    using namespace mega::event;

    // clang-format off
    std::vector< Log::Type > types = 
    {
        Log::eTrace,
        Log::eDebug,
        Log::eInfo, 
        Log::eWarn, 
        Log::eError,
        Log::eFatal
    };
    std::vector< std::string > msgs =
    {
        "Test Log Msg with Log::eTrace",
        "Test Log Msg with Log::eDebug",
        "Test Log Msg with Log::eInfo",
        "Test Log Msg with Log::eWarn",
        "Test Log Msg with Log::eError",
        "Test Log Msg with Log::eFatal"
    };
    // clang-format on
    const int iTests = types.size();

    const boost::filesystem::path logPath = m_folder / "LogMsg";
    FileStorage                   log( logPath, false );

    for( int i = 0; i < iTests; ++i )
    {
        log.record( Log::Write( types[ i ], msgs[ i ] ) );
    }

    int index = 0;
    for( auto i = log.begin< Log::Read >(), iEnd = log.end< Log::Read >(); i != iEnd; ++i, ++index )
    {
        Log::Read r = *i;
        ASSERT_EQ( r.getType(), types[ index ] );
        ASSERT_EQ( r.getMessage(), msgs[ index ] );
    }
}

TEST_F( BasicLogTest, StructureMsg )
{
    using namespace mega::event;

    // clang-format off
    std::vector< Structure::Type > types = 
    {
        Structure::eConstruct, 
        Structure::eDestruct, 
        Structure::eMake, 
        Structure::eBreak,
        Structure::eBreak
    };
    std::vector< mega::Pointer > sources =
    {
        mega::Pointer{},
        mega::max_net_ref,
        mega::min_net_ref,
        mega::max_heap_ref,
        mega::min_heap_ref,
    };
    std::vector< mega::Pointer > targets =
    {
        mega::Pointer{},
        mega::max_net_ref,
        mega::min_net_ref,
        mega::max_heap_ref,
        mega::min_heap_ref,
    };
    std::vector< mega::RelationID > relationIDs = 
    {
        mega::RelationID{ mega::TypeID{}, mega::TypeID{} },
        mega::RelationID{ mega::TypeID{}, mega::min_typeID_context },
        mega::RelationID{ mega::TypeID{}, mega::max_typeID_context },
        mega::RelationID{ mega::min_typeID_context, mega::max_typeID_context },
        mega::RelationID{ mega::max_typeID_context, mega::max_typeID_context },
    };
    // clang-format on
    const int iTests = types.size();

    const boost::filesystem::path logPath = m_folder / "StructureMsg";
    FileStorage                   log( logPath, false );

    for( int i = 0; i < iTests; ++i )
    {
        log.record( Structure::Write( sources[ i ], targets[ i ], relationIDs[ i ].getID(), types[ i ] ) );
    }

    int index = 0;
    for( auto i = log.begin< Structure::Read >(), iEnd = log.end< Structure::Read >(); i != iEnd; ++i, ++index )
    {
        Structure::Read r = *i;
        ASSERT_EQ( r.getType(), types[ index ] );
        ASSERT_EQ( r.getRelation(), relationIDs[ index ].getID() );
        ASSERT_EQ( r.getSource(), sources[ index ] );
        ASSERT_EQ( r.getTarget(), targets[ index ] );
    }
}

TEST_F( BasicLogTest, MemoryMsg )
{
    using namespace mega::event;

    const boost::filesystem::path logPath = m_folder / "MemoryMsg";
    FileStorage                   log( logPath, false );
}

#pragma pack( 1 )
struct MemoryReadHeader
{
    mega::U16       size;
    mega::Pointer ref;
    mega::U16       dataSize;
};
#pragma pack()

TEST_F( BasicLogTest, Range )
{
    using namespace mega::event;

    const boost::filesystem::path logPath = m_folder / "Range";
    FileStorage                   log( logPath, false );

    std::string      strTest = "test string";
    std::string_view strView( strTest );

    auto                           start = log.getTimeStamp();
    std::vector< mega::Pointer > expected{ mega::Pointer{}, mega::max_net_ref, mega::min_net_ref };
    {
        for( const auto& ex : expected )
        {
            log.record( Memory::Write( ex, strView ) );
        }
    }
    log.cycle();

    auto end   = log.getTimeStamp();
    auto range = log.getRange( start );
    ASSERT_TRUE( range.m_memory.m_begin != range.m_memory.m_end );

    {
        int  memCount = 0;
        auto i        = expected.begin();
        for( mega::U64 iter = range.m_memory.m_begin; iter != range.m_memory.m_end; ++i, ++memCount )
        {
            ASSERT_TRUE( memCount < expected.size() );

            Memory::Read record( reinterpret_cast< void* >( iter ) );
            ASSERT_EQ( record.getRef(), *i );

            {
                MemoryReadHeader* pHeader = reinterpret_cast< MemoryReadHeader* >( iter );
                ASSERT_EQ( pHeader->ref, *i );
            }

            iter += record.size();
        }
    }
}

TEST_F( BasicLogTest, Load )
{
    using namespace mega::event;

    const boost::filesystem::path logPath = m_folder / "Timestamp";

    {
        FileStorage log( logPath, false );

        ASSERT_EQ( log.getTimeStamp(), 0 );
        log.record( Log::Write( Log::eInfo, "0" ) );
        log.cycle();
        ASSERT_EQ( log.getTimeStamp(), 1 );
        log.record( Log::Write( Log::eInfo, "1" ) );
        log.cycle();
        ASSERT_EQ( log.getTimeStamp(), 2 );
        log.record( Log::Write( Log::eInfo, "2" ) );
        log.cycle();
        ASSERT_EQ( log.getTimeStamp(), 3 );
        log.record( Log::Write( Log::eInfo, "3" ) );
        log.cycle();
    }

    {
        FileStorage log( logPath, true );
        ASSERT_EQ( log.getTimeStamp(), 4 );

        {
            int iCounter = 0;
            for( auto i = log.begin< Log::Read >(), iEnd = log.end< Log::Read >(); i != iEnd; ++i, ++iCounter )
            {
                Log::Read r = *i;
                switch( iCounter )
                {
                    case 0:
                    {
                        ASSERT_EQ( r.getType(), Log::eInfo );
                        ASSERT_EQ( r.getMessage(), "0" );
                    }
                    break;
                    case 1:
                    {
                        ASSERT_EQ( r.getType(), Log::eInfo );
                        ASSERT_EQ( r.getMessage(), "1" );
                    }
                    break;
                    case 2:
                    {
                        ASSERT_EQ( r.getType(), Log::eInfo );
                        ASSERT_EQ( r.getMessage(), "2" );
                    }
                    break;
                    case 3:
                    {
                        ASSERT_EQ( r.getType(), Log::eInfo );
                        ASSERT_EQ( r.getMessage(), "3" );
                    }
                    break;
                    default:
                    {
                        FAIL();
                    }
                    break;
                }
            }
            ASSERT_EQ( iCounter, 4 );
        }
        {
            int iCounter = 0;
            for( auto i = log.begin< Log::Read >( 1 ), iEnd = log.begin< Log::Read >( 3 ); i != iEnd; ++i, ++iCounter )
            {
                Log::Read r = *i;
                switch( iCounter )
                {
                    case 0:
                    {
                        ASSERT_EQ( r.getType(), Log::eInfo );
                        ASSERT_EQ( r.getMessage(), "1" );
                    }
                    break;
                    case 1:
                    {
                        ASSERT_EQ( r.getType(), Log::eInfo );
                        ASSERT_EQ( r.getMessage(), "2" );
                    }
                    break;
                    default:
                    {
                        FAIL();
                    }
                    break;
                }
            }
            ASSERT_EQ( iCounter, 2 );
        }
        {
            int iCounter = 0;
            for( auto i = log.begin< Log::Read >( 3 ), iEnd = log.end< Log::Read >(); i != iEnd; ++i, ++iCounter )
            {
                Log::Read r = *i;
                ASSERT_EQ( r.getType(), Log::eInfo );
                ASSERT_EQ( r.getMessage(), "3" );
            }
            ASSERT_EQ( iCounter, 1 );
        }
    }
}

/*
TEST_F( BasicLogTest, LogMsgMany_ )
{
    using namespace mega::event;

    std::vector< LogMsg > testMsgs;
    const int             totalMsgs = 100000;
    {
        const char* msg
            = "Elit nulla aliquip fugiat ut eiusmod officia proident quis nulla. Reprehenderit amet occaecat id "
              "exercitation laboris anim culpa culpa magna irure cupidatat cillum aliquip laboris. Cupidatat dolore "
              "sint "
              "do. Enim fugiat mollit mollit do. Ea ipsum ut id Lorem cillum aliqua cupidatat non.";

        std::vector< LogMsg > rawMsgs
            = { LogMsg{ LogMsg::eTrace, msg }, LogMsg{ LogMsg::eDebug, msg }, LogMsg{ LogMsg::eInfo, msg },
                LogMsg{ LogMsg::eWarn, msg },  LogMsg{ LogMsg::eError, msg }, LogMsg{ LogMsg::eFatal, msg } };
        for ( int i = 0; i < totalMsgs; ++i )
        {
            std::copy( rawMsgs.begin(), rawMsgs.end(), std::back_inserter( testMsgs ) );
        }
    }

    const boost::filesystem::path logPath = m_folder / "LogMsgMany";
    FileStorage log( logPath, false );

    int               iCounter     = 0;
    const int         msgsPerCycle = 1000;
    mega::event::Offset lastOffset;
    for ( const auto msg : testMsgs )
    {
        if ( ( iCounter != 0 ) && ( iCounter % msgsPerCycle == 0 ) )
        {
            const mega::TimeStamp   lastCycle         = log.getTimeStamp();
            const mega::event::Offset oldOffset         = log.get( mega::event::TrackID::Log, lastCycle );
            const mega::event::Offset expectedNewOffset = log.get( mega::event::TrackID::Log );

            log.cycle();
            const mega::TimeStamp   newCycle  = log.getTimeStamp();
            const mega::event::Offset newOffset = log.get( mega::event::TrackID::Log, newCycle );

            ASSERT_EQ( newOffset, expectedNewOffset )
                << "iCounter:" << iCounter << " lastCycle:" << lastCycle << " newCycle:" << newCycle
                << " newOffset:" << newOffset.get() << " expectedNewOffset:" << expectedNewOffset.get();

            ASSERT_TRUE( newOffset != lastOffset )
                << "iCounter:" << iCounter << " lastCycle:" << lastCycle << " newCycle:" << newCycle
                << " newOffset:" << newOffset.get() << " lastOffset:" << lastOffset.get();

            ASSERT_TRUE( newOffset != oldOffset )
                << "iCounter:" << iCounter << " lastCycle:" << lastCycle << " newCycle:" << newCycle
                << " newOffset: " << newOffset.get() << " oldOffset:" << oldOffset.get();

            lastOffset = newOffset;
        }

        log.log( msg );
        ++iCounter;
    }

    log.cycle();

    const mega::TimeStamp finalTimeStamp = ( testMsgs.size() / msgsPerCycle );
    ASSERT_EQ( log.getTimeStamp(), finalTimeStamp );

    {
        auto iIter = testMsgs.begin();
        for ( auto i = log.logBegin(), iEnd = log.logEnd(); i != iEnd; ++i, ++iIter )
        {
            ASSERT_EQ( *i, *iIter );
        }
        ASSERT_EQ( iIter, testMsgs.end() );
    }

    {
        auto iIter = testMsgs.begin();
        for ( mega::TimeStamp timeStamp = 0; timeStamp != finalTimeStamp; timeStamp++ )
        {
            int  iCounter = 0;
            auto i = log.logBegin( timeStamp ), iEnd = log.logBegin( timeStamp + 1 );
            for ( ; i != iEnd; ++i, ++iIter, ++iCounter )
            {
                ASSERT_EQ( *i, *iIter ) << "Timestamp: " << timeStamp << " Counter: " << iCounter;
            }
            ASSERT_EQ( iCounter, msgsPerCycle )
                << "Timestamp: " << timeStamp << " i: " << i.position().get() << " iEnd: " << iEnd.position().get();
        }
        ASSERT_EQ( iIter, testMsgs.end() );
    }
}
*/
