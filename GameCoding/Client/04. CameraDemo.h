#pragma once

#include "IExecute.h"
#include "Geometry.h"

class GameObject;

class CameraDemo : public IExecute
{
public:
    void Init() override;
    void Update() override;
    void Render() override;

    shared_ptr<Shader> _shader;

    // Object
    shared_ptr<Geometry<VertexColorData>> _geometry;
    shared_ptr<VertexBuffer> _vertexBuffer;
    shared_ptr<IndexBuffer> _indexBuffer;

    Matrix _world = Matrix::Identity;
    Matrix _view = Matrix::Identity;
    Matrix _projection = Matrix::Identity;

    // Camera
    shared_ptr<GameObject> _camera;
};

