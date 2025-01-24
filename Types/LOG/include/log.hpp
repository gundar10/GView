#pragma once

#include "GView.hpp"

namespace GView
{
namespace Type
{
    namespace LOG
    {
        namespace Panels
        {
            enum class IDs : unsigned char
            {
                Information = 0,
            };
        };

        class LOGFile : public TypeInterface
        {
          private:
            uint64_t panelsMask{ 0 };
            
            struct event {
                std::string timestamp;
                std::string type;
                std::string details;
                std::string ipAddress;

                event(std::string ts, std::string t, std::string d, std::string ip = "")
                    : timestamp(std::move(ts)), type(std::move(t)), details(std::move(d)), ipAddress(std::move(ip)) {}
            };

            std::vector<event*> events;

          public:
            Reference<GView::Object> obj; 

          public:
            LOGFile();
            virtual ~LOGFile() = default;

            std::string_view GetTypeName() override;
            void RunCommand(std::string_view) override
            {
            }
            bool UpdateKeys(KeyboardControlsInterface* interface) override
            {
                return true;
            }
            bool Update(Reference<GView::Object> obj);
            bool HasPanel(Panels::IDs id);
            void UpdateBufferViewZones(GView::View::BufferViewer::Settings& settings);

            std::vector<event*> GetEvents()
            {
                return events;
            }

          public:
            Reference<GView::Utils::SelectionZoneInterface> selectionZoneInterface;

            uint32 GetSelectionZonesCount() override
            {
                CHECK(selectionZoneInterface.IsValid(), 0, "");
                return selectionZoneInterface->GetSelectionZonesCount();
            }

            TypeInterface::SelectionZone GetSelectionZone(uint32 index) override
            {
                static auto d = TypeInterface::SelectionZone{ 0, 0 };
                CHECK(selectionZoneInterface.IsValid(), d, "");
                CHECK(index < selectionZoneInterface->GetSelectionZonesCount(), d, "");

                return selectionZoneInterface->GetSelectionZone(index);
            }
        };

        namespace Panels
        {
            class Information : public AppCUI::Controls::TabPage
            {
              private:
                Reference<GView::Type::LOG::LOGFile> log;
                Reference<AppCUI::Controls::ListView> general;

              public:
                Information(Reference<GView::Type::LOG::LOGFile> log);

                void Update();
                virtual void OnAfterResize(int newWidth, int newHeight) override;

              private:
                void UpdateGeneralInformation();
                void RecomputePanelsPositions();
            };
        }; // namespace Panels
    }      // namespace LOG
} // namespace Type
} // namespace GView
