#pragma once

#include "Layer.h"

namespace TAGE {

	class ImGuiLayer : public Layer {
	public:
		ImGuiLayer();
		~ImGuiLayer() override;

		void OnAttach() override;
		void OnDetach() override;
		void OnImGuiRender() override;
		void Begin();
		void End();
		void OnEvent(Event& e) override;

		void BlockEvents(bool block) { _BlockEvents = block; }

	private:
		bool _BlockEvents = true;
	};

}
