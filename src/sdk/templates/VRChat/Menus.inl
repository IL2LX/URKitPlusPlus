std::string VRChatMenusModule() {
    return R"URKMENUS(#pragma once

#include "sdk/unity/unity.h"
#include "modules/modules.h"

namespace VRC {
inline void poll_menus() {
    static bool quick_menu_fired_ = false;
    static bool main_menu_fired_ = false;

    if (!quick_menu_fired_ && Unity::GameObject::Find("Canvas_QuickMenu(Clone)")) {
        quick_menu_fired_ = true;
        Modules::System::QuickMenuInit();
    }
    if (!main_menu_fired_ && Unity::GameObject::Find("Canvas_MainMenu(Clone)")) {
        main_menu_fired_ = true;
        Modules::System::MainMenuInit();
    }
}
} // namespace VRC
)URKMENUS";
}