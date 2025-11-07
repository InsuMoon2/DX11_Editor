#ifndef _LIGHT_FX_
#define _LIGHT_FX_

#include "00. Global.fx"

/////////////////
//   Struct    //
/////////////////

struct LightDesc
{
    float4 ambient;
    float4 diffuse;
    float4 specular;
    float4 emissive;
    float3 direction;
    float  padding;   // 16바이트 정렬 패딩
};

struct MaterialDesc
{
    float4 ambient;
    float4 diffuse;
    float4 specular;
    float4 emissive;
};

/////////////////
// ConstBuffer //
/////////////////

cbuffer LightBuffer
{
    LightDesc GlobalLight;
}

cbuffer MaterialBuffer
{
    MaterialDesc Material;
}

/////////
// SRV //
/////////

Texture2D DiffuseMap;
Texture2D SpecularMap;
Texture2D NormalMap;

//////////////
// Function //
//////////////

float4 ComputeLight(float3 normal, float2 uv, float3 worldPosition)
{
    float4 ambientColor = 0;
    float4 diffuseColor = 0;
    float4 specularColor = 0;
    float4 emissiveColor = 0;

    // Ambient
    {
        float4 color = GlobalLight.ambient * Material.ambient;
        ambientColor = DiffuseMap.Sample(LinearSampler, uv) * color;
    }

    // Diffuse
    {
        float4 color = DiffuseMap.Sample(LinearSampler, uv);

        float value = dot(-GlobalLight.direction, normalize(normal));
        diffuseColor = color * value * GlobalLight.diffuse * Material.diffuse;
    }

    // Specular
    {
        // 반사된 벡터
        //float3 R = reflect(LightDir, input.normal);
        float3 Reflect = GlobalLight.direction - (2 * normal * dot(GlobalLight.direction, normal));
        Reflect = normalize(Reflect);

        // 카메라의 위치를 꺼내서 - 값을 곱하면 된다.
        // 카메라가 오른쪽으로 움직이면, 물체는 왼쪽으로 움직이기 때문에
        float3 cameraPosition = CameraPosition();
        
        float3 Eye = normalize(cameraPosition - worldPosition);

        // 반사 벡터와 시야 벡터를 구했으니까 이제 내적해서 끼인각 구하기
        float value = saturate(dot(Reflect, Eye)); // Saturate => 0~1로 Clamp하는 역할
        float specular = pow(value, 10); // 제곱을 해서 값을 강하게 줘서 퍼포먼스를 낸다.

        specularColor = GlobalLight.specular * Material.specular * specular;
    }

    // Emissive
    {
        float3 cameraPosition = CameraPosition();
        float3 Eye = normalize(cameraPosition - worldPosition);

        float value = saturate(dot(Eye, normal));
        float emissive = 1.0f - value; // 역으로 1을 빼서 1은 0, 0은 1으로 만들기

        // min, max, x
        emissive = smoothstep(0.f, 1.f, emissive); // Lerp와 비슷한 역할
        emissive = pow(emissive, 3);

        emissiveColor = GlobalLight.emissive * Material.emissive * emissive;
    }

    return ambientColor + diffuseColor + specularColor + emissiveColor;
    
}

#endif

