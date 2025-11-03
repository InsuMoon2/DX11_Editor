matrix World;
matrix View;
matrix Projection;
Texture2D Texture0;
float3 LightDir;

struct VertexInput
{
    float4 position : POSITION;
    float2 uv : TEXTCOORD;
    float3 normal : NORMAL;
};

struct VertexOutput
{
    float4 position : SV_POSITION;
    float2 uv : TEXTCOORD;
    float3 normal : NORMAL;
};

VertexOutput VS(VertexInput input)
{
    VertexOutput output;
    output.position = mul(input.position, World);
    output.position = mul(output.position, View);
    output.position = mul(output.position, Projection);

    output.uv = input.uv;
    output.normal = mul(input.normal, (float3x3)World);

    return output;
}

SamplerState Sampler0;

float4 PS(VertexOutput input) : SV_TARGET
{
    float3 normal = normalize(input.normal);
    float3 light = -LightDir;

    // 1,1,1,1 하얀색이라고 가정하고, 내적된 수치를 곱해준다고 가정하면,
    // 수직이라면 하얀색으로 뜨겠지만, 비스듬하다면 점점 어두워지도록 만들어줄 수 있다.

    //return float4(1,1,1,1) * dot(light, normal);
    return Texture0.Sample(Sampler0, input.uv) * dot(light, normal);
}

RasterizerState FillModeWireFrame
{
    FillMode = Wireframe;
};

technique11 T0
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }

    pass P1
    {
        SetRasterizerState(FillModeWireFrame);
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
}

