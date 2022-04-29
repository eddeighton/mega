

#include "database/common/glob.hpp"

#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>
#include <common/file.hpp>
#include <gtest/gtest.h>

#include <utility>
#include <sstream>

namespace
{
    void create_file( const boost::filesystem::path& filePath )
    {
        auto p = boost::filesystem::createNewFileStream( filePath );
        *p << "testing";
    }

    void create_files( const boost::filesystem::path& filePath, int n )
    {
        for ( int i = 0; i != n; ++i )
        {
            std::ostringstream os;
            os << "test_file_" << i;
            auto p = boost::filesystem::createNewFileStream( filePath / os.str() );
            *p << "testing";
        }
    }
} // namespace

TEST( Glob, Empty )
{
    namespace bfs = boost::filesystem;

    const bfs::path cwd    = bfs::temp_directory_path();
    const bfs::path srcDir = cwd / "sandbox";
    bfs::create_directories( cwd / "sandbox/a/b" );
    bfs::create_directories( cwd / "sandbox/a/c" );
    const bfs::path f1 = cwd / "sandbox/a/b/foobar";
    create_file( f1 );

    {
        mega::io::Glob           glob = { cwd / "sandbox/a", "c/foobar", nullptr };
        mega::io::FilePathVector results;
        mega::io::resolveGlob( glob, srcDir, results );
        ASSERT_TRUE( results.empty() );
    }

    bfs::remove_all( srcDir );
}

TEST( Glob, Absolute )
{
    namespace bfs = boost::filesystem;

    const bfs::path cwd    = bfs::temp_directory_path();
    const bfs::path srcDir = cwd / "sandbox";
    bfs::create_directories( cwd / "sandbox/a/b" );
    bfs::create_directories( cwd / "sandbox/a/c" );
    const bfs::path f1 = cwd / "sandbox/a/b/foobar";
    create_file( f1 );

    {
        mega::io::Glob           glob = { cwd / "sandbox/a", "/a/b/foobar", nullptr };
        mega::io::FilePathVector results;
        mega::io::resolveGlob( glob, srcDir, results );
        ASSERT_EQ( results.size(), 1U );
        ASSERT_EQ( results.front(), f1 );
    }

    bfs::remove_all( srcDir );
}

TEST( Glob, SingleMatch )
{
    namespace bfs = boost::filesystem;

    const bfs::path cwd    = bfs::temp_directory_path();
    const bfs::path srcDir = cwd / "sandbox";
    bfs::create_directories( cwd / "sandbox/a/b" );
    bfs::create_directories( cwd / "sandbox/a/c" );
    const bfs::path f1 = cwd / "sandbox/a/b/foobar";
    create_file( f1 );
    ASSERT_TRUE( boost::filesystem::exists( f1 ) );

    {
        mega::io::Glob           glob = { cwd / "sandbox/a", "b/foobar", nullptr };
        mega::io::FilePathVector results;
        mega::io::resolveGlob( glob, srcDir, results );
        ASSERT_EQ( results.size(), 1U );
        ASSERT_EQ( results.front(), f1 );
    }

    bfs::remove_all( srcDir );
}

TEST( Glob, WildCardMatch )
{
    namespace bfs = boost::filesystem;

    const bfs::path cwd    = bfs::temp_directory_path();
    const bfs::path srcDir = cwd / "sandbox";
    bfs::create_directories( cwd / "sandbox/a/b" );
    bfs::create_directories( cwd / "sandbox/a/c" );
    const bfs::path f1 = cwd / "sandbox/a/b/foobar";
    create_file( f1 );
    ASSERT_TRUE( boost::filesystem::exists( f1 ) );

    {
        mega::io::Glob           glob = { cwd / "sandbox/a", "b/*", nullptr };
        mega::io::FilePathVector results;
        mega::io::resolveGlob( glob, srcDir, results );
        ASSERT_EQ( results.size(), 1U );
        ASSERT_EQ( results.front(), f1 );
    }

    bfs::remove_all( srcDir );
}

TEST( Glob, MultiWildCard )
{
    namespace bfs = boost::filesystem;

    const bfs::path cwd    = bfs::temp_directory_path();
    const bfs::path srcDir = cwd / "sandbox";
    bfs::create_directories( cwd / "sandbox/a/b" );
    bfs::create_directories( cwd / "sandbox/a/c" );
    bfs::create_directories( cwd / "sandbox/a/d" );
    create_files( cwd / "sandbox/a/b/", 5 );
    create_files( cwd / "sandbox/a/c/", 4 );
    create_files( cwd / "sandbox/a/d/", 3 );

    {
        mega::io::Glob           glob = { cwd / "sandbox/a", "b/*", nullptr };
        mega::io::FilePathVector results;
        mega::io::resolveGlob( glob, srcDir, results );
        ASSERT_EQ( results.size(), 5U );
    }

    bfs::remove_all( srcDir );
}

TEST( Glob, Branching )
{
    namespace bfs = boost::filesystem;

    const bfs::path cwd    = bfs::temp_directory_path();
    const bfs::path srcDir = cwd / "sandbox";
    bfs::create_directories( cwd / "sandbox/a/b" );
    bfs::create_directories( cwd / "sandbox/a/c" );
    bfs::create_directories( cwd / "sandbox/a/d" );
    bfs::create_directories( cwd / "sandbox/a/c/e" );
    bfs::create_directories( cwd / "sandbox/a/d/f" );
    create_files( cwd / "sandbox/a/b/", 5 );
    create_files( cwd / "sandbox/a/c/", 4 );
    create_files( cwd / "sandbox/a/d/", 3 );
    create_files( cwd / "sandbox/a/c/e", 6 );
    create_files( cwd / "sandbox/a/d/f", 7 );

    {
        mega::io::Glob           glob = { cwd / "sandbox/a", "*/*/test_file_1", nullptr };
        mega::io::FilePathVector results;
        mega::io::resolveGlob( glob, srcDir, results );
        ASSERT_EQ( results.size(), 2U );
    }

    bfs::remove_all( srcDir );
}

TEST( Glob, Glob )
{
    namespace bfs = boost::filesystem;

    const bfs::path cwd    = bfs::temp_directory_path();
    const bfs::path srcDir = cwd / "sandbox";
    bfs::create_directories( cwd / "sandbox/a/b" );
    bfs::create_directories( cwd / "sandbox/a/c" );
    bfs::create_directories( cwd / "sandbox/a/d" );
    bfs::create_directories( cwd / "sandbox/a/c/e" );
    bfs::create_directories( cwd / "sandbox/a/c/f" );
    create_files( cwd / "sandbox/a/b/", 5 );
    create_files( cwd / "sandbox/a/c/", 4 );
    create_files( cwd / "sandbox/a/d/", 3 );
    create_files( cwd / "sandbox/a/c/e", 6 );
    create_files( cwd / "sandbox/a/c/f", 7 );

    {
        mega::io::Glob           glob = { cwd / "sandbox/a", "c/**", nullptr };
        mega::io::FilePathVector results;
        mega::io::resolveGlob( glob, srcDir, results );
        ASSERT_EQ( results.size(), 4 + 6 + 7 );
    }

    bfs::remove_all( srcDir );
}