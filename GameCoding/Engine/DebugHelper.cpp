#include "pch.h"
#include "DebugHelper.h"

#include "Camera.h"
#include "Shader.h"
#include "VertexBuffer.h"
#include "RenderManager.h"

void DebugHelper::Init()
{
    _shader = make_shared<Shader>(L"99. Debug.fx");
    _vertexBuffer = make_shared<VertexBuffer>();
}

void DebugHelper::Update(float timeDelta)
{
    for (auto& line : _lines)
        line.elapsedTime += timeDelta;

    for (auto& sphere : _spheres)
        sphere.elapsedTime += timeDelta;

    for (auto& box : _boxes)
        box.elapsedTime += timeDelta;

    auto isExpired = [](const auto& item)
        {
            return item.duration > 0.f && item.elapsedTime >= item.duration;
        };

    _lines.erase(remove_if(_lines.begin(), _lines.end(), isExpired), _lines.end());
    _spheres.erase(remove_if(_spheres.begin(), _spheres.end(), isExpired), _spheres.end());
    _boxes.erase(remove_if(_boxes.begin(), _boxes.end(), isExpired), _boxes.end());
}

void DebugHelper::Render()
{
    if (_lines.empty() && _spheres.empty() && _boxes.empty())
        return;

    Matrix VP = Camera::S_MatView * Camera::S_MatProjection;
    _shader->GetMatrix("VP")->SetMatrix((float*)&VP);

    vector<VertexColorData> vertices;

    // Lines
    for (const auto& line : _lines)
    {
        vertices.push_back({ line.start, line.color });
        vertices.push_back({ line.end, line.color });
    }

    // Spheres -> Lines
    for (const auto& sphere : _spheres)
    {
        GenerateSphereLines(sphere, vertices);
    }

    // Boxes -> Lines
    for (const auto& box : _boxes)
    {
        GenerateBoxLines(box, vertices);
    }

    if (vertices.empty())
        return;

    // 버퍼 생성, 세팅
    _vertexBuffer->Create(vertices);

    uint32 stride = sizeof(VertexColorData);
    uint32 offset = 0;

    ENGINE_DC->IASetVertexBuffers(0, 1, _vertexBuffer->GetComPtr().GetAddressOf(), &stride, &offset);
    ENGINE_DC->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

    // 셰이더 적용 & Draw (Pass 0: DepthTest ON)
    _shader->Draw(0, 0, static_cast<uint32>(vertices.size()));

    ENGINE_DC->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // 1프레임짜리(duration=0) 제거
    auto isOneFrame = [](const auto& item) { return item.duration == 0.f; };
    _lines.erase(remove_if(_lines.begin(), _lines.end(), isOneFrame), _lines.end());
    _spheres.erase(remove_if(_spheres.begin(), _spheres.end(), isOneFrame), _spheres.end());
    _boxes.erase(remove_if(_boxes.begin(), _boxes.end(), isOneFrame), _boxes.end());
}

void DebugHelper::GenerateSphereLines(const DebugSphereData& sphere, vector<VertexColorData>& outVertices)
{
    const float PI = 3.14159265359f;

    // 3개의 원 (XY, YZ, XZ 평면)
    for (int32 axis = 0; axis < 3; axis++)
    {
        for (int32 i = 0; i < SPHERE_SEGMENTS; i++)
        {
            float angle1 = (2.f * PI * i) / SPHERE_SEGMENTS;
            float angle2 = (2.f * PI * (i + 1)) / SPHERE_SEGMENTS;
            Vec3 p1, p2;

            if (axis == 0) // XY
            {
                p1 = Vec3(cos(angle1) * sphere.radius, sin(angle1) * sphere.radius, 0.f);
                p2 = Vec3(cos(angle2) * sphere.radius, sin(angle2) * sphere.radius, 0.f);
            }
            else if (axis == 1) // YZ
            {
                p1 = Vec3(0.f, cos(angle1) * sphere.radius, sin(angle1) * sphere.radius);
                p2 = Vec3(0.f, cos(angle2) * sphere.radius, sin(angle2) * sphere.radius);
            }
            else // XZ
            {
                p1 = Vec3(cos(angle1) * sphere.radius, 0.f, sin(angle1) * sphere.radius);
                p2 = Vec3(cos(angle2) * sphere.radius, 0.f, sin(angle2) * sphere.radius);
            }
            outVertices.push_back({ sphere.center + p1, sphere.color });
            outVertices.push_back({ sphere.center + p2, sphere.color });
        }
    }
  
}

void DebugHelper::GenerateBoxLines(const DebugBoxData& box, vector<VertexColorData>& outVertices)
{
    Vec3 half = box.size * 0.5f;

    Vec3 v[8] = {
        box.center + Vec3(-half.x,  half.y, -half.z),
        box.center + Vec3(half.x,  half.y, -half.z),
        box.center + Vec3(half.x, -half.y, -half.z),
        box.center + Vec3(-half.x, -half.y, -half.z),
        box.center + Vec3(-half.x,  half.y,  half.z),
        box.center + Vec3(half.x,  half.y,  half.z),
        box.center + Vec3(half.x, -half.y,  half.z),
        box.center + Vec3(-half.x, -half.y,  half.z),
    };

    int edges[12][2] =
    {
        {0,1}, {1,2}, {2,3}, {3,0},
        {4,5}, {5,6}, {6,7}, {7,4},
        {0,4}, {1,5}, {2,6}, {3,7}
    };

    for (int i = 0; i < 12; i++)
    {
        outVertices.push_back({ v[edges[i][0]], box.color });
        outVertices.push_back({ v[edges[i][1]], box.color });
    }

    
}

void DebugHelper::DrawLine(const Vec3& start, const Vec3 end, const Color& color, float duration)
{
    _lines.push_back({ start, end, color, duration, 0.f });
}

void DebugHelper::DrawSphere(const Vec3& center, float radius, const Color& color, float duration)
{
    _spheres.push_back({ center, radius, color, duration, 0.f });
}

void DebugHelper::DrawBox(const Vec3& center, const Vec3& size, const Color& color, float duration)
{
    _boxes.push_back({ center, size, color, duration, 0.f });
}
