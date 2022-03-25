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

#ifndef STAGE_18_04_2019
#define STAGE_18_04_2019

#include "database/model/objects.hpp"
#include "database/io/object.hpp"
#include "database/io/file.hpp"

namespace mega
{
namespace Stages
{
    class Stage
    {
    protected:
        Stage( const io::File::FileIDtoPathMap& files );
        Stage( const boost::filesystem::path& filePath, io::Object::FileID fileID );
        virtual ~Stage();

    public:
        virtual const io::Object::Array& getObjects( io::Object::FileID fileID ) const
        {
            io::File::FileIDToFileMap::const_iterator iFind = m_fileMap.find( fileID );
            VERIFY_RTE( iFind != m_fileMap.end() );
            return iFind->second->getObjects();
        }

        const io::Object::Array& getMaster() const
        {
            THROW_RTE( "Cannot do this" );
            // return getObjects( io::Object::MASTER_FILE );
        }

    protected:
        io::File::FileIDToFileMap m_fileMap;
    };

    class Appending : public Stage
    {
    public:
        Appending( const boost::filesystem::path& filePath, io::Object::FileID fileID );

        void store() const;

        io::Object::Array& getAppendingObjects() const
        {
            THROW_RTE( "Cannot do this" );
            /*io::File::FileIDToFileMap::const_iterator iFind = m_fileMap.find( io::Object::MASTER_FILE );
            VERIFY_RTE( iFind != m_fileMap.end() );
            io::File* pFile = iFind->second;
            return pFile->getObjects();*/
        }

    public:
        template < typename T, typename... Args >
        inline T* construct( Args... args )
        {
            THROW_RTE( "Cannot do this" );
            /*typename io::File::FileIDToFileMap::const_iterator iFind = m_fileMap.find( io::Object::MASTER_FILE );
            VERIFY_RTE( iFind != m_fileMap.end() );
            io::File* pFile = iFind->second;
            T* pNewObject = ObjectFactoryImpl::create< T >(
                io::Object::MASTER_FILE, pFile->getObjects().size(), args... );
            VERIFY_RTE( pNewObject );
            pFile->getObjects().push_back( pNewObject );
            return pNewObject;*/
        }
    };

    class Creating : public Stage
    {
    protected:
        Creating( const io::File::FileIDtoPathMap& files, io::Object::FileID fileID );
        ~Creating();

    public:
        void store( const boost::filesystem::path& filePath ) const;

        template < typename T, typename... Args >
        inline T* construct( Args... args )
        {
            T* pNewObject = ObjectFactoryImpl::create< T >( m_fileID, m_newObjects.size(), args... );
            VERIFY_RTE( pNewObject );
            m_newObjects.push_back( pNewObject );
            return pNewObject;
        }
        const io::Object::Array& getNewObjects() const { return m_newObjects; }

        const io::Object::Array& getObjects( io::Object::FileID fileID ) const
        {
            if ( fileID == m_fileID )
                return m_newObjects;
            else
                return Stage::getObjects( fileID );
        }

    protected:
        io::Object::FileID m_fileID;
        io::Object::Array  m_newObjects;
    };
} // namespace Stages
} // namespace mega

#endif // STAGE_18_04_2019