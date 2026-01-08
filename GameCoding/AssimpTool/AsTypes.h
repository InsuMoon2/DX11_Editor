#pragma once

using VertexType = VertexTextureNormalTangentBlendData;

struct asBone
{
    string name;
    int32 index = -1;
    int32 parent = -1;
    Matrix transform;
};

struct asMesh
{
    string name;
    aiMesh* mesh;
    vector<VertexType> vertices;
    vector<uint32> indices;

    int32 boneIndex;
    string materialName;
};

struct asMaterial
{
    string name;
    Color  ambient;
    Color  diffuse;
    Color  specular;
    Color  emissive;
    string diffuseFile;
    string specularFile;
    string normalFile;
};

// Animation

struct asBlendWeight
{
    void Set(uint32 index, uint32 boneIndex, float weight)
    {
        float i = (float)boneIndex;
        float w = weight;

        switch (index)
        {
        case 0: indices.x = i; weights.x = w; break;
        case 1: indices.y = i; weights.y = w; break;
        case 2: indices.z = i; weights.z = w; break;
        case 3: indices.w = i; weights.w = w; break;
        }
    }

    // 1 2 3 4
    Vec4 indices = Vec4(0, 0, 0, 0);
    Vec4 weights = Vec4(0, 0, 0, 0);

};

// 정점마다 -> (관절번호, 가중치) 최대 4개
struct asBoneWeight
{
    void AddWeights(uint32 boneIndex, float weight)
    {
        if (weight <= 0.0f)
            return;

        // 가중치가 높은 애들이 앞에 오도록 배치
        auto findit =
            std::find_if(boneWeights.begin(), boneWeights.end(),
            [weight](const Pair& p)
            {
                return weight > p.second;
            });

        // (5, 0.5) (1, 0.4), (2, 0.2) 이렇게 가중치가 높은 애들을 앞에 배치하기 위함
        boneWeights.insert(findit, Pair(boneIndex, weight));
    }

    asBlendWeight GetBlendWeight()
    {
        asBlendWeight blendWeights;

        for (uint32 i = 0; i < boneWeights.size(); i++)
        {
            if (i >= 4)
                break;

            blendWeights.Set(i, boneWeights[i].first, boneWeights[i].second);
        }

        return blendWeights;
    }

    // (1, 0.3) (2, 0.2) 더하면 최종 가중치가 0.5가 될 것인데,
    // 최종 가중치를 1로 만들어주기 위해서 2를 곱해주면 0.6, 0.4가 돼서 더하면 1이 된다. 이렇게 보정해줄 함수
    void Normalize()
    {
        if (boneWeights.size() >= 4)
            boneWeights.resize(4);

        float totalWeight = 0.f;
        for (const auto& item : boneWeights)
            totalWeight += item.second;

        float scale = 1.f / totalWeight;
        for (auto item : boneWeights)
            item.second *= scale;
    }

    using Pair = pair <int32, float>;
    vector<Pair> boneWeights;
};

