#include "pch.h"
#include "MeshRenderer.h"
#include "Camera.h"
#include "Game.h"
#include "Mesh.h"
#include "Shader.h"
#include "RenderManager.h"

MeshRenderer::MeshRenderer() : Super(ComponentType::MeshRenderer)
{

}

MeshRenderer::~MeshRenderer()
{

}

void MeshRenderer::Update()
{
	if (_mesh == nullptr || _texture == nullptr || _shader == nullptr)
		return;

    //auto var = _shader->GetSRV("Texture0")->SetResource(_texture->GetComPtr().Get());
    auto var = _shader->GetSRV("DiffuseMap")->SetResource(_texture->GetComPtr().Get());

	auto world = GetTransform()->GetWorldMatrix();
    RENDER->PushTransformData(TransformDesc{ world });

	uint32 stride = _mesh->GetVertexBuffer()->GetStride();
	uint32 offset = _mesh->GetVertexBuffer()->GetOffset();

	DC->IASetVertexBuffers(0, 1, _mesh->GetVertexBuffer()->GetComPtr().GetAddressOf(), &stride, &offset);
	DC->IASetIndexBuffer(_mesh->GetIndexBuffer()->GetComPtr().Get(), DXGI_FORMAT_R32_UINT, 0);

	_shader->DrawIndexed(0, 0, _mesh->GetIndexBuffer()->GetCount(), 0, 0);
}

//void MeshRenderer::Update()
//{
//    if (_mesh == nullptr || _texture == nullptr || _shader == nullptr)
//        return;
//
//    auto var = _shader->GetSRV("DiffuseMap")->SetResource(_texture->GetComPtr().Get());
//    assert(var); // 여기서 걸리면 바로 원인 확정
//
//    auto world = GetTransform()->GetWorldMatrix();
//    RENDER->PushTransformData(TransformDesc{ world });
//
//    uint32 stride = _mesh->GetVertexBuffer()->GetStride();
//    uint32 offset = _mesh->GetVertexBuffer()->GetOffset();
//
//    DC->IASetVertexBuffers(0, 1, _mesh->GetVertexBuffer()->GetComPtr().GetAddressOf(), &stride, &offset);
//    DC->IASetIndexBuffer(_mesh->GetIndexBuffer()->GetComPtr().Get(), DXGI_FORMAT_R32_UINT, 0);
//
//    _shader->DrawIndexed(0, 0, _mesh->GetIndexBuffer()->GetCount(), 0, 0);
//}
