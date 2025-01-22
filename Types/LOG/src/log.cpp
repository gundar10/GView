#include "log.hpp"

using namespace AppCUI;
using namespace AppCUI::Application;
using namespace AppCUI::Controls;
using namespace GView::Utils;
using namespace GView::Type;
using namespace GView;

extern "C"
{
    PLUGIN_EXPORT bool Validate(const AppCUI::Utils::Buffer& buf, const std::string_view& extension)
    {
        CHECK(extension == ".log", false, "Unsupported extension: [%.*s]", extension.length(), extension.data());
        for (auto i = 0U; i < buf.GetLength(); i++)
        {
            if (std::isdigit(buf[i]) || buf[i] == '[' || buf[i] == ']')
            {
                return true; // Timestamps or log structure detected
            }
        }
        return false;
    }

    PLUGIN_EXPORT TypeInterface* CreateInstance()
    {
        return new LOG::LOGFile();
    }

    PLUGIN_EXPORT bool PopulateWindow(Reference<GView::View::WindowInterface> win)
    {
        auto log = win->GetObject()->GetContentType<LOG::LOGFile>();
        log->Update(win->GetObject());

        GView::View::BufferViewer::Settings bufferSettings;
        log->UpdateBufferViewZones(bufferSettings);
        log->selectionZoneInterface = win->GetSelectionZoneInterfaceFromViewerCreation(bufferSettings);

        // Add Information Panel
        if (log->HasPanel(LOG::Panels::IDs::Information))
        {
            win->AddPanel(Pointer<TabPage>(new LOG::Panels::Information(log)), true);
        }

        return true;
    }

    PLUGIN_EXPORT void UpdateSettings(IniSection sect)
    {
        sect["Extension"]   = { "log" };
        sect["Priority"]    = 1;
        sect["Description"] = "Log files (*.log)";
    }
}

int main()
{
    return 0;
}
