#include "pch.h"
#include "BlendSpace1D.h"

void BlendSpace1D::AddSample(const wstring& animName, float position)
{
    BlendSample sample;
    sample.animationName = animName;
    sample.position = position;
    sample.animIndex = -1;  // 나중에 BindToModel에서 설정

    _samples.push_back(sample);
    SortSamples();
}

void BlendSpace1D::RemoveSample(int32 index)
{
    if (index >= 0 && index < static_cast<int32>(_samples.size()))
    {
        _samples.erase(_samples.begin() + index);
    }
}

void BlendSpace1D::ClearSamples()
{
    _samples.clear();
}

void BlendSpace1D::SortSamples()
{
    std::sort(_samples.begin(), _samples.end());
}

void BlendSpace1D::BindToModel(shared_ptr<Model> model)
{
    for (auto& sample : _samples)
    {
        // Model에서 애니메이션 이름으로 인덱스 찾기
        sample.animIndex = model->GetAnimationIndex(sample.animationName);

        if (sample.animIndex < 0)
        {
            // 경고: 애니메이션을 찾을 수 없음
            LOG_WARNING("Selected Fail Animation");
        }
    }
}

void BlendSpace1D::SetRange(float minVal, float maxVal)
{
    _minValue = minVal;
    _maxValue = maxVal;
}

void BlendSpace1D::SetSamplePosition(int32 index, float newPosition)
{
    if (index >= 0 && index < static_cast<int32>(_samples.size()))
    {
        _samples[index].position = newPosition;
        SortSamples(); // 위치 변경 후 재정렬
    }
}

int32 BlendSpace1D::GetSampleCount() const
{
    return static_cast<int32>(_samples.size());
}

BlendSample* BlendSpace1D::GetSample(int32 index)
{
    if (index >= 0 && index < static_cast<int32>(_samples.size()))
    {
        return &_samples[index];
    }
    return nullptr;
}

void BlendSpace1D::GetBlendInfo(float parameter, int32& outAnimA, int32& outAnimB, float& outBlendRatio)
{
    if (_samples.empty())
    {
        outAnimA = -1;
        outAnimB = -1;
        outBlendRatio = 0.f;

        return;
    }

    if (_samples.size() == 1)
    {
        outAnimA = _samples[0].animIndex;
        outAnimB = _samples[0].animIndex;
        outBlendRatio = 0.f;
        return;
    }

    float clampedParam = std::clamp(parameter, _minValue, _maxValue);

    // 인접한 두 샘플 찾기
    const BlendSample* sampleA = &_samples[0];
    const BlendSample* sampleB = &_samples[0];

    for (size_t i = 0; i < _samples.size() - 1; i++)
    {
        if (clampedParam >= _samples[i].position &&
            clampedParam <= _samples[i + 1].position)
        {
            sampleA = &_samples[i];
            sampleB = &_samples[i + 1];
            break;
        }
    }

    // 파라미터가 범위 밖인 경우
    if (clampedParam <= _samples.front().position)
    {
        sampleA = &_samples.front();
        sampleB = &_samples.front();
    }
    else if (clampedParam >= _samples.back().position)
    {
        sampleA = &_samples.back();
        sampleB = &_samples.back();
    }

    // 블렌드 비율 계산
    outAnimA = sampleA->animIndex;
    outAnimB = sampleB->animIndex;

    float range = sampleB->position - sampleA->position;
    if (range > 0.0001f)
    {
        outBlendRatio = (clampedParam - sampleA->position) / range;
    }
    else
    {
        outBlendRatio = 0.f;
    }
}
