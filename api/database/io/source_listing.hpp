#ifndef IO_SOURCETREE_25_MARCH_2022
#define IO_SOURCETREE_25_MARCH_2022

#include <boost/filesystem/path.hpp>

#include <vector>
#include <iostream>

namespace mega
{
namespace io
{
    class SourceListing
    {
        friend std::istream& operator>>( std::istream& is, SourceListing& sourceListing );
        friend std::ostream& operator<<( std::ostream& os, const SourceListing& sourceListing );
        SourceListing();

    public:
        using PathArray = std::vector< boost::filesystem::path >;
        SourceListing( const PathArray& sourceFiles, bool bIsComponent );

        static SourceListing load( const boost::filesystem::path& manifestFilePath );
        void                 save( const boost::filesystem::path& manifestFilePath ) const;

        bool             isComponent() const { return m_bIsComponent; }
        const PathArray& getSourceFiles() const { return m_sourceFiles; }

    private:
        PathArray m_sourceFiles;
        bool      m_bIsComponent;
    };

    std::istream& operator>>( std::istream& is, SourceListing& sourceListing );
    std::ostream& operator<<( std::ostream& os, const SourceListing& sourceListing );

} // namespace io
} // namespace mega

#endif // IO_SOURCETREE_25_MARCH_2022
