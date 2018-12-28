#include "stdafx.h"
#include "PickingObject.h"
#include "./Objects/GameModel.h"
#include "./Physics/RayCollider.h"
#include "./Physics/SphereCollider.h"
#include "./Utilities/Gizmo.h"

PickingObject::PickingObject(ExecuteValues * values)
	: values(values), pickedModel(NULL), pickedPart(NULL), zFar(255)
{
}

PickingObject::~PickingObject()
{
}

bool PickingObject::Picking(vector<GameModel*>& objects)
{
	GameModel* tempPick = NULL;
	float nearObjDistance = INFINITY;
	RayCollider* ray = values->MainCamera->ConvertMouseToRay(values->Viewport, values->Perspective, Mouse::Get()->GetPosition());
	SphereCollider* sphere = NULL;
	for (GameModel* object : objects)
	{
		if (object->GetCollider() == NULL)
		{
			SAFE_DELETE(sphere);
			sphere = new SphereCollider(object);
		}

		float distance;
		if (Manifold::Solve(ray, object->GetCollider(), &distance))
		{
			if (distance < nearObjDistance)
			{
				nearObjDistance = distance;
				tempPick = object;
			}
		}
	}

	SAFE_DELETE(ray);
	SAFE_DELETE(sphere);

	if (tempPick != NULL)
	{
		ChangePickedObj(tempPick);
		return true;
	}
	else return false;
}

void PickingObject::MultiPickingMouseDown(D3DXVECTOR3 & mousePosition)
{
	UnPicking();
	mousePosTemp[0] = mousePosition;
}

bool PickingObject::MultiPickingMouseUp(D3DXVECTOR3 & mousePosition, vector<GameModel*>& objects)
{
	if (mousePosTemp[0] == mousePosition)
		return Picking(objects);
	mousePosTemp[1] = mousePosition;

	MakeFrustum();

	for (GameModel* object : objects)
	{
		if (ContainPoint(object->Position()))
		{
			//TODO:기즈모 색처리
			pickedModels.push_back(object);
		}
	}

	if (pickedModels.size() > 0)
		return true;

	return false;
}

void PickingObject::UnPicking()
{
	//언피킹 처리
	if (pickedModel != NULL)
	{
		//TODO:기즈모 색처리
		pickedModel = NULL;
	}
	for (GameModel* picked : pickedModels)
	{
		//TODO:기즈모 색처리
	}
	pickedModels.clear();
}

bool PickingObject::PickingBone()
{
	if (pickedModel == NULL) return false;

	ModelBone* tempPick = NULL;
	float nearObjDistance = INFINITY;

	//피킹 레이
	D3DXVECTOR3 start;
	values->MainCamera->Position(&start);

	D3DXVECTOR3 direction = values->MainCamera->Direction(values->Viewport, values->Perspective);

	D3DXMATRIX invWorld;
	D3DXMatrixInverse(&invWorld, NULL, &pickedModel->World());
	
	D3DXVec3TransformCoord(&start, &start, &invWorld);
	D3DXVec3TransformNormal(&direction, &direction, &invWorld);
	D3DXVec3Normalize(&direction, &direction);

	//충돌검사용 본의 위치 정보
	vector<D3DXMATRIX>* boneTransforms = pickedModel->GetBoneTransforms();
	for (UINT i = 0; i < pickedModel->GetModel()->Bones().size(); ++i)
	{
		D3DXMATRIX matrix = pickedModel->GetModel()->Bones()[i]->Global() * (*boneTransforms)[i];
		//검사 수행
		D3DXVECTOR3 pointVec = D3DXVECTOR3(matrix._41, matrix._42, matrix._43) - start;
		D3DXVec3Cross(&pointVec, &pointVec, &direction);
		float distance = D3DXVec3Length(&pointVec) / D3DXVec3Length(&direction);

		float dot = D3DXVec3Dot(&pointVec, &direction);
		if (dot > 0) {
			distance = D3DXVec3Length(&D3DXVECTOR3(pointVec - (dot / D3DXVec3Dot(&direction, &direction)) * direction));
		}
		else {
			distance = D3DXVec3Length(&pointVec);
		}

		if (distance < 15.0f)
		{
			float length = D3DXVec3Length(&pointVec);
			if (length < nearObjDistance)
			{
				nearObjDistance = length;
				tempPick = pickedModel->GetModel()->BoneByIndex(i);
			}
		}
	}

	if (tempPick != NULL)
	{
		this->pickedPart = tempPick;
		return true;
	}
	else return false;
}

void PickingObject::ChangePickedObj(GameModel* object)
{
	if(this->pickedModel != NULL)
		//TODO:기즈모 색처리
	this->pickedModel = object;
	//TODO:기즈모 색처리
}

void PickingObject::MakeFrustum()
{
	float left, top, right, bottom;

	left = (mousePosTemp[0].x < mousePosTemp[1].x) ? mousePosTemp[0].x : mousePosTemp[1].x;
	top = (mousePosTemp[0].y < mousePosTemp[1].y) ? mousePosTemp[0].y : mousePosTemp[1].y;
	right = (mousePosTemp[0].x > mousePosTemp[1].x) ? mousePosTemp[0].x : mousePosTemp[1].x;
	bottom = (mousePosTemp[0].y > mousePosTemp[1].y) ? mousePosTemp[0].y : mousePosTemp[1].y;

	D3DXVECTOR3 leftTop, rightTop, leftBottom, rightBottom;
	leftTop = values->MainCamera->Direction(values->Viewport, values->Perspective, D3DXVECTOR3(left, top, 0.0f));
	rightTop = values->MainCamera->Direction(values->Viewport, values->Perspective, D3DXVECTOR3(right, top, 0.0f));
	leftBottom = values->MainCamera->Direction(values->Viewport, values->Perspective, D3DXVECTOR3(left, bottom, 0.0f));
	rightBottom = values->MainCamera->Direction(values->Viewport, values->Perspective, D3DXVECTOR3(right, bottom, 0.0f));

	D3DXVECTOR3 camPos;
	values->MainCamera->Position(&camPos);

	D3DXVECTOR3 farLeftBottom = leftBottom * zFar + camPos;
	D3DXVECTOR3 farRightBottom = rightBottom * zFar + camPos;
	D3DXVECTOR3 farLeftTop = leftTop * zFar + camPos;

	leftTop += camPos;
	rightTop += camPos;
	leftBottom += camPos;
	rightBottom += camPos;

	//Make Frustum
	//D3DXPlaneFromPoints(리턴값, 첫벡터, 중앙벡터, 두번재벡터) 시계방향으로 normal벡터 생성됨 순서에 따라 면의 방향이 바뀐다.

	//left top right bottom
	D3DXPlaneFromPoints(&planes[0], &leftBottom, &camPos, &leftTop);
	D3DXPlaneFromPoints(&planes[1], &leftTop, &camPos, &rightTop);
	D3DXPlaneFromPoints(&planes[2], &rightTop, &camPos, &rightBottom);
	D3DXPlaneFromPoints(&planes[3], &rightBottom, &camPos, &leftBottom);

	//near far
	D3DXPlaneFromPoints(&planes[4], &leftTop, &leftBottom, &rightTop);
	D3DXPlaneFromPoints(&planes[5], &farRightBottom, &farLeftBottom, &farLeftTop);
}

bool PickingObject::ContainPoint(D3DXVECTOR3& point)
{
	for (int i = 0; i < 6; i++)
	{
		if (D3DXPlaneDotCoord(&planes[i], &point) < 0.0f)
			return false;
	}
	return true;
}