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

        bool structureDetected;

        if (extension != ".log") {
            structureDetected = false;
        }
        // looking for timestamp patterns
        for (auto i = 0U; i < buf.GetLength(); i++) {
            if (std::isdigit(buf[i]) || buf[i] == '[' || buf[i] == ']') {
                structureDetected = true;
                break;
            }
        }
        if (!structureDetected) {
            return false;
        }
        return true;
    }

    void CreateBufferView(Reference<GView::View::WindowInterface> win, Reference<LOG::LOGFile> log)
    {
        GView::View::BufferViewer::Settings settings;
        log->selectionZoneInterface = win->GetSelectionZoneInterfaceFromViewerCreation(settings);
    }


    PLUGIN_EXPORT TypeInterface* CreateInstance()
    {
        return new LOG::LOGFile();
    }

    PLUGIN_EXPORT bool PopulateWindow(Reference<GView::View::WindowInterface> win)
    {
        auto log = win->GetObject()->GetContentType<LOG::LOGFile>();
        log->Update(win->GetObject());
        CreateBufferView(win, log);

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
