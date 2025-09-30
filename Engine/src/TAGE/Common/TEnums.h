#pragma once
#include "TDefines.h"

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
	R16F,
	R8,
	RED_INTEGER,
	DEPTH24STENCIL8,
	DEPTH32F,
	DEPTH24,
	DEPTH_ARRAY
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