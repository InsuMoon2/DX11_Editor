#pragma once

/// <summary>
/// RenderTarget (FrameBuffer)
/// - 오프스크린 렌더링을 위한 텍스처
/// - Scene View, Game View 등에서 사용
/// </summary>
class RenderTarget
{
public:
    RenderTarget();
    ~RenderTarget();

public:
    void Create(uint32 width, uint32 height);
    void Resize(uint32 width, uint32 height);

    void BindAsTarget();
    void Clear(const Color& color = Color(0.2f, 0.2f, 0.2f, 1.f));

    // 기본 렌더타겟(백버퍼)로 복원
    static void UnbindAll();

public:
    ImTextureID GetSRV() const { return (ImTextureID)(uintptr_t)_shaderResourceView.Get(); }

    ComPtr<ID3D11ShaderResourceView> GetShaderResourceView() const { return _shaderResourceView; }
    ComPtr<ID3D11RenderTargetView>   GetRenderTargetView() const { return _renderTargetView; }
    ComPtr<ID3D11DepthStencilView>   GetDepthStencilView() const { return _depthStencilView; }

    uint32 GetWidth() const  { return _width; }
    uint32 GetHeight() const { return _height; }

private:
    void Release();

private:
    uint32 _width = 0;
    uint32 _height = 0;

    // ─────────────────────────────────────────────
    // Color Buffer (렌더링 결과 저장)
    // ─────────────────────────────────────────────
    ComPtr<ID3D11Texture2D> _texture;
    ComPtr<ID3D11RenderTargetView> _renderTargetView;
    ComPtr<ID3D11ShaderResourceView> _shaderResourceView;

    // ─────────────────────────────────────────────
    // Depth Buffer
    // ─────────────────────────────────────────────
    ComPtr<ID3D11Texture2D> _depthTexture;
    ComPtr<ID3D11DepthStencilView> _depthStencilView;

    // ─────────────────────────────────────────────
    // Viewport
    // ─────────────────────────────────────────────
    D3D11_VIEWPORT _viewport = {};

};

