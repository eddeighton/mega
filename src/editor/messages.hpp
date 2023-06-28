#ifndef MESSAGES_05_FEB_2021
#define MESSAGES_05_FEB_2021

#include <qmetatype.h>

#ifndef Q_MOC_RUN

#include "toolbox.hpp"

#include "schematic/node.hpp"
#include "schematic/editRoot.hpp"
#include "schematic/schematic.hpp"

#endif

namespace editor
{

// Q_DECLARE_METATYPE
struct MissionMsg
{
    MissionMsg( schematic::Node::PtrCst _pNode )
        : pNode( _pNode )
    {
    }
    schematic::Node::PtrCst pNode;
};

struct MissionContext
{
    MissionContext( schematic::IEditContext* _pContext )
        : pContext( _pContext )
    {
    }
    schematic::IEditContext* pContext;
};

struct ClipboardMsg
{
    ClipboardMsg() {}

    ClipboardMsg( schematic::Schematic::Ptr _pSchematic, editor::Toolbox::Palette::Ptr _pPalette,
                  const std::string& _strName )
        : pSchematic( _pSchematic )
        , pPalette( _pPalette )
        , strName( _strName )
    {
    }

    schematic::Schematic::Ptr     pSchematic;
    editor::Toolbox::Palette::Ptr pPalette;
    std::string                   strName;
};

} // namespace editor

Q_DECLARE_METATYPE( editor::ClipboardMsg )

#endif // MESSAGES_05_FEB_2021
