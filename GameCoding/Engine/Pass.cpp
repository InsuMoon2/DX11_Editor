#include "pch.h"
#include "Pass.h"

void Pass::Draw(UINT vertexCount, UINT startVertexLocation)
{
	BeginDraw();
	{
        ENGINE_DC->Draw(vertexCount, startVertexLocation);
	}
	EndDraw();
}

void Pass::DrawIndexed(UINT indexCount, UINT startIndexLocation, INT baseVertexLocation)
{
	BeginDraw();
	{
        ENGINE_DC->DrawIndexed(indexCount, startIndexLocation, baseVertexLocation);
	}
	EndDraw();
}

void Pass::DrawInstanced(UINT vertexCountPerInstance, UINT instanceCount, UINT startVertexLocation, UINT startInstanceLocation)
{
	BeginDraw();
	{
        ENGINE_DC->DrawInstanced(vertexCountPerInstance, instanceCount, startVertexLocation, startInstanceLocation);
	}
	EndDraw();
}

void Pass::DrawIndexedInstanced(UINT indexCountPerInstance, UINT instanceCount, UINT startIndexLocation, INT baseVertexLocation, UINT startInstanceLocation)
{
	BeginDraw();
	{
        ENGINE_DC->DrawIndexedInstanced(indexCountPerInstance, instanceCount, startIndexLocation, baseVertexLocation, startIndexLocation);
	}
	EndDraw();
}

void Pass::BeginDraw()
{
	pass->ComputeStateBlockMask(&stateblockMask);

    ENGINE_DC->IASetInputLayout(inputLayout.Get());
	pass->Apply(0, ENGINE_DC.Get());
}

void Pass::EndDraw()
{
	if (stateblockMask.RSRasterizerState == 1)
		ENGINE_DC->RSSetState(stateBlock->RSRasterizerState.Get());

	if (stateblockMask.OMDepthStencilState == 1)
		ENGINE_DC->OMSetDepthStencilState(stateBlock->OMDepthStencilState.Get(), stateBlock->OMStencilRef);

	if (stateblockMask.OMBlendState == 1)
		ENGINE_DC->OMSetBlendState(stateBlock->OMBlendState.Get(), stateBlock->OMBlendFactor, stateBlock->OMSampleMask);

	ENGINE_DC->HSSetShader(NULL, NULL, 0);
	ENGINE_DC->DSSetShader(NULL, NULL, 0);
	ENGINE_DC->GSSetShader(NULL, NULL, 0);
}

void Pass::Dispatch(UINT x, UINT y, UINT z)
{
	pass->Apply(0, ENGINE_DC.Get());
	ENGINE_DC->Dispatch(x, y, z);

	ID3D11ShaderResourceView* null[1] = { 0 };
	ENGINE_DC->CSSetShaderResources(0, 1, null);

	ID3D11UnorderedAccessView* nullUav[1] = { 0 };
	ENGINE_DC->CSSetUnorderedAccessViews(0, 1, nullUav, NULL);

	ENGINE_DC->CSSetShader(NULL, NULL, 0);
}
