#include "stdafx.h"
#include "TestFlyweight.h"

#include "../Objects/MeshCube.h"

#include "../LandScape/Sky.h"

TestFlyweight::TestFlyweight(ExecuteValues * values)
	:Execute(values)

{
	shader = new Shader(Shaders + L"033_Plane.hlsl");
	instanceShader = new Shader(Shaders + L"Homework/ModelInstance.hlsl", "VS", "PS", true);

	instanceCount = 1024;

	for (int i = 0; i < instanceCount; i++) {
		models.push_back(new MeshCube());
		models.back()->SetShader(shader);

		models.back()->Scale(
			Math::Random(0.5f, 15.0f),
			Math::Random(0.5f, 15.0f),
			Math::Random(0.5f, 15.0f));
		models.back()->RotationDegree(
			Math::Random(-180.0f, 180.0f),
			Math::Random(-180.0f, 180.0f),
			Math::Random(-180.0f, 180.0f));
		models.back()->Position(
			Math::Random(-200.0f, 200.0f),
			Math::Random(-100.0f, 100.0f),
			Math::Random(-200.0f, 200.0f));
		
		models.back()->Update();
		worlds.push_back(models.back()->BoneTransforms());
	}
	
	instance = new MeshCube();
	instance->SetShader(instanceShader);
	CreateInstanceBuffer();
	instance->SetInstance(instanceBuffer, instanceCount);
	instance->Update();
	instance->SetInstanceBuffer(worlds);

	bRotate = false;
	bInstance = false;

	sky = new Sky(values);
}

TestFlyweight::~TestFlyweight()
{
	SAFE_DELETE(shader);
	SAFE_DELETE(instanceShader);

	for (GameModel* model : models)
		SAFE_DELETE(model);

	SAFE_DELETE(instance);
	SAFE_RELEASE(instanceBuffer);
}

void TestFlyweight::Update()
{
	sky->Update();

	if (bInstance == false) {
		//for (GameModel* model : models)
		for (int i = 0; i < models.size(); i++)
			models[i]->Update();
	}
	else {
		instance->Update();
	}
}

void TestFlyweight::PreRender()
{

}

void TestFlyweight::Render()
{	
	sky->Render();

	ImGui::Checkbox("bInstance", &bInstance);

	//if (ImGui::Checkbox("bRotate", &bRotate)) {
	//	for (GameModel* model : models)
	//		model->SetAutoRotate(bRotate);
	//}

	int renderCall = 0;

	if (bInstance == false) {
		for (GameModel* model : models) {
			model->Render();
			renderCall++;
		}
	}
	else {
		instance->Render();
		renderCall++;
	}

	ImGui::Text("Render Call : %d", renderCall);
}

void TestFlyweight::PostRender()
{

}

void TestFlyweight::CreateInstanceBuffer()
{
	InstanceBuffer* instances = new InstanceBuffer[instanceCount];

	for (int i = 0; i < instanceCount; i++)
		instances[i].Id = i;
	
	// CreateInstanceBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT; // 어떻게 저장될지에 대한 정보
		desc.ByteWidth = sizeof(InstanceBuffer) * instanceCount; // 정점 버퍼에 들어갈 데이터의 크기
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA  data = { 0 }; // 얘를 통해서 값이 들어감 lock 대신
		data.pSysMem = instances; // 쓸 데이터의 주소
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;

		HRESULT hr = D3D::GetDevice()->CreateBuffer(
			&desc, &data, &instanceBuffer);
		assert(SUCCEEDED(hr)); // 성공되면 hr 0보다 큰 값 넘어옴
	}

	SAFE_DELETE_ARRAY(instances);
}




