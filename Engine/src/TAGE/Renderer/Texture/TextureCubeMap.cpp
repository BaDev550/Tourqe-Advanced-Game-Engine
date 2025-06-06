#include "tagepch.h"
#include "Texture.h"
#include "stb/stb_image.h"

namespace TAGE::Renderer {
	TextureCubeMap::TextureCubeMap(const std::vector<std::string>& faces)
	{
		glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &_ID);

		ASSERT(!faces.empty(), "Cubemap face paths are empty!");
		for (size_t i = 0; i < faces.size(); i++) {
			int width, height, channels;
			unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &_Channels, 0);
			ASSERT(data, "Failed to load texture image!");
			_Size.x = width;
			_Size.y = height;

			if (data) {
				glTextureStorage2D(_ID, 1, GL_RGB8, width, height);
				glTextureSubImage3D(
					_ID,
					0,
					0, 0, i,
					width, height, 1,
					GL_RGB, GL_UNSIGNED_BYTE,
					data
				);
			}
			else {
				stbi_image_free(data);
			}
		}
		glTextureParameteri(_ID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(_ID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTextureParameteri(_ID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(_ID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTextureParameteri(_ID, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}

	TextureCubeMap::~TextureCubeMap()
	{
		glDeleteTextures(1, &_ID);
	}

	void TextureCubeMap::Bind(uint32_t slot) const
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_CUBE_MAP, _ID);
	}

	void TextureCubeMap::Unbind() const
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}
}