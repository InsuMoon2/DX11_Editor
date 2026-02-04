#pragma once

struct BlendSample
{
    wstring animationName;
    int32 animIndex;
    float position; // 1D 축 위치 (0.f = Idle, 0.5f = Walk, 1.f = Run)

    bool operator < (const BlendSample& other) const
    {
        return position < other.position;
    }
};

class BlendSpace1D
{
public:
    BlendSpace1D() = default;
    ~BlendSpace1D() = default;

public:
    // ─────────────────────────────────────────────
    // 샘플 관리
    // ─────────────────────────────────────────────
    void AddSample(const wstring& animName, float position);
    void RemoveSample(int32 index);
    void ClearSamples();

    // 샘프 ㄹ정렬
    void SortSamples();

    // ─────────────────────────────────────────────
    // 런타임 바인딩
    // ─────────────────────────────────────────────
    void BindToModel(shared_ptr<Model> model);

    // 두 애니메이션과 블렌드 비율 계산
    void GetBlendInfo(float parameter,
                        int32& outAnimA,
                        int32& outAnimB,
                        float& outBlendRatio);

    const vector<BlendSample>& GetSamples() const { return _samples; }
    const wstring& GetName() const { return _name; }
    void SetName(const wstring& name) { _name = name; }

    float GetMinValue() const { return _minValue; }
    float GetMaxValue() const { return _maxValue; }
    void  SetRange(float minVal, float maxVal);

    // 에디터용?
    void SetSamplePosition(int32 index, float newPosition);
    int32 GetSampleCount() const;
    BlendSample* GetSample(int32 index);

private:
    wstring _name;
    vector<BlendSample> _samples; // position 순으로 정렬

    float _minValue = 0.f;
    float _maxValue = 1.f;

};

