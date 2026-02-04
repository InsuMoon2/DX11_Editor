#include "00. Global.fx"

DepthStencilState DS_DepthEnable
{
    DepthEnable = true;
    DepthWriteMask = All;
    DepthFunc = Less_Equal;
};

DepthStencilState DS_DepthDisable
{
    DepthEnable = false;
    DepthWriteMask = Zero;
};

struct DebugVertex
{
    float3 Position : POSITION; // Vec3 대응
    float4 Color : COLOR;
};

struct DebugVSOutput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

DebugVSOutput VS_Debug(DebugVertex input)
{
    DebugVSOutput output;
    output.position = mul(float4(input.Position.xyz, 1.0f), VP);
    output.color = input.Color;

    return output;
}

float4 PS_Debug(DebugVSOutput input) : SV_TARGET
{
    return input.color;
}

technique11 T0
{
    // Pass 0 : DepthTest On (물체에 가려짐)
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Debug()));
        SetPixelShader(CompileShader(ps_5_0, PS_Debug()));
        SetDepthStencilState(DS_DepthEnable, 0);
    }

    // Pass 1 : DepthTest Off(항상 보임)
    pass P1
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Debug()));
        SetPixelShader(CompileShader(ps_5_0, PS_Debug()));
        SetDepthStencilState(DS_DepthDisable, 0);
    }
}
