#include "tagepch.h"
#include "MaterialEditor.h"
#include "imgui.h"

#include "TAGE/AssetManager/AssetManager.h"
#include "TAGE/Utilities/Platform.h"

#include "TAGE/GUI/GUIUtils.h"

namespace TAGE {
    static bool s_OpenMaterialModal = false;
    static AssetHandle s_CurrentMaterialHandle = 0;

    void MaterialEditor::OpenModal(AssetHandle material) {
        s_CurrentMaterialHandle = material;
        s_OpenMaterialModal = true;
    }

    void MaterialEditor::Render() {
#if 0
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
                            ImGui::Image((ImTextureID)(void*)texture->GetID(), { 64, 64 }, { 1, 0 }, { 0, 1 });
                            ImGui::SameLine();
                        }

                        {
                            auto& assetManager = *Project::GetActive()->GetEditorAssetManager();
                            auto textureHandles = assetManager.GetHandlesWithType(AssetType::Texture);

                            std::vector<AssetHandle> handles;
                            std::vector<std::string> names;

                            for (const auto& h : textureHandles) {
                                if (AssetManager::IsAssetHandleValid(h)) {
                                    handles.push_back(h);
                                    names.push_back(assetManager.GetMetadata(h).FilePath.stem().string());
                                }
                            }

                            int currentIndex = -1;
                            if (handle) {
                                for (size_t i = 0; i < handles.size(); ++i) {
                                    if (handles[i] == handle) {
                                        currentIndex = static_cast<int>(i);
                                        break;
                                    }
                                }
                            }

                            if (GUI::ComboBox(label, currentIndex, names, "Select...")) {
                                if (currentIndex >= 0 && currentIndex < static_cast<int>(handles.size())) {
                                    s_CurrentMaterial->SetTexture(type, handles[currentIndex]);
                                    Project::GetActive()->GetEditorAssetManager()->SaveAsset(s_CurrentMaterialHandle);
                                }
							}
                        }
						ImGui::SameLine();
                        if (ImGui::Button(("Remove##" + std::string(label)).c_str())) {
                            s_CurrentMaterial->SetTexture(type, 0);
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
#endif
    }
}