#pragma once
#include "TDefines.h"

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

enum class ShadingMode {
	SHADED,
	WIREFRAME
};

enum class DebugRenderMode {
	NONE = 0,
	WIREFRAME,
	BOUNDING_BOX,
	BONES,
	GRID,
};

enum class LightType {
	POINT,
	DIRECTIONAL,
	SPOT
};

enum class TextureType : uint {
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

enum class ApplicationState {
	UNINITIALIZED,
	RUNNING,
	PAUSED,
	IN_EDITOR,
	STOPPED
};

enum class LogType : uint {
	LTTRACE = 0,
	LTINFO,
	LTWARN,
	LTERROR
};

enum class EEventType : uint {
	None = 0,
	WindowClose,
	WindowResize,
	WindowFocus,
	WindowLostFocus,
	WindowMoved,
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