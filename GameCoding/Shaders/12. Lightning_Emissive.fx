#include "00. Global.fx"

float4 MaterialEmissive;

MeshOutput VS(VertexTextureNormal input)
{
    MeshOutput output;
    output.position = mul(input.position, W);
    output.worldPosition = input.position;
    output.position = mul(output.position, VP);
    output.uv = input.uv;
    output.normal = mul(input.normal, (float3x3) W);

    return output;
}

// Emissive
// 외곽선을 구할 때 사용한다. (림라이트라고도 많이 부름)
float4 PS(MeshOutput input) : SV_TARGET
{
    float3 cameraPosition = -V._41_42_43;
    float3 Eye = normalize(cameraPosition - input.worldPosition);

    float value = saturate(dot(Eye, input.normal));
    float emissive = 1.0f - value; // 역으로 1을 빼서 1은 0, 0은 1으로 만들기

    // min, max, x
    emissive = smoothstep(0.f, 1.f, emissive); // Lerp와 비슷한 역할
    emissive = pow(emissive, 3);

    float4 color = MaterialEmissive * emissive;

    return color;
}

technique11 T0
{
	PASS_VP(P0, VS, PS)
};
