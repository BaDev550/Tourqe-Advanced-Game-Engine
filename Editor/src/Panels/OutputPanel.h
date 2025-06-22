//#pragma once
//#include "imgui.h"
//#include "TAGE/TAGE.h"
//
//namespace TourqeEditor {
//    class OutputPanel {
//    public:
//        void OnImGuiRender() {
//            auto& messages = GET_MESSAGES();
//
//            ImGui::Begin("Output");
//
//            if (ImGui::Button("Clear")) CLEAR_MESSAGES();
//            ImGui::SameLine();
//            ImGui::Checkbox("Auto-Scroll", &autoScroll);
//            ImGui::SameLine();
//            filter.Draw("Filter");
//
//            ImGui::Checkbox("Info", &showInfo);
//            ImGui::SameLine();
//            ImGui::Checkbox("Warn", &showWarn);
//            ImGui::SameLine();
//            ImGui::Checkbox("Error", &showError);
//
//            ImGui::Separator();
//
//            ImGui::BeginChild("LogScrollRegion", ImVec2(0, 0), false, ImGuiWindowFlags_AlwaysVerticalScrollbar);
//
//            for (const auto& msg : messages) {
//                if (!filter.PassFilter(msg.message.c_str()))
//                    continue;
//
//                bool show = false;
//                switch (msg.level) {
//                case spdlog::level::info:    show = showInfo; break;
//                case spdlog::level::warn:    show = showWarn; break;
//                case spdlog::level::err:
//                case spdlog::level::critical:show = showError; break;
//                default: show = true; break;
//                }
//
//                if (!show) continue;
//
//                ImVec4 color;
//                switch (msg.level) {
//                case spdlog::level::trace:   color = ImVec4(1.0f, 1.0f, 1.0f, 0.6f); break;
//                case spdlog::level::debug:   color = ImVec4(0.6f, 0.6f, 1.0f, 1.0f); break;
//                case spdlog::level::info:    color = ImVec4(0.2f, 0.8f, 0.2f, 1.0f); break;
//                case spdlog::level::warn:    color = ImVec4(0.9f, 0.8f, 0.2f, 1.0f); break;
//                case spdlog::level::err:     color = ImVec4(1.0f, 0.3f, 0.3f, 1.0f); break;
//                case spdlog::level::critical:color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f); break;
//                default:                     color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f); break;
//                }
//
//                ImGui::PushStyleColor(ImGuiCol_Text, color);
//                ImGui::TextUnformatted(msg.message.c_str());
//                ImGui::PopStyleColor();
//            }
//
//            if (autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
//                ImGui::SetScrollHereY(1.0f);
//
//            ImGui::EndChild();
//            ImGui::End();
//        }
//
//    private:
//        bool autoScroll = true;
//        bool showInfo = true;
//        bool showWarn = true;
//        bool showError = true;
//        ImGuiTextFilter filter;
//    };
//}