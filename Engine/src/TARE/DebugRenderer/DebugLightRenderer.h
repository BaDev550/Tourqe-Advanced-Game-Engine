#pragma once
#include "TARE/Material/Material.h"
#include "TARE/Buffers/Buffers.h"
#include "TARE/Buffers/VertexArrayBuffer.h"
#include "TARE/Shader/Shader.h"
#include "TARE/Common/Light.h"

namespace TARE::Debug
{
	struct DebugVertexData
	{
		glm::vec3 Position;
	};

	class DebugLightRenderer
	{
	public:
		void Render(const glm::mat4& viewproj, const glm::vec3& position, float radius, int segments, const glm::vec3& color, bool wireframe = true);

		void GenerateSpotLightCone(float radius, float height, int segments);
		void GeneratePointLightSphere(float radius, int latSegments, int lonSegments);
		void GenerateSunOutline(float size);

		void GenerateGeometry(float radius, int segments);

		void SetType(LightType type) { _type = type; }
	private:
		std::vector<DebugVertexData> _vertices;
		std::vector<uint> _indices;
		LightType _type;
		TAGE::MEM::Ref<Shader> _shader;
		TAGE::MEM::Ref<VertexArrayObject> _VAO;
	};
}

