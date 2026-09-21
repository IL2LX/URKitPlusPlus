std::string UiModule() {
    return R"URK(#pragma once
#include "config/mod_config.h"
#include <imgui.h>
#include <cmath>
#include <vector>

#include "localization.h"
#include "modules/modules.h"
#include "theme.h"

namespace ModUI {

inline Modules::Category &active_category() {
    static Modules::Category category = Modules::Category::Visuals;
    return category;
}

// Animation state
inline float &content_alpha() {
    static float a = 1.0f;
    return a;
}

inline float &indicator_y() {
    static float y = 18.0f;
    return y;
}

inline void initialize_style() {
    Theme::apply();
}

inline float lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

inline void render_menu() {
    if (!ModConfig::show_menu)
        return;
    Localization::initialize();

    const float dpi_scale = Theme::dpi_scale();
    const ImGuiViewport *viewport = ImGui::GetMainViewport();
    const float viewport_scale = viewport && viewport->Size.y > 0.0f ? viewport->Size.y / 1080.0f : 1.0f;
    float size_scale = dpi_scale > viewport_scale ? dpi_scale : viewport_scale;
    size_scale = ImClamp(size_scale, 1.0f, 1.35f);

    const ImVec2 base_window_size(720.0f, 420.0f);
    const ImVec2 base_min_size(600.0f, 360.0f);
    const ImVec2 base_max_size(1000.0f, 780.0f);

    ImGui::SetNextWindowSize(ImVec2(base_window_size.x * size_scale, base_window_size.y * size_scale),
                             ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSizeConstraints(ImVec2(base_min_size.x * dpi_scale, base_min_size.y * dpi_scale),
                                        ImVec2(base_max_size.x * size_scale, base_max_size.y * size_scale));

    ImGuiWindowClass window_class{};
    window_class.ViewportFlagsOverrideSet =
        ImGuiViewportFlags_NoFocusOnAppearing | ImGuiViewportFlags_NoFocusOnClick | ImGuiViewportFlags_NoTaskBarIcon;
    ImGui::SetNextWindowClass(&window_class);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

    if (!ImGui::Begin(ModConfig::display_name, &ModConfig::show_menu,
                      ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoDocking |
                          ImGuiWindowFlags_NoScrollbar)) {
        ImGui::PopStyleVar(2);
        ImGui::End();
        return;
    }
    ImGui::PopStyleVar(2);

    const Theme::Palette &p = Theme::palette();
    ImDrawList *dl = ImGui::GetWindowDrawList();
    const ImVec2 win_pos = ImGui::GetWindowPos();
    const ImVec2 win_size = ImGui::GetWindowSize();
    const float rounding = Theme::radius().xl;

    // ---- Main background ----
    dl->AddRectFilled(win_pos, ImVec2(win_pos.x + win_size.x, win_pos.y + win_size.y),
                      ImGui::GetColorU32(p.bg_base), rounding);
    dl->AddRect(win_pos, ImVec2(win_pos.x + win_size.x, win_pos.y + win_size.y),
                ImGui::GetColorU32(p.border_subtle), rounding, 0, 1.0f);

    // ---- Layout constants ----
    const float sidebar_w = 168.0f * size_scale;
    const float header_h = 52.0f;
    const float content_pad = 20.0f;

    // ---- Sidebar background ----
    dl->AddRectFilled(win_pos, ImVec2(win_pos.x + sidebar_w, win_pos.y + win_size.y),
                      ImGui::GetColorU32(p.bg_overlay), rounding, ImDrawFlags_RoundCornersLeft);
    dl->AddLine(ImVec2(win_pos.x + sidebar_w, win_pos.y + 1.0f),
                ImVec2(win_pos.x + sidebar_w, win_pos.y + win_size.y - 1.0f), ImGui::GetColorU32(p.border_subtle),
                1.0f);

    // ---- Header ----
    dl->AddRectFilled(win_pos, ImVec2(win_pos.x + win_size.x, win_pos.y + header_h),
                      ImGui::GetColorU32(p.bg_overlay), rounding, ImDrawFlags_RoundCornersTop);
    dl->AddLine(ImVec2(win_pos.x + 1.0f, win_pos.y + header_h),
                ImVec2(win_pos.x + win_size.x - 1.0f, win_pos.y + header_h), ImGui::GetColorU32(p.border_subtle),
                1.0f);

    // Title + version
    ImGui::SetCursorPos(ImVec2(20.0f, 15.0f));
    ImGui::PushFont(Theme::heading_font());
    ImGui::PushStyleColor(ImGuiCol_Text, p.text_primary);
    ImGui::TextUnformatted(ModConfig::display_name);
    ImGui::PopStyleColor();
    ImGui::PopFont();

    ImGui::SameLine(0.0f, 10.0f);
    ImGui::SetCursorPosY(19.0f);
    ImGui::PushStyleColor(ImGuiCol_Text, p.text_muted);
    ImGui::Text("v%s", ModConfig::version);
    ImGui::PopStyleColor();

    // ---- Sidebar navigation: one tab per category that has modules ----
    ImGui::SetCursorPos(ImVec2(0.0f, header_h));
    ImGui::BeginChild("##sidebar", ImVec2(sidebar_w, win_size.y - header_h), false,
                      ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    const float tab_h = 40.0f;
    const float tab_gap = 6.0f;
    const float tab_pad_x = 14.0f;
    const float start_y = 18.0f;
    const float indicator_w = 3.0f;

    // Categories present in the registry, in registration order
    std::vector<Modules::Category> tabs;
    const int module_count = Modules::System::count();
    for (int i = 0; i < module_count; ++i) {
        Modules::Module *active = Modules::System::module(i);
        if (!active)
            continue;
        const Modules::Category cat = active->category();
        bool present = false;
        for (const Modules::Category existing : tabs)
            if (existing == cat) {
                present = true;
                break;
            }
        if (!present)
            tabs.push_back(cat);
    }
    if (tabs.empty())
        tabs.push_back(Modules::Category::Visuals);

    // Row of the active category among the visible ones
    int active_row = 0;
    for (int r = 0; r < static_cast<int>(tabs.size()); ++r)
        if (tabs[r] == active_category()) {
            active_row = r;
            break;
        }
    if (active_category() != tabs[active_row]) {
        active_category() = tabs[active_row];
        content_alpha() = 0.0f;
    }

    // Smooth indicator animation
    const float target_y = start_y + static_cast<float>(active_row) * (tab_h + tab_gap);
    const float anim_speed = 14.0f;
    indicator_y() = lerp(indicator_y(), target_y, ImGui::GetIO().DeltaTime * anim_speed);

    // Draw animated indicator bar
    {
        const ImVec2 ind_min(win_pos.x + 4.0f, win_pos.y + header_h + indicator_y());
        const ImVec2 ind_max(ind_min.x + indicator_w, ind_min.y + tab_h - 8.0f);
        dl->AddRectFilled(ind_min, ind_max, ImGui::GetColorU32(p.text_primary), 2.0f);
    }

    // Helper to draw a tab
    auto draw_tab = [&](const char *label, Modules::Category category, float y) {
        const bool active = (active_category() == category);

        ImGui::SetCursorPos(ImVec2(tab_pad_x, y));
        const ImVec2 btn_min = ImGui::GetCursorScreenPos();
        const ImVec2 btn_max(btn_min.x + sidebar_w - tab_pad_x * 2.0f, btn_min.y + tab_h - 8.0f);
        const bool hovered = ImGui::IsMouseHoveringRect(btn_min, btn_max);

        if (hovered || active) {
            const ImU32 bg = active
                ? ImGui::GetColorU32(ImVec4(p.text_primary.x, p.text_primary.y, p.text_primary.z, 0.12f))
                : ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 0.045f));
            dl->AddRectFilled(btn_min, btn_max, bg, 6.0f);
        }

        ImGui::PushStyleColor(ImGuiCol_Text, active ? p.text_primary : p.text_muted);
        ImGui::PushFont(Theme::heading_font());
        ImGui::SetCursorPosY(y + 6.0f);
        ImGui::TextUnformatted(label);
        ImGui::PopFont();
        ImGui::PopStyleColor();

        if (hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
            if (active_category() != category) {
                active_category() = category;
                content_alpha() = 0.0f;
            }
        }
    };

    for (int r = 0; r < static_cast<int>(tabs.size()); ++r) {
        const Modules::Category cat = tabs[r];
        draw_tab(Modules::category_name(cat), cat, start_y + static_cast<float>(r) * (tab_h + tab_gap));
    }

    ImGui::EndChild();

    // ---- Content area: module page for the active category ----
    const float dt = ImGui::GetIO().DeltaTime;
    content_alpha() = lerp(content_alpha(), 1.0f, dt * 11.0f);

    ImGui::SetCursorPos(ImVec2(sidebar_w, header_h));
    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, content_alpha());
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(content_pad, content_pad));

    ImGui::BeginChild("##content", ImVec2(win_size.x - sidebar_w, win_size.y - header_h),
                      ImGuiChildFlags_AlwaysUseWindowPadding);

    // Category title
    ImGui::PushFont(Theme::heading_font());
    ImGui::PushStyleColor(ImGuiCol_Text, p.text_primary);
    ImGui::TextUnformatted(Modules::category_name(active_category()));
    ImGui::PopStyleColor();
    ImGui::PopFont();

    ImGui::Dummy(ImVec2(0.0f, 4.0f));

    // Subtle separator under title
    {
        const ImVec2 c = ImGui::GetCursorScreenPos();
        dl->AddLine(c, ImVec2(c.x + ImGui::GetContentRegionAvail().x, c.y), ImGui::GetColorU32(p.border_subtle),
                    1.0f);
        ImGui::Dummy(ImVec2(0.0f, 12.0f));
    }

    // Module toggles for the active category
    const int total_modules = Modules::System::count();
    int shown = 0;
    for (int index = 0; index < total_modules; ++index) {
        Modules::Module *mod = Modules::System::module(index);
        if (!mod || mod->category() != active_category())
            continue;
        bool enabled = mod->enabled();
        if (ImGui::Checkbox(mod->name(), &enabled))
            mod->SetEnabled(enabled);
        if (ImGui::IsItemHovered() && mod->description() && mod->description()[0] != '\0')
            ImGui::SetTooltip("%s", mod->description());
        ++shown;
    }
    if (shown == 0) {
        ImGui::PushStyleColor(ImGuiCol_Text, p.text_muted);
        ImGui::TextUnformatted("No modules in this category.");
        ImGui::PopStyleColor();
    }

    Modules::System::MenuRender();

    ImGui::EndChild();
    ImGui::PopStyleVar(2);

    ImGui::End();
}

} // namespace ModUI
)URK";
}


