#include "stdafx.h"
#include "Gizmo.h"

#include "./Draw/MeshQuad.h"

#include "Renders\Gizmo\GizmoModel.h"
#include "Renders\Gizmo\GizmoAxis.h"

Gizmo::Gizmo(ExecuteValues* values)
	: values(values)
	, lineOffset(1.0f), lineLength(3.0f), singleAxisThickness(0.2f), multiAxisThickness(0.05f)
	, bLocal(false)
{
	halfLineOffset = lineOffset / 2;

	#pragma region 충돌 박스
	axisBox[(int)Axis::X] = new BBox(
		D3DXVECTOR3(lineOffset, 0, 0),
		D3DXVECTOR3(lineOffset + lineLength, singleAxisThickness, singleAxisThickness));
	axisBox[(int)Axis::Y] = new BBox(
		D3DXVECTOR3(0, lineOffset, 0),
		D3DXVECTOR3(singleAxisThickness, lineOffset + lineLength, singleAxisThickness));
	axisBox[(int)Axis::Z] = new BBox(
		D3DXVECTOR3(0, 0, lineOffset),
		D3DXVECTOR3(singleAxisThickness, singleAxisThickness, lineOffset + lineLength));
	axisBox[(int)Axis::XZ] = new BBox(
		D3DXVECTOR3(0, 0, 0),
		D3DXVECTOR3(lineOffset, multiAxisThickness, lineOffset));
	axisBox[(int)Axis::XY] = new BBox(
		D3DXVECTOR3(0, 0, 0),
		D3DXVECTOR3(lineOffset, lineOffset, multiAxisThickness));
	axisBox[(int)Axis::YZ] = new BBox(
		D3DXVECTOR3(0, 0, 0),
		D3DXVECTOR3(multiAxisThickness, lineOffset, lineOffset));

	// 충돌 박스를 그릴 선들
	for (int i = 0; i < 6; i++)
		axis[i] = new DebugLine();
	#pragma endregion
	
	#pragma region 충돌 스피어
	radius = 1.0f;
	scaleFactor = 25;

	axisSphere[(int)Axis::X] = new BSphere(D3DXVECTOR3(lineLength, 0, 0), radius);
	axisSphere[(int)Axis::Y] = new BSphere(D3DXVECTOR3(0, lineLength, 0), radius);
	axisSphere[(int)Axis::Z] = new BSphere(D3DXVECTOR3(0, 0, lineLength), radius);

	#pragma endregion

	#pragma region X,Y,Z축 선
	axiss.push_back(new GizmoAxis(GizmoAxis::Kind::X, lineOffset, halfLineOffset, lineLength));
	axiss.push_back(new GizmoAxis(GizmoAxis::Kind::Y, lineOffset, halfLineOffset, lineLength));
	axiss.push_back(new GizmoAxis(GizmoAxis::Kind::Z, lineOffset, halfLineOffset, lineLength));

	axisLine = new DebugLine();

	// 각 축 및 선택된 선 색깔
	axisColor[(int)Axis::X] = D3DXCOLOR(1, 0, 0, 1);
	axisColor[(int)Axis::Y] = D3DXCOLOR(0, 1, 0, 1);
	axisColor[(int)Axis::Z] = D3DXCOLOR(0, 0, 1, 1);
	highlightColor = D3DXCOLOR(1, 1, 0, 1);
	#pragma endregion

	#pragma region Model & Quad
	//for (int i = 0; i < 3; i++) {
	//	model[0][i] = new GizmoModel(GizmoModel::Kind::Translate);
	//	model[1][i] = new GizmoModel(GizmoModel::Kind::Rotate);
	//	model[2][i] = new GizmoModel(GizmoModel::Kind::Scale);
	//	model[3][i] = new GizmoModel(GizmoModel::Kind::Scale);
	//}
	//
	//for (int i = 0; i < 4; i++) {
	//	model[i][(int)Axis::X]->RootAxis(D3DXVECTOR3(0, 90, 0));
	//	model[i][(int)Axis::Y]->RootAxis(D3DXVECTOR3(-90, 0, 0));
	//}

	for (int i = 0; i < 3; i++)
	{
		translateModel[i] = new GizmoModel(GizmoModel::Kind::Translate);
		rotateModel[i] = new GizmoModel(GizmoModel::Kind::Rotate);
		scaleModel[i] = new GizmoModel(GizmoModel::Kind::Scale);
		uniformScaleModel[i] = new GizmoModel(GizmoModel::Kind::Scale);
	}

	translateModel[(int)Axis::X]->RootAxis(D3DXVECTOR3(0, 90, 0));
	rotateModel[(int)Axis::X]->RootAxis(D3DXVECTOR3(0, 90, 0));
	scaleModel[(int)Axis::X]->RootAxis(D3DXVECTOR3(0, 90, 0));

	translateModel[(int)Axis::Y]->RootAxis(D3DXVECTOR3(-90, 0, 0));
	rotateModel[(int)Axis::Y]->RootAxis(D3DXVECTOR3(-90, 0, 0));
	scaleModel[(int)Axis::Y]->RootAxis(D3DXVECTOR3(-90, 0, 0));

	translateModel[(int)Axis::X]->SetColor(axisColor[(int)Axis::X]);
	rotateModel[(int)Axis::X]->SetColor(axisColor[(int)Axis::X]);
	scaleModel[(int)Axis::X]->SetColor(axisColor[(int)Axis::X]);

	translateModel[(int)Axis::Y]->SetColor(axisColor[(int)Axis::Y]);
	rotateModel[(int)Axis::Y]->SetColor(axisColor[(int)Axis::Y]);
	scaleModel[(int)Axis::Y]->SetColor(axisColor[(int)Axis::Y]);

	translateModel[(int)Axis::Z]->SetColor(axisColor[(int)Axis::Z]);
	rotateModel[(int)Axis::Z]->SetColor(axisColor[(int)Axis::Z]);
	scaleModel[(int)Axis::Z]->SetColor(axisColor[(int)Axis::Z]);
	
	uniformScaleModel[(int)Axis::X]->SetColor(axisColor[(int)Axis::X]);
	uniformScaleModel[(int)Axis::Y]->SetColor(axisColor[(int)Axis::X]);
	uniformScaleModel[(int)Axis::Z]->SetColor(axisColor[(int)Axis::X]);


	meshMaterial = new Material(Shaders + L"Homework/Mesh3.hlsl");
	meshMaterial->SetDiffuseMap(Textures + L"White.png");

	// XZ
	quads[0] = new MeshQuad(
		meshMaterial,
		D3DXVECTOR3(halfLineOffset, 0, halfLineOffset),
		D3DXVECTOR3(0, 1, 0), D3DXVECTOR3(1, 0, 0), lineOffset, lineOffset);

	// XY
	quads[1] = new MeshQuad(
		meshMaterial,
		D3DXVECTOR3(halfLineOffset, halfLineOffset, 0),
		D3DXVECTOR3(0, 0, -1), D3DXVECTOR3(0, 1, 0), lineOffset, lineOffset);

	// YZ
	quads[2] = new MeshQuad(
		meshMaterial,
		D3DXVECTOR3(0, halfLineOffset, halfLineOffset),
		D3DXVECTOR3(1, 0, 0), D3DXVECTOR3(0, 1, 0), lineOffset, lineOffset);
	#pragma endregion

	#pragma region Mode 및 Setting
	visible = false;
	enable = false;

	blendMode[0] = new BlendState();
	blendMode[1] = new BlendState();
	blendMode[1]->BlendEnable(true);
	blendMode[1]->SrcBlend(D3D11_BLEND_ONE);
	blendMode[1]->SrcBlendAlpha(D3D11_BLEND_ONE);
	blendMode[1]->DestBlend(D3D11_BLEND_ZERO);
	blendMode[1]->DestBlendAlpha(D3D11_BLEND_ZERO);

	cullMode[0] = new RasterizerState();
	cullMode[1] = new RasterizerState();
	cullMode[1]->CullMode(D3D11_CULL_NONE);
	
	depthMode[0] = new DepthStencilState();
	depthMode[1] = new DepthStencilState();
	depthMode[1]->DepthEnable(false);

	activeAxis = Axis::None;
	activeMode = Mode::Translate;

	ResetDeltas();

	delta = D3DXVECTOR3(0, 0, 0);

	translateDelta = D3DXVECTOR3(0, 0, 0);
	scaleDelta = D3DXVECTOR3(0, 0, 0);
	rotationDelta = D3DXVECTOR3(0, 0, 0);

	UpdateLine();

	D3DXMatrixIdentity(&rotationMatrix);

	render = NULL;

	snapEnable = false;
	precisionModeEnable = false;
	translationSnapValue = 1.0f;
	rotationSnapValue = 30;
	scaleSnapValue = 0.5f;
	
	precisionModeScale = 0.1f;
	
	translationScaleSnapDelta = D3DXVECTOR3(0, 0, 0);
	rotationSnapDelta = 0;

	#pragma endregion
}

Gizmo::~Gizmo()
{
	for (int i = 0; i < 6; i++)
		SAFE_DELETE(axisBox[i]);
	for (int i = 0; i < 3; i++)
		SAFE_DELETE(axisSphere[i]);

	SAFE_DELETE(axisLine);
	for (ILine* line : axiss)
		SAFE_DELETE(line);

	SAFE_DELETE(blendMode[0]);
	SAFE_DELETE(blendMode[1]);

	SAFE_DELETE(cullMode[0]);
	SAFE_DELETE(cullMode[1]);

	SAFE_DELETE(depthMode[0]);
	SAFE_DELETE(depthMode[1]);

	for (int i = 0; i < 6; i++)
		SAFE_DELETE(axis[i]);

	for (int i = 0; i < 3; i++)
		SAFE_DELETE(quads[i]);

	SAFE_DELETE(meshMaterial);

	for (int i = 0; i < 3; i++) {
		//SAFE_DELETE(model[0][i]);
		//SAFE_DELETE(model[1][i]);
		//SAFE_DELETE(model[2][i]);
		//SAFE_DELETE(model[3][i]);

		SAFE_DELETE(translateModel[i]);
		SAFE_DELETE(rotateModel[i]);
		SAFE_DELETE(scaleModel[i]);
		SAFE_DELETE(uniformScaleModel[i]);
	}
}

void Gizmo::Set(GameRender* render)
{
	this->render = render;

	Position(render->Position());
	if (bLocal) {
		Rotation(render->Rotation());
		D3DXVECTOR3 rot = render->Rotation();
		D3DXMatrixRotationYawPitchRoll(&rotationMatrix, rot.y, rot.x, rot.z);
	}
	else {
		Rotation(0, 0, 0);
		D3DXMatrixIdentity(&rotationMatrix);
	}

	UpdateLine();

	ResetDeltas();
}

bool Gizmo::Intersect(Ray * ray)
{
	float result = 0;
	bool check = false;

	for (int i = 0; i < 6; i++)
		check |= axisBox[i]->Intersect(ray, World(), result);

	return check;
}

void Gizmo::Update(Ray * ray)
{
	lastIntersectionPosition = intersectPosition;

	if (Keyboard::Get()->Down('1')) activeMode = Mode::Translate;
	if (Keyboard::Get()->Down('2')) activeMode = Mode::Rotate;
	if (Keyboard::Get()->Down('3')) activeMode = Mode::NonUniformScale;
	if (Keyboard::Get()->Down('4')) activeMode = Mode::UniformScale;
	if (Keyboard::Get()->Down('5')) {
		bLocal = !bLocal;
		Set(render);
	}

	if (Mouse::Get()->Press(0) && activeAxis != Axis::None) {

		switch (activeMode)
		{
		case Gizmo::Mode::UniformScale:
		case Gizmo::Mode::NonUniformScale:
		case Gizmo::Mode::Translate:
		{
			#pragma region Translate & Scale
			delta = D3DXVECTOR3(0, 0, 0);

			switch (activeAxis)
			{
			case Gizmo::Axis::XY:
			case Gizmo::Axis::X:
			{
				D3DXVECTOR3 pos = Position();
				if (bLocal)
					D3DXVec3TransformCoord(&pos, &pos, &rotationMatrix);

				D3DXPLANE plane = D3DXPLANE(0, 0, 1, pos.z);

				D3DXPlaneIntersectLine(&intersectPosition,
					&plane, &ray->Position, &(ray->Position + ray->Direction * ray->Distance));

				if (lastIntersectionPosition != D3DXVECTOR3(0, 0, 0))
				{
					tDelta = intersectPosition - lastIntersectionPosition;
				}

				delta = activeAxis == Axis::X 
					? D3DXVECTOR3(tDelta.x, 0, 0) 
					: D3DXVECTOR3(tDelta.x, tDelta.y, 0);
			}
				break;
			case Gizmo::Axis::Z:
			case Gizmo::Axis::YZ:
			case Gizmo::Axis::Y:
			{
				D3DXVECTOR3 pos = Position();
				if (bLocal)
					D3DXVec3TransformCoord(&pos, &pos, &rotationMatrix);

				D3DXPLANE plane = D3DXPLANE(-1, 0, 0, pos.x);

				D3DXPlaneIntersectLine(&intersectPosition,
					&plane, &ray->Position, &(ray->Position + ray->Direction * ray->Distance));

				if (lastIntersectionPosition != D3DXVECTOR3(0, 0, 0))
				{
					tDelta = intersectPosition - lastIntersectionPosition;
				}

				switch (activeAxis)
				{
				case Gizmo::Axis::Y:
					delta = D3DXVECTOR3(0, tDelta.y, 0);
					break;
				case Gizmo::Axis::Z:
					delta = D3DXVECTOR3(0, 0, tDelta.z);
					break;
				default:
					delta = D3DXVECTOR3(0, tDelta.y, tDelta.z);
					break;
				}
			}
				break;
			case Gizmo::Axis::XZ:
			{
				D3DXVECTOR3 pos = Position();
				if (bLocal)
					D3DXVec3TransformCoord(&pos, &pos, &rotationMatrix);

				D3DXPLANE plane = D3DXPLANE(0, -1, 0, pos.y);

				D3DXPlaneIntersectLine(&intersectPosition,
					&plane, &ray->Position, &(ray->Position + ray->Direction * ray->Distance));

				if (lastIntersectionPosition != D3DXVECTOR3(0, 0, 0))
				{
					tDelta = intersectPosition - lastIntersectionPosition;
				}

				delta = D3DXVECTOR3(tDelta.x, 0, tDelta.z);
			}
				break;
			}

			// Snap
			if (snapEnable)
			{
				float snapValue = translationSnapValue;
				if (activeMode == Mode::UniformScale || activeMode == Mode::NonUniformScale)
					snapValue = scaleSnapValue;
				if (precisionModeEnable)
				{
					delta *= precisionModeScale;
					snapValue *= precisionModeScale;
				}

				translationScaleSnapDelta += delta;

				delta = D3DXVECTOR3(
					(int)(translationScaleSnapDelta.x / snapValue) * snapValue,
					(int)(translationScaleSnapDelta.y / snapValue) * snapValue,
					(int)(translationScaleSnapDelta.z / snapValue) * snapValue);

				translationScaleSnapDelta -= delta;
			}
			else if (precisionModeEnable)
				delta *= precisionModeScale;

			if (activeMode == Mode::Translate)
			{
				// transform (local or world)
				if (bLocal)
					D3DXVec3TransformCoord(&delta, &delta, &rotationMatrix);
				translateDelta = delta;
			}
			else if (activeMode == Mode::NonUniformScale || activeMode == Mode::UniformScale)
			{
				///
				scaleDelta += delta;

			}
			#pragma endregion
		}
			break;
		case Gizmo::Mode::Rotate:
		{
			#pragma region Rotate
			delta = D3DXVECTOR3(0, 0, 0);

			switch (activeAxis)
			{
			case Gizmo::Axis::X:
			{

				D3DXPLANE plane = D3DXPLANE(0, 0, 1, Position().z);

				D3DXPlaneIntersectLine(&intersectPosition,
					&plane, &ray->Position, &(ray->Position + ray->Direction * ray->Distance));

				if (lastIntersectionPosition != D3DXVECTOR3(0, 0, 0))
				{
					tDelta = intersectPosition - lastIntersectionPosition;
				}

				delta = D3DXVECTOR3(tDelta.x, 0, 0);
			}
			break;
			case Gizmo::Axis::Z:
			case Gizmo::Axis::Y:
			{
				D3DXPLANE plane = D3DXPLANE(-1, 0, 0, Position().x);

				D3DXPlaneIntersectLine(&intersectPosition,
					&plane, &ray->Position, &(ray->Position + ray->Direction * ray->Distance));

				if (lastIntersectionPosition != D3DXVECTOR3(0, 0, 0))
				{
					tDelta = intersectPosition - lastIntersectionPosition;
				}

				switch (activeAxis)
				{
				case Gizmo::Axis::Y:
					delta = D3DXVECTOR3(0, tDelta.y, 0);
					break;
				case Gizmo::Axis::Z:
					delta = D3DXVECTOR3(0, 0, tDelta.z);
					break;
				}
			}
			break;
			}

			// Snap
			if (snapEnable)
			{
				float snapValue = translationSnapValue;
				if (precisionModeEnable)
				{
					delta *= precisionModeScale;
					snapValue *= precisionModeScale;
				}

				translationScaleSnapDelta += delta;

				delta = D3DXVECTOR3(
					(int)(translationScaleSnapDelta.x / snapValue) * snapValue,
					(int)(translationScaleSnapDelta.y / snapValue) * snapValue,
					(int)(translationScaleSnapDelta.z / snapValue) * snapValue);

				translationScaleSnapDelta -= delta;
			}
			else if (precisionModeEnable)
				delta *= precisionModeScale;

			rotationDelta = delta;

			#pragma endregion

		}
			break;
		}
		
	}
	else {
		SelectAxis(ray);
	}

	if (Mouse::Get()->Press(0))
	{
		if (translateDelta != D3DXVECTOR3(0, 0, 0)) {
			D3DXVECTOR3 pos = render->Position();
			Position(pos + translateDelta);
			render->Position(pos + translateDelta);
			UpdateLine();

			translateDelta = D3DXVECTOR3(0, 0, 0);
		}
		
		if (rotationDelta != D3DXVECTOR3(0, 0, 0)) {
			D3DXVECTOR3 rot = render->Rotation();
			if (bLocal)
				Rotation(rot + rotationDelta);

			render->Rotation(rot + rotationDelta);
			UpdateLine();

			rotationDelta = D3DXVECTOR3(0, 0, 0);
		}

		if (scaleDelta != D3DXVECTOR3(0, 0, 0))
		{
			D3DXVECTOR3 scale = render->Scale();
			//Scale(scale + scaleDelta);
			if (activeMode == Mode::NonUniformScale)
				scale += delta;
			else if (activeMode == Mode::UniformScale)
				scale *= 1 + (delta.x + delta.y + delta.z) / 3;

			render->Scale(scale);
			//UpdateLine();

			scaleDelta = D3DXVECTOR3(0, 0, 0);
		}
	}

	D3DXVECTOR3 camPos;
	values->MainCamera->Position(&camPos);

	D3DXVECTOR3 vLength = camPos - Position();
	screenScale = D3DXVec3Length(&vLength) / scaleFactor;

	for (int i = 0; i < 3; i++)
		axisSphere[i]->Radius = radius * screenScale;

	#pragma region Bounding 그리기
	//if (activeMode == Mode::Rotate) {
	//	for (int i = 0; i < 3; i++)
	//		axis[i]->Draw(World(), axisSphere[i]);
	//}
	#pragma endregion
}

void Gizmo::UpdateLine()
{
	D3DXVECTOR3 forward, right, up;
	D3DXVec3Normalize(&forward, &Direction());
	D3DXVec3Normalize(&right, &Right());
	D3DXVec3Normalize(&up, &Up());

	float distance = 3.0f;
	//for (int i = 0; i < 4; i++) {
	//	model[i][(int)Axis::X]->Position(Position() + right * distance);
	//	model[i][(int)Axis::Y]->Position(Position() + up * distance);
	//	model[i][(int)Axis::Z]->Position(Position() + forward * distance);

	//	model[i][(int)Axis::X]->Rotation(Rotation()); 
	//	model[i][(int)Axis::Y]->Rotation(Rotation());
	//	model[i][(int)Axis::Z]->Rotation(Rotation());

	//	if ((Mode)i != Mode::UniformScale)
	//	{
	//		model[i][(int)Axis::X]->SetColor(axisColor[(int)Axis::X]);
	//		model[i][(int)Axis::Y]->SetColor(axisColor[(int)Axis::Y]);
	//		model[i][(int)Axis::Z]->SetColor(axisColor[(int)Axis::Z]);
	//	}
	//	else
	//	{
	//		model[i][(int)Axis::X]->SetColor(axisColor[(int)Axis::X]);
	//		model[i][(int)Axis::Y]->SetColor(axisColor[(int)Axis::X]);
	//		model[i][(int)Axis::Z]->SetColor(axisColor[(int)Axis::X]);
	//	}
	//}

	translateModel[(int)Axis::X]->Position(Position() + right * distance);
	translateModel[(int)Axis::Y]->Position(Position() + up * distance);
	translateModel[(int)Axis::Z]->Position(Position() + forward * distance);
	translateModel[(int)Axis::X]->Rotation(Rotation());
	translateModel[(int)Axis::Y]->Rotation(Rotation());
	translateModel[(int)Axis::Z]->Rotation(Rotation());

	rotateModel[(int)Axis::X]->Position(Position() + right * distance);
	rotateModel[(int)Axis::Y]->Position(Position() + up * distance);
	rotateModel[(int)Axis::Z]->Position(Position() + forward * distance);
	rotateModel[(int)Axis::X]->Rotation(Rotation());
	rotateModel[(int)Axis::Y]->Rotation(Rotation());
	rotateModel[(int)Axis::Z]->Rotation(Rotation());

	scaleModel[(int)Axis::X]->Position(Position() + right * distance);
	scaleModel[(int)Axis::Y]->Position(Position() + up * distance);
	scaleModel[(int)Axis::Z]->Position(Position() + forward * distance);
	scaleModel[(int)Axis::X]->Rotation(Rotation());
	scaleModel[(int)Axis::Y]->Rotation(Rotation());
	scaleModel[(int)Axis::Z]->Rotation(Rotation());

	uniformScaleModel[(int)Axis::X]->Position(Position() + right * distance);
	uniformScaleModel[(int)Axis::Y]->Position(Position() + up * distance);
	uniformScaleModel[(int)Axis::Z]->Position(Position() + forward * distance);
	uniformScaleModel[(int)Axis::X]->Rotation(Rotation());
	uniformScaleModel[(int)Axis::Y]->Rotation(Rotation());
	uniformScaleModel[(int)Axis::Z]->Rotation(Rotation());

	axisLine->Draw(World(), axiss);

	for (int i = 0; i < 3; i++) {
		quads[i]->Position(Position());
		quads[i]->Rotation(Rotation());
		quads[i]->Scale(Scale());
	}

	#pragma region Bounding 그리기
	//if (activeMode != Mode::Rotate) {
	//	for (int i = 0; i < 6; i++)
	//		axis[i]->Draw(World(), axisBox[i]);
	//}
	//else
	//	for (int i = 0; i < 3; i++)
	//		axis[i]->Draw(World(), axisSphere[i]);
	#pragma endregion
}

void Gizmo::Render()
{
	#pragma region ImGui
	ImGui::Begin("Gizmo");

	ImGui::Text("Mode %d", (int)activeMode);
	ImGui::RadioButton("Translate", (int*)&activeMode, 0);
	ImGui::SameLine(150);
	ImGui::RadioButton("Rotate", (int*)&activeMode, 1);
	ImGui::RadioButton("NonUniformScale", (int*)&activeMode, 2);
	ImGui::SameLine(150);
	ImGui::RadioButton("UniformScale", (int*)&activeMode, 3);

	if (ImGui::Checkbox("Local", &bLocal)) {
		if (bLocal) {
			D3DXVECTOR3 rot = render->Rotation();
			D3DXMatrixRotationYawPitchRoll(&rotationMatrix, rot.y, rot.x, rot.z);
		}
		else {
			D3DXMatrixIdentity(&rotationMatrix);
		}
	}

	// test
	//if (render != NULL) {
	//	D3DXVECTOR3 pos, rot, scale;
	//	pos = render->Position();
	//	rot = render->Rotation();
	//	scale = render->Scale();
	//
	//	if (ImGui::DragFloat3("Position", (float*)&pos, 0.1f)) {
	//		render->Position(pos);
	//		Position(pos);
	//		UpdateLine();
	//	}
	//	if (ImGui::DragFloat3("Rotation", (float*)&rot, 0.1f)) {
	//		render->RotationDegree(rot);
	//		//UpdateLine();
	//	}
	//	if (ImGui::DragFloat3("Scale", (float*)&scale, 0.1f)) {
	//		render->Scale(scale);
	//		//UpdateLine();
	//	}
	//}
	//
	//ImGui::Text("last Intersect %f %f %f", lastIntersectionPosition.x, lastIntersectionPosition.y, lastIntersectionPosition.z);
	//ImGui::Text("Intersect %f %f %f", intersectPosition.x, intersectPosition.y, intersectPosition.z);
	//ImGui::Text("tDelta %f %f %f", tDelta.x, tDelta.y, tDelta.z);
	//ImGui::Text("delta %f %f %f", delta.x, delta.y, delta.z);
	//ImGui::Text("ScaleDelta %f %f %f", scaleDelta.x, scaleDelta.y, scaleDelta.z);
	//ImGui::Text("Ray Pos %f %f %f", startRay.Position.x,  startRay.Position.y,  startRay.Position.z);
	//ImGui::Text("Ray Dir %f %f %f", startRay.Direction.x, startRay.Direction.y, startRay.Direction.z);
	//
	//ImGui::DragFloat3("Axis X Rot", (float*)&model[(int)activeMode][(int)Axis::X]->RotationDegree());
	//ImGui::DragFloat3("Axis Y Rot", (float*)&model[(int)activeMode][(int)Axis::Y]->RotationDegree());
	//ImGui::DragFloat3("Axis Z Rot", (float*)&model[(int)activeMode][(int)Axis::Z]->RotationDegree());

	switch (activeAxis)
	{
	case Gizmo::Axis::X:	ImGui::Text("Active Axis X"); break;
	case Gizmo::Axis::Y:	ImGui::Text("Active Axis Y"); break;
	case Gizmo::Axis::Z:	ImGui::Text("Active Axis Z"); break;
	case Gizmo::Axis::XZ:	ImGui::Text("Active Axis XZ"); break;
	case Gizmo::Axis::XY:	ImGui::Text("Active Axis XY"); break;
	case Gizmo::Axis::YZ:	ImGui::Text("Active Axis YZ"); break;
	case Gizmo::Axis::None:	ImGui::Text("Active Axis None"); break;
	}

	ImGui::Checkbox("Snap", &snapEnable);
	ImGui::SameLine(120);
	ImGui::Checkbox("Precision", &precisionModeEnable);

	ImGui::End();
	#pragma endregion

	if (visible) {
		depthMode[1]->OMSetDepthStencilState();
		blendMode[1]->OMSetBlendState();
		// axis
		{ 
			#pragma region Bounding 그리기
			if (activeMode == Mode::Rotate) {
				for (int i = 0; i < 3; i++)
					axis[i]->Render();
			}
			else {
				for (int i = 0; i < 6; i++)
					axis[i]->Render();
			}
			#pragma endregion

			// X
			if (activeAxis == Axis::X)
				axisLine->Color(highlightColor);
			else
				axisLine->Color(axisColor[(int)Axis::X]);
			axisLine->Render(6, 0);
			// Y
			if (activeAxis == Axis::Y)
				axisLine->Color(highlightColor);
			else
				axisLine->Color(axisColor[(int)Axis::Y]);
			axisLine->Render(6, 6);
			// Z
			if (activeAxis == Axis::Z)
				axisLine->Color(highlightColor);
			else
				axisLine->Color(axisColor[(int)Axis::Z]);
			axisLine->Render(6, 12);

			for (int i = 0; i < 3; i++) {
				switch (activeMode)
				{
				case Gizmo::Mode::Translate: translateModel[i]->Render(); break;
				case Gizmo::Mode::Rotate:  rotateModel[i]->Render(); break;
				case Gizmo::Mode::NonUniformScale:  scaleModel[i]->Render(); break;
				case Gizmo::Mode::UniformScale:  uniformScaleModel[i]->Render(); break;
				}
				//model[(int)activeMode][i]->Render();

				if (activeAxis == (Axis)(i + 3)) {
					cullMode[1]->RSSetState();
					quads[i]->Render();
					cullMode[0]->RSSetState();
				}
			}
		}
		depthMode[0]->OMSetDepthStencilState();
		blendMode[0]->OMSetBlendState();
	}
}

void Gizmo::SelectAxis(Ray* ray)
{
	float closestIntersection = D3D11_FLOAT32_MAX;

	float intersection = 0;
	
	#pragma region X,Y,Z Boxes
	if (axisBox[(int)Axis::X]->Intersect(ray, World(), intersection)) {
		if (intersection < closestIntersection)
		{
			activeAxis = Axis::X;
			closestIntersection = intersection;
		}
	}

	if (axisBox[(int)Axis::Y]->Intersect(ray, World(), intersection)) {
		if (intersection < closestIntersection)
		{
			activeAxis = Axis::Y;
			closestIntersection = intersection;
		}
	}

	if (axisBox[(int)Axis::Z]->Intersect(ray, World(), intersection)) {
		if (intersection < closestIntersection)
		{
			activeAxis = Axis::Z;
			closestIntersection = intersection;
		}
	}
	#pragma endregion

	if (activeMode == Mode::Rotate || activeMode == Mode::UniformScale ||
		activeMode == Mode::NonUniformScale)
	{
		#pragma region BoundingSpheres
		if (axisSphere[(int)Axis::X]->Intersect(ray, World(), intersection)) {
			if (intersection < closestIntersection)
			{
				activeAxis = Axis::X;
				closestIntersection = intersection;
			}
		}

		if (axisSphere[(int)Axis::Y]->Intersect(ray, World(), intersection)) {
			if (intersection < closestIntersection)
			{
				activeAxis = Axis::Y;
				closestIntersection = intersection;
			}
		}

		if (axisSphere[(int)Axis::Z]->Intersect(ray, World(), intersection)) {
			if (intersection < closestIntersection)
			{
				activeAxis = Axis::Z;
				closestIntersection = intersection;
			}
		}
		#pragma endregion
	}

	if (activeMode == Mode::Translate || activeMode == Mode::NonUniformScale ||
		activeMode == Mode::UniformScale)
	{
		if (closestIntersection >= D3D11_FLOAT32_MAX)
			closestIntersection = -D3D11_FLOAT32_MAX;

		#pragma region BoundingBoxes
		if (axisBox[(int)Axis::XY]->Intersect(ray, World(), intersection)) {
			if (intersection > closestIntersection)
			{
				activeAxis = Axis::XY;
				closestIntersection = intersection;
			}
		}
		if (axisBox[(int)Axis::XZ]->Intersect(ray, World(), intersection)) {
			if (intersection > closestIntersection)
			{
				activeAxis = Axis::XZ;
				closestIntersection = intersection;
			}
		}
		if (axisBox[(int)Axis::YZ]->Intersect(ray, World(), intersection)) {
			if (intersection > closestIntersection)
			{
				activeAxis = Axis::YZ;
				closestIntersection = intersection;
			}
		}
		#pragma endregion
	}

	if (closestIntersection >= D3D11_FLOAT32_MAX || closestIntersection <= -D3D11_FLOAT32_MAX)
		activeAxis = Axis::None;
}

void Gizmo::ResetDeltas()
{
	tDelta = D3DXVECTOR3(0, 0, 0);
	lastIntersectionPosition = D3DXVECTOR3(0, 0, 0);
	intersectPosition = D3DXVECTOR3(0, 0, 0);
}
