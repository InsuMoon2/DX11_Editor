#pragma once

#include "VertexData.h"

struct DebugLineData
{
    Vec3    start;
    Vec3    end;

    Color   color;

    float    duration;     // 총 지속시간
    float    elapsedTime;  // 경과 시간
};

struct DebugSphereData
{
    Vec3    center;
    float   radius;

    Color    color;

    float    duration;
    float    elapsedTime;
};

struct DebugBoxData
{
    Vec3    center;
    Vec3    size;

    Color   color;

    float   duration;
    float   elapsedTime;
};

// TODO : Capsule
struct DebugCapsuleData
{
    
};

// TODO : 충돌체 만들고, 충돌판정 시 Color 변경
class DebugHelper
{
    DECLARE_SINGLE(DebugHelper)

public:
    void Init();
    void Update(float timeDelta);
    void Render();

public:
    void DrawLine(const Vec3& start, const Vec3 end, const Color& color, float duration = 0.f);
    void DrawSphere(const Vec3& center, float radius, const Color& color, float duration = 0.f);
    void DrawBox(const Vec3& center, const Vec3& size, const Color& color, float duration = 0.f);

private:
    void GenerateSphereLines(const DebugSphereData& sphere, vector<VertexColorData>& outVertices);
    void GenerateBoxLines(const DebugBoxData& box, vector<VertexColorData>& outVertices);

private:
    shared_ptr<Shader> _shader;
    shared_ptr<VertexBuffer> _vertexBuffer;

    vector<DebugLineData> _lines;
    vector<DebugSphereData> _spheres;
    vector<DebugBoxData> _boxes;

    const int32 SPHERE_SEGMENTS = 24;
};

#define DEBUG_HELPER        GET_SINGLE(DebugHelper)
