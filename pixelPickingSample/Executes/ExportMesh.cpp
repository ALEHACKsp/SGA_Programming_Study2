#include "stdafx.h"
#include "ExportMesh.h"
#include "../Fbx/Exporter.h"

ExportMesh::ExportMesh(ExecuteValues * values)
	: Execute(values)
{
	Fbx::Exporter* exporter = NULL;
	
	////Capsule
	//exporter = new Fbx::Exporter(Assets + L"Meshes/Capsule.Fbx");
	//exporter->ExportMaterial(Models + L"Capsule/", L"Capsule.material");
	//exporter->ExportMesh(Models + L"Capsule/", L"Capsule.mesh");
	//SAFE_DELETE(exporter);
	//
	////Cube
	//exporter = new Fbx::Exporter(Assets + L"Meshes/Cube.Fbx");
	//exporter->ExportMaterial(Models + L"Cube/", L"Cube.material");
	//exporter->ExportMesh(Models + L"Cube/", L"Cube.mesh");
	//SAFE_DELETE(exporter);
	//
	////Cylinder
	//exporter = new Fbx::Exporter(Assets + L"Meshes/Cylinder.Fbx");
	//exporter->ExportMaterial(Models + L"Cylinder/", L"Cylinder.material");
	//exporter->ExportMesh(Models + L"Cylinder/", L"Cylinder.mesh");
	//SAFE_DELETE(exporter);
	//
	////Plane
	//exporter = new Fbx::Exporter(Assets + L"Meshes/Plane.Fbx");
	//exporter->ExportMaterial(Models + L"Plane/", L"Plane.material");
	//exporter->ExportMesh(Models + L"Plane/", L"Plane.mesh");
	//SAFE_DELETE(exporter);
	//
	////Quad
	//exporter = new Fbx::Exporter(Assets + L"Meshes/Quad.Fbx");
	//exporter->ExportMaterial(Models + L"Quad/", L"Quad.material");
	//exporter->ExportMesh(Models + L"Quad/", L"Quad.mesh");
	//SAFE_DELETE(exporter);
	//
	////Sphere
	//exporter = new Fbx::Exporter(Assets + L"Meshes/Sphere.Fbx");
	//exporter->ExportMaterial(Models + L"Sphere/", L"Sphere.material");
	//exporter->ExportMesh(Models + L"Sphere/", L"Sphere.mesh");
	//SAFE_DELETE(exporter);
	//
	////Teapot
	//exporter = new Fbx::Exporter(Assets + L"Meshes/Teapot.Fbx");
	//exporter->ExportMaterial(Models + L"Teapot/", L"Teapot.material");
	//exporter->ExportMesh(Models + L"Teapot/", L"Teapot.mesh");
	//SAFE_DELETE(exporter);
	//
	////Tank
	//exporter = new Fbx::Exporter(Assets + L"Tank/Tank.Fbx");
	//exporter->ExportMaterial(Models + L"Tank/", L"Tank.material");
	//exporter->ExportMesh(Models + L"Tank/", L"Tank.mesh");
	//SAFE_DELETE(exporter);

	////StanfordBunny
	//exporter = new Fbx::Exporter(Assets + L"StanfordBunny/stanford-bunny.Fbx");
	//exporter->ExportMaterial(Models + L"StanfordBunny/", L"StanfordBunny.material");
	//exporter->ExportMesh(Models + L"StanfordBunny/", L"StanfordBunny.mesh");
	//SAFE_DELETE(exporter);

	////Swat
	//exporter = new Fbx::Exporter(Assets + L"Swat/Swat.Fbx");
	//exporter->ExportMaterial(Models + L"Swat/", L"Swat.material");
	//exporter->ExportMesh(Models + L"Swat/", L"Swat.mesh");
	//SAFE_DELETE(exporter);

	//AK
	exporter = new Fbx::Exporter(Assets + L"AK/AK.Fbx");
	exporter->ExportMaterial(Models + L"AK/", L"AK.material");
	exporter->ExportMesh(Models + L"AK/", L"AK.mesh");
	SAFE_DELETE(exporter);
}

ExportMesh::~ExportMesh()
{
}

void ExportMesh::Update()
{
	
}

void ExportMesh::PreRender()
{
}

void ExportMesh::Render()
{
}

void ExportMesh::PostRender()
{
}

void ExportMesh::ResizeScreen()
{
}
