#include "log.hpp"
#include <regex>

using namespace GView::Type::LOG;

LOGFile::LOGFile() : panelsMask(0)
{
    this->panelsMask |= (1ULL << (unsigned char) Panels::IDs::Information);
}

std::string_view LOGFile::GetTypeName()
{
    return "LOG File";
}

bool LOGFile::Update(Reference<GView::Object> _obj)
{
    this->obj = _obj;

    // Get the content of the file as a BufferView
    const auto contentBuffer = this->obj->GetData().GetEntireFile();

    // Convert BufferView to string
    const std::string content(reinterpret_cast<const char*>(contentBuffer.GetData()), contentBuffer.GetLength());

    // Parse log lines
    std::istringstream stream(content);

    std::string line;
    while (std::getline(stream, line)) {
        line.erase(0, line.find_first_not_of(" \t\r\n")); // Trim left
        line.erase(line.find_last_not_of(" \t\r\n") + 1); // Trim right

        // Skip empty lines
        if (line.empty())
            continue;

        // Format 1: [2010-04-24 07:51:54,393] DEBUG - [main] BulkOpsClient.main(): Execution begin.
        std::regex format1(R"(\[([^\]]+)\]\s+(\w+)\s+-\s+(.*))");

        // Format 2: 123.123.123.123 - - [26/Apr/2000:00:23:51 -0400] "GET /cgi-bin/... HTTP/1.0" 200 36 ...
        std::regex format2(R"((\d{1,3}(?:\.\d{1,3}){3})\s+-\s+-\s+\[([^\]]+)\]\s+"([^"]+)\")");

        // Format 3: | 2017-01-01 - 07:01:43 | JawaharlalNehru | view | Main.WebHome | Mozilla | 253.111.182.73 |
        std::regex format3(R"(\|\s+([\d\-]+\s+\-\s+[\d:]+)\s+\|\s+(\w+)\s+\|.+\|\s+(\d{1,3}(?:\.\d{1,3}){3})\s+\|)");

        std::smatch match;
        if (std::regex_search(line, match, format1)) {
            // Extract for Format 1
            if (match.size() >= 4) {
                events.push_back(new event(match[1].str(), match[2].str(), match[3].str()));
            }
        } else if (std::regex_search(line, match, format2)) {
            // Extract for Format 2
            if (match.size() >= 4) {
                events.push_back(new event(match[2].str(), "HTTP Request", match[3].str(), match[1].str()));
            }
        } else if (std::regex_search(line, match, format3)) {
            // Extract for Format 3
            if (match.size() >= 4) {
                events.push_back(new event(match[1].str(), "Event Log", match[2].str(), match[3].str()));
            }
        } else {
            // Unrecognized format, store raw line
            events.push_back(new event("", "UNKNOWN", line));
        }
    }

    std::sort(events.begin(), events.end(), [](const event* a, const event* b) { return a->type < b->type; });

    return true;
}


bool LOGFile::HasPanel(Panels::IDs id)
{
    return (this->panelsMask & (1ULL << ((unsigned char) id))) != 0;
}

void LOGFile::UpdateBufferViewZones(GView::View::BufferViewer::Settings& settings)
{
    const auto color = ColorPair{ Color::Gray, Color::Transparent };

    // Get the content of the file as a BufferView
    const auto contentBuffer = this->obj->GetData().GetEntireFile();

    // Convert BufferView to string
    const std::string content(reinterpret_cast<const char*>(contentBuffer.GetData()), contentBuffer.GetLength());

    std::istringstream stream(content);

    uint64 offset = 0;
    std::string line;
    while (std::getline(stream, line)) {
        settings.AddZone(offset, line.size(), color, "LOG Entry");
        offset += line.size() + 1; // +1 for newline
    }
}
