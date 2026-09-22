std::string VRChatHighlightsFxModule() {
    return R"URKHIGHLIGHTSFX(#pragma once

#include "sdk/unity/unity.h"

struct HighlightsFX : Unity::Object {
    using Unity::Object::Object;
    static constexpr Unity::TypeRef unity_type() {
        return { "Assembly-CSharp.dll", "", "HighlightsFX"};
    }

    static HighlightsFX Instance() {
        return URK::Unity::detail::InvokeStatic<HighlightsFX>(unity_type(), "Method_HighlightsFX_0");
    }

    void EnableOutline(Unity::Renderer renderer, bool on) const {
        Call<void>("Method_Void_Renderer_Boolean_0", renderer, on);
    }

    void EnableOutline(Unity::Renderer target, Unity::Color color, bool on) const {
        Call<void>("Method_Void_Renderer_Boolean_0", target, color, on);
    }
};
)URKHIGHLIGHTSFX";
}

