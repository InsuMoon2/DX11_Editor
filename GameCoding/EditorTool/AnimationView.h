#pragma once

#include "EditorWindow.h"
#include "ImSequencer.h"

class RenderTarget;
class Model;
class ModelAnimator;

namespace SeqColors
{
    // Notify (이벤트) - 초록 계열
    constexpr unsigned int NotifyBg = 0xFF22AA44;        // 진한 초록
    constexpr unsigned int NotifyBgHover = 0xFF33CC55;   // 밝은 초록
    constexpr unsigned int NotifyBorder = 0xFF115522;    // 어두운 초록

    // NotifyState (구간) - 주황 계열
    constexpr unsigned int StateBg = 0xFF4488DD;         // 주황
    constexpr unsigned int StateBgHover = 0xFF55AAEE;    // 밝은 주황
    constexpr unsigned int StateBorder = 0xFF336699;     // 어두운 주황

    // 핸들
    constexpr unsigned int HandleBg = 0xFF225588;        // 핸들 기본
    constexpr unsigned int HandleBgHover = 0xFF3377AA;   // 핸들 호버

    // 선택
    constexpr unsigned int Selected = 0xFF00FFFF;        // 시안 (선택 테두리)
    constexpr unsigned int Text = 0xFFFFFFFF;            // 흰색 텍스트
}

// ─────────────────────────────────────────────
// 시퀀서 인터페이스 구현
// ─────────────────────────────────────────────
struct AnimSequence : public ImSequencer::SequenceInterface
{
    int frameMin = 0;
    int frameMax = 0;

    AnimNotifyContainer* notifyContainer = nullptr;

    int selectedNotifyIndex = -1;
    int selectedNotifyStateIndex = -1;

    // 필수 구현 요소
    int GetFrameMin() const override { return frameMin; }
    int GetFrameMax() const override { return frameMax; }
    int GetItemCount() const override { return 3; } // 애니메이션 트랙 개수

    void Get(int index, int** start, int** end, int* type, unsigned int* color) override;

    const char* GetItemLabel(int index) const override;

	void CustomDraw(int index, ImDrawList* draw_list,
		const ImRect& rc, const ImRect& legendRect,
		const ImRect& clippingRect, const ImRect& legendClippingRect);

    size_t GetCustomHeight(int) override { return 20; }

    bool    clickedOnNotify = false;
    int     draggingNotifyIndex = -1;
    float   dragOffset = 0.f;

    int     draggingStateIndex = -1;
    int     stateDragMode = 0; // 0 : 없음, 1 : 전체 이동, 2 : 왼쪽 핸들, 3 : 오른쪽 핸들
    float   stateDragOffset = 0.f;
};

/// <summary>
/// Animation View 창
/// - ImGuiSequencer로 애니메이션 타임라인 표시
/// - 프레임 제어 및 Notify 편집
/// </summary>
class AnimationView : public EditorWindow
{
public:
    AnimationView();
    virtual ~AnimationView();

public:
    void Init() override;
    void Update() override;
    void OnGui() override;

    // Content Browser에서 호출
    void SetAnimation(shared_ptr<Model> model, int animIndex, vector<wstring>& animPaths);
    void SetModelAnimator(shared_ptr<ModelAnimator> animator) { _previewAnimator = animator; };

public:
    void SetCurrentFrame(int frame) { _currentFrame = frame; _playbackTime = frame / 30.f; }
    void ClearSelection();

private:
    void UpdateCameraInput();

    void DrawPreview();
    void DrawControls();
    void DrawSequencer();
    void DrawAnimationList();

    void NotifyDetailView();
    void CloseDetailView();

private:
    vector<wstring> _animNames;
    vector<wstring> _animPaths;

    shared_ptr<GameObject>      _previewCharacter;  
    shared_ptr<Model>           _previewModel;
    shared_ptr<ModelAnimator>   _previewAnimator;

    shared_ptr<Shader> _shader;

    int _animIndex = 0;

    // 재생 상태
    bool _isPlaying = false;
    int _currentFrame = 0;
    int _frameMin = 0;
    int _frameMax = 100;        // 이건 임시 최대치고, 따로 조절을 해야함

    float _playbackTime = 0;    // 애니메이션 재생 시간 (초 단위)

private:
    // 시퀀서 관련
    AnimSequence _sequence;
    bool _sequencerExpanded = true;
    int _selectedEntry = -1;
    int _firstFrame = 0;

    // 프리뷰용 RenderTarget
    shared_ptr<RenderTarget> _previewRenderTarget;
    shared_ptr<GameObject>   _previewCamera;

private:
    // 카메라 조작용
    POINT _lastMousePos = { 0, 0 };
    float _moveSpeed = 10.f;
    float _rotSpeed = 0.005f;
    bool _isFocused = false;
    bool _isHovered = false;

private:
	// Notify
	int _selectedNotifyIndex = -1;
	int _selectedNotifyStateIndex = -1;

};

