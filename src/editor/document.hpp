
#ifndef DOCUMENT_01_FEB_2021
#define DOCUMENT_01_FEB_2021

#ifndef Q_MOC_RUN

#include "schematic/file.hpp"
#include "schematic/schematic.hpp"
#include "schematic/compilation_stage.hpp"

#include "common/tick.hpp"
#include "common/scheduler.hpp"

#include "boost/filesystem/path.hpp"

#include <optional>
#include <memory>
#include <list>

#endif

namespace editor
{

class Document;

class DocumentChangeObserver
{
public:
    virtual void OnDocumentChanged( Document* pDocument )                               = 0;
    virtual void OnDocumentError( Document* pDocument, const std::string& strErrorMsg ) = 0;
};

class Document
{
    class UndoHistory
    {
        using PathList = std::list< boost::filesystem::path >;

    public:
        UndoHistory( Document& document );
        ~UndoHistory();

        void                 onNewVersion();
        schematic::File::Ptr onUndo();
        schematic::File::Ptr onRedo();

        const Timing::UpdateTick& getLastRecordTick() const { return m_lastRecord; }

    private:
        boost::filesystem::path getUndoFileName() const;

    private:
        Document&                                m_document;
        PathList                                 m_history, m_future;
        std::optional< boost::filesystem::path > m_current;
        boost::filesystem::path                  m_tempFolder;
        Timing::UpdateTick                       m_lastRecord;
    };

public:
    using Ptr = std::shared_ptr< Document >;

    Document( DocumentChangeObserver& observer, const schematic::CompilationStage config );
    Document( DocumentChangeObserver& observer, const schematic::CompilationStage config,
              const boost::filesystem::path& path );

    bool                                     isModified() const;
    const std::string&                       getUniqueObjectName() const { return m_uniqueObjectName; }
    std::optional< boost::filesystem::path > getFilePath() const { return m_optPath; }

    virtual schematic::File::Ptr getFile() const = 0;

    virtual void setCompilationConfig( schematic::CompilationStage config );

    virtual void onEditted( bool bCommandCompleted );

    void saved( const boost::filesystem::path& filePath );

    virtual void save()                                   = 0;
    virtual void saveAs( const std::string& strFilePath ) = 0;

    virtual void undo() = 0;
    virtual void redo() = 0;

protected:
    DocumentChangeObserver&                  m_documentChangeObserver;
    const std::string                        m_uniqueObjectName;
    Timing::UpdateTick                       m_lastModifiedTick;
    std::optional< boost::filesystem::path > m_optPath;
    schematic::CompilationStage              m_compilationConfig;
    UndoHistory                              m_undoHistory;
};

class SchematicDocument : public Document
{
public:
    using Ptr = std::shared_ptr< SchematicDocument >;

    SchematicDocument( DocumentChangeObserver& observer, schematic::Schematic::Ptr pSchematic,
                       schematic::CompilationStage config );
    SchematicDocument( DocumentChangeObserver& observer, schematic::Schematic::Ptr pSchematic,
                       schematic::CompilationStage config, const boost::filesystem::path& path );

    virtual void setCompilationConfig( schematic::CompilationStage config );
    virtual void onEditted( bool bCommandCompleted );

    virtual schematic::File::Ptr getFile() const { return m_pSchematic; }
    schematic::Schematic::Ptr    getSchematic() const { return m_pSchematic; }

    virtual void save();
    virtual void saveAs( const std::string& strFilePath );
    virtual void undo();
    virtual void redo();

protected:
    void calculateDerived( const schematic::CompilationStage config );
    void calculateDerived();

private:
    schematic::Schematic::Ptr m_pSchematic;
};

} // namespace editor

#endif // DOCUMENT_01_FEB_2021