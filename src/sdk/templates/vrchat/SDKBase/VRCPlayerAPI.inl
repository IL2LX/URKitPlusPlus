std::string VRChatPlayerApiModule() {
    return R"URKVRCPLAYERAPI(#pragma once

#include "sdk/unity/unity.h"

namespace VRC::SDKBase {
struct VRCPlayerApi : Unity::Object {
    using Unity::Object::Object;
    static constexpr Unity::TypeRef unity_type() {
        return {"VRCSDKBase.dll", "VRC.SDKBase", "VRCPlayerApi"};
    }

	// --- Fields

	bool islocal() const {
		return GetProperty<bool>("isLocal");
	}

	std::string GetDisplayName() const {
		return Call<std::string>("displayName");
	}

	void SetDisplayName(const std::string& name) const {
		Call<std::string>("displayName", name);
	}

    Unity::GameObject GameObject() const {
        return GetProperty<Unity::GameObject>("gameObject");
    }

	// --- properties

	std::int32_t GetPlayerId() const {
		return GetProperty<std::int32_t>("playerId");
	}

	// --- Methods
	int GetPlayerCount() const {
		return Call<int>("GetPlayerCount");
	}

	static VRCPlayerApi GetPlayerByObject(Unity::GameObject obj) {
		return URK::Unity::detail::InvokeStatic<VRCPlayerApi>(unity_type(), "GetPlayerByGameObject", obj);
	}

	static VRCPlayerApi GetPlayerById(int playerId) {
		return URK::Unity::detail::InvokeStatic<VRCPlayerApi>(unity_type(), "GetPlayerById", playerId);
	}

	bool IsOwner(Unity::GameObject obj) {
		return URK::Unity::detail::InvokeStatic<bool>(unity_type(), "IsOwner", obj);
	}

	void TakeOwnership(Unity::GameObject obj) const {
		CallExact<void>("TakeOwnership", { "UnityEngine.GameObject" }, obj);
	}

	void TeleportTo(Unity::Vector3 teleportPos, Unity::Quaternion teleportRot) const {
		CallExact<void>("TeleportTo", { "UnityEngine.Vector3", "UnityEngine.Quaternion" }, teleportPos, teleportRot);
	}

	void Respawn() const {
		CallExact<void>("Respawn", {});
	}

	bool IsPlayerGrounded() const {
		return Call<bool>("IsPlayerGrounded");
	}

	bool IsUserInVR() const {
		return Call<bool>("IsUserInVR");
	}

	float GetRunSpeed() const {
		return Call<float>("GetRunSpeed");
	}

	void SetRunSpeed(float speed) const {
		Call<void>("SetRunSpeed", speed);
	}

	float GetwalkSpeed() const {
		return Call<float>("GetWalkSpeed");
	}

	float SetwalkSpeed(float speed) const {
		return Call<float>("SetWalkSpeed", speed);
	}

	float GetStrafeSpeed() const {
		return Call<float>("GetStrafeSpeed");
	}

	void SetStrafeSpeed(float speed) const {
		Call<void>("SetStrafeSpeed", speed);
	}

	float GetJumpImpulse() const {
		return Call<float>("GetJumpImpulse");
	}

	void SetJumpImpulse(float impulse) const {
		Call<void>("SetJumpImpulse", impulse);
	}

	float GetVoiceGain() const {
		return Call<float>("GetVoiceGain");
	}

	float GetVoiceDistanceNear() const {
		return Call<float>("GetVoiceDistanceNear");
	}

	void SetVoiceDistanceNear(float distance) const {
		Call<void>("SetVoiceDistanceNear", distance);
	}

	float GetVoiceDistanceFar() const {
		return Call<float>("GetVoiceDistanceFar");
	}

	void SetVoiceDistanceFar(float distance) const {
		Call<void>("SetVoiceDistanceFar", distance);
	}

	float GetVoiceVolumetricRadius() const {
		return Call<float>("GetVoiceVolumetricRadius");
	}

	void SetVoiceVolumetricRadius(float radius) const {
		Call<void>("SetVoiceVolumetricRadius", radius);
	}

	bool GetVoiceLowpass() const {
		return Call<bool>("GetVoiceLowpass");
	}

	void SetVoiceLowpass(bool enabled) const {
		Call<void>("SetVoiceLowpass", enabled);
	}

	void SetAvatarAudioGain(float gain) const {
		Call<void>("SetAvatarAudioGain", gain);
	}

	void SetAvatarAudioFarRadius(float radius) const {
		Call<void>("SetAvatarAudioFarRadius", radius);
	}

	void SetAvatarAudioNearRadius(float radius) const {
		Call<void>("SetAvatarAudioNearRadius", radius);
	}

	void SetAvatarAudioVolumetricRadius(float radius) const {
		Call<void>("SetAvatarAudioVolumetricRadius", radius);
	}

	void SetAvatarAudioForceSpatial(bool forceSpatial) const {
		Call<void>("SetAvatarAudioForceSpatial", forceSpatial);
	}

	void SetAvatarAudioCustomCurve(bool enabled) const {
		Call<void>("SetAvatarAudioCustomCurve", enabled);
	}

	std::vector<std::string> GetAvailableLanguages() const {
		return CallStringArrayExact("GetAvailableLanguages", {});
	}

	std::string GetCurrentLanguage() const {
		return CallExact<std::string>("GetCurrentLanguage", {});
	}

	std::vector<Unity::GameObject> GetPlayerObjects() const {
		return CallArrayExact<Unity::GameObject>("GetPlayerObjects", {});
	}

	Unity::Component FindComponentInPlayerObjects(Unity::Component ref) const {
		return CallExact<Unity::Component>("FindComponentInPlayerObjects", { "UnityEngine.Component" }, ref);
	}

	void Immobilize(bool immobilize) const {
		CallExact<void>("Immobilize", { "System.Boolean" }, immobilize);
	}

	Unity::Vector3 GetVelocity() const {
		return CallExact<Unity::Vector3>("GetVelocity", {});
	}

	void SetVelocity(Unity::Vector3 velocity) const {
		CallExact<void>("SetVelocity", { "UnityEngine.Vector3" }, velocity);
	}

	Unity::Vector3 GetPosition() const {
		return CallExact<Unity::Vector3>("GetPosition", {});
	}

	Unity::Quaternion GetRotation() const {
		return CallExact<Unity::Quaternion>("GetRotation", {});
	}

	float GetAvatarEyeHeightAsMeters() const {
		return CallExact<float>("GetAvatarEyeHeightAsMeters", {});
	}

	float GetAvatarEyeHeightMaximumAsMeters() const {
		return CallExact<float>("GetAvatarEyeHeightMaximumAsMeters", {});
	}

	float GetAvatarEyeHeightMinimumAsMeters() const {
		return CallExact<float>("GetAvatarEyeHeightMinimumAsMeters", {});
	}

	bool GetManualAvatarScalingAllowed() const {
		return CallExact<bool>("GetManualAvatarScalingAllowed", {});
	}

	void SetAvatarEyeHeightByMeters(float height) const {
		CallExact<void>("SetAvatarEyeHeightByMeters", { "System.Single" }, height);
	}

	void SetAvatarEyeHeightByMultiplier(float multiplier) const {
		CallExact<void>("SetAvatarEyeHeightByMultiplier", { "System.Single" }, multiplier);
	}

	void SetAvatarEyeHeightMaximumByMeters(float height) const {
		CallExact<void>("SetAvatarEyeHeightMaximumByMeters", { "System.Single" }, height);
	}

	void SetAvatarEyeHeightMinimumByMeters(float height) const {
		CallExact<void>("SetAvatarEyeHeightMinimumByMeters", { "System.Single" }, height);
	}

	void SetManualAvatarScalingAllowed(bool allowed) const {
		CallExact<void>("SetManualAvatarScalingAllowed", { "System.Boolean" }, allowed);
	}
};
} // namespace VRC::SDKBase
)URKVRCPLAYERAPI";
}