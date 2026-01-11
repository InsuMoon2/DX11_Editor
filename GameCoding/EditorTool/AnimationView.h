#pragma once

#include "EditorWindow.h"
#include "ImSequencer.h"

class RenderTarget;
class Model;
class ModelAnimator;

// ─────────────────────────────────────────────
// 시퀀서 인터페이스 구현
// ─────────────────────────────────────────────
struct AnimSequence : public ImSequencer::SequenceInterface
{
    int frameMin = 0;
    int frameMax = 0;

	AnimNotifyContainer* notifyContainer = nullptr;

    // 필수 구현 요소
    int GetFrameMin() const override { return frameMin; }
    int GetFrameMax() const override { return frameMax; }
    int GetItemCount() const override { return 1; } // 애니메이션 트랙 1개

	void Get(int index, int** start, int** end, int* type, unsigned int* color) override;

    const char* GetItemLabel(int index) const override { return "Animation"; }

	void CustomDraw(int index, ImDrawList* draw_list,
		const ImRect& rc, const ImRect& legendRect,
		const ImRect& clippingRect, const ImRect& legendClippingRect);

    size_t GetCustomHeight(int) override { return 20; }

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
    void SetModelAnimator(shared_ptr<ModelAnimator> animator) { _modelAnimator = animator; };

private:
    void UpdateCameraInput();

    void DrawPreview();
    void DrawControls();
    void DrawSequencer();
    void DrawAnimationList();
	void DrawNotifyPanel();

private:
    vector<wstring> _animNames;
    vector<wstring> _animPaths;

    shared_ptr<Model> _model;
    shared_ptr<ModelAnimator> _modelAnimator;

    shared_ptr<GameObject>   _previewCharacter;  
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
	// 노티파이 편집용
	int _selectedNotifyIndex = -1;
	int _selectedNotifyStateIndex = -1;
	char _newNotifyName[128] = "";
	int _newNotifyFrame = 0;
	int _newNotifyStateStartFrame = 0;
	int _newNotifyStateEndFrame = 0;

};

