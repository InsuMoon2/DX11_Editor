#include "pch.h"
#include "RenderTarget.h"

RenderTarget::RenderTarget()
{
}

RenderTarget::~RenderTarget()
{
    Release();
}

void RenderTarget::Create(uint32 width, uint32 height)
{
    _width = width;
    _height = height;

    // ─────────────────────────────────────────────
    // 1. Color Texture 생성 (렌더링 결과 저장)
    // ─────────────────────────────────────────────
    {
        D3D11_TEXTURE2D_DESC desc = {};
        desc.Width = _width;
        desc.Height = _height;
        desc.MipLevels = 1;
        desc.ArraySize = 1;
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = 0;
        HRESULT hr = DEVICE->CreateTexture2D(&desc, nullptr, _texture.GetAddressOf());
        CHECK(hr);
    }

    // ─────────────────────────────────────────────
    // 2. RenderTargetView 생성 (렌더링 대상)
    // ─────────────────────────────────────────────
    {
        D3D11_RENDER_TARGET_VIEW_DESC desc = {};
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
        desc.Texture2D.MipSlice = 0;
        HRESULT hr = DEVICE->CreateRenderTargetView(_texture.Get(), &desc, _renderTargetView.GetAddressOf());
        CHECK(hr);
    }

    // ─────────────────────────────────────────────
    // 3. ShaderResourceView 생성 (ImGui에서 표시용)
    // ─────────────────────────────────────────────
    {
        D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        desc.Texture2D.MostDetailedMip = 0;
        desc.Texture2D.MipLevels = 1;
        HRESULT hr = DEVICE->CreateShaderResourceView(_texture.Get(), &desc, _shaderResourceView.GetAddressOf());
        CHECK(hr);
    }

    // ─────────────────────────────────────────────
    // 4. Depth Texture 생성
    // ─────────────────────────────────────────────
    {
        D3D11_TEXTURE2D_DESC desc = {};
        desc.Width = _width;
        desc.Height = _height;
        desc.MipLevels = 1;
        desc.ArraySize = 1;
        desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = 0;
        HRESULT hr = DEVICE->CreateTexture2D(&desc, nullptr, _depthTexture.GetAddressOf());
        CHECK(hr);
    }

    // ─────────────────────────────────────────────
    // 5. DepthStencilView 생성
    // ─────────────────────────────────────────────
    {
        D3D11_DEPTH_STENCIL_VIEW_DESC desc = {};
        desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        desc.Texture2D.MipSlice = 0;
        HRESULT hr = DEVICE->CreateDepthStencilView(_depthTexture.Get(), &desc, _depthStencilView.GetAddressOf());
        CHECK(hr);
    }

    // ─────────────────────────────────────────────
    // 6. Viewport 설정
    // ─────────────────────────────────────────────
    _viewport.TopLeftX = 0.0f;
    _viewport.TopLeftY = 0.0f;
    _viewport.Width = static_cast<float>(_width);
    _viewport.Height = static_cast<float>(_height);
    _viewport.MinDepth = 0.0f;
    _viewport.MaxDepth = 1.0f;

}

void RenderTarget::Resize(uint32 width, uint32 height)
{
    if (_width == width && _height == height)
        return;

    Release();

    Create(width, height);
}

void RenderTarget::BindAsTarget()
{
    // ─────────────────────────────────────────────
    // 이 RenderTarget을 렌더링 대상으로 설정
    // ─────────────────────────────────────────────
    DC->OMSetRenderTargets(1, _renderTargetView.GetAddressOf(), _depthStencilView.Get());
    DC->RSSetViewports(1, &_viewport);
}

void RenderTarget::Clear(const Color& color)
{
    // ─────────────────────────────────────────────
    // RenderTarget 클리어
    // ─────────────────────────────────────────────
    DC->ClearRenderTargetView(_renderTargetView.Get(), (float*)&color);
    DC->ClearDepthStencilView(_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void RenderTarget::UnbindAll()
{
    // ─────────────────────────────────────────────
    // RenderTarget 해제 (nullptr로 설정)
    // 이후 기본 백버퍼로 복원 필요
    // ─────────────────────────────────────────────
    ID3D11RenderTargetView* nullRTV = nullptr;
    DC->OMSetRenderTargets(1, &nullRTV, nullptr);

    GRAPHICS->BindBackBuffer();  // 백버퍼로 복원
}

void RenderTarget::Release()
{
    _texture.Reset();
    _renderTargetView.Reset();
    _shaderResourceView.Reset();
    _depthTexture.Reset();
    _depthStencilView.Reset();
}
