#pragma once

#include "../Viewer/Perspective.h"
#include "../Viewer/Viewport.h"
#include "../Viewer/Camera.h"

// ���α׷� ������ ���� �� �־��
struct ExecuteValues
{
	class ViewProjectionBuffer* ViewProjection;

	class Perspective* Perspective;
	class Viewport* Viewport;
	class Camera* mainCamera; // ���⿡ ���� ī�޶� ���� ī�޶�
};

class Execute
{
public:
	Execute(ExecuteValues* values) 
		: values(values)
	{
	}
	
	// ��� �߻� Ŭ�������� �Ҹ����� ��� virtual �Ƚᵵ �������� �̷����
	virtual ~Execute() {} // virtual ���ϸ� ȣ�� �ȵ� protected �־

	virtual void Update() = 0;
	
	virtual void PreRender() = 0;
	virtual void Render() = 0;
	virtual void PostRender() = 0;

	virtual void ResizeScreen() = 0;

#if CASE
#else
	//virtual void Position(D3DXVECTOR3 * vec) = 0;
	virtual D3DXVECTOR3* GetPosition() = 0;
#endif

protected:
	ExecuteValues* values;
};