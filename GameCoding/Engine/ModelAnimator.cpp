#include "pch.h"
#include "ModelAnimator.h"
#include "Material.h"
#include "ModelMesh.h"
#include "Model.h"
#include "ModelAnimation.h"
#include "AnimNotifyManager.h"

REGISTER_COMPONENT(ModelAnimator, ComponentType::Animator);

ModelAnimator::ModelAnimator()
    : Super(ComponentType::Animator)
{

}

ModelAnimator::ModelAnimator(shared_ptr<Shader> shader)
    : Super(ComponentType::Animator), _shader(shader)
{
    // TEST
    //_tweenDesc.next.animIndex = rand() % 3;
    //_tweenDesc.tweenSumTime += rand() % 100;
}

ModelAnimator::~ModelAnimator()
{

}

//void ModelAnimator::Update()
//{
//    if (_model == nullptr)
//        return;
//
//    if (_texture == nullptr)
//        CreateTexture();
//
//    // Anim Update
//    ImGui::InputInt("AnimIndex", &_keyframeDesc.animIndex);
//    _keyframeDesc.animIndex %= _model->GetAnimationCount();
//    ImGui::InputInt("CurrFrame", (int*)&_keyframeDesc.currFrame);
//    _keyframeDesc.currFrame %= _model->GetAnimationByIndex(_keyframeDesc.animIndex)->frameCount;
//
//    // 애니메이션 현재 프레임 정보
//    RENDER->PushKeyframeData(_keyframeDesc);
//
//    // SRV를 통해 정보 전달
//    _shader->GetSRV("TransformMap")->SetResource(_srv.Get());
//
//    // Bones
//    BoneDesc boneDesc;
//
//    const uint32 boneCount = _model->GetBoneCount();
//    for (uint32 i = 0; i < boneCount; i++)
//    {
//        shared_ptr<ModelBone> bone = _model->GetBoneByIndex(i);
//        boneDesc.transforms[i] = bone->transform;
//    }
//    RENDER->PushBoneData(boneDesc);
//
//    // Transform
//    auto world = GetTransform()->GetWorldMatrix();
//    RENDER->PushTransformData(TransformDesc{ world });
//
//    const auto& meshes = _model->GetMeshes();
//    for (auto& mesh : meshes)
//    {
//        if (mesh->material)
//            mesh->material->Update();
//
//        // BoneIndex
//        _shader->GetScalar("BoneIndex")->SetInt(mesh->boneIndex);
//
//        uint32 stride = mesh->vertexBuffer->GetStride();
//        uint32 offset = mesh->vertexBuffer->GetOffset();
//
//        DC->IASetVertexBuffers(0, 1, mesh->vertexBuffer->GetComPtr().GetAddressOf(), &stride, &offset);
//        DC->IASetIndexBuffer(mesh->indexBuffer->GetComPtr().Get(), DXGI_FORMAT_R32_UINT, 0);
//
//        _shader->DrawIndexed(0, _pass, mesh->indexBuffer->GetCount(), 0, 0);
//    }
//}

//void ModelAnimator::Update()
//{
//    if (_model == nullptr)
//        return;
//
//    if (_texture == nullptr)
//        CreateTexture();
//
//    _keyframeDesc.sumTime += DT;
//
//    shared_ptr<ModelAnimation> current = _model->GetAnimationByIndex(_keyframeDesc.animIndex);
//    if (current)
//    {
//        float timePerFrame = 1 / (current->frameRate * _keyframeDesc.speed);
//        if (_keyframeDesc.sumTime >= timePerFrame)
//        {
//            _keyframeDesc.sumTime = 0.f;
//            _keyframeDesc.currFrame = (_keyframeDesc.currFrame + 1) % current->frameCount;
//            _keyframeDesc.nextFrame = (_keyframeDesc.currFrame + 1) % current->frameCount; // 이전거에 1더하기
//        }
//
//        _keyframeDesc.ratio = (_keyframeDesc.sumTime / timePerFrame);
//    }
//
//    // Anim Update
//    ImGui::InputInt("AnimIndex", &_keyframeDesc.animIndex);
//    _keyframeDesc.animIndex %= _model->GetAnimationCount();
//    ImGui::InputInt("CurrFrame", (int*)&_keyframeDesc.currFrame);
//
//    ImGui::InputFloat("Speed", &_keyframeDesc.speed, 0.5f, 4.f);
//
//    // 애니메이션 현재 프레임 정보
//    RENDER->PushKeyframeData(_keyframeDesc);
//
//    // SRV를 통해 정보 전달
//    _shader->GetSRV("TransformMap")->SetResource(_srv.Get());
//
//    // Bones
//    BoneDesc boneDesc;
//
//    const uint32 boneCount = _model->GetBoneCount();
//    for (uint32 i = 0; i < boneCount; i++)
//    {
//        shared_ptr<ModelBone> bone = _model->GetBoneByIndex(i);
//        boneDesc.transforms[i] = bone->transform;
//    }
//    RENDER->PushBoneData(boneDesc);
//
//    // Transform
//    auto world = GetTransform()->GetWorldMatrix();
//    RENDER->PushTransformData(TransformDesc{ world });
//
//    const auto& meshes = _model->GetMeshes();
//    for (auto& mesh : meshes)
//    {
//        if (mesh->material)
//            mesh->material->Update();
//
//        // BoneIndex
//        _shader->GetScalar("BoneIndex")->SetInt(mesh->boneIndex);
//
//        uint32 stride = mesh->vertexBuffer->GetStride();
//        uint32 offset = mesh->vertexBuffer->GetOffset();
//
//        DC->IASetVertexBuffers(0, 1, mesh->vertexBuffer->GetComPtr().GetAddressOf(), &stride, &offset);
//        DC->IASetIndexBuffer(mesh->indexBuffer->GetComPtr().Get(), DXGI_FORMAT_R32_UINT, 0);
//
//        _shader->DrawIndexed(0, _pass, mesh->indexBuffer->GetCount(), 0, 0);
//    }
//}

//void ModelAnimator::Update()
//{
//    if (_model == nullptr)
//        return;
//
//    if (_texture == nullptr)
//        CreateTexture();
//
//    TweenDesc& desc = _tweenDesc;
//
//    desc.curr.sumTime += DT;
//    // 현재 애니메이션
//    {
//        shared_ptr<ModelAnimation> currentAnim = _model->GetAnimationByIndex(desc.curr.animIndex);
//        if (currentAnim)
//        {
//            float timePerFrame = 1 / (currentAnim->frameRate * desc.curr.speed);
//            if (desc.curr.sumTime >= timePerFrame)
//            {
//                int prevFrame = desc.curr.currFrame;
//
//                desc.curr.sumTime = 0;
//                desc.curr.currFrame = (desc.curr.currFrame + 1) % currentAnim->frameCount;
//                desc.curr.nextFrame = (desc.curr.currFrame + 1) % currentAnim->frameCount;
//
//                CheckNotifies(prevFrame, desc.curr.currFrame);
//            }
//
//            desc.curr.ratio = (desc.curr.sumTime / timePerFrame);
//        }
//    }
//
//    // 다음 애니메이션이 예약돼 있다면
//    if (desc.next.animIndex >= 0)
//    {
//        desc.tweenSumTime += DT;
//        desc.tweenRatio = desc.tweenSumTime / desc.tweenDuration;
//
//        if (desc.tweenRatio >= 1.f)
//        {
//            // 애니메이션 교체 성공
//            desc.curr = desc.next;
//            desc.ClearNextAnim();
//        }
//        else
//        {
//            // 교체중
//            shared_ptr<ModelAnimation> nextAnim = _model->GetAnimationByIndex(desc.next.animIndex);
//            desc.next.sumTime += DT;
//
//            float timePerFrame = 1.f / (nextAnim->frameRate * desc.next.speed);
//
//            if (desc.next.ratio >= 1.f)
//            {
//                desc.next.sumTime = 0;
//
//                desc.next.currFrame = (desc.next.currFrame + 1) % nextAnim->frameCount;
//                desc.next.nextFrame = (desc.next.currFrame + 1) % nextAnim->frameCount;
//            }
//
//            desc.next.ratio = desc.next.sumTime / timePerFrame;
//        }
//    }
//
//    // Anim Update
//    //ImGui::InputInt("AnimIndex", &desc.curr.animIndex);
//    //desc.curr.animIndex %= _model->GetAnimationCount();
//    //ImGui::InputInt("CurrFrame", (int*)&desc.curr.currFrame);
//    //
//    //// 다른 애니메이션 선택
//    //static int32 nextAnimIndex = 0;
//    //if (ImGui::InputInt("NextAnimIndex", &nextAnimIndex))
//    //{
//    //    nextAnimIndex %= _model->GetAnimationCount();
//    //    desc.ClearNextAnim(); // 기존꺼 밀어주기
//    //    desc.next.animIndex = nextAnimIndex;
//    //}
//    //
//    //if (_model->GetAnimationCount() > 0)
//    //    desc.curr.animIndex %= _model->GetAnimationCount();
//    //
//    //ImGui::InputFloat("Speed", &desc.curr.speed, 0.5f, 4.f);
//
//    // 애니메이션 현재 프레임 정보
//    RENDER->PushTweenData(desc);
//
//    // SRV를 통해 정보 전달
//    _shader->GetSRV("TransformMap")->SetResource(_srv.Get());
//
//    // Bones
//    BoneDesc boneDesc;
//
//    const uint32 boneCount = _model->GetBoneCount();
//    for (uint32 i = 0; i < boneCount; i++)
//    {
//        shared_ptr<ModelBone> bone = _model->GetBoneByIndex(i);
//        boneDesc.transforms[i] = bone->transform;
//    }
//    RENDER->PushBoneData(boneDesc);
//
//    // Transform
//    auto world = GetTransform()->GetWorldMatrix();
//    RENDER->PushTransformData(TransformDesc{ world });
//
//    const auto& meshes = _model->GetMeshes();
//    for (auto& mesh : meshes)
//    {
//        if (mesh->material)
//            mesh->material->Update();
//
//        // BoneIndex
//        _shader->GetScalar("BoneIndex")->SetInt(mesh->boneIndex);
//
//        uint32 stride = mesh->vertexBuffer->GetStride();
//        uint32 offset = mesh->vertexBuffer->GetOffset();
//
//        ENGINE_DC->IASetVertexBuffers(0, 1, mesh->vertexBuffer->GetComPtr().GetAddressOf(), &stride, &offset);
//        ENGINE_DC->IASetIndexBuffer(mesh->indexBuffer->GetComPtr().Get(), DXGI_FORMAT_R32_UINT, 0);
//
//        _shader->DrawIndexed(0, _pass, mesh->indexBuffer->GetCount(), 0, 0);
//    }
//}

void ModelAnimator::SetModel(shared_ptr<Model> model)
{
    _model = model;

    const auto& materials = model->GetMaterials();
    for (auto& material : materials)
    {
        material->SetShader(_shader);
    }
}

void ModelAnimator::UpdateTweenData()
{
    TweenDesc& desc = _tweenDesc;

    desc.curr.sumTime += DT;
    // 현재 애니메이션
    {
        shared_ptr<ModelAnimation> currentAnim = _model->GetAnimationByIndex(desc.curr.animIndex);
        if (currentAnim)
        {
            float timePerFrame = 1 / (currentAnim->frameRate * desc.curr.speed);
            if (desc.curr.sumTime >= timePerFrame)
            {
                int prevFrame = desc.curr.currFrame;

                desc.curr.sumTime = 0;
                desc.curr.currFrame = (desc.curr.currFrame + 1) % currentAnim->frameCount;
                desc.curr.nextFrame = (desc.curr.currFrame + 1) % currentAnim->frameCount;

                CheckNotifies(prevFrame, desc.curr.currFrame);
            }

            desc.curr.ratio = (desc.curr.sumTime / timePerFrame);
        }
    }

    // 다음 애니메이션이 예약돼 있다면
    if (desc.next.animIndex >= 0)
    {
        desc.tweenSumTime += DT;
        desc.tweenRatio = desc.tweenSumTime / desc.tweenDuration;

        if (desc.tweenRatio >= 1.f)
        {
            // 애니메이션 교체 성공
            desc.curr = desc.next;
            desc.ClearNextAnim();
        }
        else
        {
            // 교체중
            shared_ptr<ModelAnimation> nextAnim = _model->GetAnimationByIndex(desc.next.animIndex);
            desc.next.sumTime += DT;

            float timePerFrame = 1.f / (nextAnim->frameRate * desc.next.speed);

            if (desc.next.ratio >= 1.f)
            {
                desc.next.sumTime = 0;

                desc.next.currFrame = (desc.next.currFrame + 1) % nextAnim->frameCount;
                desc.next.nextFrame = (desc.next.currFrame + 1) % nextAnim->frameCount;
            }

            desc.next.ratio = desc.next.sumTime / timePerFrame;
        }
    }
}

void ModelAnimator::Update()
{
    
}

void ModelAnimator::RenderInstancing(shared_ptr<InstancingBuffer>& buffer)
{
    if (_model == nullptr)
        return;

    if (_texture == nullptr)
        CreateTexture();

    // SRV를 통해 정보 전달
    _shader->GetSRV("TransformMap")->SetResource(_srv.Get());

    // Bones
    BoneDesc boneDesc;

    const uint32 boneCount = _model->GetBoneCount();
    for (uint32 i = 0; i < boneCount; i++)
    {
        shared_ptr<ModelBone> bone = _model->GetBoneByIndex(i);
        boneDesc.transforms[i] = bone->transform;
    }
    RENDER->PushBoneData(boneDesc);

    const auto& meshes = _model->GetMeshes();
    for (auto& mesh : meshes)
    {
        if (mesh->material)
            mesh->material->Update();

        // BoneIndex
        _shader->GetScalar("BoneIndex")->SetInt(mesh->boneIndex);

        mesh->vertexBuffer->PushData();
        mesh->indexBuffer->PushData();

        buffer->PushData();

        _shader->DrawIndexedInstanced(0, _pass, mesh->indexBuffer->GetCount(), buffer->GetCount());
    }
}

InstanceID ModelAnimator::GetInstanceID()
{
    return make_pair((uint64)_model.get(), (uint64)_shader.get());
}

void ModelAnimator::SetNextAnimation(int32 animIndex)
{
    if (_tweenDesc.next.animIndex >= 0)  // 이미 예약된 게 있으면
        return;

    _tweenDesc.next.animIndex = animIndex;
    _tweenDesc.next.currFrame = 0;
    _tweenDesc.next.nextFrame = 0;
    _tweenDesc.next.sumTime = 0;
    _tweenDesc.next.ratio = 0;
    _tweenDesc.tweenSumTime = 0;
    _tweenDesc.tweenRatio = 0;
}

bool ModelAnimator::IsPlaying(int32 animIndex)
{
    return _tweenDesc.curr.animIndex == animIndex && _tweenDesc.next.animIndex < 0;
}

bool ModelAnimator::IsAnimationEnd()
{
    if (_model == nullptr) return false;

    auto anim = _model->GetAnimationByIndex(_tweenDesc.curr.animIndex);
    if (anim == nullptr) return false;

    // 현재 프레임이 (전체 프레임 - 2) 이상이면 거의 끝난 것
    return _tweenDesc.curr.currFrame >= anim->frameCount - 2;
}

wstring ModelAnimator::GetCurrentAnimationName()
{
    if (!_model)
    {
        return L"";
    }

    auto anim = _model->GetAnimationByIndex(_tweenDesc.curr.animIndex);
    if (anim)
    {
        return anim->name;
    }

    return L"";
}

void ModelAnimator::CheckNotifies(int prevFrame, int currFrame)
{
    wstring animName = GetCurrentAnimationName();
    if (animName.empty())
        return;

    auto* container = GET_SINGLE(AnimNotifyManager)->GetContainer(animName);
    if (!container)
        return;

    // ─────────────────────────────────────────────
    // 단일 프레임 Notify 체크
    // ─────────────────────────────────────────────
    for (auto& notify : container->notifies)
    {
        bool triggered = false;
        if (prevFrame <= currFrame)
        {
            // 일반 진행
            triggered = (notify.frame > prevFrame && notify.frame <= currFrame);
        }
        else
        {
            // 루프 : prevFrame -> 끝 -> 0 -> currFrame
            triggered = (notify.frame > prevFrame || notify.frame <= currFrame);
        }

        if (triggered && _notifyCallback)
        {
            _notifyCallback(notify.name);
        }
    }

    // ─────────────────────────────────────────────
    // NotifyState 체크 (시작/종료)
    // ─────────────────────────────────────────────
    for (int i = 0; i < (int)container->notifyStates.size(); i++)
    {
        auto& state = container->notifyStates[i];
        bool wasActive = _activeNotifyStates.count(i) > 0;
        bool isActive = (currFrame >= state.startFrame && currFrame <= state.endFrame);

        if (isActive && !wasActive)
        {
            // 시작
            _activeNotifyStates.insert(i);
            if (_notifyStateCallback)
                _notifyStateCallback(state.name, true);
        }
        else if (!isActive && wasActive)
        {
            // 종료
            _activeNotifyStates.erase(i);
            if (_notifyStateCallback)
                _notifyStateCallback(state.name, false);
        }

    }

}

void ModelAnimator::CreateTexture()
{
    if (_model->GetAnimationCount() == 0)
        return;

    _animTransforms.resize(_model->GetAnimationCount());

    for (uint32 i = 0; i < _model->GetAnimationCount(); i++)
        CreateAnimationTransform(i);

    // Create Texture
    {
        D3D11_TEXTURE2D_DESC desc;
        ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
        desc.Width = MAX_MODEL_TRANSFORMS * 4;
        desc.Height = MAX_MODEL_KEYFRAMES;
        desc.ArraySize = _model->GetAnimationCount();
        desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; // 16바이트
        desc.Usage = D3D11_USAGE_IMMUTABLE;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        desc.MipLevels = 1;
        desc.SampleDesc.Count = 1;

        const uint32 dataSize = MAX_MODEL_TRANSFORMS * sizeof(Matrix);
        const uint32 pageSize = dataSize * MAX_MODEL_KEYFRAMES;
        void* mallocPtr = ::malloc(pageSize * _model->GetAnimationCount());

        // 파편화된 데이터를 조립한다.
        for (uint32 c = 0; c < _model->GetAnimationCount(); c++)
        {
            uint32 startOffset = c * pageSize;

            BYTE* pageStartPtr = reinterpret_cast<BYTE*>(mallocPtr) + startOffset;

            for (uint32 f = 0; f < MAX_MODEL_KEYFRAMES; f++)
            {
                void* ptr = pageStartPtr + dataSize * f;
                ::memcpy(ptr, _animTransforms[c].transforms[f].data(), dataSize);
            }
        }

        // 리소스 만들기
        vector<D3D11_SUBRESOURCE_DATA> subResources(_model->GetAnimationCount());

        for (uint32 c = 0; c < _model->GetAnimationCount(); c++)
        {
            void* ptr = (BYTE*)mallocPtr + c * pageSize;
            subResources[c].pSysMem = ptr;
            subResources[c].SysMemPitch = dataSize;
            subResources[c].SysMemSlicePitch = pageSize;
        }

        HRESULT hr = DEVICE->CreateTexture2D(&desc, subResources.data(), _texture.GetAddressOf());
        CHECK(hr);

        ::free(mallocPtr);
    }

    // Create SRV
    {
        D3D11_SHADER_RESOURCE_VIEW_DESC desc;
        ZeroMemory(&desc, sizeof(desc));
        desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
        desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
        desc.Texture2DArray.MipLevels = 1;
        desc.Texture2DArray.ArraySize = _model->GetAnimationCount();

        HRESULT hr = DEVICE->CreateShaderResourceView(_texture.Get(), &desc, _srv.GetAddressOf());
        CHECK(hr);
    }

}

void ModelAnimator::CreateAnimationTransform(uint32 index)
{
    vector<Matrix> tempAnimBoneTramsforms(MAX_MODEL_TRANSFORMS, Matrix::Identity);

    shared_ptr<ModelAnimation> animation = _model->GetAnimationByIndex(index);

    for (uint32 f = 0; f < animation->frameCount; f++)
    {
        for (uint32 b = 0; b < _model->GetBoneCount(); b++)
        {
            shared_ptr<ModelBone> bone = _model->GetBoneByIndex(b);

            Matrix matAnimation;

            shared_ptr<ModelKeyframe> frame = animation->GetKeyframe(bone->name);
            if (frame != nullptr)
            {
                ModelKeyframeData& data = frame->transforms[f];

                Matrix S, R, T;
                S = Matrix::CreateScale(data.scale.x, data.scale.y, data.scale.z);
                R = Matrix::CreateFromQuaternion(data.rotation);
                T = Matrix::CreateTranslation(data.translation.x, data.translation.y, data.translation.z);

                matAnimation = S * R * T;
            }
            else
            {
                matAnimation = Matrix::Identity;
            }

            // T Pose
            Matrix toRootmatrix = bone->transform;
            Matrix invGlobal = toRootmatrix.Invert();

            int32 parentIndex = bone->parentIndex;

            // Animation
            Matrix matParent = Matrix::Identity;
            if (parentIndex >= 0)
            {
                matParent = tempAnimBoneTramsforms[parentIndex];
            }

            tempAnimBoneTramsforms[b] = matAnimation * matParent;

            // 결론
            _animTransforms[index].transforms[f][b] = invGlobal * tempAnimBoneTramsforms[b];
        }
    }
}
