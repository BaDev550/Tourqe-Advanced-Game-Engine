#include "tagepch.h"
#include "Entity.h"
#include "TAGE/World/Scene/Scene.h"

namespace TAGE {
	Entity Entity::GetParent() const {
		return _Scene->TryGetEntityWithUUID(GetParentUUID());
	}

	void Entity::SetParent(Entity parent)
	{
		Entity currentParent = GetParent();
		if (currentParent == parent)
			return;

		if (currentParent)
			currentParent.RemoveChild(*this);
		SetParentUUID(parent.GetUUID());

		if (parent)
		{
			auto& parentChildren = parent.Children();
			UUID uuid = GetUUID();
			if (std::find(parentChildren.begin(), parentChildren.end(), uuid) == parentChildren.end())
				parentChildren.emplace_back(GetUUID());
		}
	}

	bool Entity::IsAncestorOf(Entity entity) const {
		const auto& children = Children();

		if (children.empty())
			return false;

		for (UUID child : children)
		{
			if (child == entity.GetUUID())
				return true;
		}

		for (UUID child : children)
		{
			if (_Scene->GetEntityByUUID(child).IsAncestorOf(entity))
				return true;
		}

		return false;
	}
}