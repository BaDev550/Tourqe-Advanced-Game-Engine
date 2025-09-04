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
                    auto showTexture = [](TAGE::AssetHandle handle, const char* label, TextureType type) {
                        auto texture = AssetManager::GetAsset<TARE::Texture2D>(handle);
                        if (texture) {
                            if (ImGui::ImageButton(("IMAGE##" + std::string(label)).c_str(), (ImTextureID)(void*)texture->GetID(), { 64, 64 }, { 1, 0 }, { 0, 1 })) {
								SelectTexture(type);
								Project::GetActive()->GetEditorAssetManager()->SaveAsset(s_CurrentMaterialHandle);
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
                }

                ImGui::End();
            }
        }
    }

    void MaterialEditor::SelectTexture(TextureType type)
    {
        auto path = Platform::FileDialog::OpenFile("Image Files (*.png;*.jpg;*.jpeg;*.bmp)\0*.png;*.jpg;*.jpeg;*.bmp\0");
        if (!path.empty()) {
            auto relativePath = std::filesystem::relative(path, Project::GetActive()->GetAssetDirectory());
            auto newTextureHandle = Project::GetActive()->GetEditorAssetManager()->ImportAsset(relativePath);
            auto& s_CurrentMaterial = AssetManager::GetAsset<TARE::Material>(s_CurrentMaterialHandle);
            s_CurrentMaterial->SetTexture(type, newTextureHandle);
        }
    }
}