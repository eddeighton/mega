


#include "database/common/component_info.hpp"

#include "database/model/environment.hxx"
#include "database/model/file_info.hxx"
#include "database/model/manifest.hxx"

#include <boost/filesystem/operations.hpp>
#include <gtest/gtest.h>

#include <utility>
#include <sstream>

using namespace mega::io;

TEST( FileSystem, Basic )
{
    /*
    mega::io::Environment environment(
        boost::filesystem::current_path(), boost::filesystem::temp_directory_path() );

    {
        mega::io::Database< mega::io::stage::Stage_Component > database( environment );

        mega::io::ComponentInfo componentInfo(
            "test",
            boost::filesystem::current_path() / "test",
            { boost::filesystem::current_path() / "test/foo.mega" },
            { boost::filesystem::current_path() / "somehwere" } );

        database.construct< mega::Component >( componentInfo );
        database.store();
    }

    {
        mega::io::Database< mega::io::stage::Stage_ObjectParse > database(
            environment, boost::filesystem::current_path() / "test/foo.mega" );

        const mega::Component* pComponent = database.one< mega::Component >();

        ASSERT_EQ( pComponent->getStrName(), "test" );
        ASSERT_EQ( pComponent->getDirectory(), boost::filesystem::current_path() / "test" );
    }*/
}
