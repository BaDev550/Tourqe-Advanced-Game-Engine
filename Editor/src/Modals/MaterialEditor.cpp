#include "MaterialEditor.h"
#include "imgui.h"

#include "TAGE/AssetManager/AssetManager.h"
#include "TAGE/Utilities/Platform.h"

namespace TAGE::Editor {
    static bool s_OpenMaterialModal = false;
    static AssetHandle s_CurrentMaterialHandle = 0;

    void MaterialEditor::OpenModal(AssetHandle material) {
        s_CurrentMaterialHandle = material;
        s_OpenMaterialModal = true;
    }

    void MaterialEditor::Render() {
        if (s_OpenMaterialModal) {
            if (ImGui::Begin("Material Editor", &s_OpenMaterialModal, ImGuiWindowFlags_AlwaysAutoResize)) {
                ImGui::Text("Material Editor");
                ImGui::Separator();
				auto& s_CurrentMaterial = AssetManager::GetAsset<TARE::Material>(s_CurrentMaterialHandle);

                if (s_CurrentMaterial) {
                    auto textures = s_CurrentMaterial->GetTextureMaps();
                    auto showTexture = [=](TAGE::AssetHandle handle, const char* label, TextureType type) {
                        auto texture = AssetManager::GetAsset<TARE::Texture2D>(handle);
                        if (texture) {
                            if (ImGui::ImageButton(("IMAGE##" + std::string(label)).c_str(), (ImTextureID)(void*)texture->GetID(), { 64, 64 }, { 1, 0 }, { 0, 1 })) {
								SelectTexture(type);
								Project::GetActive()->GetEditorAssetManager()->SaveAsset(s_CurrentMaterialHandle);
                            }
							ImGui::SameLine();
                            if (ImGui::Button(("Remove##" + std::string(label)).c_str())) {
								s_CurrentMaterial->SetTexture(type, 0);
                            }
                        }
                        else {
                            if (ImGui::Button(("Select Texture" + std::string(label)).c_str())) {
                                SelectTexture(type);
                                Project::GetActive()->GetEditorAssetManager()->SaveAsset(s_CurrentMaterialHandle);
                            }
                        }
                        if (ImGui::CollapsingHeader(("Debug Info ##" + std::string(label)).c_str())) {
                            ImGui::Text("%s", label);
                            ImGui::Text("%s Handle: %llu", label, handle);
                        }
                        };

                    showTexture(textures.Diffuse, "Diffuse", TextureType::DIFFUSE);
                    showTexture(textures.Specular, "Specular", TextureType::SPECULAR);
                    showTexture(textures.Normal, "Normal", TextureType::NORMAL);
                    showTexture(textures.Roughness, "Roughness", TextureType::ROUGHNESS);
                    showTexture(textures.Metallic, "Metallic", TextureType::METALLIC);
                    showTexture(textures.AmbientOcclusion, "AO", TextureType::AMBIENT_OCCLUSION);

					glm::vec3 color = glm::vec3(s_CurrentMaterial->GetColors().Diffuse);
                    if (ImGui::ColorEdit3("Diffuse Color", glm::value_ptr(color))) {
						s_CurrentMaterial->SetColor(TextureType::DIFFUSE, glm::vec4(color, 1.0f));
                    }

                    if (ImGui::Button("Save")) {
					    Project::GetActive()->GetEditorAssetManager()->SaveAsset(s_CurrentMaterialHandle);
					}
                }

                ImGui::End();
            }
        }
    }

    void MaterialEditor::SelectTexture(TextureType type)
    {
        auto path = Platform::FileDialog::OpenFile(
            "Image Files (*.png;*.jpg;*.jpeg;*.bmp)\0*.png;*.jpg;*.jpeg;*.bmp\0"
        );

        if (!path.empty() && std::filesystem::exists(path))
        {
            std::filesystem::path sourcePath = std::filesystem::absolute(path);
            std::filesystem::path targetPath = Project::GetActive()->GetAssetDirectory() / sourcePath.filename();

            try
            {
                auto newTextureHandle = Project::GetActive()->GetEditorAssetManager()->ImportAsset(sourcePath, targetPath);

                auto& s_CurrentMaterial = AssetManager::GetAsset<TARE::Material>(s_CurrentMaterialHandle);
                s_CurrentMaterial->SetTexture(type, newTextureHandle);
            }
            catch (const std::exception& e)
            {
                LOG_ERROR("Failed to import asset '{}': {}", sourcePath.string(), e.what());
            }
        }
        else
        {
            LOG_WARN("File path invalid or does not exist: '{}'", path);
        }
    }
}