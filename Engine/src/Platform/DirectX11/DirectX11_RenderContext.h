#pragma once
#include "TARE/Common/RenderContext.h"
#include <d3d11.h>
#include <wrl/client.h>

struct GLFWwindow;
namespace TARE {
	class DirectX11_RenderContext : public RenderContext
	{
	public:
		DirectX11_RenderContext(GLFWwindow* window);
		~DirectX11_RenderContext();

        virtual void Init() override;
        virtual void SwapBuffers() override;

    private:
        void CreateDeviceAndSwapChain();
        void CreateRenderTargetAndDepthStencil();

    private:
        GLFWwindow* _Window = nullptr;
        HWND _HWND = nullptr;

        Microsoft::WRL::ComPtr<ID3D11Device> _Device;
        Microsoft::WRL::ComPtr<ID3D11DeviceContext> _Context;
        Microsoft::WRL::ComPtr<IDXGISwapChain> _SwapChain;
        Microsoft::WRL::ComPtr<ID3D11RenderTargetView> _RenderTargetView;
        Microsoft::WRL::ComPtr<ID3D11DepthStencilView> _DepthStencilView;
        Microsoft::WRL::ComPtr<ID3D11Texture2D> _DepthStencilBuffer;
	};
}