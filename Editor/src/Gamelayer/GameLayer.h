#pragma once

#include "TAGE/TAGE.h"

namespace TAGE {
	class GameLayer : public Layer
	{
	public:
		GameLayer();
		virtual ~GameLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		void OnUpdate(float dt) override;
		virtual void OnImGuiRender() override;
		virtual void OnEvent(Event& event) override;
	private:
		MEM::Ref<Scene> _ActiveScene;
		uint textureID;
		glm::vec2 _ViewportSize;
	};
}