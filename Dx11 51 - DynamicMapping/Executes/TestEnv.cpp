#include "stdafx.h"
#include "TestEnv.h"

#include "./Environment/CubeSky.h"
#include "./Viewer/Perspective.h"

TestEnv::TestEnv()
{
	BuildCubeTexture(300, 300);
	BuildGeometry();
	BuildViewMatrix(0, 3.48f, 0); // bunny의 위치

	render2D = new Render2D();
	render2D->Position(0, 0);
	render2D->Scale(300, 300);

	sphereDist = 5.0f;

	bContext = false;
}

TestEnv::~TestEnv()
{
	SAFE_DELETE(plane);
	SAFE_DELETE(planeMaterial);

	SAFE_DELETE(box);
	SAFE_DELETE(boxMaterial);

	SAFE_DELETE(quad);
	SAFE_DELETE(quadMaterial);

	for (int i = 0; i < 10; i++)
		SAFE_DELETE(cylinder[i]);
	SAFE_DELETE(cylinderMaterial);

	for (int i = 0; i < 10; i++)
		SAFE_DELETE(sphere[i]);
	SAFE_DELETE(sphereMaterial);

	SAFE_DELETE(sphere2);

	SAFE_DELETE(bunny);

	SAFE_DELETE(sky);

	SAFE_DELETE(dsv);
	SAFE_DELETE(viewport);

	SAFE_DELETE(render2D);

	SAFE_DELETE(perspective);
}

void TestEnv::Update()
{
	sky->Update();
	bunny->Update();

	sphere2->Position(
		sinf(Time::Get()->Running()) * sphereDist,
		3.48f, 
		cosf(Time::Get()->Running()) * sphereDist);
}

void TestEnv::PreRender()
{
	viewport->RSSetViewport();

	D3DXMATRIX proj;
	perspective->GetMatrix(&proj);

	#pragma region 모든 Context의 View Proj 변경
	if (bContext) {

		Context::Get()->SetProjection(proj);

		for (int i = 0; i < 6; i++)
		{
			Context::Get()->SetView(view[i]);

			D3D::Get()->SetRenderTarget(rtv[i], dsv->DSV());
			D3D::Get()->Clear(D3DXCOLOR(1, 1, 1, 1), rtv[i], dsv->DSV());

			sky->Render();

			plane->Render();
			box->Render();

			sphere2->Render();

			for (int i = 0; i < 10; i++)
				cylinder[i]->Render();

			for (int i = 0; i < 10; i++)
				sphere[i]->Render();

			bunny->Render();
		}

		Context::Get()->UpdatePerFrame();
		Context::Get()->UpdateProjection();

	}
	#pragma endregion

	#pragma region 모든 Shader의 Techinque 변경 (View2 Projection2) 넣어주기
	else 
	{

		sky->GetMaterial()->GetShader()->AsMatrix("Projection2")->SetMatrix(proj);
		planeMaterial->GetShader()->AsMatrix("Projection2")->SetMatrix(proj);
		boxMaterial->GetShader()->AsMatrix("Projection2")->SetMatrix(proj);
		planeMaterial->GetShader()->AsMatrix("Projection2")->SetMatrix(proj);
		cylinderMaterial->GetShader()->AsMatrix("Projection2")->SetMatrix(proj);
		sphereMaterial->GetShader()->AsMatrix("Projection2")->SetMatrix(proj);
		for (Material* material : bunny->GetModel()->Materials())
			material->GetShader()->AsMatrix("Projection2")->SetMatrix(proj);

		for (int i = 0; i < 6; i++)
		{
			D3D::Get()->SetRenderTarget(rtv[i], dsv->DSV());
			D3D::Get()->Clear(D3DXCOLOR(1, 1, 1, 1), rtv[i], dsv->DSV());

			sky->GetMaterial()->GetShader()->AsMatrix("View2")->SetMatrix(view[i]);
			planeMaterial->GetShader()->AsMatrix("View2")->SetMatrix(view[i]);
			boxMaterial->GetShader()->AsMatrix("View2")->SetMatrix(view[i]);
			planeMaterial->GetShader()->AsMatrix("View2")->SetMatrix(view[i]);
			cylinderMaterial->GetShader()->AsMatrix("View2")->SetMatrix(view[i]);
			sphereMaterial->GetShader()->AsMatrix("View2")->SetMatrix(view[i]);
			for (Material* material : bunny->GetModel()->Materials())
				material->GetShader()->AsMatrix("View2")->SetMatrix(view[i]);

			sky->PreRender();

			plane->PreRender();
			box->PreRender();

			sphere2->PreRender();

			for (int i = 0; i < 10; i++)
				cylinder[i]->PreRender();

			for (int i = 0; i < 10; i++)
				sphere[i]->PreRender();

			bunny->PreRender();
		}

	}
	#pragma endregion

	D3D::GetDC()->GenerateMips(srv);
}

void TestEnv::Render()
{
	ImGui::Checkbox("Context Change Way", &bContext);
	D3DXVECTOR3 scale = bunny->Scale();
	if (ImGui::DragFloat3("Scale", (float*)&scale, 0.1f))
		bunny->Scale(scale);
	ImGui::DragFloat("Sphere Dist", &sphereDist, 0.1f);

	sky->Render();

	plane->Render();
	box->Render();

	sphere2->Render();

	for (int i = 0; i < 10; i++)
		cylinder[i]->Render();

	for (int i = 0; i < 10; i++)
		sphere[i]->Render();

	bunny->Render();

	//quadMaterial->GetShader()->AsShaderResource("DiffuseMap")->SetResource(rtv->SRV());
	//quad->Render();

	//render2D->Render();
}

void TestEnv::PostRender()
{

}

void TestEnv::BuildGeometry()
{
	//sky = new CubeSky(Textures + L"sunsetcube1024.dds");
	sky = new CubeSky(Textures + L"snowcube1024.dds");
	//sky = new CubeSky(Textures + L"grasscube1024.dds");
	//sky = new CubeSky(Textures + L"SkyCube.png");

	floor = Textures + L"Floor.png";
	stone = Textures + L"Stones.png";
	brick = Textures + L"Bricks.png";

	wstring shaderFile = Shaders + L"055_Mesh.fx";

	boxMaterial = new Material(shaderFile);
	boxMaterial->SetDiffuseMap(stone);
	boxMaterial->SetAmbient(1.0f, 1.0f, 1.0f);
	boxMaterial->SetDiffuse(1.0f, 1.0f, 1.0f);
	boxMaterial->SetSpecular(0.8f, 0.8f, 0.8f, 16.0f);

	box = new MeshCube(boxMaterial, 1, 1, 1);
	box->Position(0, 0.5f, 0);
	box->Scale(5.0f, 1.0f, 5.0f);

	planeMaterial = new Material(shaderFile);
	planeMaterial->SetDiffuseMap(floor);
	planeMaterial->SetAmbient(0.8f, 0.8f, 0.8f);
	planeMaterial->SetDiffuse(0.8f, 0.8f, 0.8f);
	planeMaterial->SetSpecular(0.0f, 0.0f, 0.0f, 1.0f);

	plane = new MeshGrid(planeMaterial, 60, 40, 20, 30, 3, 3);

	quadMaterial = new Material(Shaders + L"052_Mesh.fx");
	quadMaterial->SetDiffuseMap(floor);
	quadMaterial->SetAmbient(0.8f, 0.8f, 0.8f);
	quadMaterial->SetDiffuse(0.8f, 0.8f, 0.8f);
	quadMaterial->SetSpecular(0.0f, 0.0f, 0.0f, 1.0f);

	quad = new MeshQuad(quadMaterial);
	quad->Scale(5, 3, 1);
	quad->Position(0, 1.5f, -5.0f);

	cylinderMaterial = new Material(shaderFile);
	cylinderMaterial->SetDiffuseMap(brick);
	cylinderMaterial->SetAmbient(1.0f, 1.0f, 1.0f);
	cylinderMaterial->SetDiffuse(1.0f, 1.0f, 1.0f);
	cylinderMaterial->SetSpecular(0.8f, 0.8f, 0.5f, 16.0f);

	sphereMaterial = new Material(shaderFile);
	sphereMaterial->SetDiffuseMap(stone);
	sphereMaterial->SetAmbient(0.6f, 0.8f, 0.9f);
	sphereMaterial->SetDiffuse(0.6f, 0.8f, 0.9f);
	sphereMaterial->SetSpecular(0.9f, 0.9f, 0.9f, 16.0f);

	for (UINT i = 0; i < 5; i++)
	{
		cylinder[i * 2] = new MeshCylinder(cylinderMaterial, 0.5f, 0.3f, 3.0f, 20, 20);
		cylinder[i * 2]->Position(-5.0f, 1.5f, -10.0f + i * 5.0f);

		cylinder[i * 2 + 1] = new MeshCylinder(cylinderMaterial, 0.5f, 0.3f, 3.0f, 20, 20);
		cylinder[i * 2 + 1]->Position(5.0f, 1.5f, -10.0f + i * 5.0f);

		sphere[i * 2] = new MeshSphere(sphereMaterial, 0.5f, 20, 20);
		sphere[i * 2]->Position(-5.0f, 3.5f, -10.0f + i * 5.0f);

		sphere[i * 2 + 1] = new MeshSphere(sphereMaterial, 0.5f, 20, 20);
		sphere[i * 2 + 1]->Position(5.0f, 3.5f, -10.0f + i * 5.0f);
	}

	sphere2 = new MeshSphere(sphereMaterial, 1.0f, 20, 20);
	sphere2->Position(0, 0, 0);

	bunny = new GameModel
	(
		Shaders + L"057_DynamicCube.fx",
		Models + L"Meshes/", L"Sphere.material",
		Models + L"Meshes/", L"Sphere.mesh"
	);
	bunny->Position(0, 3.48f, 0);
	//bunny->Scale(0.01f, 0.01f, 0.01f);

	bunny->SetDiffuseMap(Textures + L"Stones.png");
	bunny->SetNormalMap(Textures + L"Mozaic.jpg");
	bunny->SetDiffuse(0.8f, 0.8f, 0.8f);
	bunny->SetSpecular(0.8f, 0.8f, 0.8f, 16.0f);

	for (Material* material : bunny->GetModel()->Materials())
		material->GetShader()->AsShaderResource("CubeMap")->SetResource(srv);

	for (ModelMesh* mesh : bunny->GetModel()->Meshes())
		mesh->Pass(1);

	SpotLightDesc spotLight;
	spotLight.Ambient = D3DXCOLOR(0, 0, 0, 1);
	spotLight.Diffuse = D3DXCOLOR(1, 0, 0, 1);
	spotLight.Specular = D3DXCOLOR(1, 0, 0, 1);
	spotLight.Attenuation = D3DXVECTOR3(1, 0, 0);
	spotLight.Spot = 20.0f;
	spotLight.Position = D3DXVECTOR3(0, 10, 0);
	spotLight.Direction = D3DXVECTOR3(0, -1, 0);

	Context::Get()->AddSpotLight(spotLight);
}

void TestEnv::BuildCubeTexture(UINT width, UINT height)
{
	DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// Texture
	{
		D3D11_TEXTURE2D_DESC desc;
		desc.Width = width;
		desc.Height = height;
		desc.MipLevels = 0;
		desc.ArraySize = 6;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Format = format;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS | D3D11_RESOURCE_MISC_TEXTURECUBE;

		HRESULT hr = D3D::GetDevice()->CreateTexture2D(&desc, NULL, &cubeTexture);
		assert(SUCCEEDED(hr));
	}

	// RTV
	{
		D3D11_RENDER_TARGET_VIEW_DESC desc;
		desc.Format = format;
		desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
		desc.Texture2DArray.ArraySize = 1;
		desc.Texture2DArray.MipSlice = 0;

		for (int i = 0; i < 6; i++) 
		{
			desc.Texture2DArray.FirstArraySlice = i;

			HRESULT hr = D3D::GetDevice()->CreateRenderTargetView(cubeTexture, &desc, &rtv[i]);
			assert(SUCCEEDED(hr));
		}
	}

	// SRV
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		desc.Format = format;
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		desc.TextureCube.MostDetailedMip = 0; // mipmap 몇 개까지 최대 지원할지
		desc.TextureCube.MipLevels = -1;

		HRESULT hr = D3D::GetDevice()->CreateShaderResourceView(cubeTexture, &desc, &srv);
		assert(SUCCEEDED(hr));
	}

	// stencil 안써서 깊이만 32bit 쓰겠다는거 원래는 d24 s8 임
	dsv = new DepthStencilView(width, height, DXGI_FORMAT_D32_FLOAT);
	viewport = new Viewport(width, height);
}

void TestEnv::BuildViewMatrix(float x, float y, float z)
{
	D3DXVECTOR3 target[] = 
	{
		D3DXVECTOR3(x + 1, y, z),
		D3DXVECTOR3(x - 1, y, z),
		D3DXVECTOR3(x, y + 1, z),
		D3DXVECTOR3(x, y - 1, z),
		D3DXVECTOR3(x, y, z + 1),
		D3DXVECTOR3(x, y, z - 1),
	};

	D3DXVECTOR3 up[] =
	{
		D3DXVECTOR3(0, 1, 0),
		D3DXVECTOR3(0, 1, 0),
		D3DXVECTOR3(0, 0, -1),
		D3DXVECTOR3(0, 0, 1),
		D3DXVECTOR3(0, 1, 0),
		D3DXVECTOR3(0, 1, 0)
	};

	D3DXVECTOR3 position(x, y, z);

	for (int i = 0; i < 6; i++)
		D3DXMatrixLookAtLH(&view[i], &position, &target[i], &up[i]);

	perspective = new Perspective(1, 1, Math::PI * 0.5f, 0.1f, 1000.0f);
}
