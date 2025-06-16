#pragma once
#include "TDefines.h"

enum class ShaderDataType : uint8 {
	NONE = 0,
	FLOAT,
	INT,
	BOOL,
	VEC2,
	VEC3,
	VEC4,
	MAT3,
	MAT4
};

enum class RAPI : uint8 {
	UNDEFINED = 0,
	OPENGL,
	VULKAN
};

enum class LoadingAssetType : uint8 {
	TEXTURE,
	MODEL
};

enum class LoadingState : uint8 {
	AWAITING_DISK,
	LOADING_DISK,
	LOADING_COMPLETE,
};

enum class ModelType : uint8 {
	SKINNED_MODEL,
	MODEL
};

enum ClearMode {
	COLOR = 0x00004000,
	DEPTH = 0x00000100,
	STENCIL = 0x00000400,
	COLOR_DEPTH = COLOR | DEPTH,
	COLOR_STENCIL = COLOR | STENCIL,
	DEPTH_STENCIL = DEPTH | STENCIL,
	COLOR_DEPTH_STENCIL = COLOR | DEPTH | STENCIL
};

enum class ShadingMode : uint8 {
	SHADED,
	WIREFRAME
};

enum class RenderMode : uint8 {
	DEFAULT = 0,
	POSITIONS,
	NORMALS,
	ALBEDO,
	LIGHTS
};

enum class DebugRenderMode : uint8 {
	NONE = 0,
	WIREFRAME,
	BOUNDING_BOX,
	BONES,
	GRID,
};

enum class LightType : uint8 {
	POINT,
	DIRECTIONAL,
	SPOT
};

enum class TextureType : uint8 {
	DIFFUSE = 0,
	SPECULAR,
	NORMAL,
	EMISSIVE,
	HEIGHT,
	ROUGHNESS,
	METALLIC,
	AMBIENT_OCCLUSION,
	COUNT
};

enum class ApplicationState : uint8 {
	UNINITIALIZED,
	RUNNING,
	PAUSED,
	IN_EDITOR,
	STOPPED
};

enum class LogType : uint8 {
	LTTRACE = 0,
	LTINFO,
	LTWARN,
	LTERROR
};

enum class EEventType : uint8 {
	None = 0,
	WindowClose,
	WindowResize,
	WindowFocus,
	WindowLostFocus,
	WindowMoved,
	AppStarted,
	AppTick,
	AppUpdate,
	AppRender,
	KeyPressed,
	KeyReleased,
	MouseButtonPressed,
	MouseButtonReleased,
	MouseMoved,
	MouseScrolled
};

enum class SystemUpdateType : uint8 {
	UPDATE = 0,
	PAUSE,
	RENDER_ONLY,
};