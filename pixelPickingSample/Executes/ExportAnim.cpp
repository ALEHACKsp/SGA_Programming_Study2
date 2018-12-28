#include "stdafx.h"
#include "ExportAnim.h"
#include "../Fbx/Exporter.h"
#include "../Landscape/Terrain.h"
#include "../Landscape/Sky.h"
#include "./Objects/PickingObject.h"
#include "./Objects/GameModel.h"
#include "./Lights/PointLight.h"
#include "./Lights/SpotLight.h"

ExportAnim::ExportAnim(ExecuteValues * values)
	: Execute(values)
{
	Fbx::Exporter* exporter = NULL;
	
	////Kachujin Mesh
	//exporter = new Fbx::Exporter(Assets + L"Kachujin/Kachujin.Fbx");
	//exporter->ExportMaterial(Models + L"Kachujin/", L"Kachujin.material");
	//exporter->ExportMesh(Models + L"Kachujin/", L"Kachujin.mesh");
	//SAFE_DELETE(exporter);
	//
	////Samba_Dancing
	//exporter = new Fbx::Exporter(Assets + L"Kachujin/Samba_Dancing.Fbx");
	//exporter->ExportAnimation(Models + L"Kachujin/", L"Samba_Dance.anim", 0);
	//SAFE_DELETE(exporter);
	//
	////Taunt
	//exporter = new Fbx::Exporter(Assets + L"Kachujin/Taunt.Fbx");
	//exporter->ExportAnimation(Models + L"Kachujin/", L"Taunt.anim", 0);
	//SAFE_DELETE(exporter);
	//
	////Running
	//exporter = new Fbx::Exporter(Assets + L"Kachujin/Running.Fbx");
	//exporter->ExportAnimation(Models + L"Kachujin/", L"Running.anim", 0);
	//SAFE_DELETE(exporter);
	//
	////Paladin Mesh
	//exporter = new Fbx::Exporter(Assets + L"Paladin/Paladin.Fbx");
	//exporter->ExportMaterial(Models + L"Paladin/", L"Paladin.material");
	//exporter->ExportMesh(Models + L"Paladin/", L"Paladin.mesh");
	//SAFE_DELETE(exporter);
	//
	////Idle
	//exporter = new Fbx::Exporter(Assets + L"Paladin/Idle.Fbx");
	//exporter->ExportAnimation(Models + L"Paladin/", L"Idle.anim", 0);
	//SAFE_DELETE(exporter);
	//
	////Running
	//exporter = new Fbx::Exporter(Assets + L"Paladin/Running.Fbx");
	//exporter->ExportAnimation(Models + L"Paladin/", L"Running.anim", 0);
	//SAFE_DELETE(exporter);
	//
	////Slash
	//exporter = new Fbx::Exporter(Assets + L"Paladin/Slash.Fbx");
	//exporter->ExportAnimation(Models + L"Paladin/", L"Slash.anim", 0);
	//SAFE_DELETE(exporter);
	//
	////Hit
	//exporter = new Fbx::Exporter(Assets + L"Paladin/Hit.Fbx");
	//exporter->ExportAnimation(Models + L"Paladin/", L"Hit.anim", 0);
	//SAFE_DELETE(exporter);
	//
	////Dying
	//exporter = new Fbx::Exporter(Assets + L"Paladin/Dying.Fbx");
	//exporter->ExportAnimation(Models + L"Paladin/", L"Dying.anim", 0);
	//SAFE_DELETE(exporter);
	//
	////Vanguard Mesh
	//exporter = new Fbx::Exporter(Assets + L"Vanguard/Vanguard.Fbx");
	//exporter->ExportMaterial(Models + L"Vanguard/", L"Vanguard.material");
	//exporter->ExportMesh(Models + L"Vanguard/", L"Vanguard.mesh");
	//SAFE_DELETE(exporter);
	//
	////Idle
	//exporter = new Fbx::Exporter(Assets + L"Vanguard/Idle.Fbx");
	//exporter->ExportAnimation(Models + L"Vanguard/", L"Idle.anim", 0);
	//SAFE_DELETE(exporter);
	//
	////Hit
	//exporter = new Fbx::Exporter(Assets + L"Vanguard/Hit.Fbx");
	//exporter->ExportAnimation(Models + L"Vanguard/", L"Hit.anim", 0);
	//SAFE_DELETE(exporter);
	//
	////Punch
	//exporter = new Fbx::Exporter(Assets + L"Vanguard/Punch.Fbx");
	//exporter->ExportAnimation(Models + L"Vanguard/", L"Punch.anim", 0);
	//SAFE_DELETE(exporter);
	//
	////Dying
	//exporter = new Fbx::Exporter(Assets + L"Vanguard/Dying.Fbx");
	//exporter->ExportAnimation(Models + L"Vanguard/", L"Dying.anim", 0);
	//SAFE_DELETE(exporter);
	//
	////Walking
	//exporter = new Fbx::Exporter(Assets + L"Vanguard/Walking.Fbx");
	//exporter->ExportAnimation(Models + L"Vanguard/", L"Walking.anim", 0);
	//SAFE_DELETE(exporter);

	//SwatRifleRun
	exporter = new Fbx::Exporter(Assets + L"Swat/RunForward.Fbx");
	exporter->ExportAnimation(Models + L"Swat/", L"RunForward.anim", 0);
	SAFE_DELETE(exporter);
	exporter = new Fbx::Exporter(Assets + L"Swat/RunForwardLeft.Fbx");
	exporter->ExportAnimation(Models + L"Swat/", L"RunForwardLeft.anim", 0);
	SAFE_DELETE(exporter);
	exporter = new Fbx::Exporter(Assets + L"Swat/RunForwardRight.Fbx");
	exporter->ExportAnimation(Models + L"Swat/", L"RunForwardRight.anim", 0);
	SAFE_DELETE(exporter);
	exporter = new Fbx::Exporter(Assets + L"Swat/RunLeft.Fbx");
	exporter->ExportAnimation(Models + L"Swat/", L"RunLeft.anim", 0);
	SAFE_DELETE(exporter);
	exporter = new Fbx::Exporter(Assets + L"Swat/RunRight.Fbx");
	exporter->ExportAnimation(Models + L"Swat/", L"RunRight.anim", 0);
	SAFE_DELETE(exporter);
	exporter = new Fbx::Exporter(Assets + L"Swat/RunBackward.Fbx");
	exporter->ExportAnimation(Models + L"Swat/", L"RunBackward.anim", 0);
	SAFE_DELETE(exporter);
	exporter = new Fbx::Exporter(Assets + L"Swat/RunBackwardLeft.Fbx");
	exporter->ExportAnimation(Models + L"Swat/", L"RunBackwardLeft.anim", 0);
	SAFE_DELETE(exporter);
	exporter = new Fbx::Exporter(Assets + L"Swat/RunBackwardRight.Fbx");
	exporter->ExportAnimation(Models + L"Swat/", L"RunBackwardRight.anim", 0);
	SAFE_DELETE(exporter);
	exporter = new Fbx::Exporter(Assets + L"Swat/Reload.Fbx");
	exporter->ExportAnimation(Models + L"Swat/", L"Reload.anim", 0);
	SAFE_DELETE(exporter);
	exporter = new Fbx::Exporter(Assets + L"Swat/Idle.Fbx");
	exporter->ExportAnimation(Models + L"Swat/", L"Idle.anim", 0);
	SAFE_DELETE(exporter);
	exporter = new Fbx::Exporter(Assets + L"Swat/HitReaction.Fbx");
	exporter->ExportAnimation(Models + L"Swat/", L"HitReaction.anim", 0);
	SAFE_DELETE(exporter);
	exporter = new Fbx::Exporter(Assets + L"Swat/FiringRifle.Fbx");
	exporter->ExportAnimation(Models + L"Swat/", L"FiringRifle.anim", 0);
	SAFE_DELETE(exporter);
	exporter = new Fbx::Exporter(Assets + L"Swat/Dying.Fbx");
	exporter->ExportAnimation(Models + L"Swat/", L"Dying.anim", 0);
	SAFE_DELETE(exporter);
}

ExportAnim::~ExportAnim()
{
}

void ExportAnim::Update()
{
	
}

void ExportAnim::PreRender()
{
}

void ExportAnim::Render()
{
}

void ExportAnim::PostRender()
{
}

void ExportAnim::ResizeScreen()
{
}
