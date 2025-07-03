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
	MAT4,
	SHORT3,
	USHORT2,
	BYTE3,
	BYTE3_NORM,
	UBYTE2_NORM
};

enum class RAPI : uint8 {
	UNDEFINED = 0,
	OPENGL,
	VULKAN,
	DIRECTX11
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

enum DrawPropeties {
	DEPTH_TEST = 0x0B71,
	CULL_TEST = 0x0B44,
	BLEND_TEST = 0x0BE2,
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

enum class DebugPrimitiveType {
	LINE,
	SPHERE,
	CAPSULE
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

enum EventCategory
{
	None = 0,
	EventCategoryApplication = BIT(0),
	EventCategoryInput = BIT(1),
	EventCategoryKeyboard = BIT(2),
	EventCategoryMouse = BIT(3),
	EventCategoryMouseButton = BIT(4)
};

enum class SystemUpdateType : uint8 {
	UPDATE = 0,
	PAUSE,
	RENDER_ONLY,
};

enum class FramebufferTextureFormat : uint8 {
	NONE = 0,
	RGBA,
	RGBA8,
	RGBA16F,
	RED_INTEGER,
	DEPTH24STENCIL8,
	DEPTH32F,
	DEPTH24,
};

enum class CullMode : uint8
{
	NONE = 0,
	FRONT,
	BACK,
};

enum class StencilMode : uint8 {
	DISABLE,
	ENABLE,
	ENABLE_FOR_READING
};

enum class BitmapFormat : uint8
{
	UBYTE,
	FLOAT,
};

enum class BitmapType : uint8
{
	T2D,
	CUBE
};

enum class ColliderShapeType : uint8 {
	BOX, 
	SPHERE, 
	CAPSULE, 
	MESH
};

enum class CollisionResponseType : uint8 {
	NONE = 0, 
	OVERLAP, 
	BLOCK 
};

enum class PhysicsBodyType : uint8 {
	STATIC = 0,
	DYNAMIC,
	KINEMATIC 
};

enum class ContactType : uint8 {
	COLLISION,
	TRIGGER
};

enum class RayDrawType {
	FOR_FRAME = 0,
	FOR_DURATION 
};