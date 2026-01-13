#include "pch.h"
#include "InstancingManager.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "ModelRenderer.h"
#include "ModelAnimation.h"
#include "ModelAnimator.h"

void InstancingManager::Render(vector<shared_ptr<GameObject>>& gameObjects)
{
    ClearData();

    RenderMeshRenderer(gameObjects);
    RenderModelRenderer(gameObjects);
    RenderAnimRenderer(gameObjects);
}

void InstancingManager::ClearData()
{
    for (auto& pair : _buffers)
    {
        /* shared_ptr은 & 로 가져오면 주소를 가져오는 것이기 때문에 레퍼런스 카운트가 증가하지 않는다.  */
        shared_ptr<InstancingBuffer>& buffer = pair.second;
        buffer->ClearData();
    }
}

void InstancingManager::RenderMeshRenderer(vector<shared_ptr<GameObject>>& gameObjects)
{
    map<InstanceID, vector<shared_ptr<GameObject>>> cache;

    // 분류 단계
    for (shared_ptr<GameObject>& gameObject : gameObjects)
    {
        if (gameObject == nullptr) continue;

        if (gameObject->GetMeshRenderer() == nullptr)
            continue;

        const InstanceID instanceId = gameObject->GetMeshRenderer()->GetInstanceID();
        cache[instanceId].push_back(gameObject);
    }

    for (auto& pair : cache)
    {
        const vector<shared_ptr<GameObject>>& vec = pair.second;

        /*if (vec.size() == 1)
        {
            
        }
        else*/
        {
            const InstanceID instanceId = pair.first;

            for (int32 i = 0; i < vec.size(); i++)
            {
                const shared_ptr<GameObject>& gameObject = vec[i];
                InstancingData data;
                data.world = gameObject->GetTransform()->GetWorldMatrix();

                AddData(instanceId, data);
            }

            // 그리기 [하나만 Draw하는 것이 중요하다]
            shared_ptr<InstancingBuffer>& buffer = _buffers[instanceId];
            vec[0]->GetMeshRenderer()->RenderInstancing(buffer);
        }

        
    }
}

void InstancingManager::RenderModelRenderer(vector<shared_ptr<GameObject>>& gameObjects)
{
    map<InstanceID, vector<shared_ptr<GameObject>>> cache;

    // 분류 단계
    for (shared_ptr<GameObject>& gameObject : gameObjects)
    {
        if (gameObject == nullptr) continue;

        if (gameObject->GetModelRenderer() == nullptr)
            continue;

        const InstanceID instanceId = gameObject->GetModelRenderer()->GetInstanceID();
        cache[instanceId].push_back(gameObject);
    }

    for (auto& pair : cache)
    {
        const vector<shared_ptr<GameObject>>& vec = pair.second;

        /*if (vec.size() == 1)
        {

        }
        else*/
        {
            const InstanceID instanceId = pair.first;

            for (int32 i = 0; i < vec.size(); i++)
            {
                const shared_ptr<GameObject>& gameObject = vec[i];
                InstancingData data;
                data.world = gameObject->GetTransform()->GetWorldMatrix();

                AddData(instanceId, data);
            }

            // 그리기 [하나만 Draw하는 것이 중요하다]
            shared_ptr<InstancingBuffer>& buffer = _buffers[instanceId];
            vec[0]->GetModelRenderer()->RenderInstancing(buffer);
        }
    }
}

void InstancingManager::RenderAnimRenderer(vector<shared_ptr<GameObject>>& gameObjects)
{
    map<InstanceID, vector<shared_ptr<GameObject>>> cache;

    // 분류 단계
    for (shared_ptr<GameObject>& gameObject : gameObjects)
    {
        if (gameObject == nullptr) continue;

        if (gameObject->GetModelAnimator() == nullptr)
            continue;

        const InstanceID instanceId = gameObject->GetModelAnimator()->GetInstanceID();
        cache[instanceId].push_back(gameObject);
    }

    for (auto& pair : cache)
    {
        shared_ptr<InstancedTweenDesc> tweenDesc = make_shared<InstancedTweenDesc>();

        const vector<shared_ptr<GameObject>>& vec = pair.second;

        /*if (vec.size() == 1)
        {

        }
        else*/
        {
            const InstanceID instanceId = pair.first;

            for (int32 i = 0; i < vec.size(); i++)
            {
                const shared_ptr<GameObject>& gameObject = vec[i];
                InstancingData data;
                data.world = gameObject->GetTransform()->GetWorldMatrix();

                AddData(instanceId, data);

                // Instancing
                gameObject->GetModelAnimator()->UpdateTweenData();
                tweenDesc->tweens[i] = gameObject->GetModelAnimator()->GetTweenDesc();;
            }

            RENDER->PushTweenData(*tweenDesc.get());

            // 그리기 [하나만 Draw하는 것이 중요하다]
            shared_ptr<InstancingBuffer>& buffer = _buffers[instanceId];
            vec[0]->GetModelAnimator()->RenderInstancing(buffer);
        }
    }
}

void InstancingManager::AddData(InstanceID instanceId, InstancingData& data)
{
    if (_buffers.find(instanceId) == _buffers.end())
        _buffers[instanceId] = make_shared<InstancingBuffer>();

    _buffers[instanceId]->AddData(data);
}
