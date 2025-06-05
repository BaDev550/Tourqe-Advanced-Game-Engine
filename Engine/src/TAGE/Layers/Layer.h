#pragma once
#include "TAGE/Events/Event.h"

namespace TAGE
{
	class Layer
	{
	public:
		Layer(const char* name = "Layer") : _Name(name) {}
		virtual ~Layer() {}
		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(float dt) {}
		virtual void OnImGuiRender() {}
		virtual void OnEvent(Event& event) {}
		inline const char* GetName() const { return _Name; }
	protected:
		const char* _Name;
	};
}