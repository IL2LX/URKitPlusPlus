std::string VRChatLocalizableStringExtensionsModule() {
    return R"URKLOCALIZABLE(#pragma once
#include "sdk/unity/unity.h"
#include <string>

namespace VRC::Localization {

    inline constexpr Unity::TypeRef kLocalizableStringExtensionsType{"Localization.LocalizableString.dll", "VRC.Localization", "LocalizableStringExtensions" };

    struct LocalizableString : Unity::Object {
        using Unity::Object::Object;

        std::string Key() const {
            return Call<std::string>("get_Key");
        }

        std::string FallbackText() const {
            return Call<std::string>("get_FallbackText");
        }

        bool HasKey() const {
            return Call<bool>("get_HasKey");
        }

        bool IsLocalized() const {
            return Call<bool>("get_IsLocalized");
        }

        bool IsEmpty() const {
            return Call<bool>("get_IsEmpty");
        }

        // --- Methods

        std::string ToString() const {
            return Call<std::string>("ToString");
        }

        std::string GetFormattedFallbackText() const {
            return Call<std::string>("GetFormattedFallbackText");
        }

        std::string GetKeyWithoutVariantKey() const {
            return Call<std::string>("GetKeyWithoutVariantKey");
        }
    };

    struct LocalizableStringExtensions {
        static LocalizableString Localize(const std::string& text) {
            return Localize(text, nullptr, nullptr, nullptr);
        }

        static LocalizableString Localize(const std::string& text, void* arg0, void* arg1 = nullptr,
            void* arg2 = nullptr) {
            return URK::Unity::detail::InvokeStatic<LocalizableString>(kLocalizableStringExtensionsType, "Localize", text, arg0, arg1, arg2);
        }

        static LocalizableString LocalizeWithFallback(const std::string& key, const std::string& fallback,
            void* arg0 = nullptr, void* arg1 = nullptr,
            void* arg2 = nullptr) {
            return URK::Unity::detail::InvokeStatic<LocalizableString>(kLocalizableStringExtensionsType, "LocalizeWithFallback", key, fallback, arg0, arg1, arg2);
        }

        static LocalizableString LocalizeVariant(const std::string& key, const std::string& variantKey,
            void* arg0 = nullptr, void* arg1 = nullptr, void* arg2 = nullptr) {
            return URK::Unity::detail::InvokeStatic<LocalizableString>(kLocalizableStringExtensionsType, "LocalizeVariant", key, variantKey, arg0, arg1, arg2);
        }

        static LocalizableString LocalizeVariantWithFallback(const std::string& key, const std::string& variantKey,
            const std::string& fallback, void* arg0 = nullptr,
            void* arg1 = nullptr, void* arg2 = nullptr) {
            return URK::Unity::detail::InvokeStatic<LocalizableString>(kLocalizableStringExtensionsType, "LocalizeVariantWithFallback", key, variantKey, fallback, arg0, arg1,arg2);
        }

        static LocalizableString NoTranslation(const std::string& text) {
            return NoTranslation(text, nullptr, nullptr, nullptr);
        }

        static LocalizableString NoTranslation(const std::string& text, void* arg0, void* arg1 = nullptr,
            void* arg2 = nullptr) {
            return URK::Unity::detail::InvokeStatic<LocalizableString>(kLocalizableStringExtensionsType, "NoTranslation", text, arg0, arg1, arg2);
        }
    };
} // namespace VRC::Localization
)URKLOCALIZABLE";
}