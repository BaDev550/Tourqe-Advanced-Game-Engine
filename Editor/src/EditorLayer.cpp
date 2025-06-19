#include "EditorLayer.h"
#include "imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace TourqeEditor {
	EditorLayer::EditorLayer()
		: Layer("EditorLayer")
	{
		m_SceneHierarchyPanel = TAGE::MEM::MakeScope<SceneHierarchyPanel>(TAGE::Application::Get()->GetScene());
	}

	void EditorLayer::OnAttach()
	{
		//m_IconPlay = TextureImporter::LoadTexture2D("Resources/Icons/PlayButton.png");
		//m_IconPause = TextureImporter::LoadTexture2D("Resources/Icons/PauseButton.png");
		//m_IconSimulate = TextureImporter::LoadTexture2D("Resources/Icons/SimulateButton.png");
		//m_IconStep = TextureImporter::LoadTexture2D("Resources/Icons/StepButton.png");
		//m_IconStop = TextureImporter::LoadTexture2D("Resources/Icons/StopButton.png");
	}

	void EditorLayer::OnDetach()
	{
	}

	void EditorLayer::OnUpdate(float dt)
	{
		auto[mx, my] = ImGui::GetMousePos();
		mx -= m_ViewportBounds[0].x;
		my -= m_ViewportBounds[0].y;
		glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
		my = viewportSize.y - my;
		int mouseX = (int)mx;
		int mouseY = (int)my;
	}

	void EditorLayer::OnImGuiRender()
	{
		static bool dockspaceOpen = true;
		static bool opt_fullscreen_persistant = true;
		bool opt_fullscreen = opt_fullscreen_persistant;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}

		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
		ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// DockSpace
		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();
		float minWinSizeX = style.WindowMinSize.x;
		style.WindowMinSize.x = 370.0f;
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		style.WindowMinSize.x = minWinSizeX;

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{

				ImGui::Separator();

				if (ImGui::MenuItem("Exit"))
					TAGE::Application::Get()->Close();
				
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		ImGui::Begin("Viewport");

		auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
		auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
		auto viewportOffset = ImGui::GetWindowPos();
		m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
		m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

		m_ViewportFocused = ImGui::IsWindowFocused();
		m_ViewportHovered = ImGui::IsWindowHovered();

		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

		TAGE::Application::Get()->GetImGuiLayer()->BlockEvents(!m_ViewportHovered);

		auto renderer = TAGE::Application::Get()->GetRenderer();
		auto deferred = renderer->GetDeferredRendering();
		deferred.GetLightShader()->Use();
		uint64 textureID = deferred.GetLightingBuffer()->GetColorAttachment(0);
		ImGui::Image((ImTextureID)(void*)(uintptr_t)textureID, ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

		ImGui::Text("G-Buffer Textures:");
		ImGui::Image((ImTextureID)(void*)(uintptr_t)deferred.GetGBuffer()->GetColorAttachment(0), ImVec2{ m_ViewportSize.x / 3, m_ViewportSize.y / 3 }, ImVec2{0, 1}, ImVec2{1, 0}); // Position
		ImGui::SameLine();
		ImGui::Image((ImTextureID)(void*)(uintptr_t)deferred.GetGBuffer()->GetColorAttachment(1), ImVec2{ m_ViewportSize.x / 3, m_ViewportSize.y / 3 }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 }); // Normal
		ImGui::SameLine();
		ImGui::Image((ImTextureID)(void*)(uintptr_t)deferred.GetGBuffer()->GetColorAttachment(2), ImVec2{ m_ViewportSize.x / 3, m_ViewportSize.y / 3 }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

		ImGui::Image((ImTextureID)(void*)(uintptr_t)deferred.GetGBuffer()->GetDepthAttachment(), ImVec2{ m_ViewportSize.x / 3, m_ViewportSize.y / 3 }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
		ImGui::SameLine();
		ImGui::Image((ImTextureID)(void*)(uintptr_t)renderer->GetShadowMap().GetTextureID(), ImVec2{ m_ViewportSize.x / 3, m_ViewportSize.y / 3 }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
		ImGui::End();

		m_SceneHierarchyPanel->OnImGuiRender();
		//m_ContentBrowserPanel->OnImGuiRender();

		ImGui::End();
	}
}
