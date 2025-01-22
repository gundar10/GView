#include "log.hpp"

using namespace GView::Type::LOG;

namespace GView::Type::LOG::Panels
{
Information::Information(Reference<GView::Type::LOG::LOGFile> log) : TabPage("Informa&Tion")
{
    this->log = log;
    general   = Factory::ListView::Create(this, "x:0,y:0,w:100%,h:10", { "n:Field,w:12", "n:Value,w:100" }, ListViewFlags::None);

    this->Update();
}

void Information::OnAfterResize(int newWidth, int newHeight)
{
    RecomputePanelsPositions();
}

void Information::Update()
{
    UpdateGeneralInformation();
    RecomputePanelsPositions();
}

void Information::UpdateGeneralInformation()
{
    general->DeleteAllItems();

    LocalString<256> ls;
    NumericFormatter nf;

    general->AddItem("Info").SetType(ListViewItem::Type::Category);

    general->AddItem({ "Filename", log->obj->GetName() });
    general->AddItem({ "First Timestamp", log->GetEvents().empty() ? "N/A" : log->GetEvents().front()->timestamp });
    general->AddItem({ "Last Timestamp", log->GetEvents().empty() ? "N/A" : log->GetEvents().back()->timestamp });

    std::string dataLines = std::to_string(log->GetEvents().size()) + " entries";
    general->AddItem({ "Extracted Data", dataLines });

    for (int i = 0; i < log->GetEvents().size(); i++)
    {
        std::string data = log->GetEvents()[i]->timestamp + "   " + log->GetEvents()[i]->type + "   " + log->GetEvents()[i]->details;
        general->AddItem({ "", data });
    }
}

void Information::RecomputePanelsPositions()
{
    if (this->general != nullptr)
    {
        if (this->general->GetItemsCount() > 15)
        {
            this->general->Resize(this->GetWidth(), 18);
        }
        else
        {
            this->general->Resize(this->GetWidth(), this->general->GetItemsCount() + 3);
        }
    }
}

} // namespace GView::Type::LOG::Panels
