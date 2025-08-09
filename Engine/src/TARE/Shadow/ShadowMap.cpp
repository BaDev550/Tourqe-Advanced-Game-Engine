#include "tagepch.h"
#include "ShadowMap.h"
#include "TARE/Common/RenderCommands.h"

namespace TARE {
	ShadowMap::ShadowMap(int width, int height)
	{
		this->width = width;
		this->height = height;
		FramebufferSpecification spec({
				FramebufferTextureFormat::DEPTH_ARRAY
			}, 1, width, height, 5);
		_depthFBO = Framebuffer::Create(spec);
		_depthShader = ShaderLibrary::Add("ShadowDepth", "shaders/Shadow/depth_vertex", "shaders/Shadow/depth_fragment", "shaders/Shadow/depth_geometry");
		_depthShader->CreateUBO("LightSpaceMatricesUBO", sizeof(glm::mat4x4) * 16, 3);
	}

	void ShadowMap::BeginRender(const TAGE::MEM::Ref<Camera>& cam, const glm::vec3& lightDir)
	{
		_camera = cam;

		_depthFBO->Bind();
		RenderCommand::Clear(DEPTH);
		_depthShader->Use();
		const auto lightMatrices = getLightSpaceMatrices(lightDir);
		for (size_t i = 0; i < lightMatrices.size(); ++i) {
			_depthShader->UpdateUBO(3, &lightMatrices[i], i * sizeof(glm::mat4x4));
		}

		RenderCommand::SetCullingMode(CullMode::FRONT);
	}

	void ShadowMap::EndRender()
	{
		RenderCommand::SetCullingMode(CullMode::BACK);
		_depthFBO->Unbind();
	}

	void ShadowMap::BindTexture(uint slot) const
	{
		RenderCommand::BindTextureFromID(_depthFBO->GetDepthAttachment(), slot);
	}

    std::vector<glm::vec4> getFrustumCornersWorldSpace(const glm::mat4& projview)
    {
        const auto inv = glm::inverse(projview);

        std::vector<glm::vec4> frustumCorners;
        for (unsigned int x = 0; x < 2; ++x)
        {
            for (unsigned int y = 0; y < 2; ++y)
            {
                for (unsigned int z = 0; z < 2; ++z)
                {
                    const glm::vec4 pt = inv * glm::vec4(2.0f * x - 1.0f, 2.0f * y - 1.0f, 2.0f * z - 1.0f, 1.0f);
                    frustumCorners.push_back(pt / pt.w);
                }
            }
        }

        return frustumCorners;
    }
    std::vector<glm::vec4> getFrustumCornersWorldSpace(const glm::mat4& proj, const glm::mat4& view) { return getFrustumCornersWorldSpace(proj * view); }

    glm::mat4 ShadowMap::getLightSpaceMatrix(const float nearPlane, const float farPlane, const glm::vec3& lightDir) const
    {
        if (auto camera = _camera.lock()) {

            const auto proj = glm::perspective(
                glm::radians(camera->GetFOV()), (float)width / (float)height, nearPlane,
                farPlane);
            const auto corners = getFrustumCornersWorldSpace(proj, camera->GetViewMatrix());

            glm::vec3 center = glm::vec3(0, 0, 0);
            for (const auto& v : corners)
            {
                center += glm::vec3(v);
            }
            center /= corners.size();
            const auto lightView = glm::lookAt(center + lightDir, center, glm::vec3(0.0f, 1.0f, 0.0f));

            float minX = std::numeric_limits<float>::max();
            float maxX = std::numeric_limits<float>::lowest();
            float minY = std::numeric_limits<float>::max();
            float maxY = std::numeric_limits<float>::lowest();
            float minZ = std::numeric_limits<float>::max();
            float maxZ = std::numeric_limits<float>::lowest();
            for (const auto& v : corners)
            {
                const auto trf = lightView * v;
                minX = std::min(minX, trf.x);
                maxX = std::max(maxX, trf.x);
                minY = std::min(minY, trf.y);
                maxY = std::max(maxY, trf.y);
                minZ = std::min(minZ, trf.z);
                maxZ = std::max(maxZ, trf.z);
            }

            constexpr float zMult = 10.0f;
            if (minZ < 0)
            {
                minZ *= zMult;
            }
            else
            {
                minZ /= zMult;
            }
            if (maxZ < 0)
            {
                maxZ /= zMult;
            }
            else
            {
                maxZ *= zMult;
            }

            const glm::mat4 lightProjection = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);
            return lightProjection * lightView;
        }
    }

	std::vector<glm::mat4> ShadowMap::getLightSpaceMatrices(const glm::vec3& lightDir) const
	{
        if (auto camera = _camera.lock()) {
            std::vector<glm::mat4> ret;
            for (size_t i = 0; i < _shadowCascadeLevels.size() + 1; ++i)
            {
                if (i == 0) {
                    ret.push_back(getLightSpaceMatrix(camera->GetNearClip(), _shadowCascadeLevels[i], lightDir));
                }
                else if (i < _shadowCascadeLevels.size()) {
                    ret.push_back(getLightSpaceMatrix(_shadowCascadeLevels[i - 1], _shadowCascadeLevels[i], lightDir));
                }
                else {
                    ret.push_back(getLightSpaceMatrix(_shadowCascadeLevels[i - 1], camera->GetFarClip(), lightDir));
                }
            }
		    return ret;
        }
	}
}