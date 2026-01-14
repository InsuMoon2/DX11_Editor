#pragma once

class AnimNotifyState;
class AnimNotify;

// ─────────────────────────────────────────────
// 단일 프레임 노티파이 데이터
// ─────────────────────────────────────────────
struct AnimNotifyData
{
    wstring name;
    int frame = 0;

    // 추후 확장용
    // wstring soundPath;
    // wstring effectPath;
};

// ─────────────────────────────────────────────
// 구간 노티파이 데이터 (시작~끝 프레임)
// ─────────────────────────────────────────────
struct AnimNotifyStateData
{
    wstring name;
    int startFrame = 0;
    int endFrame = 0;
};

// ─────────────────────────────────────────────
// 애니메이션별 노티파이 컨테이너
// ─────────────────────────────────────────────
struct AnimNotifyContainer
{
    wstring animationName;

    vector<shared_ptr<AnimNotify>> notifies;
    vector<shared_ptr<AnimNotifyState>> notifyStates;
};
