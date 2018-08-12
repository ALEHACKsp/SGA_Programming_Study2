#include "stdafx.h"
#include "GameModel.h"

GameModel::GameModel(wstring matFolder, wstring matFile, 
	wstring meshFolder, wstring meshFile)
	: velocity(0,0,0)
{
	model = new Model();
	model->ReadMaterial(matFolder, matFile);
	model->ReadMesh(meshFolder, meshFile);

	shader = new Shader(Shaders + L"./Homework/Model.hlsl");

	for (Material* material : model->Materials())
		material->SetShader(shader);

	D3DXMatrixIdentity(&matRotation);

	renderBuffer = new RenderBuffer();

	//turretBone = model->BoneByName(L"turret_geo");
}

GameModel::GameModel(wstring name, Model* model, ExecuteValues* values,
	bool isRectCollider, bool isControl)
	: velocity(0,0,0)
	, name(name), isRectCollider(isRectCollider)
	, isControl(isControl)
{
	this->model = model;

	shader = new Shader(Shaders + L"./Homework/Model.hlsl");

	for (Material* material : model->Materials())
		material->SetShader(shader);

	D3DXMatrixIdentity(&matRotation);

	renderBuffer = new RenderBuffer();

	if (isRectCollider == false)
		collider = new CircleCollider(values);
	else
		collider = new RectCollider(values);

	D3DXVECTOR3 min, max;
	model->MinMaxVertex(&min, &max);
	D3DXVECTOR3 center = (min + max) / 2.0f;
	D3DXVECTOR3 temp = max - center;

	min.y = 0;
	max.y = 0;
	D3DXVECTOR3 halfSize;
	if (isRectCollider)
		halfSize = (max - min) / 2.0f;
	else
	{
		// 임시적으로 크기 min max 를 통해 circle collider의 radius을 구해봣음
		// 현재 문제점은 크기 변환 시 크기에 대응해서 circle collider가 늘어나지 않음
		temp.y = 0;
		float maxTemp = max(temp.x, max(temp.y, temp.z));
		halfSize = { 
			cosf(D3DX_PI / 4.0f) * maxTemp, 0, 
			sinf(D3DX_PI / 4.0f) * maxTemp };
	}

	if(isRectCollider)
		((RectCollider*)collider)->SetBound(&center, &halfSize);
	else
		((CircleCollider*)collider)->SetBound(&center, &halfSize);
}

GameModel::~GameModel()
{
	SAFE_DELETE(collider);

	SAFE_DELETE(renderBuffer);

	SAFE_DELETE(shader);
	SAFE_DELETE(model);
}

void GameModel::Velocity(D3DXVECTOR3 & vec)
{
	velocity = vec;
}

D3DXVECTOR3 GameModel::Velocity()
{
	return velocity;
}

void GameModel::Update()
{
	CalcPosition();

	//D3DXMATRIX R;
	//D3DXMatrixRotationY(&R, sinf(Time::Get()->Running() * 2.0f) *0.25f);
	//
	//D3DXMATRIX L = turretBone->Local();
	//turretBone->Local(L * R);

	if (isControl)
	{
		D3DXVECTOR3 position = Position();
		float moveSpeed = 10.0f;
		float rotateSpeed = 60.0f;

		D3DXVECTOR3 forward, right;
		forward = Direction();
		right = Right();

		if (!Mouse::Get()->Press(1)) {
			if (Keyboard::Get()->Press('W'))
				position += -forward * moveSpeed * Time::Delta();
			else if (Keyboard::Get()->Press('S'))
				position += forward * moveSpeed * Time::Delta();

			if (Keyboard::Get()->Press('A'))
				position += right * moveSpeed * Time::Delta();
			else if (Keyboard::Get()->Press('D'))
				position += -right * moveSpeed * Time::Delta();

			if (Keyboard::Get()->Press('E'))
				Rotate(D3DXVECTOR2(rotateSpeed * Time::Delta(), 0));
			else if (Keyboard::Get()->Press('Q'))
				Rotate(D3DXVECTOR2(-rotateSpeed * Time::Delta(), 0));

			Position(position);
		}
	}

	D3DXMATRIX t = Transformed();
	model->CopyGlobalBoneTo(boneTransforms, t);
}

void GameModel::Render()
{
	model->Buffer()->Bones(&boneTransforms[0], boneTransforms.size());
	model->Buffer()->SetVSBuffer(2);

	for (ModelMesh* mesh : model->Meshes())
	{
		// bone index
		int index = mesh->ParentBoneIndex();

		renderBuffer->Data.Index = index;
		renderBuffer->SetVSBuffer(3);

		mesh->Render();
	}

	if(isRectCollider)
		((RectCollider*)collider)->Render(&Transformed());
	else
		((CircleCollider*)collider)->Render(&Transformed());
}

void GameModel::Rotate(D3DXVECTOR2 amount)
{
	//amount *= Time::Delta();

	// x, y 뒤집어둠 카메라 움직임때랑 같은 이유
	D3DXMATRIX axis;
	D3DXMatrixRotationAxis(&axis, &Right(), amount.y);

	D3DXMATRIX Y;
	D3DXMatrixRotationY(&Y, Math::ToRadian(amount.x));

	matRotation = axis * Y;

	D3DXMATRIX R = World();
	World(matRotation * R);
}

void GameModel::CalcPosition()
{
	if (D3DXVec3Length(&velocity) <= 0.0f)
		return;


	D3DXVECTOR3 vec(0, 0, 0);
	if (velocity.z != 0.0f)
		vec += Direction() * velocity.z;

	if (velocity.y != 0.0f)
		vec += Up() * velocity.y;

	if (velocity.x != 0.0f)
		vec += Right() * velocity.x;

	D3DXVECTOR3 pos = Position() + vec * Time::Delta();
	Position(pos);
}
