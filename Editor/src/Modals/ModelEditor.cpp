#include "ModelEditor.h"
#include "imgui.h"

namespace TAGE::Editor {
	static bool s_OpenModelModal = false;
	static MEM::Ref<TARE::Model> s_CurrentModel = nullptr;

	void ModelEditor::OpenModal(MEM::Ref<TARE::Model> model) {
		s_CurrentModel = model;
		s_OpenModelModal = true;
	}

	void ModelEditor::Render() {
		if (s_OpenModelModal) {
			if (ImGui::Begin("Model Editor"))
			{
				for (auto& mesh : s_CurrentModel->GetMeshes()) {
					
				}
			}
		}
	}
}