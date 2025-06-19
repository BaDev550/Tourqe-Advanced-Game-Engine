#include "tagepch.h"
#include "DirectX11_RenderContext.h"
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#define NOMINMAX
#include <GLFW/glfw3native.h>

#include <stdexcept>

namespace TARE {
	DirectX11_RenderContext::DirectX11_RenderContext(GLFWwindow* window) : _Window(window)
	{
		_HWND = glfwGetWin32Window(_Window);
	}
	DirectX11_RenderContext::~DirectX11_RenderContext()
	{
	}
	void DirectX11_RenderContext::Init()
	{
		CreateDeviceAndSwapChain();
		CreateRenderTargetAndDepthStencil();

        LOG_INFO("DirectX initialized successfully");
        LOG_INFO("DirectX Version: 11");
	}

	void DirectX11_RenderContext::SwapBuffers()
	{
        _SwapChain->Present(1, 0);
	}

	void DirectX11_RenderContext::CreateDeviceAndSwapChain()
	{
        DXGI_SWAP_CHAIN_DESC scDesc = {};
        scDesc.BufferCount = 1;
        scDesc.BufferDesc.Width = 0;
        scDesc.BufferDesc.Height = 0;
        scDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        scDesc.BufferDesc.RefreshRate.Numerator = 60;
        scDesc.BufferDesc.RefreshRate.Denominator = 1;
        scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        scDesc.OutputWindow = _HWND;
        scDesc.SampleDesc.Count = 1;
        scDesc.SampleDesc.Quality = 0;
        scDesc.Windowed = TRUE;
        scDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

        UINT createDeviceFlags = 0;
#if defined(_DEBUG)
        createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

        D3D_FEATURE_LEVEL featureLevel;
        const D3D_FEATURE_LEVEL featureLevels[] = {
            D3D_FEATURE_LEVEL_11_1,
            D3D_FEATURE_LEVEL_11_0,
        };

        HRESULT hr = D3D11CreateDeviceAndSwapChain(
            nullptr,
            D3D_DRIVER_TYPE_HARDWARE,
            nullptr,
            createDeviceFlags,
            featureLevels,
            _countof(featureLevels),
            D3D11_SDK_VERSION,
            &scDesc,
            _SwapChain.GetAddressOf(),
            _Device.GetAddressOf(),
            &featureLevel,
            _Context.GetAddressOf()
        );

        if (FAILED(hr))
            throw std::runtime_error("Failed to create D3D11 device and swap chain");
	}

	void DirectX11_RenderContext::CreateRenderTargetAndDepthStencil()
	{
        Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
        _SwapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
        _Device->CreateRenderTargetView(backBuffer.Get(), nullptr, _RenderTargetView.GetAddressOf());

        D3D11_TEXTURE2D_DESC dsDesc = {};
        dsDesc.Width = 1280;
        dsDesc.Height = 720;
        dsDesc.MipLevels = 1;
        dsDesc.ArraySize = 1;
        dsDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        dsDesc.SampleDesc.Count = 1;
        dsDesc.Usage = D3D11_USAGE_DEFAULT;
        dsDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

        _Device->CreateTexture2D(&dsDesc, nullptr, _DepthStencilBuffer.GetAddressOf());
        _Device->CreateDepthStencilView(_DepthStencilBuffer.Get(), nullptr, _DepthStencilView.GetAddressOf());

        _Context->OMSetRenderTargets(1, _RenderTargetView.GetAddressOf(), _DepthStencilView.Get());
	}
}