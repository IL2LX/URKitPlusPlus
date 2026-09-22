std::string VRChatApiUserModule() {
    return R"URKAPIUSER(#pragma once

#include "sdk/unity/unity.h"
#include <string>

namespace VRC::Core {
struct APIUser : Unity::Object {
    using Unity::Object::Object;
    static constexpr Unity::TypeRef unity_type() {
        return {"VRCCore-Standalone.dll", "VRC.Core", "APIUser"};
    }

    static APIUser GetCurrentUser() {
        return URK::Unity::detail::InvokeStatic<APIUser>(unity_type(), "get_CurrentUser");
    }

    std::string DisplayName() const {
        return Call<std::string>("get_displayName");
    }

	void SetDisplayName(const std::string& name) const {
		Call<void>("set_displayName", name);
	}

	std::string Username() const {
		return Call<std::string>("get_username");
	}

	void SetUsername(const std::string& username) const {
		Call<void>("set_username", username);
	}

	std::string Pronouns() const {
		return Call<std::string>("get_pronouns");
	}

	void SetPronouns(const std::string& pronouns) const {
		Call<void>("set_pronouns", pronouns);
	}

	std::string Bio() const {
		return Call<std::string>("get_bio");
	}

	void SetBio(const std::string& bio) const {
		Call<void>("set_bio", bio);
	}

    std::vector<std::string> BioLinks() const {
        std::vector<std::string> links;
        const Unity::Object list = CallExact<Unity::Object>("get_bioLinks", {});
        if (!list)
            return links;
        const int count = list.Call<int>("get_Count");
        if (count <= 0)
            return links;
        links.reserve(static_cast<std::size_t>(count));
        for (int i = 0; i < count; ++i) {
            const std::string link = list.CallExact<std::string>("get_Item", { "System.Int32" }, i);
            if (!link.empty())
                links.push_back(link);
        }
        return links;
    }

    std::string Location() const {
        return Call<std::string>("get_location");
    }

	void SetLocation(const std::string& location) const {
		Call<void>("set_location", location);
	}

	std::string AvatarID() const {
		return Call<std::string>("get_avatarId");
	}

	void SetAvatarID(const std::string& avatarId) const {
		Call<void>("set_avatarId", avatarId);
	}

	std::string FallbackId() const {
		return Call<std::string>("get_fallbackId");
	}

	void SetFallbackId(const std::string& fallbackId) const {
		Call<void>("set_fallbackId", fallbackId);
	}

	bool IsBoopingEnabled() const {
		return Call<bool>("get_isBoopingEnabled");
	}

	void IsBoopingEnabled(bool isBoopingEnabled) const {
		Call<void>("set_isBoopingEnabled", isBoopingEnabled);
	}

	bool IsFriend() const {
		return Call<bool>("get_isFriend");
	}

	void SetIsFriend(bool isFriend) const {
		Call<void>("set_isFriend", isFriend);
	}

	std::string LastLogin() const {
		return Call<std::string>("get_last_login");
	}

	std::string LastActivity() const {
		return Call<std::string>("get_last_activity");
	}

	std::string DateJoined() const {
		return Call<std::string>("get_date_joined");
	}

	std::string CurrentAvatarImageUrl() const {
		return Call<std::string>("get_currentAvatarImageUrl");
	}

	std::string CurrentAvatarThumbnailImageUrl() const {
		return Call<std::string>("get_currentAvatarThumbnailImageUrl");
	}

	std::string CurrentAvatarAssetUrl() const {
		return Call<std::string>("get_currentAvatarAssetUrl");
	}

	std::vector<std::string> CurrentAvatarTags() const {
		std::vector<std::string> tags;
		const Unity::Object list = CallExact<Unity::Object>("get_currentAvatarTags", {});
		if (!list)
			return tags;
		const int count = list.Call<int>("get_Count");
		if (count <= 0)
			return tags;
		tags.reserve(static_cast<std::size_t>(count));
		for (int i = 0; i < count; ++i) {
			const std::string tag = list.CallExact<std::string>("get_Item", { "System.Int32" }, i);
			if (!tag.empty())
				tags.push_back(tag);
		}
		return tags;
	}

	std::vector<std::string> FriendIDs() const {
		std::vector<std::string> ids;
		const Unity::Object list = CallExact<Unity::Object>("get_friendIDs", {});
		if (!list)
			return ids;
		const int count = list.Call<int>("get_Count");
		if (count <= 0)
			return ids;
		ids.reserve(static_cast<std::size_t>(count));
		for (int i = 0; i < count; ++i) {
			const std::string id = list.CallExact<std::string>("get_Item", { "System.Int32" }, i);
			if (!id.empty())
				ids.push_back(id);
		}
		return ids;
	}

	std::string HomeLocation() const {
		return Call<std::string>("get_homeLocation");
	}

	std::vector<std::string> Tags() const {
		std::vector<std::string> tags;
		const Unity::Object list = CallExact<Unity::Object>("get_tags", {});
		if (!list)
			return tags;
		const int count = list.Call<int>("get_Count");
		if (count <= 0)
			return tags;
		tags.reserve(static_cast<std::size_t>(count));
		for (int i = 0; i < count; ++i) {
			const std::string tag = list.CallExact<std::string>("get_Item", { "System.Int32" }, i);
			if (!tag.empty())
				tags.push_back(tag);
		}
		return tags;
	}

	std::string Status() const {
		return Call<std::string>("get_status");
	}

	std::string StatusDescription() const {
		return Call<std::string>("get_statusDescription");
	}

	std::vector<std::string> StatusHistory() const {
		std::vector<std::string> statuses;
		const Unity::Object list = CallExact<Unity::Object>("get_statusHistory", {});
		if (!list)
			return statuses;
		const int count = list.Call<int>("get_Count");
		if (count <= 0)
			return statuses;
		statuses.reserve(static_cast<std::size_t>(count));
		for (int i = 0; i < count; ++i) {
			const std::string status = list.CallExact<std::string>("get_Item", { "System.Int32" }, i);
			if (!status.empty())
				statuses.push_back(status);
		}
		return statuses;
	}

	bool AllowAvatarCopying() const {
		return Call<bool>("get_allowAvatarCopying");
	}

	void SetAllowAvatarCopying(bool allowAvatarCopying) const {
		Call<void>("set_allowAvatarCopying", allowAvatarCopying);
	}

	bool HasSharedConnectionsOptOut() const {
		return Call<bool>("get_hasSharedConnectionsOptOut");
	}

	bool HasDiscordFriendsOptOut() const {
		return Call<bool>("get_hasDiscordFriendsOptOut");
	}

	std::string UserIcon() const {
		return Call<std::string>("get_userIcon");
	}

	std::string Note() const {
		return Call<std::string>("get_note");
	}

	std::string IconUrl() const {
		return Call<std::string>("get_iconUrl");
	}

	std::string ThumbnailUrl() const {
		return Call<std::string>("get_thumbnailUrl");
	}

	std::string IsAgeVerified() const {
		return Call<std::string>("get_ageVerified");
	}

	bool IsAdult() const {
		return Call<bool>("get_isAdult");
	}

	std::string DiscordID() const {
		return Call<std::string>("get_discordId");
	}

	std::string IconFrame() const {
		return Call<std::string>("get_iconFrame");
	}

	std::string ProfileEffect() const {
		return Call<std::string>("get_profileEffect");
	}

	std::string NameplateEffect() const {
		return Call<std::string>("get_nameplateEffect");
	}

	std::string BannerType() const {
		return Call<std::string>("get_bannerType");
	}

	std::string BannerUrl() const {
		return Call<std::string>("get_bannerUrl");
	}

	std::string BannerColor() const {
		return Call<std::string>("get_bannerColor");
	}

	std::string LastPlatform() const {
		return Call<std::string>("get_last_platform");
	}

	std::string Platform() const {
		return Call<std::string>("get_platform");
	}

	bool IsOnMobile() const {
		return Call<bool>("get_IsOnMobile");
	}

	std::string ProfilePicImageUrl() const {
		return Call<std::string>("get_profilePicImageUrl");
	}

	std::string ProfilePicThumbnailImageUrl() const {
		return Call<std::string>("get_profilePicThumbnailImageUrl");
	}

	bool HasNoPowers() const {
		return Call<bool>("get_hasNoPowers");
	}

	bool HasScriptingAccess() const {
		return Call<bool>("get_hasScriptingAccess");
	}

	bool HasModerationPowers() const {
		return Call<bool>("get_hasModerationPowers");
	}

	bool HasVIPAccess() const {
		return Call<bool>("get_hasVIPAccess");
	}

	bool HasSuperPowers() const {
		return Call<bool>("get_hasSuperPowers");
	}

	bool CanPublishWorldsAndAvatars() const {
		return Call<bool>("get_canPublishWorldsAndAvatars");
	}

	bool CanPublishAllContent() const {
		return Call<bool>("get_canPublishAllContent");
	}

	bool CanPublishAvatars() const {
		return Call<bool>("get_canPublishAvatars");
	}

	bool CanPublishWorlds() const {
		return Call<bool>("get_canPublishWorlds");
	}

	bool CanPublishProps() const {
		return Call<bool>("get_canPublishProps");
	}

	bool IsUntrusted() const {
		return Call<bool>("get_isUntrusted");
	}

	bool IsEarlyAdopter() const {
		return Call<bool>("get_isEarlyAdopter");
	}

	bool IsSupporter() const {
		return Call<bool>("get_isSupporter");
	}

	bool IsCreator() const {
		return Call<bool>("get_isCreator");
	}

	bool HasBasicTrustLevel() const {
		return Call<bool>("get_hasBasicTrustLevel");
	}

	bool HasKnownTrustLevel() const {
		return Call<bool>("get_hasKnownTrustLevel");
	}

	bool HasTrustedTrustLevel() const {
		return Call<bool>("get_hasTrustedTrustLevel");
	}

	bool HasVeteranTrustLevel() const {
		return Call<bool>("get_hasVeteranTrustLevel");
	}

	bool HasLegendTrustLevel() const {
		return Call<bool>("get_hasLegendTrustLevel");
	}

	bool HasNegativeTrustLevel() const {
		return Call<bool>("get_hasNegativeTrustLevel");
	}

	bool HasVeryNegativeTrustLevel() const {
		return Call<bool>("get_hasVeryNegativeTrustLevel");
	}

	bool CanSetStatusOffline() const {
		return Call<bool>("get_canSetStatusOffline");
	}

	bool StatusIsSetToOffline() const {
		return Call<bool>("get_statusIsSetToOffline");
	}

	bool StatusIsSetToJoinMe() const {
		return Call<bool>("get_statusIsSetToJoinMe");
	}

	bool StatusIsSetToAskMe() const {
		return Call<bool>("get_statusIsSetToAskMe");
	}

	bool CanSeeAllUsersStatus() const {
		return Call<bool>("get_canSeeAllUsersStatus");
	}

	bool IsSelf() const {
		return Call<bool>("get_IsSelf");
	}

	bool HasRequestedToBeFriend() const {
		return Call<bool>("get_HasRequestedToBeFriend");
	}

	bool HasFriendRequestPending() const {
		return Call<bool>("get_HasFriendRequestPending");
	}

	std::string Id() const {
		return Call<std::string>("get_id");
	}

	bool Populated() const {
		return Call<bool>("get_Populated");
	}
};
} // namespace VRC::Core)URKAPIUSER";
}