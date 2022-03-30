//  Copyright (c) Deighton Systems Limited. 2019. All Rights Reserved.
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

#include "database/stages/stage.hpp"

namespace mega
{
namespace Stages
{
    /*
    Stage::Stage( const io::File::FileIDtoPathMap& files )
    {
        ObjectFactoryImpl objectFactory;
        for( io::File::FileIDtoPathMap::const_iterator i = files.begin(),
            iEnd = files.end(); i!=iEnd; ++i )
        {
            io::File::load( objectFactory, m_fileMap, i->second, i->first );
        }
    }
    
    Stage::Stage( const boost::filesystem::path& filePath, io::ObjectInfo::FileID fileID )
    {
        ObjectFactoryImpl objectFactory;
        io::File::load( objectFactory, m_fileMap, filePath, fileID );
    }
    
    Stage::~Stage()
    {
        for( io::File::FileIDToFileMap::iterator 
            i = m_fileMap.begin(), iEnd = m_fileMap.end();
            i!=iEnd; ++i )
        {
            for( const io::Object* pObject : i->second->getObjects() )
            {
                delete pObject;
            }
            delete i->second;
        }
    }

    Appending::Appending( const boost::filesystem::path& filePath, io::ObjectInfo::FileID fileID )
        :   Stage( filePath, fileID )
    {
        
    }
    
    void Appending::store() const
    {
        io::File::FileIDtoPathMap files;
        for( io::File::FileIDToFileMap::const_iterator 
            i = m_fileMap.begin(),
            iEnd = m_fileMap.end(); i!=iEnd; ++i )
        {
            io::File* pFile = i->second;
            files.insert( std::make_pair( pFile->getFileID(), pFile->getPath() ) );
        }
        for( io::File::FileIDToFileMap::const_iterator 
            i = m_fileMap.begin(),
            iEnd = m_fileMap.end(); i!=iEnd; ++i )
        {
            io::File* pFile = i->second;
            io::File::store( pFile->getPath(), pFile->getFileID(), files, pFile->getObjects() );
        }
    }
    
    Creating::Creating( const io::File::FileIDtoPathMap& files, io::ObjectInfo::FileID fileID )
        :   Stage( files ),
            m_fileID( fileID )
    {
    }
    
    Creating::~Creating()
    {
        for( io::Object* pObject : m_newObjects )
        {
            delete pObject;
        }
    }
    
    void Creating::store( const boost::filesystem::path& filePath ) const
    {
        io::File::FileIDtoPathMap files;
        for( io::File::FileIDToFileMap::const_iterator 
            i = m_fileMap.begin(),
            iEnd = m_fileMap.end(); i!=iEnd; ++i )
        {
            io::File* pFile = i->second;
            files.insert( std::make_pair( pFile->getFileID(), pFile->getPath() ) );
        }
        files.insert( std::make_pair( m_fileID, filePath ) );
        io::File::store( filePath, m_fileID, files, m_newObjects );
    }*/

}
}
