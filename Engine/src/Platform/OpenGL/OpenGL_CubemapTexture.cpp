#include "tagepch.h"
#include "OpenGL_Texture.h"
#include "stb/stb_image.h"
#include <GLAD/glad.h>

namespace TARE {
	static glm::vec3 FaceCoordsToXYZ(int x, int y, int FaceID, int FaceSize) {
		float A = 2.0f * float(x) / FaceSize;
		float B = 2.0f * float(y) / FaceSize;
		glm::vec3 Ret;
		switch (FaceID)
		{
		case CUBEMAP_FACE_POSITIVE_X:
			Ret = glm::vec3(A - 1.0f, 1.0f, 1.0f - B);
			break;
		case CUBEMAP_FACE_NEGATIVE_X:
			Ret = glm::vec3(1.0f - A, -1.0f, 1.0f - B);
			break;
		case CUBEMAP_FACE_POSITIVE_Y:
			Ret = glm::vec3(1.0f - B, A - 1.0f, 1.0f);
			break;
		case CUBEMAP_FACE_NEGATIVE_Y:
			Ret = glm::vec3(B - 1.0f, A - 1.0f, -1.0f);
			break;
		case CUBEMAP_FACE_POSITIVE_Z:
			Ret = glm::vec3(-1.0f, A - 1.0f, 1.0f - B);
			break;
		case CUBEMAP_FACE_NEGATIVE_Z:
			Ret = glm::vec3(1.0f, 1.0f - A, 1.0f - B);
			break;
		default:
			ASSERT_NOMSG(false);
			break;
		}
		return glm::normalize(Ret);
	}

	void OpenGL_CubemapTexture::Bind(uint8 slot) const
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_CUBE_MAP, _ID);
	}
	void OpenGL_CubemapTexture::Unbind() const
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}

	bool OpenGL_CubemapTexture::LoadCubemap(const std::string& path)
	{
		_Path = path;
		const float* data = stbi_loadf(path.c_str(), &_Width, &_Height, &_Channels, 0);
		if (!data)
		{
			LOG_ERROR("Failed to load cubemap texture from path: {}", path);
			return false;
		}

		Bitmap In(_Width, _Height, _Channels, BitmapFormat::FLOAT, (void*)data);
		std::vector<Bitmap> Cubemap;
		ConvertEquirectangularToCubemap(In, Cubemap);

		stbi_image_free((void*)data);
		LoadCubemapData(Cubemap);

		return true;
	}
	bool OpenGL_CubemapTexture::ConvertEquirectangularToCubemap(Bitmap in, std::vector<Bitmap>& map)
	{
		int FaceSize = in.w_ / 4;
		map.resize(CUBEMAP_FACE_NUM);
		for (int i = 0; i < CUBEMAP_FACE_NUM; i++) {
			map[i].Init(FaceSize, FaceSize, in.comp_, in.fmt_);
		}

		int MaxW = in.w_ - 1;
		int MaxH = in.h_ - 1;

		for (int face = 0; face < CUBEMAP_FACE_NUM; face++) {
			for (int y = 0; y < FaceSize; y++) {
				for (int x = 0; x < FaceSize; x++) {
					glm::vec3 p = FaceCoordsToXYZ(x, y, face, FaceSize);
					float R = sqrtf(p.x * p.x + p.y * p.y);
					float phi = atan2f(p.y, p.x);
					float theta = atan2f(p.z, R);

					float u = (float)((phi + PI) / (2.0f * PI));
					float v = (float)((PI / 2.0f - theta) / PI);

					float U = u * in.w_;
					float V = v * in.h_;

					int U1 = CLAMP(int(roundf(U)), 0, MaxW);
					int V1 = CLAMP(int(roundf(V)), 0, MaxH);
					int U2 = CLAMP(U1 + 1, 0, MaxW);
					int V2 = CLAMP(V1 + 1, 0, MaxH);

					float s = U - U1;
					float t = V - V1;

					glm::vec4 bottomLeft = in.getPixel(U1, V1);
					glm::vec4 bottomRight = in.getPixel(U2, V1);
					glm::vec4 topLeft = in.getPixel(U1, V2);
					glm::vec4 topRight = in.getPixel(U2, V2);

					glm::vec4 color = 
						bottomLeft * (1 - s) * (1- t) +
						bottomRight * (s) * (1 - t) +
						topLeft * (1 - s) * t +
						topRight * (s) * (t);

					map[face].setPixel(x, y, color);
				}
			}
		}

		return true;
	}
	bool OpenGL_CubemapTexture::LoadCubemapData(std::vector<Bitmap>& bitmaps)
	{
		glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &_ID);
		glTextureParameteri(_ID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(_ID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTextureParameteri(_ID, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTextureParameteri(_ID, GL_TEXTURE_BASE_LEVEL, 0);
		glTextureParameteri(_ID, GL_TEXTURE_MAX_LEVEL, 0);
		glTextureParameteri(_ID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(_ID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTextureStorage2D(_ID, 1, GL_RGBA32F, bitmaps[0].w_, bitmaps[0].h_);

		for (int i = 0; i < CUBEMAP_FACE_NUM; i++) {
			const void* data = bitmaps[i].data_.data();
			glTextureSubImage3D(_ID, 0, 0, 0, i, bitmaps[0].w_, bitmaps[0].h_, 1, GL_RGBA, GL_FLOAT, data);
		}
		return true;
	}

	void OpenGL_CubemapTexture::AttachCubemapFaceToFBO(int face, uint FBO)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, FBO);
		glFramebufferTexture2D(
			GL_FRAMEBUFFER,
			GL_DEPTH_ATTACHMENT,
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + face,
			_ID,
			0
		);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	uint OpenGL_CubemapTexture::LoadShadowPointCubemap()
	{
		glGenTextures(1, &_ID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, _ID);
		for (unsigned int i = 0; i < 6; ++i)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
				SHADOW_MAP_RESOLUTION, SHADOW_MAP_RESOLUTION, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

		return _ID;
	}
}