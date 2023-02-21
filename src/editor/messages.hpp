#ifndef MESSAGES_05_FEB_2021
#define MESSAGES_05_FEB_2021

#include <qmetatype.h>

#ifndef Q_MOC_RUN

#include "toolbox.hpp"

#include "map/node.hpp"
#include "map/editRoot.hpp"
#include "map/schematic.hpp"

#endif

namespace editor
{
    
    //Q_DECLARE_METATYPE
    struct MissionMsg
    {
        MissionMsg( map::Node::PtrCst _pNode ) : pNode( _pNode ) {}
        map::Node::PtrCst pNode;
    };

    struct MissionContext
    {
        MissionContext( map::IEditContext* _pContext )
            :   pContext( _pContext )
        {}
        map::IEditContext* pContext;
    };

    struct ClipboardMsg
    {
        ClipboardMsg(){}
        
        ClipboardMsg( map::Schematic::Ptr _pSchematic, editor::Toolbox::Palette::Ptr _pPalette )
            :   pSchematic( _pSchematic ),
                pPalette( _pPalette )
        {}
        ClipboardMsg( map::Schematic::Ptr _pSchematic, editor::Toolbox::Palette::Ptr _pPalette, const boost::filesystem::path& filePath )
            :   pSchematic( _pSchematic ),
                pPalette( _pPalette ),
                optFilePath( filePath )
        {}

        map::Schematic::Ptr pSchematic;
        editor::Toolbox::Palette::Ptr pPalette;
        std::optional< boost::filesystem::path > optFilePath;
    };

}

Q_DECLARE_METATYPE( editor::ClipboardMsg )

#endif // MESSAGES_05_FEB_2021
