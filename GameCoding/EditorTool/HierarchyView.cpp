#include "pch.h"
#include "HierarchyView.h"
#include "GameObject.h"
#include "InspectorView.h"
#include "EditorManager.h"
#include "SceneView.h"

HierarchyView::HierarchyView()
    : EditorWindow("Hierarchy")
{

}

HierarchyView::~HierarchyView()
{
}

void HierarchyView::Init()
{
    // GameObject 생성 이벤트 구독
    EVENTS->Subscribe(EventType::GameObject_Created, [this](Event& e)
        {
            auto& event = static_cast<GameObjectCreateEvent&>(e);
            AddObject(event.GetGameObject());
            LOG_WARNING("GameObject Created !");
        });

    // GameObject 삭제 이벤트 구독
    EVENTS->Subscribe(EventType::GameObject_Destroyed, [this](Event& e)
        {
            auto& event = static_cast<GameObjectCreateEvent&>(e);
            RemoveObject(event.GetGameObject());
            LOG_WARNING("GameObject Destroyed!");
        });
}

void HierarchyView::Update()
{
    
}

void HierarchyView::OnGui()
{
    ImGui::Begin("Hierarchy", nullptr, ImGuiWindowFlags_NoNavInputs);

    if (ImGui::IsWindowFocused())
        HandleInput();

    DrawToolbar();
    DrawObjectList();

    ImGui::End();
}

shared_ptr<GameObject> HierarchyView::GetSelectedObject() const
{
    return _selectedObjects.empty() ? nullptr : _selectedObjects[0];
}

void HierarchyView::SetSelectedObject(shared_ptr<GameObject> obj)
{
    _selectedObjects.clear();

    if (obj)
        _selectedObjects.push_back(obj);
}

void HierarchyView::AddSelectedObjects(shared_ptr<GameObject> obj)
{
    auto it = find(_selectedObjects.begin(), _selectedObjects.end(), obj);

    if (obj && it == _selectedObjects.end())
        _selectedObjects.push_back(obj);
}

void HierarchyView::RemoveSelectedObject(shared_ptr<GameObject> obj)
{
    auto it = find(_selectedObjects.begin(), _selectedObjects.end(), obj);

    if (it != _selectedObjects.end())
        _selectedObjects.erase(it);
}

void HierarchyView::ClearSelection()
{
    _selectedObjects.clear();
}

bool HierarchyView::IsSelected(shared_ptr<GameObject> obj) const
{
    return find(_selectedObjects.begin(), _selectedObjects.end(), obj) != _selectedObjects.end();
}

void HierarchyView::AddSelectionTowards(int direction)
{
    if (_sceneObjects.empty())
        return;

    int currentIndex = GetCurrentSelectedIndex();

    if (currentIndex < 0)
        currentIndex = 0;

    int newIndex = max(0, min((int)_sceneObjects.size() - 1, currentIndex + direction));

    if (!IsSelected(_sceneObjects[newIndex]))
        AddSelectedObjects(_sceneObjects[newIndex]);
}

void HierarchyView::MoveSelection(int direction)
{
    if (_sceneObjects.empty())
        return;

    int currentIndex = GetCurrentSelectedIndex();

    if (currentIndex < 0)
        currentIndex = 0;

    int newIndex = max(0, min((int)_sceneObjects.size() - 1, currentIndex + direction));

    ClearSelection();
    AddSelectedObjects(_sceneObjects[newIndex]);

    // Inspector 업데이트
    auto inspector = dynamic_pointer_cast<InspectorView>(
        GET_SINGLE(EditorManager)->GetWindow(L"Inspector"));

    if (inspector)
        inspector->SetTarget(_sceneObjects[newIndex]);
}

int HierarchyView::GetCurrentSelectedIndex()
{
    if (_selectedObjects.empty() || _sceneObjects.empty())
        return -1;

    auto it = find(_sceneObjects.begin(), _sceneObjects.end(), _selectedObjects.back());
    if (it != _sceneObjects.end())
        return (int)distance(_sceneObjects.begin(), it);

    return -1;
}

void HierarchyView::AddObject(shared_ptr<GameObject> obj)
{
    _sceneObjects.push_back(obj);
}

void HierarchyView::RemoveObject(shared_ptr<GameObject> obj)
{
    auto it = find(_sceneObjects.begin(), _sceneObjects.end(), obj);
    if (it != _sceneObjects.end())
    {
        _sceneObjects.erase(it);
    }
}

shared_ptr<GameObject> HierarchyView::CloneGameObject(shared_ptr<GameObject> original)
{
    if (!original)
        return nullptr;

    return original->Clone();
}

void HierarchyView::HandleInput()
{
    // ═══════════════════════════════════════════════
    // 단축키 처리 (창이 포커스 되었을 때만)
    // ═══════════════════════════════════════════════
    if (ImGui::IsWindowFocused())
    {
        if (!_selectedObjects.empty())
        {
            if (INPUT->GetButton(KEY_TYPE::CTRL))
            {
                if (INPUT->GetButtonDown(KEY_TYPE::UP))
                {
                    AddSelectionTowards(-1);
                    _keyRepeatTimer = 0.f;
                }
                else if (INPUT->GetButton(KEY_TYPE::UP))
                {
                    _keyRepeatTimer += DT;
                    if (_keyRepeatTimer >= _keyRepeatDelay)
                    {
                        AddSelectionTowards(-1);

                        _keyRepeatTimer = 0.f;
                    }
                }

                if (INPUT->GetButtonDown(KEY_TYPE::DOWN))
                {
                    AddSelectionTowards(1);
                    _keyRepeatTimer = 0.f;
                }
                else if (INPUT->GetButton(KEY_TYPE::DOWN))
                {
                    _keyRepeatTimer += DT;

                    if (_keyRepeatTimer >= _keyRepeatDelay)
                    {
                        AddSelectionTowards(1);
                        _keyRepeatTimer = 0.f;
                    }
                }
            }
            else
            {
                if (INPUT->GetButtonDown(KEY_TYPE::UP))
                {
                    MoveSelection(-1);
                    _keyRepeatTimer = 0.f;
                }
                else if (INPUT->GetButton(KEY_TYPE::UP))
                {
                    // 누르고 있을 때
                    _keyRepeatTimer += DT;
                    if (_keyRepeatTimer >= _keyRepeatDelay)
                    {
                        MoveSelection(-1);
                        _keyRepeatTimer = 0.f;
                    }
                }

                if (INPUT->GetButtonDown(KEY_TYPE::DOWN))
                {
                    MoveSelection(1);
                    _keyRepeatTimer = 0.f;
                }
                else if (INPUT->GetButton(KEY_TYPE::DOWN))
                {
                    _keyRepeatTimer += DT;
                    if (_keyRepeatTimer >= _keyRepeatDelay)
                    {
                        MoveSelection(1);
                        _keyRepeatTimer = 0.f;
                    }
                }
            }

            if (INPUT->GetButtonDown(KEY_TYPE::ENTER))
            {
                if (!_selectedObjects.empty())
                {
                    auto obj = _selectedObjects[0];

                    // 카메라 포커스 (더블클릭처럼)
                    auto sceneView = dynamic_pointer_cast<SceneView>(
                        GET_SINGLE(EditorManager)->GetWindow(L"Scene"));

                    if (sceneView && obj->GetTransform())
                    {
                        Vec3 targetPos = obj->GetTransform()->GetPosition();
                        sceneView->FocusOnPosition(targetPos);
                    }
                }
            }
        }

        // Ctrl+C: 복사
        if (INPUT->GetButton(KEY_TYPE::CTRL) && INPUT->GetButtonDown(KEY_TYPE::C))
        {
            _clipboard = _selectedObjects;
            LOG_INFO("Copied " + to_string(_clipboard.size()) + " objects");
        }

        // Ctrl+V: 붙여넣기
        if (INPUT->GetButton(KEY_TYPE::CTRL) && INPUT->GetButtonDown(KEY_TYPE::V))
        {
            ClearSelection();
            for (auto& original : _clipboard)
            {
                auto clone = original->Clone();
                if (clone)
                {
                    EVENTS->Publish(make_shared<GameObjectCreateEvent>(clone));
                    AddSelectedObjects(clone);
                }
            }
            LOG_INFO("Pasted " + to_string(_clipboard.size()) + " objects");
        }

        // Ctrl+D: 바로 복제
        if (INPUT->GetButton(KEY_TYPE::CTRL) && INPUT->GetButtonDown(KEY_TYPE::D))
        {
            vector<shared_ptr<GameObject>> clones;
            for (auto& original : _selectedObjects)
            {
                auto clone = original->Clone();
                if (clone)
                {
                    EVENTS->Publish(make_shared<GameObjectCreateEvent>(clone));
                    clones.push_back(clone);
                }
            }
            _selectedObjects = clones;
            LOG_INFO("Duplicated " + to_string(clones.size()) + " objects");
        }
    }
}

void HierarchyView::DrawToolbar()
{
    // Create Object 버튼
    if (ImGui::Button("Create Object"))
    {
        auto obj = make_shared<GameObject>();
        obj->GetOrAddTransform();
        EVENTS->Publish(make_shared<GameObjectCreateEvent>(obj));
    }

    ImGui::Separator();
}

void HierarchyView::DrawObjectList()
{
    // ─────────────────────────────────────────────
    // 오브젝트 목록 표시
    // ─────────────────────────────────────────────
    for (int i = 0; i < _sceneObjects.size(); i++)
    {
        auto& obj = _sceneObjects[i];
        if (!obj) continue;

        // 선택 상태 확인
        bool isSelected = IsSelected(obj);

        // 트리 노드 또는 선택 가능한 아이템으로 표시
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

        if (isSelected)
            flags |= ImGuiTreeNodeFlags_Selected;

        // 오브젝트 이름 (없으면 인덱스로 표시)
        string name = "GameObject_" + to_string(i);

        ImGui::TreeNodeEx((void*)(intptr_t)i, flags, name.c_str());

        // 클릭하면 선택
        if (ImGui::IsItemClicked())
        {
            if (INPUT->GetButton(KEY_TYPE::CTRL))
            {
                // Ctrl + 클릭: 토글
                if (IsSelected(obj))
                    RemoveSelectedObject(obj);
                else
                    AddSelectedObjects(obj);
            }
            else if (INPUT->GetButton(KEY_TYPE::SHIFT))
            {
                // Shift + 클릭 ex) 1 ~ 5번까지 전부 다 선택
                if (!_selectedObjects.empty())
                {
                    int startIndex = -1;

                    auto it = find(_sceneObjects.begin(), _sceneObjects.end(), _selectedObjects[0]);
                    if (it != _sceneObjects.end())
                    {
                        startIndex = (int)distance(_sceneObjects.begin(), it);
                    }

                    // 클릭한 오브젝트의 인덱스
                    int endIndex = i;

                    // 범위 내 전부 선택
                    int minIdx = min(startIndex, endIndex);
                    int maxIdx = max(startIndex, endIndex);

                    ClearSelection();
                    for (int i = minIdx; i <= maxIdx; i++)
                    {
                        AddSelectedObjects(_sceneObjects[i]);
                    }

                }
                else
                {
                    // 선택된 게 없으면 단일 선택
                    AddSelectedObjects(obj);
                }
            }
            else
            {
                // 일반 클릭: 단일 선택
                ClearSelection();
                AddSelectedObjects(obj);
            }

            // Ispector에 선택된 오브젝트를 전달
            auto inspector = dynamic_pointer_cast<InspectorView>(
                GET_SINGLE(EditorManager)->GetWindow(L"Inspector"));

            if (inspector && !_selectedObjects.empty())
                inspector->SetTarget(_selectedObjects[0]);
        }

        // === 더블 클릭 시 카메라 이동 ===
        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
        {
            // 카메라 Lerp 이동
            auto sceneView = dynamic_pointer_cast<SceneView>(
                GET_SINGLE(EditorManager)->GetWindow(L"Scene"));

            if (sceneView && obj->GetTransform())
            {
                Vec3 targetPos = obj->GetTransform()->GetPosition();
                sceneView->FocusOnPosition(targetPos);
            }
        }
    }
}

