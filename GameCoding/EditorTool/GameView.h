#pragma once

#include "EditorWindow.h"

/// <summary>
/// Game View 윈도우
/// - 게임 플레이 화면을 미리보기
/// - 실제 게임 카메라의 시점으로 렌더링
/// </summary>
class GameView : public EditorWindow
{
public:
    GameView();
    virtual ~GameView();

public:
    void Init() override;
    void Update() override;
    void OnGui() override;

private:
    // ─────────────────────────────────────────────
    // 뷰포트 관련 변수
    // ─────────────────────────────────────────────
    Vec2 _viewportSize = {};
    bool _isHovered = false;
    bool _isFocused = false;
};

