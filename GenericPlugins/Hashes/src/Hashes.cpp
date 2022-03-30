#include "Hashes.hpp"

namespace GView::GenericPlugins::Hashes
{
HashesDialog::HashesDialog() : Window("Hashes", "d:c,w:70,h:20", WindowFlags::Sizeable | WindowFlags::Maximized)
{
    hashesList = Factory::ListView::Create(this, "x:0,y:0,w:100%,h:20");
    hashesList->AddColumn("Type", TextAlignament::Left, 12);
    hashesList->AddColumn("Value", TextAlignament::Left, 100);

    close                              = Factory::Button::Create(this, "&Close", "d:b,w:20", CMD_BUTTON_CLOSE);
    close->Handlers()->OnButtonPressed = this;
}

bool HashesDialog::ComputeHashes(Reference<GView::Object> object)
{
    CHECK(adler32.Init(), false, "");
    CHECK(crc32Neg.Init(CRC32Type::NEGL), false, "");
    CHECK(crc32Zero.Init(CRC32Type::ZERO), false, "");

    constexpr auto block = 0x1000ULL;
    auto offset          = 0ULL;
    auto left            = object->cache.GetSize();

    Buffer buffer;
    do
    {
        if (left < block)
        {
            printf("");
        }

        const auto sizeToRead = (left >= block ? block : left);
        left -= (left >= block ? block : left);

        buffer = object->cache.CopyToBuffer(offset, static_cast<uint32>(sizeToRead), false);
        CHECK(buffer.IsValid(), false, "");

        CHECK(adler32.Update(buffer), false, "");
        CHECK(crc32Neg.Update(buffer), false, "");
        CHECK(crc32Zero.Update(buffer), false, "");

        offset += sizeToRead;
    } while (left > 0);

    uint32 adler32hash = 0;
    CHECK(adler32.Final(adler32hash), false, "");
    uint32 crc32NegHash = 0;
    CHECK(crc32Neg.Final(crc32NegHash), false, "");
    uint32 crc32Zerohash = 0;
    CHECK(crc32Zero.Final(crc32Zerohash), false, "");

    LocalString<256> ls;
    NumericFormatter nf;
    hashesList->AddItem("Adler32", ls.Format("0x%.8X", adler32hash));
    hashesList->AddItem("CRC32(-1)", ls.Format("0x%.8X", crc32NegHash));
    hashesList->AddItem("CRC32(0)", ls.Format("0x%.8X", crc32Zerohash));

    return true;
}

void HashesDialog::OnButtonPressed(Reference<Button>)
{
    Exit(0);
}
} // namespace GView::GenericPlugins::Hashes

extern "C"
{
    PLUGIN_EXPORT bool Run(const string_view command, Reference<GView::Object> object)
    {
        if (command == "Hashes")
        {
            GView::GenericPlugins::Hashes::HashesDialog dlg;
            dlg.ComputeHashes(object);
            dlg.Show();
            return true;
        }
        return false;
    }

    PLUGIN_EXPORT void UpdateSettings(IniSection sect)
    {
        sect["command.Hashes"] = Input::Key::Shift | Input::Key::F5;
    }
}
