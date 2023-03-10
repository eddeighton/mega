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

#ifndef SOURCES_15_APRIL_2022
#define SOURCES_15_APRIL_2022

#include "database/common/api.hpp"
#include "database/common/serialisation.hpp"

#include <boost/filesystem/path.hpp>

namespace mega::io
{
class Environment;
class Manifest;

class EGDB_EXPORT FilePath
{
protected:
    FilePath( const boost::filesystem::path& filePath )
        : m_filePath( filePath )
    {
    }

public:
    FilePath() = default;
    const boost::filesystem::path& path() const { return m_filePath; }

    bool operator==( const FilePath& cmp ) const { return m_filePath == cmp.m_filePath; }
    bool operator<( const FilePath& cmp ) const { return m_filePath < cmp.m_filePath; }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& boost::serialization::make_nvp( "file_path", m_filePath );
    }

protected:
    boost::filesystem::path m_filePath;
};

class EGDB_EXPORT SourceFilePath : public FilePath
{
    friend class BuildEnvironment;

protected:
    SourceFilePath( const boost::filesystem::path& filePath )
        : FilePath( filePath )
    {
    }

public:
    SourceFilePath() = default;
    bool operator==( const SourceFilePath& cmp ) const { return m_filePath == cmp.m_filePath; }
    bool operator<( const SourceFilePath& cmp ) const { return m_filePath < cmp.m_filePath; }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& boost::serialization::make_nvp( "source_file_path", m_filePath );
    }
};

class EGDB_EXPORT manifestFilePath : public SourceFilePath
{
    friend class Environment;
    friend class BuildEnvironment;
    manifestFilePath( const boost::filesystem::path& filePath )
        : SourceFilePath( filePath )
    {
    }

public:
    manifestFilePath() = default;
    bool operator==( const manifestFilePath& cmp ) const { return m_filePath == cmp.m_filePath; }
    bool operator<( const manifestFilePath& cmp ) const { return m_filePath < cmp.m_filePath; }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& boost::serialization::make_nvp( "manifest_file_path", m_filePath );
    }

    static const boost::filesystem::path extension() { return ".manifest"; }
};

class EGDB_EXPORT megaFilePath : public SourceFilePath
{
    friend class BuildEnvironment;
    megaFilePath( const boost::filesystem::path& filePath )
        : SourceFilePath( filePath )
    {
    }

public:
    megaFilePath() = default;

    bool operator==( const megaFilePath& cmp ) const { return m_filePath == cmp.m_filePath; }
    bool operator<( const megaFilePath& cmp ) const { return m_filePath < cmp.m_filePath; }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& boost::serialization::make_nvp( "mega_file_path", m_filePath );
    }
    static const boost::filesystem::path extension() { return ".mega"; }
};

class EGDB_EXPORT cppFilePath : public SourceFilePath
{
    friend class BuildEnvironment;

    cppFilePath( const boost::filesystem::path& filePath )
        : SourceFilePath( filePath )
    {
    }

public:
    cppFilePath() = default;

    bool operator==( const cppFilePath& cmp ) const { return m_filePath == cmp.m_filePath; }
    bool operator<( const cppFilePath& cmp ) const { return m_filePath < cmp.m_filePath; }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& boost::serialization::make_nvp( "cpp_file_path", m_filePath );
    }
    static const boost::filesystem::path extension() { return ".cpp"; }
};

class EGDB_EXPORT schFilePath : public SourceFilePath
{
    friend class BuildEnvironment;

    schFilePath( const boost::filesystem::path& filePath )
        : SourceFilePath( filePath )
    {
    }

public:
    schFilePath() = default;

    bool operator==( const schFilePath& cmp ) const { return m_filePath == cmp.m_filePath; }
    bool operator<( const schFilePath& cmp ) const { return m_filePath < cmp.m_filePath; }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& boost::serialization::make_nvp( "schematic_file_path", m_filePath );
    }
    static const boost::filesystem::path extension() { return ".sch"; }
};

class EGDB_EXPORT BuildFilePath : public FilePath
{
    friend class Environment;

protected:
    BuildFilePath( const boost::filesystem::path& filePath )
        : FilePath( filePath )
    {
    }

public:
    BuildFilePath() = default;
    bool operator==( const BuildFilePath& cmp ) const { return m_filePath == cmp.m_filePath; }
    bool operator<( const BuildFilePath& cmp ) const { return m_filePath < cmp.m_filePath; }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& boost::serialization::make_nvp( "build_file_path", m_filePath );
    }
};

class EGDB_EXPORT ComponentListingFilePath : public BuildFilePath
{
    friend class BuildEnvironment;

    ComponentListingFilePath( const boost::filesystem::path& filePath )
        : BuildFilePath( filePath )
    {
    }

public:
    ComponentListingFilePath() = default;

    bool operator==( const ComponentListingFilePath& cmp ) const { return m_filePath == cmp.m_filePath; }
    bool operator<( const ComponentListingFilePath& cmp ) const { return m_filePath < cmp.m_filePath; }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& boost::serialization::make_nvp( "component_listing_file_path", m_filePath );
    }
};

class EGDB_EXPORT CompilationFilePath : public BuildFilePath
{
    friend class Environment;

    CompilationFilePath( const boost::filesystem::path& filePath )
        : BuildFilePath( filePath )
    {
    }

public:
    CompilationFilePath() = default;

    bool operator==( const CompilationFilePath& cmp ) const { return m_filePath == cmp.m_filePath; }
    bool operator<( const CompilationFilePath& cmp ) const { return m_filePath < cmp.m_filePath; }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& boost::serialization::make_nvp( "compilation_file_path", m_filePath );
    }
    static const boost::filesystem::path extension() { return ".db"; }
};

class EGDB_EXPORT GeneratedHPPSourceFilePath : public BuildFilePath
{
    friend class BuildEnvironment;

    GeneratedHPPSourceFilePath( const boost::filesystem::path& filePath )
        : BuildFilePath( filePath )
    {
    }

public:
    GeneratedHPPSourceFilePath() = default;

    bool operator==( const GeneratedHPPSourceFilePath& cmp ) const { return m_filePath == cmp.m_filePath; }
    bool operator<( const GeneratedHPPSourceFilePath& cmp ) const { return m_filePath < cmp.m_filePath; }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& boost::serialization::make_nvp( "generated_hxx_file_path", m_filePath );
    }
    static const boost::filesystem::path extension() { return ".hpp"; }
};

class EGDB_EXPORT GeneratedCPPSourceFilePath : public BuildFilePath
{
    friend class BuildEnvironment;

    GeneratedCPPSourceFilePath( const boost::filesystem::path& filePath )
        : BuildFilePath( filePath )
    {
    }

public:
    GeneratedCPPSourceFilePath() = default;

    bool operator==( const GeneratedCPPSourceFilePath& cmp ) const { return m_filePath == cmp.m_filePath; }
    bool operator<( const GeneratedCPPSourceFilePath& cmp ) const { return m_filePath < cmp.m_filePath; }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& boost::serialization::make_nvp( "generated_cxx_file_path", m_filePath );
    }
    static const boost::filesystem::path extension() { return ".cpp"; }
};

class EGDB_EXPORT PrecompiledHeaderFile : public BuildFilePath
{
    friend class BuildEnvironment;

    PrecompiledHeaderFile( const boost::filesystem::path& filePath )
        : BuildFilePath( filePath )
    {
    }

public:
    PrecompiledHeaderFile() = default;

    bool operator==( const PrecompiledHeaderFile& cmp ) const { return m_filePath == cmp.m_filePath; }
    bool operator<( const PrecompiledHeaderFile& cmp ) const { return m_filePath < cmp.m_filePath; }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& boost::serialization::make_nvp( "precompiled_header_file", m_filePath );
    }
    static const boost::filesystem::path extension() { return ".pch"; }
};

class EGDB_EXPORT ObjectFilePath : public BuildFilePath
{
    friend class BuildEnvironment;

    ObjectFilePath( const boost::filesystem::path& filePath )
        : BuildFilePath( filePath )
    {
    }

public:
    ObjectFilePath() = default;

    bool operator==( const ObjectFilePath& cmp ) const { return m_filePath == cmp.m_filePath; }
    bool operator<( const ObjectFilePath& cmp ) const { return m_filePath < cmp.m_filePath; }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& boost::serialization::make_nvp( "cpp_object_file", m_filePath );
    }
    static const boost::filesystem::path extension() { return ".obj"; }
};

class EGDB_EXPORT ComponentFilePath : public BuildFilePath
{
    friend class BuildEnvironment;

    ComponentFilePath( const boost::filesystem::path& filePath )
        : BuildFilePath( filePath )
    {
    }

public:
    ComponentFilePath() = default;

    bool operator==( const ComponentFilePath& cmp ) const { return m_filePath == cmp.m_filePath; }
    bool operator<( const ComponentFilePath& cmp ) const { return m_filePath < cmp.m_filePath; }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& boost::serialization::make_nvp( "component_file", m_filePath );
    }
};

class EGDB_EXPORT MapFilePath : public BuildFilePath
{
    friend class BuildEnvironment;

    MapFilePath( const boost::filesystem::path& filePath )
        : BuildFilePath( filePath )
    {
    }

public:
    MapFilePath() = default;

    bool operator==( const MapFilePath& cmp ) const { return m_filePath == cmp.m_filePath; }
    bool operator<( const MapFilePath& cmp ) const { return m_filePath < cmp.m_filePath; }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& boost::serialization::make_nvp( "map_file", m_filePath );
    }
    static const boost::filesystem::path extension() { return ".map"; }
};

inline void to_json( nlohmann::json& j, const megaFilePath& p )
{
    j = nlohmann::json{ { "megaFilePath", p.path().string() } };
}
inline void to_json( nlohmann::json& j, const cppFilePath& p )
{
    j = nlohmann::json{ { "cppFilePath", p.path().string() } };
}
inline void to_json( nlohmann::json& j, const schFilePath& p )
{
    j = nlohmann::json{ { "schFilePath", p.path().string() } };
}
inline void to_json( nlohmann::json& j, const ComponentFilePath& p )
{
    j = nlohmann::json{ { "componentFilePath", p.path().string() } };
}
} // namespace mega::io

#endif // SOURCES_15_APRIL_2022
