#include "tagepch.h"
#include "LayerStack.h"

namespace TAGE {
	LayerStack::LayerStack() {}

	LayerStack::~LayerStack() {
		for (Layer* layer : m_Layers)
			delete layer;
	}

	void LayerStack::PushLayer(Layer* layer) {
		m_Layers.emplace(m_Layers.begin() + m_LayerInsertIndex, layer);
		m_LayerInsertIndex++;
		layer->OnAttach();
		LOG_INFO("Layer {0} pushed!", layer->GetName());
	}

	void LayerStack::PushOverlay(Layer* overlay) {
		m_Layers.emplace_back(overlay);
		overlay->OnAttach();
		LOG_INFO("Overlay {0} pushed!", overlay->GetName());
	}

	void LayerStack::PopLayer(Layer* layer) {
		auto it = std::find(m_Layers.begin(), m_Layers.begin() + m_LayerInsertIndex, layer);
		if (it != m_Layers.begin() + m_LayerInsertIndex) {
			layer->OnDetach();
			m_Layers.erase(it);
			m_LayerInsertIndex--;
		}
	}

	void LayerStack::PopOverlay(Layer* overlay) {
		auto it = std::find(m_Layers.begin() + m_LayerInsertIndex, m_Layers.end(), overlay);
		if (it != m_Layers.end()) {
			overlay->OnDetach();
			m_Layers.erase(it);
		}
	}

	void LayerStack::Clear()
	{
		for (Layer* layer : m_Layers)
			layer->OnDetach();
		m_Layers.clear();
		m_LayerInsertIndex = 0;
		LOG_INFO("LayerStack cleared!");
	}
}
