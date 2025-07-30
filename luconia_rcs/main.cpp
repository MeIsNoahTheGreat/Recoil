#include "macro.hpp"
#include <thread>
#include <chrono>
#include "overlay.h"

macro* mouse = new macro();

int x, y;
int delay = 10;
float thickness, length;
bool on, crosshair;
bool showMenu = true;

void rcsLoop() {
    while (true) {
        if (!mouse->keyDown(0x02)) continue;
        if (mouse->keyDown(0x02) && mouse->keyDown(0x01) && on) {
            mouse->MoveMouseRelative(x, y);
            std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        }
    }
}

void DrawCrosshair(ImDrawList* draw_list, ImVec2 center, float length, float thickness, ImU32 color) {
    float half_thick = thickness * 0.5f;

    draw_list->AddRectFilled(
        ImVec2(center.x - length, center.y - half_thick),
        ImVec2(center.x, center.y + half_thick),
        color
    );
    draw_list->AddRectFilled(
        ImVec2(center.x, center.y - half_thick),
        ImVec2(center.x + length, center.y + half_thick),
        color
    );
    draw_list->AddRectFilled(
        ImVec2(center.x - half_thick, center.y - length),
        ImVec2(center.x + half_thick, center.y),
        color
    );
    draw_list->AddRectFilled(
        ImVec2(center.x - half_thick, center.y),
        ImVec2(center.x + half_thick, center.y + length),
        color
    );
}
int main() {
    std::thread rcs(rcsLoop);
    rcs.detach();

    x = 0;
    y = 0;
    thickness = 1.75f;
    length = 6.f;

    overlay::SetupWindow();
    if (!(overlay::CreateDeviceD3D(overlay::Window)))
        return 1;

    while (!overlay::ShouldQuit)
    {
        overlay::Render();
        ImDrawList* drawList = ImGui::GetBackgroundDrawList();

        SetWindowDisplayAffinity(overlay::Window, WDA_EXCLUDEFROMCAPTURE);

        if (mouse->keyDown(0x2E)) {
            showMenu = !showMenu;
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }

        if (crosshair) {
            DrawCrosshair(drawList, ImVec2(GetSystemMetrics(SM_CXSCREEN) / 2, GetSystemMetrics(SM_CYSCREEN) / 2), length, thickness, ImColor(255, 255, 255, 170));
        }

        if (showMenu) {
            ImGui::Begin("Luconia", nullptr);
            
            ImGui::Checkbox("RCS", &on);
            if (on)
            {
                ImGui::SliderInt("X Compensation", &x, -25, 25);
                ImGui::SliderInt("Y Compensation", &y, -25, 25);
                ImGui::Separator();
                ImGui::SliderInt("Delay", &delay, 1, 30);
            }
            ImGui::SeparatorText("Crosshair");
            ImGui::Checkbox("Crosshair", &crosshair);
            if (crosshair) {
                ImGui::SliderFloat("Length", &length, 1, 1000);
                ImGui::SliderFloat("Thickness", &thickness, 1, 10);
            }

            ImGui::End();
        }

        overlay::EndRender();
    }

    overlay::CloseOverlay();
}