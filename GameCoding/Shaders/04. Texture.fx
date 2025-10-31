matrix World;
matrix View;
matrix Projection;

Texture2D Texture0;

struct VertexInput
{
    float4 position : POSITION;
    float2 uv : TEXTCOORD;
};

struct VertexOutput
{
    float4 position : SV_POSITION;
    float2 uv : TEXTCOORD;
};

VertexOutput VS(VertexInput input)
{
    VertexOutput output;
    output.position = mul(input.position, World);
    output.position = mul(output.position, View);
    output.position = mul(output.position, Projection);

    output.uv = input.uv;

    return output;
}

SamplerState Smapler0;



float4 PS(VertexOutput input) : SV_TARGET
{
    return Texture0.Sample(Smapler0, input.uv);
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

