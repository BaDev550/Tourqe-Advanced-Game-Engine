#pragma once
#include "BaseComponents.h"
#include "RenderComponents.h"
#include "PhysicsComponents.h"
#include "ScriptingComponents.h"

namespace TAGE {
	template<typename... Component>
	struct ComponentGroup
	{
	};

	using AllComponents = ComponentGroup<
		RelationshipComponent, TransformComponent, MeshComponent,
		RigidBodyComponent, ColliderComponent,
		CameraComponent, SkyboxComponent,
		ScriptComponent, LightComponent>;
}