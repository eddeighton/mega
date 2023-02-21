
#ifndef DOCUMENT_01_FEB_2021
#define DOCUMENT_01_FEB_2021

#ifndef Q_MOC_RUN

#include "map/file.hpp"
#include "map/schematic.hpp"

#include "common/tick.hpp"
#include "common/scheduler.hpp"

#include "boost/filesystem/path.hpp"

#include <optional>
#include <memory>
#include <list>

#endif

namespace editor
{
    class Document
    {
        class UndoHistory
        {
            using PathList = std::list< boost::filesystem::path >;
        public:
            UndoHistory( Document& document );
            ~UndoHistory();
            
            void onNewVersion();
            map::File::Ptr onUndo();
            map::File::Ptr onRedo();
            
            const Timing::UpdateTick& getLastRecordTick() const { return m_lastRecord; }
        
        private:
            boost::filesystem::path getUndoFileName() const;

        private:
            Document& m_document;
            PathList m_history, m_future;
            std::optional< boost::filesystem::path > m_current;
            boost::filesystem::path m_tempFolder;
            Timing::UpdateTick m_lastRecord;
        };
        
    public:
        using Ptr = std::shared_ptr< Document >;
        
        Document( task::Scheduler& scheduler );
        Document( task::Scheduler& scheduler, const boost::filesystem::path& path );
        
        bool isModified() const;
        const std::string& getUniqueObjectName() const { return m_uniqueObjectName; }
        std::optional< boost::filesystem::path > getFilePath() const { return m_optPath; }

        virtual map::File::Ptr getFile() const = 0;
        
        void setCompilationConfig( const map::File::CompilationConfig& config );
        
        virtual void onEditted( bool bCommandCompleted );
        
        void saved( const boost::filesystem::path& filePath );
        
        virtual void save()=0;
        virtual void saveAs( const std::string& strFilePath )=0;
        
        virtual void undo()=0;
        virtual void redo()=0;

    protected:
        void reschedule();
        
    protected:
        const std::string m_uniqueObjectName;
        task::Scheduler& m_scheduler;
        Timing::UpdateTick m_lastModifiedTick;
        std::optional< boost::filesystem::path > m_optPath;
        map::File::CompilationConfig m_compilationConfig;
        task::Schedule::Ptr m_pSchedule;
        task::Scheduler::Run::Ptr m_pRun;
        UndoHistory m_undoHistory;
    };

    class SchematicDocument : public Document
    {
    public:
        using Ptr = std::shared_ptr< SchematicDocument >;

        SchematicDocument( task::Scheduler& scheduler, map::Schematic::Ptr pSchematic );
        SchematicDocument( task::Scheduler& scheduler, map::Schematic::Ptr pSchematic, const boost::filesystem::path& path );
        
        virtual map::File::Ptr getFile() const { return m_pSchematic; }
        map::Schematic::Ptr getSchematic() const { return m_pSchematic; }

        virtual void save();
        virtual void saveAs( const std::string& strFilePath );
        virtual void undo();
        virtual void redo();
    private:
        map::Schematic::Ptr m_pSchematic;
    };

} //namespace editor

#endif //DOCUMENT_01_FEB_2021