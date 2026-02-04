#include "pch.h"
#include "HierarchyView.h"
#include "GameObject.h"
#include "InspectorView.h"
#include "EditorManager.h"
#include "SceneView.h"
#include "Utils.h"

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

            // [이름 중복 체크 및 번호 부여]
            auto obj = EnsureUniqueName(event);

            CUR_SCENE->Add_Scene(obj);
            LOG_WARNING("GameObject Created !");
        });

    // GameObject 삭제 이벤트 구독
    EVENTS->Subscribe(EventType::GameObject_Destroyed, [this](Event& e)
        {
            auto& event = static_cast<GameObjectDestroyedEvent&>(e);

            CUR_SCENE->Remove_Scene(event.GetGameObject());
            LOG_WARNING("GameObject Destroyed !");
        });
}

void HierarchyView::Update()
{
    
}

void HierarchyView::OnGui()
{
    string title = "Hierarchy";
    ImGui::Begin(title.c_str(), nullptr, ImGuiWindowFlags_NoNavInputs);

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

        if (INPUT->GetButtonDown(KEY_TYPE::DEL))
        {
            // 선택된 오브젝트들 복사 (삭제 중 순회 문제 방지)
            auto toDelete = _selectedObjects;

            for (auto obj : toDelete)
            {
                if (obj)
                {
                    EVENTS->Publish(make_shared<GameObjectDestroyedEvent>(obj));
                }
            }

            ClearSelection();

            // Inspector 초기화
            auto inspector = dynamic_pointer_cast<InspectorView>(
                GET_SINGLE(EditorManager)->GetWindow(L"Inspector"));
            if (inspector)
                inspector->SetTarget(nullptr);

            LOG_INFO("Deleted " + to_string(toDelete.size()) + " objects");
        }
    }
}

void HierarchyView::DrawToolbar()
{
    // Create Object 버튼
    if (ImGui::Button("Create Object"))
    {
        ImGui::OpenPopup("CreateObjectPopup");
    }

    if (ImGui::BeginPopup("CreateObjectPopup"))
    {
        // 검색창
        static char searchBuf[128] = "";
        ImGui::InputTextWithHint("##Sarch", "Search Class...", searchBuf, IM_ARRAYSIZE(searchBuf));
        ImGui::Separator();

        string searchStr = searchBuf;

        const auto& creators = GameObjectFactory::GetAllCreator();

        for (const auto& [name, creator] : creators)
        {
            // 검색어 필터링
            if (searchStr.empty() || name.find(searchStr) != string::npos)
            {
                if (ImGui::Selectable(name.c_str()))
                {
                    shared_ptr<GameObject> newObj = creator();
                    if (newObj)
                    {
                        // 이름 세팅
                        if (newObj->GetName() == L"GameObject")
                            newObj->SetName(Utils::ToWString(name));

                        newObj->GetOrAddTransform();

                        EVENTS->Publish(make_shared<GameObjectCreateEvent>(newObj));
                    }
                    ImGui::CloseCurrentPopup();
                }
            }
        }
        ImGui::EndPopup();
    }

    ImGui::Separator();
}

void HierarchyView::DrawObjectList()
{
    // 오브젝트 정렬
    sort(_sceneObjects.begin(), _sceneObjects.end(),
        [](shared_ptr<GameObject>& first, shared_ptr<GameObject>& second)
        {
            if (!first || !second)
                return false;

            return first->GetName() < second->GetName();
        });

    // ─────────────────────────────────────────────
    // 오브젝트 목록 표시
    // ─────────────────────────────────────────────

    // 검색창 만들기
    static char searchBuf[128] = "";
    ImGui::InputTextWithHint("##SearchHierarchy", "Search...", searchBuf, IM_ARRAYSIZE(searchBuf));
    ImGui::Separator();

    string searchStr = searchBuf; // string 변환
    int filteredCount = 0;

    // 미리 카운팅
    if (searchStr.empty())
    {
        filteredCount = _sceneObjects.size();
    }
    else
    {
        for (auto& obj : _sceneObjects)
        {
            string nameStr = Utils::ToString(obj->GetName());
            if (nameStr.find(searchStr) != string::npos)
                filteredCount++;
        }
    }

    ImGui::Text("Objects : %d / %d", filteredCount, _sceneObjects.size());
    ImGui::Separator();

    for (int i = 0; i < _sceneObjects.size(); i++)
    {
        auto& obj = _sceneObjects[i];
        if (!obj) continue;

        // 검색어 필터링
        string nameStr = Utils::ToString(obj->GetName());

        // 검색어가 비어있지 않고(""), 이름에 검색어가 포함되어 있지 않으면
        if (!searchStr.empty() && nameStr.find(searchStr) == string::npos)
        {
            continue; // 목록 안보여주기
        }

        // 선택 상태 확인
        bool isSelected = IsSelected(obj);

        // 트리 노드 또는 선택 가능한 아이템으로 표시
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

        if (isSelected)
            flags |= ImGuiTreeNodeFlags_Selected;

        //string name = "GameObject_" + to_string(i);
        string name = Utils::ToString(obj->GetName());
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

shared_ptr<GameObject> HierarchyView::EnsureUniqueName(GameObjectCreateEvent event)
{
    auto obj = event.GetGameObject();

    auto& sceneObjects = CUR_SCENE->GetObjects();
    wstring baseName = obj->GetName();
    wstring uniqueName = baseName;
    int count = 1;

    while (true)
    {
        bool isDuplicate = false;

        for (auto& sceneObj : sceneObjects)
        {
            if (sceneObj == obj) continue;

            if (sceneObj->GetName() == uniqueName)
            {
                isDuplicate = true;
                break;
            }
        }

        // 중복없으면 루프 탈출
        if (!isDuplicate)
            break;

        // 중복이면 번호 증가해서
        count++;
        uniqueName = baseName + L"_" + to_wstring(count);
    }

    obj->SetName(uniqueName);

    return obj;
}
