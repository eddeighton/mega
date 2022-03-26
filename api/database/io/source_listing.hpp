#ifndef IO_SOURCETREE_25_MARCH_2022
#define IO_SOURCETREE_25_MARCH_2022

#include <boost/filesystem/path.hpp>

#include <vector>

namespace mega
{
namespace io
{
    class SourceListing
    {
    public:
        using PathArray = std::vector< boost::filesystem::path >;
        SourceListing( const PathArray& sourceFiles, bool bIsComponent );

        static SourceListing load(const boost::filesystem::path& manifestFilePath);
        void save( const boost::filesystem::path& manifestFilePath ) const;

        bool isComponent() const { return m_bIsComponent; }
        const PathArray& getSourceFiles() const { return m_sourceFiles; }

        static boost::filesystem::path filepath( const boost::filesystem::path& buildDir );
    private:
        PathArray m_sourceFiles;
        bool      m_bIsComponent;
    };

} // namespace io
} // namespace mega

#endif // IO_SOURCETREE_25_MARCH_2022
