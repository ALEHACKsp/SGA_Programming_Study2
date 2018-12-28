#include "stdafx.h"
#include "GizmoComponent.h"
#include "./Objects/GameModel.h"
#include "./Draw/DebugLine.h"
#include "./Physics/RayCollider.h"
#include "./Physics/BoxCollider.h"
#include "./Physics/SphereCollider.h"
#include "./Physics/PlaneCollider.h"

GizmoComponent::GizmoComponent(ExecuteValues* values)
	: values(values)
{
	D3DXMatrixIdentity(&sceneWorld);
	_isActive = true;
	enabled = true;
	D3DXMatrixIdentity(&_rotationMatrix);
	_localForward = D3DXVECTOR3(0, 0, 1);
	_localUp = D3DXVECTOR3(0, 1, 0);
	_localRight = D3DXVECTOR3(1, 0, 0);
	D3DXMatrixIdentity(&_gizmoWorld);
	_axisTextOffset = D3DXVECTOR3(0, 0.5f, 0);

	_translationDelta = D3DXVECTOR3(0, 0, 0);
	D3DXMatrixIdentity(&_rotationDelta);
	_scaleDelta = D3DXVECTOR3(0, 0, 0);

	_selectionBoxShader = new Shader(Shaders + L"");
	_lineShader = new Shader(Shaders + L"");
	_meshShader = new Shader(Shaders + L"");
	_quadShader = new Shader(Shaders + L"");

	depthState[0] = new DepthStencilState();
	depthState[1] = new DepthStencilState();
	depthState[1]->DepthEnable(false);

	blendState[0] = new BlendState();
	blendState[1] = new BlendState();
	blendState[1]->BlendEnable(true);

	rasterizerState[0] = new RasterizerState();
	rasterizerState[1] = new RasterizerState();
	rasterizerState[1]->CullMode(D3D11_CULL_NONE);

	worldBuffer = new WorldBuffer();

	Initialize();
}

GizmoComponent::~GizmoComponent()
{
	SAFE_DELETE(worldBuffer);

	SAFE_DELETE(blendState[0]);
	SAFE_DELETE(blendState[1]);
	SAFE_DELETE(rasterizerState[0]);
	SAFE_DELETE(rasterizerState[1]);
	SAFE_DELETE(depthState[0]);
	SAFE_DELETE(depthState[1]);

	SAFE_DELETE(_quadShader);
	SAFE_DELETE(_meshShader);
	SAFE_DELETE(_lineShader);
	SAFE_DELETE(_selectionBoxShader);

}

void GizmoComponent::Initialize()
{
	// -- Set local-space offset -- //
	D3DXMatrixLookAtLH(&_modelLocalSpace[0], &D3DXVECTOR3(LINE_LENGTH, 0, 0), &D3DXVECTOR3(-1, 0, 0), &D3DXVECTOR3(0, 1, 0));
	D3DXMatrixLookAtLH(&_modelLocalSpace[1], &D3DXVECTOR3(0, LINE_LENGTH, 0), &D3DXVECTOR3(0, -1, 0), &D3DXVECTOR3(-1, 0, 0));
	D3DXMatrixLookAtLH(&_modelLocalSpace[2], &D3DXVECTOR3(0, 0, LINE_LENGTH), &D3DXVECTOR3(0, 0, 1), &D3DXVECTOR3(0, 1, 0));

	// -- Colors: X,Y,Z,Highlight -- //
	_axisColors[0] = D3DXCOLOR(1, 0, 0, 1);
	_axisColors[1] = D3DXCOLOR(0, 1, 0, 1);
	_axisColors[2] = D3DXCOLOR(0, 0, 1, 1);
	_highlightColor = D3DXCOLOR(1, 0.8431f, 0, 1);

	// text projected in 3D
	_axisText[0] = "X";
	_axisText[1] = "Y";
	_axisText[2] = "Z";

	// translucent quads

	const float halfLineOffset = LINE_OFFSET / 2;
	_quads[0] = new Quad(D3DXVECTOR3(halfLineOffset, halfLineOffset, 0), D3DXVECTOR3(0, 0, -1), D3DXVECTOR3(0, 1, 0), LINE_OFFSET, LINE_OFFSET); //XY
	_quads[1] = new Quad(D3DXVECTOR3(halfLineOffset, 0, halfLineOffset), D3DXVECTOR3(0, 1, 0), D3DXVECTOR3(1, 0, 0), LINE_OFFSET, LINE_OFFSET); //XZ
	_quads[2] = new Quad(D3DXVECTOR3(0, halfLineOffset, halfLineOffset), D3DXVECTOR3(1, 0, 0), D3DXVECTOR3(0, 1, 0), LINE_OFFSET, LINE_OFFSET); //ZY 

	vector<VertexColor> vertexList;

	// helper to apply colors
	D3DXCOLOR xColor = _axisColors[0];
	D3DXCOLOR yColor = _axisColors[1];
	D3DXCOLOR zColor = _axisColors[2];


	// -- X Axis -- // index 0 - 5
	_translationLineVertices[0] = VertexColor(D3DXVECTOR3(halfLineOffset, 0, 0), xColor);
	_translationLineVertices[1] = VertexColor(D3DXVECTOR3(LINE_LENGTH, 0, 0), xColor);

	_translationLineVertices[2] = VertexColor(D3DXVECTOR3(LINE_OFFSET, 0, 0), xColor);
	_translationLineVertices[3] = VertexColor(D3DXVECTOR3(LINE_OFFSET, LINE_OFFSET, 0), xColor);

	_translationLineVertices[4] = VertexColor(D3DXVECTOR3(LINE_OFFSET, 0, 0), xColor);
	_translationLineVertices[5] = VertexColor(D3DXVECTOR3(LINE_OFFSET, 0, LINE_OFFSET), xColor);

	// -- Y Axis -- // index 6 - 11
	_translationLineVertices[6] = VertexColor(D3DXVECTOR3(0, halfLineOffset, 0), yColor);
	_translationLineVertices[7] = VertexColor(D3DXVECTOR3(0, LINE_LENGTH, 0), yColor);

	_translationLineVertices[8] = VertexColor(D3DXVECTOR3(0, LINE_OFFSET, 0), yColor);
	_translationLineVertices[9] = VertexColor(D3DXVECTOR3(LINE_OFFSET, LINE_OFFSET, 0), yColor);

	_translationLineVertices[10] = VertexColor(D3DXVECTOR3(0, LINE_OFFSET, 0), yColor);
	_translationLineVertices[11] = VertexColor(D3DXVECTOR3(0, LINE_OFFSET, LINE_OFFSET), yColor);

	// -- Z Axis -- // index 12 - 17
	_translationLineVertices[12] = VertexColor(D3DXVECTOR3(0, 0, halfLineOffset), zColor);
	_translationLineVertices[13] = VertexColor(D3DXVECTOR3(0, 0, LINE_LENGTH), zColor);

	_translationLineVertices[14] = VertexColor(D3DXVECTOR3(0, 0, LINE_OFFSET), zColor);
	_translationLineVertices[15] = VertexColor(D3DXVECTOR3(LINE_OFFSET, 0, LINE_OFFSET), zColor);

	_translationLineVertices[16] = VertexColor(D3DXVECTOR3(0, 0, LINE_OFFSET), zColor);
	_translationLineVertices[17] = VertexColor(D3DXVECTOR3(0, LINE_OFFSET, LINE_OFFSET), zColor);
	
	//Create Vertex Buffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(VertexColor) * 18;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = _translationLineVertices;

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &_translationLineBuffer);
		assert(SUCCEEDED(hr));
	}
	
	//const float length = 50;
	//_lineRenderer = new LineRenderer(_graphics, Color.Gold);
	//_lineRenderer.Add(Vector3.Backward*length, Vector3.Forward*length);
	//_lineRenderer.Add(Vector3.Up*length, Vector3.Down*length);
	//_lineRenderer.Add(Vector3.Left*length, Vector3.Right*length);

	CreateGizmoBuffer();
}

void GizmoComponent::Update()
{
	D3DXVECTOR3 mousePosition = Mouse::Get()->GetPosition();

	// show or hide the orientation-lines helper.
	//_showLines = _currentKeys.IsKeyDown(Keys.Space);

	if (_isActive)
	{
		_lastIntersectionPosition = _intersectPosition;

		if (Mouse::Get()->Press(0) && ActiveAxis != GizmoAxis::None)
		{
			switch (ActiveMode)
			{
			case GizmoMode::UniformScale:
			case GizmoMode::NonUniformScale:
			case GizmoMode::Translate:
			{
				//Translate & Scale

				D3DXVECTOR3 delta = D3DXVECTOR3(0, 0, 0);
				RayCollider* ray = values->MainCamera->ConvertMouseToRay(values->Viewport, values->Perspective, _currentMouseState);

				D3DXMATRIX transform;
				float temp;
				D3DXMatrixInverse(&transform, &temp, &_rotationMatrix);
				D3DXVECTOR4 position;
				D3DXVec3Transform(&position, &ray->GetTransform()->GetWorldPosition(), &transform);
				ray->GetTransform()->SetWorldPosition(D3DXVECTOR3(position.x, position.y, position.z));
				D3DXVECTOR3 direction;
				D3DXVec3TransformNormal(&direction, &ray->GetDirection(), &transform);
				ray->SetDirection(direction);

				switch (ActiveAxis)
				{
				case GizmoAxis::XY:
				case GizmoAxis::X:
				{
					D3DXVECTOR3 normal = D3DXVECTOR3(0, 0, 1);
					D3DXVec3TransformCoord(&normal, &normal, &_rotationMatrix);
					PlaneCollider* plane = new PlaneCollider(D3DXVECTOR3(_position.x, _position.y, _position.z), normal);
				
					float dist;

					if (Manifold::Solve(plane, ray, &dist))
					{
						_intersectPosition = (ray->GetPosition() + (ray->GetDirection() * dist));
						if (_lastIntersectionPosition != D3DXVECTOR3(0, 0, 0))
						{
							_tDelta = _intersectPosition - _lastIntersectionPosition;
						}
						delta = ActiveAxis == GizmoAxis::X ? D3DXVECTOR3(_tDelta.x, 0, 0) : D3DXVECTOR3(_tDelta.x, _tDelta.y, 0);
					}
					SAFE_DELETE(plane);
				}
				break;
				case GizmoAxis::Z:
				case GizmoAxis::YZ:
				case GizmoAxis::Y:
				{
					D3DXVECTOR3 normal = D3DXVECTOR3(1, 0, 0);
					D3DXVec3TransformCoord(&normal, &normal, &_rotationMatrix);
					PlaneCollider* plane = new PlaneCollider(D3DXVECTOR3(_position.x, _position.y, _position.z), normal);
					
					float dist;

					if (Manifold::Solve(plane, ray, &dist))
					{
						_intersectPosition = (ray->GetPosition() + (ray->GetDirection() * dist));
						if (_lastIntersectionPosition != D3DXVECTOR3(0, 0, 0))
						{
							_tDelta = _intersectPosition - _lastIntersectionPosition;
						}
						switch (ActiveAxis)
						{
						case GizmoAxis::Y:
							delta = D3DXVECTOR3(0, _tDelta.y, 0);
							break;
						case GizmoAxis::Z:
							delta = D3DXVECTOR3(0, 0, _tDelta.z);
							break;
						default:
							delta = D3DXVECTOR3(0, _tDelta.y, _tDelta.z);
							break;
						}
					}
					SAFE_DELETE(plane);
				}
				break;
				case GizmoAxis::ZX:
				{
					D3DXVECTOR3 normal = D3DXVECTOR3(0, 1, 0);
					D3DXVec3TransformCoord(&normal, &normal, &_rotationMatrix);
					PlaneCollider* plane = new PlaneCollider(D3DXVECTOR3(_position.x, _position.y, _position.z), normal);

					float dist;

					if (Manifold::Solve(plane, ray, &dist))
					{
						_intersectPosition = (ray->GetPosition() + (ray->GetDirection() * dist));
						if (_lastIntersectionPosition != D3DXVECTOR3(0, 0, 0))
						{
							_tDelta = _intersectPosition - _lastIntersectionPosition;
						}
					}
					delta = D3DXVECTOR3(_tDelta.x, 0, _tDelta.z);
					SAFE_DELETE(plane);
				}
				break;
				}


				if (SnapEnabled)
				{
					float snapValue = TranslationSnapValue;
					if (ActiveMode == GizmoMode::UniformScale || ActiveMode == GizmoMode::NonUniformScale)
						snapValue = ScaleSnapValue;
					if (PrecisionModeEnabled)
					{
						delta *= PRECISION_MODE_SCALE;
						snapValue *= PRECISION_MODE_SCALE;
					}

					_translationScaleSnapDelta += delta;

					delta = D3DXVECTOR3(
						(int)(_translationScaleSnapDelta.x / snapValue) * snapValue,
						(int)(_translationScaleSnapDelta.y / snapValue) * snapValue,
						(int)(_translationScaleSnapDelta.z / snapValue) * snapValue);

					_translationScaleSnapDelta -= delta;
				}
				else if (PrecisionModeEnabled)
					delta *= PRECISION_MODE_SCALE;


				if (ActiveMode == GizmoMode::Translate)
				{
					// transform (local or world)
					D3DXVECTOR4 temp;
					D3DXVec3Transform(&temp, &delta, &_rotationMatrix);
					_translationDelta = D3DXVECTOR3(temp.x, temp.y, temp.z);
				}
				else if (ActiveMode == GizmoMode::NonUniformScale || ActiveMode == GizmoMode::UniformScale)
				{
					// -- Apply Scale -- //
					_scaleDelta += delta;
				}
				SAFE_DELETE(ray);
			}
			break;
			case GizmoMode::Rotate:
			{
				float delta = mousePosition.x - _lastMouseState.x;
				delta *= Time::Get()->Delta();

				if (SnapEnabled)
				{
					float snapValue = Math::ToRadian(RotationSnapValue);
					if (PrecisionModeEnabled)
					{
						delta *= PRECISION_MODE_SCALE;
						snapValue *= PRECISION_MODE_SCALE;
					}

					_rotationSnapDelta += delta;

					float snapped = (int)(_rotationSnapDelta / snapValue) * snapValue;
					_rotationSnapDelta -= snapped;

					delta = snapped;
				}
				else if (PrecisionModeEnabled)
					delta *= PRECISION_MODE_SCALE;

				// rotation matrix to transform - if more than one objects selected, always use world-space.
				D3DXMATRIX rot;
				D3DXMatrixIdentity(&rot);
				rot._11 = sceneWorld._11;
				rot._12 = sceneWorld._12;
				rot._13 = sceneWorld._13;
				rot._21 = sceneWorld._21;
				rot._22 = sceneWorld._22;
				rot._23 = sceneWorld._23;
				rot._31 = sceneWorld._31;
				rot._32 = sceneWorld._32;
				rot._33 = sceneWorld._33;

				D3DXMATRIX temp;
				switch (ActiveAxis)
				{
				case GizmoAxis::X:
					D3DXMatrixRotationAxis(&temp, &D3DXVECTOR3(_rotationMatrix._11, _rotationMatrix._12, _rotationMatrix._13), delta);
					rot *= temp;
					break;
				case GizmoAxis::Y:
					D3DXMatrixRotationAxis(&temp, &D3DXVECTOR3(_rotationMatrix._21, _rotationMatrix._22, _rotationMatrix._23), delta);
					rot *= temp;
					break;
				case GizmoAxis::Z:
					D3DXMatrixRotationAxis(&temp, &D3DXVECTOR3(_rotationMatrix._31, _rotationMatrix._32, _rotationMatrix._33), delta);
					rot *= temp;
					break;
				}
				_rotationDelta = rot;
			}
			break;
			}
		}
		else
		{
			if (!Mouse::Get()->Press(0) && !Mouse::Get()->Press(1))
				SelectAxis(mousePosition);
		}

		SetGizmoPosition();

		// -- Trigger Translation, Rotation & Scale events -- //
		if (Mouse::Get()->Press(0) /*&& !IsAnyModifierPressed()*/)
		{
			if (_translationDelta != D3DXVECTOR3(0, 0, 0))
			{
				for (GameModel* entity : *selection)
					entity->Position(entity->Position() + _translationDelta);
				_translationDelta = D3DXVECTOR3(0, 0, 0);
			}
			D3DXMATRIX mat;
			D3DXMatrixIdentity(&mat);
			if (_rotationDelta != mat)
			{
				for (GameModel* entity : *selection)
					entity->World(entity->World() * _rotationDelta);
				_rotationDelta = mat;
			}
			if (_scaleDelta != D3DXVECTOR3(0, 0, 0))
			{
				for (GameModel* entity : *selection)
					entity->Scale(entity->Scale() + _translationDelta);
				_scaleDelta = D3DXVECTOR3(0, 0, 0);
			}
		}
	}

	_lastMouseState = _currentMouseState;

	if (selection->size() < 1)
	{
		_isActive = false;
		ActiveAxis = GizmoAxis::None;
		return;
	}
	// helps solve visual lag (1-frame-lag) after selecting a new entity
	if (!_isActive)
		SetGizmoPosition();

	_isActive = true;

	// -- Scale Gizmo to fit on-screen -- //
	D3DXVECTOR3 vLength = values->MainCamera->Position() - _position;
	const float scaleFactor = 25.0f;

	_screenScale = D3DXVec3Length(&vLength) / scaleFactor;
	D3DXMatrixScaling(&_screenScaleMatrix, _screenScale, _screenScale, _screenScale);

	_localForward = (*selection).front()->Direction();
	_localUp = (*selection).front()->Up();
	// -- Vector Rotation (Local/World) -- //
	D3DXVec3Normalize(&_localForward, &_localForward);
	D3DXVec3Cross(&_localRight, &_localUp, &_localForward);
	D3DXVec3Cross(&_localUp, &_localRight, &_localForward);
	D3DXVec3Normalize(&_localRight, &_localRight);
	D3DXVec3Normalize(&_localUp, &_localUp);

	// -- Create Both World Matrices -- //
	D3DXMATRIX mat;
	D3DXMatrixLookAtLH(&mat, &_position, &_localForward, &_localUp);
	_objectOrientedWorld = _screenScaleMatrix * mat;
	D3DXMatrixLookAtLH(&mat, &_position, &D3DXVECTOR3(sceneWorld._31, sceneWorld._32, sceneWorld._33), &D3DXVECTOR3(sceneWorld._21, sceneWorld._22, sceneWorld._23));
	_axisAlignedWorld = _screenScaleMatrix * mat;

	// Assign World
	if (ActiveSpace == TransformSpace::World ||
		ActiveMode == GizmoMode::Rotate ||
		ActiveMode == GizmoMode::NonUniformScale ||
		ActiveMode == GizmoMode::UniformScale)
	{
		_gizmoWorld = _axisAlignedWorld;

		// align lines, boxes etc. with the grid-lines
		_rotationMatrix._11 = sceneWorld._11;
		_rotationMatrix._12 = sceneWorld._12;
		_rotationMatrix._13 = sceneWorld._13;

		_rotationMatrix._21 = sceneWorld._21;
		_rotationMatrix._22 = sceneWorld._22;
		_rotationMatrix._23 = sceneWorld._23;

		_rotationMatrix._31 = sceneWorld._31;
		_rotationMatrix._32 = sceneWorld._32;
		_rotationMatrix._33 = sceneWorld._33;
	}
	else
	{
		_gizmoWorld = _objectOrientedWorld;

		// align lines, boxes etc. with the selected object
		_rotationMatrix._11 = _localRight.x;
		_rotationMatrix._12 = _localRight.y;
		_rotationMatrix._13 = _localRight.z;

		_rotationMatrix._21 = _localUp.x;
		_rotationMatrix._22 = _localUp.y;
		_rotationMatrix._23 = _localUp.z;

		_rotationMatrix._31 = _localForward.x;
		_rotationMatrix._32 = _localForward.y;
		_rotationMatrix._33 = _localForward.z;
	}

	// -- Reset Colors to default -- //
	ApplyColor(GizmoAxis::X, _axisColors[0]);
	ApplyColor(GizmoAxis::Y, _axisColors[1]);
	ApplyColor(GizmoAxis::Z, _axisColors[2]);

	// -- Apply Highlight -- //
	ApplyColor(ActiveAxis, _highlightColor);

}

void GizmoComponent::Render()
{
	Draw();
}

void GizmoComponent::PostRender()
{
	Draw2D();
}

void GizmoComponent::NextPivotType()
{
	if (ActivePivot == PivotType::WorldOrigin)
		ActivePivot = PivotType::ObjectCenter;
	else
		ActivePivot = (PivotType)((int)ActivePivot + 1);
}

void GizmoComponent::SelectEntities(D3DXVECTOR2 mouseloc, bool addToSelection, bool removeFromSelection)
{
	if (ActiveAxis == GizmoAxis::None)
	{
		if (!addToSelection && !removeFromSelection)
			selection->clear();
		PickObject(mouseloc, removeFromSelection);
	}
	ResetDeltas();
}

void GizmoComponent::Clear()
{
	if (selection != NULL)
		selection->clear();
}

void GizmoComponent::Draw()
{
	if (!_isActive) return;
	depthState[1]->OMSetDepthStencilState();

	// -- Draw Lines -- //

	worldBuffer->SetMatrix(_gizmoWorld);
	worldBuffer->SetVSBuffer(1);

	_lineShader->Render();

	UINT stride = sizeof(VertexColor);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &_translationLineBuffer, &stride, &offset);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3D::GetDC()->Draw(18, 0);

	switch (ActiveMode)
	{
	case GizmoMode::NonUniformScale:
	case GizmoMode::Translate:
		switch (ActiveAxis)
		{
			//Draw Quads
		case GizmoAxis::ZX:
		case GizmoAxis::YZ:
		case GizmoAxis::XY:
		{
			blendState[1]->OMSetBlendState();
			rasterizerState[1]->RSSetState();

			worldBuffer->SetMatrix(_gizmoWorld);
			worldBuffer->SetVSBuffer(1);

			_quadShader->Render();

			Quad* activeQuad;
			switch (ActiveAxis)
			{
			case GizmoAxis::XY:
				activeQuad = _quads[0];
				break;
			case GizmoAxis::ZX:
				activeQuad = _quads[1];
				break;
			case GizmoAxis::YZ:
				activeQuad = _quads[2];
				break;
			}

			UINT stride = sizeof(VertexTextureNormal);
			UINT offset = 0;

			D3D::GetDC()->IASetVertexBuffers(0, 1, &activeQuad->VertexBuffer, &stride, &offset);
			D3D::GetDC()->IASetIndexBuffer(activeQuad->IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
			D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			D3D::GetDC()->DrawIndexed(6, 0, 0);

			blendState[0]->OMSetBlendState();
			rasterizerState[0]->RSSetState();
		}
		break;
		}
		break;
	case GizmoMode::UniformScale:
		//Draw Quads
		if (ActiveAxis != GizmoAxis::None)
		{
			blendState[1]->OMSetBlendState();
			rasterizerState[1]->RSSetState();

			worldBuffer->SetMatrix(_gizmoWorld);
			worldBuffer->SetVSBuffer(1);

			_quadShader->Render();

			UINT stride = sizeof(VertexTextureNormal);
			UINT offset = 0;
			
			for (int i = 0; i < 3; i++)
			{
				D3D::GetDC()->IASetVertexBuffers(0, 1, &_quads[i]->VertexBuffer, &stride, &offset);
				D3D::GetDC()->IASetIndexBuffer(_quads[i]->IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
				D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

				D3D::GetDC()->DrawIndexed(6, 0, 0);
			}
			blendState[0]->OMSetBlendState();
			rasterizerState[0]->RSSetState();
		}
		break;
	}

	// draw the 3d meshes
	for (int i = 0; i < 3; i++) //(order: x, y, z)
	{
		UINT gizmoMode;
		switch (ActiveMode)
		{
		case GizmoMode::Translate:
			gizmoMode = 1;
			break;
		case GizmoMode::Rotate:
			gizmoMode = 0;
			break;
		default:
			gizmoMode = 3;
			break;
		}

		D3DXCOLOR color;
		switch (ActiveMode)
		{
		case GizmoMode::UniformScale:
			color = _axisColors[0];
			break;
		default:
			color = _axisColors[i];
			break;
		}

		worldBuffer->SetMatrix(_modelLocalSpace[i] * _gizmoWorld);
		worldBuffer->SetVSBuffer(1);

		_quadShader->Render();

		//TODO::마저고쳐라
		//_meshEffect.DiffuseColor = color;
		//_meshEffect.EmissiveColor = color;
		
		_meshShader->Render();

		UINT stride = sizeof(VertexTextureNormal);
		UINT offset = 0;

		D3D::GetDC()->IASetVertexBuffers(0, 1, &gizmoVertexBuffer[gizmoMode], &stride, &offset);
		D3D::GetDC()->IASetIndexBuffer(gizmoIndexBuffer[gizmoMode], DXGI_FORMAT_R32_UINT, 0);
		D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		D3D::GetDC()->DrawIndexed(gizmoIndexCount[gizmoMode], 0, 0);
	}

	if (SelectionBoxesIsVisible)
		DrawSelectionBox();

	depthState[0]->OMSetDepthStencilState();
}

bool GizmoComponent::IsAnyModifierPressed()
{
	return Keyboard::Get()->Press(VK_LCONTROL) || Keyboard::Get()->Press(VK_RCONTROL) ||
		Keyboard::Get()->Press(VK_LSHIFT) || Keyboard::Get()->Press(VK_RSHIFT) ||
		Keyboard::Get()->Press(VK_LMENU) || Keyboard::Get()->Press(VK_RMENU);
}

void GizmoComponent::ApplyColor(GizmoAxis axis, D3DXCOLOR color)
{
	switch (ActiveMode)
	{
	case GizmoMode::NonUniformScale:
	case GizmoMode::Translate:
		switch (axis)
		{
		case GizmoAxis::X:
			ApplyLineColor(0, 6, color);
			break;
		case GizmoAxis::Y:
			ApplyLineColor(6, 6, color);
			break;
		case GizmoAxis::Z:
			ApplyLineColor(12, 6, color);
			break;
		case GizmoAxis::XY:
			ApplyLineColor(0, 4, color);
			ApplyLineColor(6, 4, color);
			break;
		case GizmoAxis::YZ:
			ApplyLineColor(6, 2, color);
			ApplyLineColor(12, 2, color);
			ApplyLineColor(10, 2, color);
			ApplyLineColor(16, 2, color);
			break;
		case GizmoAxis::ZX:
			ApplyLineColor(0, 2, color);
			ApplyLineColor(4, 2, color);
			ApplyLineColor(12, 4, color);
			break;
		}
		break;
	case GizmoMode::Rotate:
		switch (axis)
		{
		case GizmoAxis::X:
			ApplyLineColor(0, 6, color);
			break;
		case GizmoAxis::Y:
			ApplyLineColor(6, 6, color);
			break;
		case GizmoAxis::Z:
			ApplyLineColor(12, 6, color);
			break;
		}
		break;
	case GizmoMode::UniformScale:
		ApplyLineColor(0, 18, ActiveAxis == GizmoAxis::None ? _axisColors[0] : _highlightColor);
		break;
	}
}

void GizmoComponent::ApplyLineColor(int startindex, int count, D3DXCOLOR color)
{
	for (int i = startindex; i < (startindex + count); i++)
	{
		_translationLineVertices[i].Color = color;
	}
}

void GizmoComponent::SelectAxis(D3DXVECTOR3 mousePosition)
{
	if (!enabled)
		return;

	float closestintersection = INFINITY;
	RayCollider* ray = values->MainCamera->ConvertMouseToRay(values->Viewport, values->Perspective, mousePosition);
	if (ActiveMode == GizmoMode::Translate)
	{
		// transform ray into local-space of the boundingboxes.
		D3DXVECTOR3 direction;
		D3DXVECTOR4 position;
		D3DXMATRIX _gizmoWorldInv;
		float temp;
		D3DXMatrixInverse(&_gizmoWorldInv, &temp, &_gizmoWorld);
		D3DXVec3TransformNormal(&direction, &ray->GetDirection(), &_gizmoWorldInv);
		ray->SetDirection(direction);
		D3DXVec3Transform(&position, &ray->GetTransform()->GetWorldPosition(), &_gizmoWorldInv);
		ray->GetTransform()->SetWorldPosition(D3DXVECTOR3(position.x, position.y, position.z));
	}
	
	float dist;
	if (Manifold::Solve(ray, xAxisBox, &dist))
		if (dist < closestintersection)
		{
			ActiveAxis = GizmoAxis::X;
			closestintersection = dist;
		}
	if (Manifold::Solve(ray, yAxisBox, &dist))
	{
		if (dist < closestintersection)
		{
			ActiveAxis = GizmoAxis::Y;
			closestintersection = dist;
		}
	}
	if (Manifold::Solve(ray, zAxisBox, &dist))
	{
		if (dist < closestintersection)
		{
			ActiveAxis = GizmoAxis::Z;
			closestintersection = dist;
		}
	}
	
	if (ActiveMode == GizmoMode::Rotate || ActiveMode == GizmoMode::UniformScale ||
		ActiveMode == GizmoMode::NonUniformScale)
		{
		//BoundingSpheres
		if (Manifold::Solve(ray, xSphere, &dist))
			if (dist < closestintersection)
			{
				ActiveAxis = GizmoAxis::X;
				closestintersection = dist;
			}
		if (Manifold::Solve(ray, ySphere, &dist))
			if (dist < closestintersection)
			{
				ActiveAxis = GizmoAxis::Y;
				closestintersection = dist;
			}
		if (Manifold::Solve(ray, zSphere, &dist))
			if (dist < closestintersection)
			{
				ActiveAxis = GizmoAxis::Z;
				closestintersection = dist;
			}
	}
	if (ActiveMode == GizmoMode::Translate || ActiveMode == GizmoMode::NonUniformScale ||
		ActiveMode == GizmoMode::UniformScale)
	{
		// if no axis was hit (x,y,z) set value to lowest possible to select the 'farthest' intersection for the XY,XZ,YZ boxes. 
		// This is done so you may still select multi-axis if you're looking at the gizmo from behind!
		if (closestintersection >= INFINITY)
			closestintersection = -INFINITY;

		//BoundingBoxes
		if (Manifold::Solve(ray, xyBox, &dist))
			if (dist > closestintersection)
			{
				ActiveAxis = GizmoAxis::XY;
				closestintersection = dist;
			}
		if (Manifold::Solve(ray, xzAxisBox, &dist))
			if (dist > closestintersection)
			{
				ActiveAxis = GizmoAxis::ZX;
				closestintersection = dist;
			}
		if (Manifold::Solve(ray, yzBox, &dist))
			if (dist > closestintersection)
			{
				ActiveAxis = GizmoAxis::YZ;
				closestintersection = dist;
			}
	}
	if (closestintersection >= INFINITY || closestintersection <= -INFINITY)
		ActiveAxis = GizmoAxis::None;
}

void GizmoComponent::SetGizmoPosition()
{
	switch (ActivePivot)
	{
	case PivotType::ObjectCenter:
		if (selection->size() > 0)
			_position = selection->front()->GetTransform()->GetWorldPosition();
		break;
	case PivotType::SelectionCenter:
		_position = GetSelectionCenter();
		break;
	case PivotType::WorldOrigin:
		_position.x = sceneWorld._41;
		_position.y = sceneWorld._42;
		_position.z = sceneWorld._43;
		break;
	}
	_position += _translationDelta;
}

D3DXVECTOR3 GizmoComponent::GetSelectionCenter()
{
	if (selection->size() == 0)
		return D3DXVECTOR3(0, 0, 0);

	D3DXVECTOR3 center = D3DXVECTOR3(0, 0, 0);
	for (GameModel* selected : *selection)
		center += selected->GetTransform()->GetWorldPosition();

	return center / (float)selection->size();
}

void GizmoComponent::Draw2D()
{
	wstring str = String::Format(L"FPS : %.0f", ImGui::GetIO().Framerate);
	RECT rect = { 0, 0, 300, 300 };
	DirectWrite::RenderText(str, rect, 12);

	rect.top += 12;
	D3DXVECTOR3 vec;
	values->MainCamera->Position(&vec);
	str = String::Format(L"CameraPos : %.0f, %.0f, %.0f", vec.x, vec.y, vec.z);
	DirectWrite::RenderText(str, rect, 12);

	rect.top += 12;
	D3DXVECTOR2 rot;
	values->MainCamera->RotationDegree(&rot);
	str = String::Format(L"CameraRot : %.0f, %.0f", rot.x, rot.y);
	DirectWrite::RenderText(str, rect, 12);

	// -- Draw Axis identifiers ("X,Y,Z") -- // 
	for (int i = 0; i < 3; i++)
	{
		D3DXVECTOR3 screenPos = D3DXVECTOR3(_modelLocalSpace[i]._41, _modelLocalSpace[i]._42, _modelLocalSpace[i]._43);
		screenPos -= D3DXVECTOR3(_modelLocalSpace[i]._31, _modelLocalSpace[i]._32, _modelLocalSpace[i]._33);
		screenPos += _axisTextOffset;
		D3DXVec3TransformCoord(&screenPos, &screenPos, &_gizmoWorld);
		D3DXVec3TransformCoord(&screenPos, &screenPos, &values->ViewProjection->GetViewProjection());

		if (screenPos.z < 0.0f || screenPos.z > 1.0f)
			continue;

		D3DXCOLOR color = _axisColors[i];
		switch (i)
		{
		case 0:
			if (ActiveAxis == GizmoAxis::X || ActiveAxis == GizmoAxis::XY || ActiveAxis == GizmoAxis::ZX)
				color = _highlightColor;
			break;
		case 1:
			if (ActiveAxis == GizmoAxis::Y || ActiveAxis == GizmoAxis::XY || ActiveAxis == GizmoAxis::YZ)
				color = _highlightColor;
			break;
		case 2:
			if (ActiveAxis == GizmoAxis::Z || ActiveAxis == GizmoAxis::YZ || ActiveAxis == GizmoAxis::ZX)
				color = _highlightColor;
			break;
		}
		RECT rect = { (LONG)screenPos.x, (LONG)screenPos.y, 300, 300 };
		DirectWrite::RenderText(_axisText[i], rect, 12, L"돋움체", color);
	}

	// -- Draw StatusInfo -- //
	ImGui::Begin("DrawStatusInfo");
	{
		switch (ActiveMode)
		{
		case GizmoMode::Translate:
			ImGui::Text("Mode : Translate");
			break;
		case GizmoMode::Rotate:
			ImGui::Text("Mode : Rotate");
			break;
		case GizmoMode::NonUniformScale:
			ImGui::Text("Mode : NonUniformScale");
			break;
		case GizmoMode::UniformScale:
			ImGui::Text("Mode : UniformScale");
			break;
		}
		switch (ActiveSpace)
		{
		case TransformSpace::Local:
			ImGui::Text("Space : Local");
			break;
		case TransformSpace::World:
			ImGui::Text("Space : World");
			break;
		}
		SnapEnabled ? ImGui::Text("Snapping : ON") : ImGui::Text("Snapping : OFF");
		PrecisionModeEnabled ? ImGui::Text("Precision : ON") : ImGui::Text("Precision : OFF");
		switch (ActivePivot)
		{
		case PivotType::ObjectCenter:
			ImGui::Text("Pivot : ObjectCenter");
			break;
		case PivotType::SelectionCenter:
			ImGui::Text("Pivot : SelectionCenter");
			break;
		case PivotType::WorldOrigin:
			ImGui::Text("Pivot : WorldOrigin");
			break;
		}
	}
	ImGui::End();
}

void GizmoComponent::CreateSelectionBox()
{
	D3DXCOLOR lineColor = D3DXCOLOR(1, 1, 1, 1);
	const float lineLength = 5.0f;

	_selectionBoxVertices.clear();
	
	for (GameModel* tansformable : *selection)
	{
		BoxCollider* boundingBox = reinterpret_cast<BoxCollider*>(tansformable->GetCollider());
		D3DXVECTOR3 boundingBoxCorners[8];
		boundingBox->GetCorners(boundingBoxCorners);

		// --- Corner 0 --- // 
		_selectionBoxVertices.push_back(VertexColor(boundingBoxCorners[0], lineColor));
		_selectionBoxVertices.push_back(VertexColor(boundingBoxCorners[0] +
			D3DXVECTOR3(0, (boundingBoxCorners[3].y - boundingBoxCorners[0].y) / lineLength, 0), lineColor));

		_selectionBoxVertices.push_back(VertexColor(boundingBoxCorners[0], lineColor));
		_selectionBoxVertices.push_back(VertexColor(boundingBoxCorners[0] +
			D3DXVECTOR3(0, 0, (boundingBoxCorners[4].z - boundingBoxCorners[0].z) / lineLength), lineColor));

		_selectionBoxVertices.push_back(VertexColor(boundingBoxCorners[0], lineColor));
		_selectionBoxVertices.push_back(VertexColor(boundingBoxCorners[0] +
			D3DXVECTOR3((boundingBoxCorners[1].x - boundingBoxCorners[0].x) / lineLength, 0, 0), lineColor));

		// --- Corner 1 --- // 
		_selectionBoxVertices.push_back(VertexColor(boundingBoxCorners[1], lineColor));
		_selectionBoxVertices.push_back(VertexColor(boundingBoxCorners[1] +
			D3DXVECTOR3(0, (boundingBoxCorners[2].y - boundingBoxCorners[1].y) / lineLength, 0), lineColor));

		_selectionBoxVertices.push_back(VertexColor(boundingBoxCorners[1], lineColor));
		_selectionBoxVertices.push_back(VertexColor(boundingBoxCorners[1] +
			D3DXVECTOR3(0, 0, (boundingBoxCorners[5].z - boundingBoxCorners[1].z) / lineLength), lineColor));

		_selectionBoxVertices.push_back(VertexColor(boundingBoxCorners[1], lineColor));
		_selectionBoxVertices.push_back(VertexColor(boundingBoxCorners[1] +
			D3DXVECTOR3((boundingBoxCorners[0].x - boundingBoxCorners[1].x) / lineLength, 0, 0), lineColor));


		// --- Corner 2 --- // 
		_selectionBoxVertices.push_back(VertexColor(boundingBoxCorners[2], lineColor));
		_selectionBoxVertices.push_back(VertexColor(boundingBoxCorners[2] +
			D3DXVECTOR3(0, (boundingBoxCorners[1].y - boundingBoxCorners[2].y) / lineLength, 0), lineColor));

		_selectionBoxVertices.push_back(VertexColor(boundingBoxCorners[2], lineColor));
		_selectionBoxVertices.push_back(VertexColor(boundingBoxCorners[2] +
			D3DXVECTOR3(0, 0, (boundingBoxCorners[6].z - boundingBoxCorners[2].z) / lineLength), lineColor));

		_selectionBoxVertices.push_back(VertexColor(boundingBoxCorners[2], lineColor));
		_selectionBoxVertices.push_back(VertexColor(boundingBoxCorners[2] +
			D3DXVECTOR3((boundingBoxCorners[3].x - boundingBoxCorners[2].x) / lineLength, 0, 0), lineColor));

		// --- Corner 3 --- // 
		_selectionBoxVertices.push_back(VertexColor(boundingBoxCorners[3], lineColor));
		_selectionBoxVertices.push_back(VertexColor(boundingBoxCorners[3] +
			D3DXVECTOR3(0, (boundingBoxCorners[0].y - boundingBoxCorners[3].y) / lineLength, 0), lineColor));

		_selectionBoxVertices.push_back(VertexColor(boundingBoxCorners[3], lineColor));
		_selectionBoxVertices.push_back(VertexColor(boundingBoxCorners[3] +
			D3DXVECTOR3(0, 0, (boundingBoxCorners[7].z - boundingBoxCorners[3].z) / lineLength), lineColor));

		_selectionBoxVertices.push_back(VertexColor(boundingBoxCorners[3], lineColor));
		_selectionBoxVertices.push_back(VertexColor(boundingBoxCorners[3] +
			D3DXVECTOR3((boundingBoxCorners[2].x - boundingBoxCorners[3].x) / lineLength, 0, 0), lineColor));

		// --- Corner 4 --- // 
		_selectionBoxVertices.push_back(VertexColor(boundingBoxCorners[4], lineColor));
		_selectionBoxVertices.push_back(VertexColor(boundingBoxCorners[4] +
			D3DXVECTOR3(0, (boundingBoxCorners[7].y - boundingBoxCorners[4].y) / lineLength, 0), lineColor));

		_selectionBoxVertices.push_back(VertexColor(boundingBoxCorners[4], lineColor));
		_selectionBoxVertices.push_back(VertexColor(boundingBoxCorners[4] +
			D3DXVECTOR3(0, 0, (boundingBoxCorners[0].z - boundingBoxCorners[4].z) / lineLength), lineColor));

		_selectionBoxVertices.push_back(VertexColor(boundingBoxCorners[4], lineColor));
		_selectionBoxVertices.push_back(VertexColor(boundingBoxCorners[4] +
			D3DXVECTOR3((boundingBoxCorners[5].x - boundingBoxCorners[4].x) / lineLength, 0, 0), lineColor));

		// --- Corner 5 --- // 
		_selectionBoxVertices.push_back(VertexColor(boundingBoxCorners[5], lineColor));
		_selectionBoxVertices.push_back(VertexColor(boundingBoxCorners[5] +
			D3DXVECTOR3(0, (boundingBoxCorners[6].y - boundingBoxCorners[5].y) / lineLength, 0), lineColor));

		_selectionBoxVertices.push_back(VertexColor(boundingBoxCorners[5], lineColor));
		_selectionBoxVertices.push_back(VertexColor(boundingBoxCorners[5] +
			D3DXVECTOR3(0, 0, (boundingBoxCorners[1].z - boundingBoxCorners[5].z) / lineLength), lineColor));

		_selectionBoxVertices.push_back(VertexColor(boundingBoxCorners[5], lineColor));
		_selectionBoxVertices.push_back(VertexColor(boundingBoxCorners[5] +
			D3DXVECTOR3((boundingBoxCorners[4].x - boundingBoxCorners[5].x) / lineLength, 0, 0), lineColor));

		// --- Corner 6 --- // 
		_selectionBoxVertices.push_back(VertexColor(boundingBoxCorners[6], lineColor));
		_selectionBoxVertices.push_back(VertexColor(boundingBoxCorners[6] +
			D3DXVECTOR3(0, (boundingBoxCorners[5].y - boundingBoxCorners[6].y) / lineLength, 0), lineColor));

		_selectionBoxVertices.push_back(VertexColor(boundingBoxCorners[6], lineColor));
		_selectionBoxVertices.push_back(VertexColor(boundingBoxCorners[6] +
			D3DXVECTOR3(0, 0, (boundingBoxCorners[2].z - boundingBoxCorners[6].z) / lineLength), lineColor));

		_selectionBoxVertices.push_back(VertexColor(boundingBoxCorners[6], lineColor));
		_selectionBoxVertices.push_back(VertexColor(boundingBoxCorners[6] +
			D3DXVECTOR3((boundingBoxCorners[7].x - boundingBoxCorners[6].x) / lineLength, 0, 0), lineColor));


		// --- Corner 7 --- // 
		_selectionBoxVertices.push_back(VertexColor(boundingBoxCorners[7], lineColor));
		_selectionBoxVertices.push_back(VertexColor(boundingBoxCorners[7] +
			D3DXVECTOR3(0, (boundingBoxCorners[4].y - boundingBoxCorners[7].y) / lineLength, 0), lineColor));

		_selectionBoxVertices.push_back(VertexColor(boundingBoxCorners[7], lineColor));
		_selectionBoxVertices.push_back(VertexColor(boundingBoxCorners[7] +
			D3DXVECTOR3(0, 0, (boundingBoxCorners[3].z - boundingBoxCorners[7].z) / lineLength), lineColor));

		_selectionBoxVertices.push_back(VertexColor(boundingBoxCorners[7], lineColor));
		_selectionBoxVertices.push_back(VertexColor(boundingBoxCorners[7] +
			D3DXVECTOR3((boundingBoxCorners[6].x - boundingBoxCorners[7].x) / lineLength, 0, 0), lineColor));
	}

	//Create Vertex Buffer
	{
	D3D11_BUFFER_DESC desc = { 0 };
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(VertexColor) * _selectionBoxVertices.size();
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA data = { 0 };
	data.pSysMem = &_selectionBoxVertices[0];

	HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &_selectionBoxVertexBuffer);
	assert(SUCCEEDED(hr));
	}
}

void GizmoComponent::DrawSelectionBox()
{
	CreateSelectionBox();

	const float boxScale = 1.02f;

	if (selection->size() > 0)
	{
		D3DXMATRIX matrix;
		D3DXMatrixIdentity(&matrix);
		worldBuffer->SetMatrix(matrix);
		worldBuffer->SetVSBuffer(1);

		_selectionBoxShader->Render();

		UINT stride = sizeof(VertexTextureNormal);
		UINT offset = 0;

		D3D::GetDC()->IASetVertexBuffers(0, 1, &_selectionBoxVertexBuffer, &stride, &offset);
		D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		D3D::GetDC()->Draw(_selectionBoxVertices.size(), 0);
	}
}

void GizmoComponent::CreateGizmoBuffer()
{
	//rotate
	{
		D3DXVECTOR3 positions[] =
		{
			D3DXVECTOR3(0.8555959f, -0.03825768f, 0.07387591f), D3DXVECTOR3(0.6341127f, 0.4809631f, -2.745459E-08f), D3DXVECTOR3(0.8750901f, -0.03482031f, 1.21745E-10f), D3DXVECTOR3(0.8023366f, -0.04764872f, 0.1279567f), D3DXVECTOR3(0.6341127f, 0.4809631f, -2.745459E-08f), D3DXVECTOR3(0.8555959f, -0.03825768f, 0.07387591f), D3DXVECTOR3(0.7295831f, -0.06047713f, 0.1477517f), D3DXVECTOR3(0.6341127f, 0.4809631f, -2.745459E-08f), D3DXVECTOR3(0.8023366f, -0.04764872f, 0.1279567f), D3DXVECTOR3(0.6568297f, -0.07330552f, 0.1279567f), D3DXVECTOR3(0.6341127f, 0.4809631f, -2.745459E-08f), D3DXVECTOR3(0.7295831f, -0.06047713f, 0.1477517f), D3DXVECTOR3(0.7295831f, -0.06047713f, -0.1477517f), D3DXVECTOR3(0.6341127f, 0.4809631f, -2.745459E-08f), D3DXVECTOR3(0.6568297f, -0.07330553f, -0.1279567f), D3DXVECTOR3(0.8023366f, -0.04764872f, -0.1279567f), D3DXVECTOR3(0.6341127f, 0.4809631f, -2.745459E-08f), D3DXVECTOR3(0.7295831f, -0.06047713f, -0.1477517f), D3DXVECTOR3(0.8555959f, -0.03825768f, -0.07387582f), D3DXVECTOR3(0.6341127f, 0.4809631f, -2.745459E-08f), D3DXVECTOR3(0.8023366f, -0.04764872f, -0.1279567f), D3DXVECTOR3(0.8750901f, -0.03482031f, 1.21745E-10f), D3DXVECTOR3(0.6341127f, 0.4809631f, -2.745459E-08f), D3DXVECTOR3(0.8555959f, -0.03825768f, -0.07387582f), D3DXVECTOR3(0.2666126f, 0.7766161f,0.0f), D3DXVECTOR3(0.2286974f, 0.6661729f, -0.04836776f), D3DXVECTOR3(0.2221922f, 0.6472238f, -5.979921E-09f), D3DXVECTOR3(0.2444024f, 0.7119199f, 0.06840233f), D3DXVECTOR3(0.2601074f, 0.7576669f, 0.04836775f), D3DXVECTOR3(0.2286974f, 0.6661729f, 0.04836775f), D3DXVECTOR3(0.8565058f, -0.06866174f, -4.687798E-11f), D3DXVECTOR3(0.8402017f, -0.07153661f, 0.06178666f), D3DXVECTOR3(0.8750901f, -0.03482031f, 1.21745E-10f), D3DXVECTOR3(0.8555959f, -0.03825768f, 0.07387591f), D3DXVECTOR3(0.8402017f, -0.07153661f, 0.06178666f), D3DXVECTOR3(0.7956579f, -0.07939088f, 0.1070175f), D3DXVECTOR3(0.8023366f, -0.04764872f, 0.1279567f), D3DXVECTOR3(0.73481f, -0.09012f, 0.1235731f), D3DXVECTOR3(0.7295831f, -0.06047713f, 0.1477517f), D3DXVECTOR3(0.7956579f, -0.07939088f, 0.1070175f), D3DXVECTOR3(0.6568297f, -0.07330552f, 0.1279567f), D3DXVECTOR3(0.6739621f, -0.1008491f, 0.1070175f), D3DXVECTOR3(0.73481f, -0.09012f, 0.1235731f), D3DXVECTOR3(0.6739621f, -0.1008491f, 0.1070175f), D3DXVECTOR3(0.6294184f, -0.1087034f, 0.06178655f), D3DXVECTOR3(0.6035704f, -0.08269656f, 0.07387579f), D3DXVECTOR3(0.5840762f, -0.08613393f, -1.994514E-08f), D3DXVECTOR3(0.6131142f, -0.1115782f, -1.208573E-08f), D3DXVECTOR3(0.6294184f, -0.1087034f, 0.06178655f), D3DXVECTOR3(0.6294184f, -0.1087034f, -0.06178657f), D3DXVECTOR3(0.6035705f, -0.08269657f, -0.07387583f), D3DXVECTOR3(0.6131142f, -0.1115782f, -1.208573E-08f), D3DXVECTOR3(0.6739621f, -0.1008491f, -0.1070175f), D3DXVECTOR3(0.6568297f, -0.07330553f, -0.1279567f), D3DXVECTOR3(0.6294184f, -0.1087034f, -0.06178657f), D3DXVECTOR3(0.7295831f, -0.06047713f, -0.1477517f), D3DXVECTOR3(0.6739621f, -0.1008491f, -0.1070175f), D3DXVECTOR3(0.73481f, -0.09012f, -0.1235731f), D3DXVECTOR3(0.8023366f, -0.04764872f, -0.1279567f), D3DXVECTOR3(0.7956579f, -0.07939088f, -0.1070175f), D3DXVECTOR3(0.73481f, -0.09012f, -0.1235731f), D3DXVECTOR3(0.8023366f, -0.04764872f, -0.1279567f), D3DXVECTOR3(0.7956579f, -0.07939088f, -0.1070175f), D3DXVECTOR3(0.8555959f, -0.03825768f, -0.07387582f), D3DXVECTOR3(0.8402017f, -0.07153661f, -0.06178656f), D3DXVECTOR3(0.8565058f, -0.06866174f, -4.687798E-11f), D3DXVECTOR3(0.6341127f, 0.4809631f, -2.745459E-08f), D3DXVECTOR3(0.6341127f, 0.4809631f, -2.745459E-08f), D3DXVECTOR3(0.6568297f, -0.07330552f, 0.1279567f), D3DXVECTOR3(0.6035704f, -0.08269656f, 0.07387579f), D3DXVECTOR3(0.6341127f, 0.4809631f, -2.745459E-08f), D3DXVECTOR3(0.5840762f, -0.08613393f, -1.994514E-08f), D3DXVECTOR3(0.6341127f, 0.4809631f, -2.745459E-08f), D3DXVECTOR3(0.6035705f, -0.08269657f, -0.07387583f), D3DXVECTOR3(0.6341127f, 0.4809631f, -2.745459E-08f), D3DXVECTOR3(0.6568297f, -0.07330553f, -0.1279567f), D3DXVECTOR3(0.73481f, -0.09012f, -0.1235731f), D3DXVECTOR3(0.6739621f, -0.1008491f, -0.1070175f), D3DXVECTOR3(0.7956579f, -0.07939088f, -0.1070175f), D3DXVECTOR3(0.8402017f, -0.07153661f, -0.06178656f), D3DXVECTOR3(0.6294184f, -0.1087034f, -0.06178657f), D3DXVECTOR3(0.6131142f, -0.1115782f, -1.208573E-08f), D3DXVECTOR3(0.8565058f, -0.06866174f, -4.687798E-11f), D3DXVECTOR3(0.6294184f, -0.1087034f, 0.06178655f), D3DXVECTOR3(0.8402017f, -0.07153661f, 0.06178666f), D3DXVECTOR3(0.7956579f, -0.07939088f, 0.1070175f), D3DXVECTOR3(0.6739621f, -0.1008491f, 0.1070175f), D3DXVECTOR3(0.73481f, -0.09012f, 0.1235731f), D3DXVECTOR3(0.2601074f, 0.7576669f, -0.04836776f), D3DXVECTOR3(0.2286974f, 0.6661729f, -0.04836776f), D3DXVECTOR3(0.2666126f, 0.7766161f,0.0f), D3DXVECTOR3(0.2444024f, 0.7119199f, -0.06840233f), D3DXVECTOR3(0.2666126f, 0.7766161f,0.0f), D3DXVECTOR3(0.2221922f, 0.6472238f, -5.979921E-09f), D3DXVECTOR3(0.2601074f, 0.7576669f, 0.04836775f), D3DXVECTOR3(0.2286974f, 0.6661729f, 0.04836775f), D3DXVECTOR3(0.2666126f, 0.7766161f,0.0f), D3DXVECTOR3(-3.589167E-08f, 0.8211058f,0.0f), D3DXVECTOR3(0.2601074f, 0.7576669f, -0.04836776f), D3DXVECTOR3(-3.501594E-08f, 0.8010713f, -0.04836776f), D3DXVECTOR3(0.2444024f, 0.7119199f, -0.06840233f), D3DXVECTOR3(-3.290172E-08f, 0.7527035f, -0.06840233f), D3DXVECTOR3(-3.078749E-08f, 0.7043357f, -0.04836776f), D3DXVECTOR3(0.2286974f, 0.6661729f, -0.04836776f), D3DXVECTOR3(0.2221922f, 0.6472238f, -5.979921E-09f), D3DXVECTOR3(-2.991175E-08f, 0.6843011f, -5.979921E-09f), D3DXVECTOR3(-3.078749E-08f, 0.7043357f, 0.04836775f), D3DXVECTOR3(0.2286974f, 0.6661729f, 0.04836775f), D3DXVECTOR3(0.2444024f, 0.7119199f, 0.06840233f), D3DXVECTOR3(-3.290172E-08f, 0.7527035f, 0.06840233f), D3DXVECTOR3(-3.501594E-08f, 0.8010713f, 0.04836775f), D3DXVECTOR3(0.2601074f, 0.7576669f, 0.04836775f), D3DXVECTOR3(-3.589167E-08f, 0.8211058f,0.0f), D3DXVECTOR3(0.2666126f, 0.7766161f,0.0f), D3DXVECTOR3(-3.501594E-08f, 0.8010713f, -0.04836776f), D3DXVECTOR3(-3.589167E-08f, 0.8211058f,0.0f), D3DXVECTOR3(-0.2601076f, 0.7576669f, -0.04836776f), D3DXVECTOR3(-0.2666128f, 0.776616f,0.0f), D3DXVECTOR3(-0.4920281f, 0.6321578f, -0.04836776f), D3DXVECTOR3(-0.46232f, 0.5939888f, -0.06840233f), D3DXVECTOR3(-0.2444026f, 0.7119199f, -0.06840233f), D3DXVECTOR3(-3.290172E-08f, 0.7527035f, -0.06840233f), D3DXVECTOR3(-0.2286976f, 0.6661729f, -0.04836776f), D3DXVECTOR3(-3.078749E-08f, 0.7043357f, -0.04836776f), D3DXVECTOR3(-0.5043336f, 0.6479679f,0.0f), D3DXVECTOR3(-0.6874022f, 0.4491023f,0.0f), D3DXVECTOR3(-0.67063f, 0.4381445f, -0.04836776f), D3DXVECTOR3(-0.6301381f, 0.4116898f, -0.06840233f), D3DXVECTOR3(-0.5896463f, 0.3852352f, -0.04836776f), D3DXVECTOR3(-0.4326119f, 0.5558199f, -0.04836776f), D3DXVECTOR3(-0.7959802f, 0.2015696f,0.0f), D3DXVECTOR3(-0.7765587f, 0.1966514f, -0.04836776f), D3DXVECTOR3(-0.729671f, 0.1847778f, -0.06840233f), D3DXVECTOR3(-0.6827832f, 0.1729042f, -0.04836776f), D3DXVECTOR3(-0.572874f, 0.3742773f, -5.979921E-09f), D3DXVECTOR3(-0.4203064f, 0.5400098f, -5.979921E-09f), D3DXVECTOR3(-0.8183013f, -0.06780636f,0.0f), D3DXVECTOR3(-0.7983351f, -0.06615191f, -0.04836776f), D3DXVECTOR3(-0.7501326f, -0.06215774f, -0.06840233f), D3DXVECTOR3(-0.7019301f, -0.05816356f, -0.04836776f), D3DXVECTOR3(-0.6633617f, 0.167986f, -5.979921E-09f), D3DXVECTOR3(-0.6827832f, 0.1729042f, 0.04836775f), D3DXVECTOR3(-0.5896463f, 0.3852352f, 0.04836775f), D3DXVECTOR3(-0.7335998f, -0.3217866f, -0.04836776f), D3DXVECTOR3(-0.6893058f, -0.3023575f, -0.06840233f), D3DXVECTOR3(-0.6450118f, -0.2829284f, -0.04836776f), D3DXVECTOR3(-0.6266648f, -0.2748806f, -5.979921E-09f), D3DXVECTOR3(-0.6819639f, -0.05650912f, -5.979921E-09f), D3DXVECTOR3(-0.7019301f, -0.05816356f, 0.04836775f), D3DXVECTOR3(-0.7519467f, -0.3298344f,0.0f), D3DXVECTOR3(-0.5893673f, -0.5425507f, -0.04836776f), D3DXVECTOR3(-0.5537819f, -0.5097921f, -0.06840233f), D3DXVECTOR3(-0.5181966f, -0.4770336f, -0.04836776f), D3DXVECTOR3(-0.5034567f, -0.4634645f, -5.979921E-09f), D3DXVECTOR3(-0.6450118f, -0.2829284f, 0.04836775f), D3DXVECTOR3(-0.6041072f, -0.5561198f,0.0f), D3DXVECTOR3(-0.3908032f, -0.722141f,0.0f), D3DXVECTOR3(-0.3812678f, -0.7045211f, -0.04836776f), D3DXVECTOR3(-0.3582473f, -0.6619829f, -0.06840233f), D3DXVECTOR3(-0.3352268f, -0.6194448f, -0.04836776f), D3DXVECTOR3(-0.3256914f, -0.6018249f, -5.979921E-09f), D3DXVECTOR3(-0.3352268f, -0.6194448f, 0.04836775f), D3DXVECTOR3(-0.5181966f, -0.4770336f, 0.04836775f), D3DXVECTOR3(-0.1351495f, -0.809907f,0.0f), D3DXVECTOR3(-0.1318519f, -0.7901458f, -0.04836776f), D3DXVECTOR3(-0.1238909f, -0.7424376f, -0.06840233f), D3DXVECTOR3(-0.1159298f, -0.6947296f, -0.04836776f), D3DXVECTOR3(-0.1126322f, -0.6749682f, -5.979921E-09f), D3DXVECTOR3(-0.1159298f, -0.6947296f, 0.04836775f), D3DXVECTOR3(0.1351496f, -0.809907f,0.0f), D3DXVECTOR3(0.1318521f, -0.7901456f, -0.04836776f), D3DXVECTOR3(0.123891f, -0.7424375f, -0.06840233f), D3DXVECTOR3(0.1159299f, -0.6947295f, -0.04836776f), D3DXVECTOR3(0.1126323f, -0.6749682f, -5.979921E-09f), D3DXVECTOR3(0.3812678f, -0.7045211f, -0.04836776f), D3DXVECTOR3(0.3582473f, -0.6619829f, -0.06840233f), D3DXVECTOR3(0.3352268f, -0.6194448f, -0.04836776f), D3DXVECTOR3(0.3256914f, -0.6018249f, -5.979921E-09f), D3DXVECTOR3(0.1159299f, -0.6947295f, 0.04836775f), D3DXVECTOR3(0.3908032f, -0.722141f,0.0f), D3DXVECTOR3(0.5893673f, -0.5425507f, -0.04836776f), D3DXVECTOR3(0.553782f, -0.5097921f, -0.06840233f), D3DXVECTOR3(0.5181966f, -0.4770336f, -0.04836776f), D3DXVECTOR3(0.5034568f, -0.4634645f, -5.979921E-09f), D3DXVECTOR3(0.3352268f, -0.6194448f, 0.04836775f), D3DXVECTOR3(0.123891f, -0.7424375f, 0.06840233f), D3DXVECTOR3(0.6041072f, -0.5561197f,0.0f), D3DXVECTOR3(0.7519467f, -0.3298344f,0.0f), D3DXVECTOR3(0.7335998f, -0.3217866f, -0.04836776f), D3DXVECTOR3(0.6893058f, -0.3023575f, -0.06840233f), D3DXVECTOR3(0.6450118f, -0.2829284f, -0.04836776f), D3DXVECTOR3(0.6266648f, -0.2748806f, -5.979921E-09f), D3DXVECTOR3(0.6450118f, -0.2829284f, 0.04836775f), D3DXVECTOR3(0.5181966f, -0.4770336f, 0.04836775f), D3DXVECTOR3(0.8183013f, -0.06780633f,0.0f), D3DXVECTOR3(0.7983351f, -0.06615189f, -0.04836776f), D3DXVECTOR3(0.7501326f, -0.06215771f, -0.06840233f), D3DXVECTOR3(0.7019301f, -0.05816354f, -0.04836776f), D3DXVECTOR3(0.6819639f, -0.0565091f, -5.979921E-09f), D3DXVECTOR3(0.7019301f, -0.05816354f, 0.04836775f), D3DXVECTOR3(0.6893058f, -0.3023575f, 0.06840233f), D3DXVECTOR3(0.553782f, -0.5097921f, 0.06840233f), D3DXVECTOR3(0.3582473f, -0.6619829f, 0.06840233f), D3DXVECTOR3(0.7501326f, -0.06215771f, 0.06840233f), D3DXVECTOR3(0.7983351f, -0.06615189f, 0.04836775f), D3DXVECTOR3(0.7335998f, -0.3217866f, 0.04836775f), D3DXVECTOR3(0.5893673f, -0.5425507f, 0.04836775f), D3DXVECTOR3(0.3812678f, -0.7045211f, 0.04836775f), D3DXVECTOR3(0.1318521f, -0.7901456f, 0.04836775f), D3DXVECTOR3(-0.1238909f, -0.7424376f, 0.06840233f), D3DXVECTOR3(0.8183013f, -0.06780633f,0.0f), D3DXVECTOR3(0.7519467f, -0.3298344f,0.0f), D3DXVECTOR3(0.6041072f, -0.5561197f,0.0f), D3DXVECTOR3(0.3908032f, -0.722141f,0.0f), D3DXVECTOR3(0.1351496f, -0.809907f,0.0f), D3DXVECTOR3(-0.1318519f, -0.7901458f, 0.04836775f), D3DXVECTOR3(-0.3812678f, -0.7045211f, 0.04836775f), D3DXVECTOR3(-0.3582473f, -0.6619829f, 0.06840233f), D3DXVECTOR3(-0.5537819f, -0.5097921f, 0.06840233f), D3DXVECTOR3(-0.1351495f, -0.809907f,0.0f), D3DXVECTOR3(-0.3908032f, -0.722141f,0.0f), D3DXVECTOR3(-0.5893673f, -0.5425507f, 0.04836775f), D3DXVECTOR3(-0.6041072f, -0.5561198f,0.0f), D3DXVECTOR3(-0.7519467f, -0.3298344f,0.0f), D3DXVECTOR3(-0.7335998f, -0.3217866f, 0.04836775f), D3DXVECTOR3(-0.6893058f, -0.3023575f, 0.06840233f), D3DXVECTOR3(-0.7501326f, -0.06215774f, 0.06840233f), D3DXVECTOR3(-0.729671f, 0.1847778f, 0.06840233f), D3DXVECTOR3(-0.8183013f, -0.06780636f,0.0f), D3DXVECTOR3(-0.7983351f, -0.06615191f, 0.04836775f), D3DXVECTOR3(-0.7765587f, 0.1966514f, 0.04836775f), D3DXVECTOR3(-0.7959802f, 0.2015696f, 0.0f), D3DXVECTOR3(-0.67063f, 0.4381445f, 0.04836775f), D3DXVECTOR3(-0.6301381f, 0.4116898f, 0.06840233f), D3DXVECTOR3(-0.46232f, 0.5939888f, 0.06840233f), D3DXVECTOR3(-0.4326119f, 0.5558199f, 0.04836775f), D3DXVECTOR3(-0.6874022f, 0.4491023f, 0.0f), D3DXVECTOR3(-0.4920281f, 0.6321578f, 0.04836775f), D3DXVECTOR3(-0.2601076f, 0.7576669f, 0.04836775f), D3DXVECTOR3(-0.2444026f, 0.7119199f, 0.06840233f), D3DXVECTOR3(-0.2286976f, 0.6661729f, 0.04836775f), D3DXVECTOR3(-0.2221924f, 0.6472238f, -5.979921E-09f), D3DXVECTOR3(-0.5043336f, 0.6479679f, 0.0f), D3DXVECTOR3(-0.2666128f, 0.776616f, 0.0f), D3DXVECTOR3(-3.589167E-08f, 0.8211058f, 0.0f), D3DXVECTOR3(-3.501594E-08f, 0.8010713f, 0.04836775f), D3DXVECTOR3(-3.290172E-08f, 0.7527035f, 0.06840233f), D3DXVECTOR3(-3.078749E-08f, 0.7043357f, 0.04836775f), D3DXVECTOR3(-2.991175E-08f, 0.6843011f, -5.979921E-09f)
		};
		D3DXVECTOR3 normals[] =
		{
			D3DXVECTOR3(0.9059955f, 0.4232872f, -1.371837E-07f), D3DXVECTOR3(0.9059956f, 0.4232872f, -1.228431E-07f), D3DXVECTOR3(0.7785772f, 0.40082f, 0.4828673f), D3DXVECTOR3(0.7785773f, 0.40082f, 0.4828673f), D3DXVECTOR3(0.7785772f, 0.40082f, 0.4828673f), D3DXVECTOR3(0.4304637f, 0.3394382f, 0.8363509f), D3DXVECTOR3(0.4304637f, 0.3394382f, 0.8363509f), D3DXVECTOR3(0.4304637f, 0.3394382f, 0.8363509f), D3DXVECTOR3(-0.04506789f, 0.2555891f, 0.9657345f), D3DXVECTOR3(-0.04506789f, 0.2555891f, 0.9657345f), D3DXVECTOR3(-0.04506789f, 0.2555891f, 0.9657345f), D3DXVECTOR3(-0.5205989f, 0.1717402f, 0.8363504f), D3DXVECTOR3(-0.5205987f, 0.1717401f, -0.8363505f), D3DXVECTOR3(-0.5205988f, 0.1717401f, -0.8363506f), D3DXVECTOR3(-0.04506751f, 0.2555891f, -0.9657345f), D3DXVECTOR3(-0.04506748f, 0.2555891f, -0.9657345f), D3DXVECTOR3(-0.04506751f, 0.2555891f, -0.9657345f), D3DXVECTOR3(0.4304642f, 0.3394381f, -0.8363506f), D3DXVECTOR3(0.4304642f, 0.3394381f, -0.8363506f), D3DXVECTOR3(0.4304642f, 0.3394381f, -0.8363506f), D3DXVECTOR3(0.7785773f, 0.4008199f, -0.4828672f), D3DXVECTOR3(0.7785774f, 0.4008199f, -0.4828672f), D3DXVECTOR3(0.7785773f, 0.4008199f, -0.4828672f), D3DXVECTOR3(0.9059956f, 0.4232872f, -1.228431E-07f), D3DXVECTOR3(0.9458174f, -0.3246992f, -3.694643E-07f), D3DXVECTOR3(0.9458173f, -0.3246992f, -3.694643E-07f), D3DXVECTOR3(0.9458174f, -0.3246992f, -3.694643E-07f), D3DXVECTOR3(0.9458171f, -0.3246998f, 9.610906E-07f), D3DXVECTOR3(0.9458171f, -0.3246998f, 9.610907E-07f), D3DXVECTOR3(0.9458172f, -0.3246998f, 9.610907E-07f), D3DXVECTOR3(0.8765272f, -0.4813523f, -1.317158E-07f), D3DXVECTOR3(0.7736639f, -0.4994903f, 0.3898124f), D3DXVECTOR3(0.8765272f, -0.4813523f, -1.081172E-07f), D3DXVECTOR3(0.7736638f, -0.4994903f, 0.3898124f), D3DXVECTOR3(0.7736639f, -0.4994903f, 0.3898124f), D3DXVECTOR3(0.4926363f, -0.5490433f, 0.6751748f), D3DXVECTOR3(0.4926364f, -0.5490433f, 0.6751747f), D3DXVECTOR3(0.1087466f, -0.6167336f, 0.7796243f), D3DXVECTOR3(0.1087462f, -0.6167336f, 0.7796242f), D3DXVECTOR3(0.4926363f, -0.5490433f, 0.6751748f), D3DXVECTOR3(-0.2751434f, -0.6844237f, 0.6751741f), D3DXVECTOR3(-0.2751433f, -0.6844237f, 0.6751743f), D3DXVECTOR3(0.1087466f, -0.6167336f, 0.7796243f), D3DXVECTOR3(-0.2751433f, -0.6844237f, 0.6751743f), D3DXVECTOR3(-0.5561701f, -0.7339765f, 0.389812f), D3DXVECTOR3(-0.5561701f, -0.7339765f, 0.3898118f), D3DXVECTOR3(-0.6590332f, -0.7521139f, -1.189289E-07f), D3DXVECTOR3(-0.6590332f, -0.7521139f, -1.229347E-07f), D3DXVECTOR3(-0.5561701f, -0.7339765f, 0.389812f), D3DXVECTOR3(-0.5561704f, -0.7339762f, -0.3898122f), D3DXVECTOR3(-0.5561703f, -0.7339761f, -0.3898122f), D3DXVECTOR3(-0.6590332f, -0.7521139f, -1.229347E-07f), D3DXVECTOR3(-0.2751434f, -0.6844237f, -0.6751744f), D3DXVECTOR3(-0.2751433f, -0.6844236f, -0.6751744f), D3DXVECTOR3(-0.5561704f, -0.7339762f, -0.3898122f), D3DXVECTOR3(0.1087466f, -0.6167334f, -0.7796243f), D3DXVECTOR3(-0.2751434f, -0.6844237f, -0.6751744f), D3DXVECTOR3(0.1087466f, -0.6167335f, -0.7796243f), D3DXVECTOR3(0.4926367f, -0.5490429f, -0.6751748f), D3DXVECTOR3(0.492637f, -0.5490429f, -0.6751746f), D3DXVECTOR3(0.1087466f, -0.6167335f, -0.7796243f), D3DXVECTOR3(0.4926367f, -0.5490429f, -0.6751748f), D3DXVECTOR3(0.492637f, -0.5490429f, -0.6751746f), D3DXVECTOR3(0.7736641f, -0.4994899f, -0.3898124f), D3DXVECTOR3(0.7736642f, -0.4994899f, -0.3898122f), D3DXVECTOR3(0.8765272f, -0.4813523f, -1.317158E-07f), D3DXVECTOR3(-0.8687121f, 0.1103585f, 0.4828669f), D3DXVECTOR3(-0.5205989f, 0.1717402f, 0.8363504f), D3DXVECTOR3(-0.5205989f, 0.1717402f, 0.8363504f), D3DXVECTOR3(-0.8687121f, 0.1103585f, 0.4828669f), D3DXVECTOR3(-0.99613f, 0.08789111f, -3.821545E-07f), D3DXVECTOR3(-0.99613f, 0.08789112f, -3.787663E-07f), D3DXVECTOR3(-0.8687118f, 0.1103584f, -0.4828674f), D3DXVECTOR3(-0.8687118f, 0.1103584f, -0.4828674f), D3DXVECTOR3(-0.5205987f, 0.1717401f, -0.8363505f), D3DXVECTOR3(-0.5205988f, 0.1717401f, -0.8363506f), D3DXVECTOR3(0.1736481f, -0.9848077f, 1.524726E-08f), D3DXVECTOR3(0.1736481f, -0.9848077f, 1.524726E-08f), D3DXVECTOR3(0.1736481f, -0.9848077f, 1.524726E-08f), D3DXVECTOR3(0.1736481f, -0.9848077f, 1.524726E-08f), D3DXVECTOR3(0.1736481f, -0.9848077f, 1.524726E-08f), D3DXVECTOR3(0.1736481f, -0.9848078f, 1.524726E-08f), D3DXVECTOR3(0.1736481f, -0.9848078f, 1.524726E-08f), D3DXVECTOR3(0.1736481f, -0.9848077f, 1.524726E-08f), D3DXVECTOR3(0.1736481f, -0.9848077f, 1.524726E-08f), D3DXVECTOR3(0.1736481f, -0.9848077f, 1.524726E-08f), D3DXVECTOR3(0.1736481f, -0.9848077f, 1.524726E-08f), D3DXVECTOR3(0.1736481f, -0.9848077f, 1.524726E-08f), D3DXVECTOR3(0.9458171f, -0.3246998f, 6.88348E-07f), D3DXVECTOR3(0.9458171f, -0.3246998f, 6.88348E-07f), D3DXVECTOR3(0.9458171f, -0.3246998f, 6.88348E-07f), D3DXVECTOR3(0.9458171f, -0.3246998f, 6.88348E-07f), D3DXVECTOR3(0.9458173f, -0.3246995f, -3.401E-07f), D3DXVECTOR3(0.9458172f, -0.3246994f, -3.401E-07f), D3DXVECTOR3(0.9458172f, -0.3246994f, -3.401E-07f), D3DXVECTOR3(0.9458173f, -0.3246994f, -3.401E-07f), D3DXVECTOR3(0.1645946f, 0.9863613f, 6.405578E-08f), D3DXVECTOR3(-1.380007E-07f,1.0f, 5.411793E-08f), D3DXVECTOR3(0.1202883f, 0.7208481f, -0.682575f), D3DXVECTOR3(-9.311898E-08f, 0.7261207f, -0.6875673f), D3DXVECTOR3(0.006711854f, 0.04022167f, -0.9991683f), D3DXVECTOR3(-1.799099E-08f, 0.04022265f, -0.9991907f), D3DXVECTOR3(3.013317E-08f, -0.6858391f, -0.7277533f), D3DXVECTOR3(-0.113704f, -0.6813912f, -0.7230334f), D3DXVECTOR3(-0.1645945f, -0.9863613f, 0.0f), D3DXVECTOR3(8.989642E-08f, -1.0f, 9.988491E-09f), D3DXVECTOR3(2.762207E-08f, -0.6858392f, 0.7277532f), D3DXVECTOR3(-0.1137041f, -0.6813913f, 0.7230334f), D3DXVECTOR3(0.006711856f, 0.04022165f, 0.9991683f), D3DXVECTOR3(-1.927606E-08f, 0.04022267f, 0.9991908f), D3DXVECTOR3(-8.779789E-08f, 0.7261208f, 0.6875672f), D3DXVECTOR3(0.1202883f, 0.7208481f, 0.682575f), D3DXVECTOR3(-1.380007E-07f,1.0f, 5.411793E-08f), D3DXVECTOR3(0.1645946f, 0.9863613f, 6.405578E-08f), D3DXVECTOR3(-9.311898E-08f, 0.7261207f, -0.6875673f), D3DXVECTOR3(-1.380007E-07f,1.0f, 5.411793E-08f), D3DXVECTOR3(-0.2357711f, 0.6867775f, -0.6875672f), D3DXVECTOR3(-0.3246994f, 0.9458172f, 3.247076E-08f), D3DXVECTOR3(-0.4459926f, 0.5730113f, -0.6875672f), D3DXVECTOR3(-0.02470523f, 0.03174134f, -0.9991907f), D3DXVECTOR3(-0.01306031f, 0.03804342f, -0.9991908f), D3DXVECTOR3(-1.799099E-08f, 0.04022265f, -0.9991907f), D3DXVECTOR3(0.2226916f, -0.6486784f, -0.7277532f), D3DXVECTOR3(3.013317E-08f, -0.6858391f, -0.7277533f), D3DXVECTOR3(-0.6142127f, 0.7891405f, 1.082359E-08f), D3DXVECTOR3(-0.8371665f, 0.5469482f, 2.164717E-08f), D3DXVECTOR3(-0.6078842f, 0.3971505f, -0.6875671f), D3DXVECTOR3(-0.03367296f, 0.02199964f, -0.9991908f), D3DXVECTOR3(0.5741615f, -0.3751186f, -0.7277532f), D3DXVECTOR3(0.4212511f, -0.5412235f, -0.7277531f), D3DXVECTOR3(-0.9694002f, 0.2454855f, 3.247075E-08f), D3DXVECTOR3(-0.7039018f, 0.1782521f, -0.6875671f), D3DXVECTOR3(-0.03899198f, 0.009874066f, -0.9991908f), D3DXVECTOR3(0.6648525f, -0.1683635f, -0.7277533f), D3DXVECTOR3(0.8371665f, -0.5469482f, -9.98849E-09f), D3DXVECTOR3(0.6142127f, -0.7891406f, -9.98849E-09f), D3DXVECTOR3(-0.9965845f, -0.08257929f, 4.329434E-08f), D3DXVECTOR3(-0.7236408f, -0.05996253f, -0.6875671f), D3DXVECTOR3(-0.04008535f, -0.003321519f, -0.9991908f), D3DXVECTOR3(0.6834964f, 0.05663615f, -0.7277533f), D3DXVECTOR3(0.9694002f, -0.2454854f, 0.0f), D3DXVECTOR3(0.6648526f, -0.1683635f, 0.7277532f), D3DXVECTOR3(0.5741615f, -0.3751186f, 0.7277531f), D3DXVECTOR3(-0.6649622f, -0.2916794f, -0.687567f), D3DXVECTOR3(-0.03683478f, -0.01615721f, -0.9991908f), D3DXVECTOR3(0.6280731f, 0.2754985f, -0.7277532f), D3DXVECTOR3(0.9157734f, 0.4016954f, 9.98849E-09f), D3DXVECTOR3(0.9965844f, 0.0825794f, 0.0f), D3DXVECTOR3(0.6834965f, 0.05663615f, 0.7277533f), D3DXVECTOR3(-0.9157733f, -0.4016955f, 7.576507E-08f), D3DXVECTOR3(-0.5342244f, -0.4917881f, -0.6875673f), D3DXVECTOR3(-0.02959257f, -0.02724189f, -0.9991908f), D3DXVECTOR3(0.5045884f, 0.4645062f, -0.727753f), D3DXVECTOR3(0.735724f, 0.6772815f, 0.0f), D3DXVECTOR3(0.6280732f, 0.2754986f, 0.727753f), D3DXVECTOR3(-0.735724f, -0.6772814f, 6.494152E-08f), D3DXVECTOR3(-0.4759474f, -0.8794737f, 6.494152E-08f), D3DXVECTOR3(-0.3455953f, -0.6386039f, -0.6875675f), D3DXVECTOR3(-0.01914371f, -0.03537445f, -0.9991908f), D3DXVECTOR3(0.3264235f, 0.6031776f, -0.727753f), D3DXVECTOR3(0.4759476f, 0.8794737f, 0.0f), D3DXVECTOR3(0.3264236f, 0.6031777f, 0.7277529f), D3DXVECTOR3(0.5045884f, 0.4645063f, 0.727753f), D3DXVECTOR3(-0.1645945f, -0.9863613f, 3.247075E-08f), D3DXVECTOR3(-0.1195154f, -0.7162174f, -0.6875672f), D3DXVECTOR3(-0.006620373f, -0.03967379f, -0.9991908f), D3DXVECTOR3(0.1128854f, 0.6764852f, -0.7277531f), D3DXVECTOR3(0.1645945f, 0.9863613f, 9.988488E-09f), D3DXVECTOR3(0.1128854f, 0.6764854f, 0.727753f), D3DXVECTOR3(0.1645947f, -0.9863613f, 3.247076E-08f), D3DXVECTOR3(0.1195156f, -0.7162173f, -0.6875673f), D3DXVECTOR3(0.006620397f, -0.03967382f, -0.9991907f), D3DXVECTOR3(-0.1128855f, 0.6764853f, -0.7277529f), D3DXVECTOR3(-0.1645946f, 0.9863612f, 9.988488E-09f), D3DXVECTOR3(0.3455953f, -0.638604f, -0.6875673f), D3DXVECTOR3(0.01914376f, -0.03537461f, -0.9991908f), D3DXVECTOR3(-0.3264235f, 0.6031777f, -0.7277529f), D3DXVECTOR3(-0.4759475f, 0.8794736f, 0.0f), D3DXVECTOR3(-0.1128855f, 0.6764854f, 0.7277529f), D3DXVECTOR3(0.4759474f, -0.8794737f, 6.494152E-08f), D3DXVECTOR3(0.5342246f, -0.4917883f, -0.6875671f), D3DXVECTOR3(0.02959256f, -0.02724191f, -0.9991907f), D3DXVECTOR3(-0.5045884f, 0.4645063f, -0.7277529f), D3DXVECTOR3(-0.7357241f, 0.6772813f, 0.0f), D3DXVECTOR3(-0.3264236f, 0.6031778f, 0.7277529f), D3DXVECTOR3(0.006620401f, -0.03967384f, 0.9991908f), D3DXVECTOR3(0.7357241f, -0.6772814f, 6.494149E-08f), D3DXVECTOR3(0.9157733f, -0.4016955f, 5.411789E-08f), D3DXVECTOR3(0.6649624f, -0.2916794f, -0.6875669f), D3DXVECTOR3(0.03683472f, -0.01615719f, -0.9991908f), D3DXVECTOR3(-0.6280732f, 0.2754985f, -0.7277531f), D3DXVECTOR3(-0.9157733f, 0.4016954f, 0.0f), D3DXVECTOR3(-0.6280733f, 0.2754985f, 0.727753f), D3DXVECTOR3(-0.5045885f, 0.4645064f, 0.7277529f), D3DXVECTOR3(0.9694002f, -0.2454853f, 8.540772E-08f), D3DXVECTOR3(0.7084532f, -0.1794045f, -0.6825746f), D3DXVECTOR3(0.03953008f, -0.01001027f, -0.9991682f), D3DXVECTOR3(-0.6696741f, 0.1695845f, -0.7230335f), D3DXVECTOR3(-0.9694003f, 0.2454855f, 0.0f), D3DXVECTOR3(-0.6696742f, 0.1695846f, 0.7230335f), D3DXVECTOR3(0.03683477f, -0.01615721f, 0.9991908f), D3DXVECTOR3(0.0295926f, -0.02724192f, 0.9991908f), D3DXVECTOR3(0.01914376f, -0.03537461f, 0.9991907f), D3DXVECTOR3(0.03953006f, -0.01001028f, 0.9991683f), D3DXVECTOR3(0.7084532f, -0.1794046f, 0.6825746f), D3DXVECTOR3(0.6649624f, -0.2916794f, 0.6875668f), D3DXVECTOR3(0.5342246f, -0.4917882f, 0.687567f), D3DXVECTOR3(0.3455953f, -0.638604f, 0.6875673f), D3DXVECTOR3(0.1195156f, -0.7162173f, 0.6875672f), D3DXVECTOR3(-0.006620373f, -0.0396738f, 0.9991908f), D3DXVECTOR3(0.9694002f, -0.2454853f, 8.540772E-08f), D3DXVECTOR3(0.9157733f, -0.4016955f, 5.411789E-08f), D3DXVECTOR3(0.7357241f, -0.6772814f, 6.494149E-08f), D3DXVECTOR3(0.4759474f, -0.8794737f, 6.494152E-08f), D3DXVECTOR3(0.1645947f, -0.9863613f, 3.247076E-08f), D3DXVECTOR3(-0.1195154f, -0.7162175f, 0.6875672f), D3DXVECTOR3(-0.3455953f, -0.6386039f, 0.6875674f), D3DXVECTOR3(-0.01914372f, -0.03537444f, 0.9991908f), D3DXVECTOR3(-0.02959258f, -0.02724191f, 0.9991907f), D3DXVECTOR3(-0.1645945f, -0.9863613f, 3.247075E-08f), D3DXVECTOR3(-0.4759474f, -0.8794737f, 6.494152E-08f), D3DXVECTOR3(-0.5342244f, -0.4917881f, 0.6875673f), D3DXVECTOR3(-0.735724f, -0.6772814f, 6.494152E-08f), D3DXVECTOR3(-0.9157733f, -0.4016955f, 7.576507E-08f), D3DXVECTOR3(-0.6649622f, -0.2916794f, 0.6875669f), D3DXVECTOR3(-0.03683475f, -0.01615722f, 0.9991908f), D3DXVECTOR3(-0.04008533f, -0.003321527f, 0.9991907f), D3DXVECTOR3(-0.03899201f, 0.009874073f, 0.9991908f), D3DXVECTOR3(-0.9965845f, -0.08257929f, 4.329434E-08f), D3DXVECTOR3(-0.7236408f, -0.05996254f, 0.6875671f), D3DXVECTOR3(-0.7039018f, 0.1782521f, 0.6875671f), D3DXVECTOR3(-0.9694002f, 0.2454855f, 3.247075E-08f), D3DXVECTOR3(-0.6078841f, 0.3971505f, 0.687567f), D3DXVECTOR3(-0.03367299f, 0.02199966f, 0.9991907f), D3DXVECTOR3(-0.02470526f, 0.03174135f, 0.9991907f), D3DXVECTOR3(0.4212511f, -0.5412235f, 0.727753f), D3DXVECTOR3(-0.8371665f, 0.5469482f, 2.164717E-08f), D3DXVECTOR3(-0.4459926f, 0.5730113f, 0.6875672f), D3DXVECTOR3(-0.2357711f, 0.6867775f, 0.6875672f), D3DXVECTOR3(-0.01306031f, 0.03804342f, 0.9991907f), D3DXVECTOR3(0.2226916f, -0.6486784f, 0.7277532f), D3DXVECTOR3(0.3246996f, -0.9458172f, 9.988489E-09f), D3DXVECTOR3(-0.6142127f, 0.7891405f, 1.082359E-08f), D3DXVECTOR3(-0.3246994f, 0.9458172f, 3.247076E-08f), D3DXVECTOR3(-1.380007E-07f, 1.0f, 5.411793E-08f), D3DXVECTOR3(-8.779789E-08f, 0.7261208f, 0.6875672f), D3DXVECTOR3(-1.927606E-08f, 0.04022267f, 0.9991908f), D3DXVECTOR3(2.762207E-08f, -0.6858392f, 0.7277532f), D3DXVECTOR3(8.989642E-08f, -1.0f, 9.988491E-09f)

		};
		UINT indices[] =
		{
			0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 32, 31, 34, 35, 33, 36, 33, 35, 37, 38, 39, 36, 39, 38, 40, 38, 41, 42, 41, 38, 43, 44, 40, 45, 40, 44, 46, 45, 47, 48, 47, 45, 49, 50, 51, 51, 50, 46, 52, 53, 54, 50, 54, 53, 55, 53, 56, 55, 56, 57, 58, 55, 59, 60, 59, 55, 61, 62, 63, 64, 63, 62, 63, 64, 32, 64, 65, 32, 66, 67, 68, 66, 68, 69, 70, 66, 69, 70, 69, 71, 72, 70, 71, 72, 71, 73, 74, 72, 73, 74, 73, 75, 76, 77, 78, 79, 78, 77, 80, 79, 77, 80, 81, 79, 79, 81, 82, 81, 83, 82, 84, 82, 83, 84, 83, 85, 86, 85, 83, 87, 85, 86, 88, 89, 90, 91, 89, 88, 92, 93, 94, 95, 94, 93, 96, 97, 98, 99, 98, 97, 98, 99, 100, 101, 100, 99, 102, 100, 101, 103, 100, 102, 103, 102, 104, 105, 104, 102, 106, 104, 105, 104, 106, 107, 107, 106, 108, 109, 108, 106, 110, 108, 109, 108, 110, 111, 110, 112, 111, 113, 111, 112, 114, 115, 116, 117, 116, 115, 116, 117, 118, 118, 119, 116, 116, 119, 120, 120, 114, 116, 114, 120, 121, 122, 121, 120, 121, 122, 123, 124, 118, 117, 124, 125, 118, 126, 118, 125, 126, 127, 118, 118, 127, 119, 127, 128, 119, 119, 128, 129, 129, 122, 119, 120, 119, 122, 125, 130, 126, 131, 126, 130, 131, 132, 126, 127, 126, 132, 132, 133, 127, 128, 127, 133, 128, 133, 134, 134, 135, 128, 129, 128, 135, 135, 122, 129, 130, 136, 131, 137, 131, 136, 131, 137, 132, 138, 132, 137, 139, 132, 138, 133, 132, 139, 133, 139, 140, 140, 134, 133, 141, 134, 140, 134, 141, 142, 142, 135, 134, 137, 136, 143, 137, 143, 138, 144, 138, 143, 138, 144, 139, 145, 139, 144, 146, 139, 145, 139, 146, 147, 147, 140, 139, 148, 140, 147, 140, 148, 141, 149, 143, 136, 143, 149, 150, 143, 150, 144, 151, 144, 150, 144, 151, 145, 152, 145, 151, 153, 145, 152, 145, 153, 146, 146, 153, 154, 154, 148, 146, 147, 146, 148, 155, 150, 149, 155, 156, 150, 157, 150, 156, 150, 157, 151, 158, 151, 157, 151, 158, 159, 151, 159, 152, 152, 159, 153, 160, 153, 159, 161, 153, 160, 153, 161, 162, 162, 154, 153, 156, 163, 157, 164, 157, 163, 164, 165, 157, 157, 165, 158, 165, 166, 158, 158, 166, 159, 159, 166, 160, 167, 160, 166, 168, 160, 167, 160, 168, 161, 163, 169, 164, 170, 164, 169, 164, 170, 165, 171, 165, 170, 165, 171, 166, 172, 166, 171, 173, 166, 172, 166, 173, 167, 167, 173, 168, 170, 169, 174, 170, 174, 171, 175, 171, 174, 176, 171, 175, 172, 171, 176, 177, 172, 176, 172, 177, 173, 173, 177, 178, 178, 168, 173, 179, 174, 169, 174, 179, 180, 174, 180, 175, 181, 175, 180, 182, 175, 181, 176, 175, 182, 183, 176, 182, 176, 183, 177, 177, 183, 184, 184, 178, 177, 178, 184, 185, 178, 185, 168, 186, 180, 179, 186, 187, 180, 188, 180, 187, 189, 180, 188, 181, 180, 189, 190, 181, 189, 182, 181, 190, 182, 190, 183, 191, 183, 190, 192, 183, 191, 183, 192, 193, 193, 184, 183, 187, 194, 188, 195, 188, 194, 188, 195, 189, 196, 189, 195, 197, 189, 196, 189, 197, 190, 190, 197, 191, 198, 191, 197, 199, 191, 198, 191, 199, 192, 192, 199, 200, 200, 201, 192, 193, 192, 201, 201, 202, 193, 184, 193, 202, 202, 185, 184, 203, 200, 199, 204, 200, 203, 200, 204, 205, 205, 206, 200, 201, 200, 206, 202, 201, 206, 202, 206, 207, 202, 207, 185, 185, 207, 208, 185, 208, 209, 209, 168, 185, 204, 210, 205, 211, 205, 210, 205, 211, 206, 212, 206, 211, 212, 213, 206, 207, 206, 213, 213, 214, 207, 208, 207, 214, 208, 214, 215, 215, 209, 208, 215, 216, 209, 217, 209, 216, 209, 217, 168, 161, 168, 217, 217, 218, 161, 162, 161, 218, 218, 154, 162, 219, 215, 214, 215, 219, 216, 220, 216, 219, 216, 220, 221, 221, 218, 216, 217, 216, 218, 222, 221, 220, 222, 223, 221, 224, 221, 223, 225, 221, 224, 221, 225, 218, 154, 218, 225, 225, 148, 154, 148, 225, 226, 226, 227, 148, 141, 148, 227, 223, 228, 224, 229, 224, 228, 226, 224, 229, 224, 226, 225, 229, 228, 230, 227, 229, 230, 226, 229, 227, 231, 230, 228, 230, 231, 232, 230, 232, 227, 233, 227, 232, 227, 233, 141, 142, 141, 233, 233, 234, 142, 235, 142, 234, 135, 142, 235, 236, 232, 231, 232, 236, 237, 232, 237, 233, 234, 233, 237, 237, 238, 234, 239, 234, 238, 239, 240, 234, 235, 234, 240, 235, 240, 135, 241, 135, 240, 122, 135, 241, 241, 123, 122, 242, 237, 236, 242, 243, 237, 238, 237, 243, 243, 244, 238, 245, 238, 244, 238, 245, 239, 246, 239, 245, 239, 246, 240, 247, 240, 246, 240, 247, 241, 248, 241, 247, 123, 241, 248
		};
		const UINT count = (UINT)(sizeof(positions) / sizeof(D3DXVECTOR3));
		gizmoIndexCount[0] = (UINT)(sizeof(indices) / sizeof(UINT));

		VertexColorNormal vertices[count];
		for (UINT i = 0; i < count; ++i)
		{
			vertices[i].Position = positions[i];
			vertices[i].Normal = normals[i];
		}
		//Create Vertex Buffer
		{
			D3D11_BUFFER_DESC desc = { 0 };
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.ByteWidth = sizeof(VertexTextureNormalTangent) * count;
			desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

			D3D11_SUBRESOURCE_DATA data = { 0 };
			data.pSysMem = vertices;

			HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &gizmoVertexBuffer[0]);
			assert(SUCCEEDED(hr));
		}

		//Create Index Buffer
		{
			D3D11_BUFFER_DESC desc = { 0 };
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.ByteWidth = sizeof(indices);
			desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

			D3D11_SUBRESOURCE_DATA data = { 0 };
			data.pSysMem = indices;

			HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &gizmoIndexBuffer[0]);
			assert(SUCCEEDED(hr));
		}
	}

	//translate
	{
		D3DXVECTOR3 positions[] =
		{
			D3DXVECTOR3(0.0f, 0.0f, 0.9f), D3DXVECTOR3(-8.742278E-09f, 0.2f, 0.0f), D3DXVECTOR3(0.09999999f, 0.1732051f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.9f), D3DXVECTOR3(-0.1f, 0.1732051f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.9f), D3DXVECTOR3(-0.1732051f, 0.1f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.9f), D3DXVECTOR3(-0.2f, 3.019916E-08f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.9f), D3DXVECTOR3(-0.1732051f, -0.09999995f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.9f), D3DXVECTOR3(-0.1000001f, -0.173205f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.9f), D3DXVECTOR3(-9.298245E-08f, -0.2f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.9f), D3DXVECTOR3(0.0999999f, -0.1732051f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.9f), D3DXVECTOR3(0.173205f, -0.1000001f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.9f), D3DXVECTOR3(0.2f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.9f), D3DXVECTOR3(0.1732051f, 0.09999999f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.9f), D3DXVECTOR3(0.09999999f, 0.1732051f, 0.0f), D3DXVECTOR3(0.09999999f, 0.1732051f, 0.0f), D3DXVECTOR3(-8.742278E-09f, 0.2f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(-0.1f, 0.1732051f, 0.0f), D3DXVECTOR3(-0.1732051f, 0.1f, 0.0f), D3DXVECTOR3(-0.2f, 3.019916E-08f, 0.0f), D3DXVECTOR3(-0.1732051f, -0.09999995f, 0.0f), D3DXVECTOR3(-0.1000001f, -0.173205f, 0.0f), D3DXVECTOR3(-9.298245E-08f, -0.2f, 0.0f), D3DXVECTOR3(0.0999999f, -0.1732051f, 0.0f), D3DXVECTOR3(0.173205f, -0.1000001f, 0.0f), D3DXVECTOR3(0.2f, 0.0f, 0.0f), D3DXVECTOR3(0.1732051f, 0.09999999f, 0.0f), D3DXVECTOR3(0.09999999f, 0.1732051f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.9f), D3DXVECTOR3(0.0f, 0.0f, 0.9f), D3DXVECTOR3(0.0f, 0.0f, 0.9f), D3DXVECTOR3(0.0f, 0.0f, 0.9f), D3DXVECTOR3(0.0f, 0.0f, 0.9f), D3DXVECTOR3(0.0f, 0.0f, 0.9f), D3DXVECTOR3(0.0f, 0.0f, 0.9f), D3DXVECTOR3(0.0f, 0.0f, 0.9f), D3DXVECTOR3(0.0f, 0.0f, 0.9f), D3DXVECTOR3(0.0f, 0.0f, 0.9f), D3DXVECTOR3(0.0f, 0.0f, 0.9f), D3DXVECTOR3(0.0f, 0.0f, 0.9f), D3DXVECTOR3(0.0f, 0.0f, 0.9f)
		};
		D3DXVECTOR3 normals[] =
		{
			D3DXVECTOR3(0.2530548f, 0.9444141f, 0.2098698f), D3DXVECTOR3(-1.118612E-07f, 0.9761871f, 0.2169304f), D3DXVECTOR3(0.4880935f, 0.8454028f, 0.2169304f), D3DXVECTOR3(-0.2530551f, 0.944414f, 0.2098698f), D3DXVECTOR3(-0.4880936f, 0.8454028f, 0.2169304f), D3DXVECTOR3(-0.691359f, 0.691359f, 0.2098698f), D3DXVECTOR3(-0.8454027f, 0.4880936f, 0.2169304f), D3DXVECTOR3(-0.944414f, 0.2530551f, 0.2098698f), D3DXVECTOR3(-0.976187f, 2.033839E-07f, 0.2169304f), D3DXVECTOR3(-0.9444141f, -0.2530547f, 0.2098698f), D3DXVECTOR3(-0.8454029f, -0.4880932f, 0.2169304f), D3DXVECTOR3(-0.6913593f, -0.6913588f, 0.2098698f), D3DXVECTOR3(-0.4880938f, -0.8454026f, 0.2169304f), D3DXVECTOR3(-0.2530552f, -0.944414f, 0.2098698f), D3DXVECTOR3(-4.169371E-07f, -0.976187f, 0.2169304f), D3DXVECTOR3(0.2530544f, -0.9444142f, 0.2098698f), D3DXVECTOR3(0.488093f, -0.8454031f, 0.2169304f), D3DXVECTOR3(0.6913587f, -0.6913594f, 0.2098698f), D3DXVECTOR3(0.8454025f, -0.4880939f, 0.2169304f), D3DXVECTOR3(0.944414f, -0.2530553f, 0.2098698f), D3DXVECTOR3(0.9761871f, -1.220304E-07f, 0.2169304f), D3DXVECTOR3(0.944414f, 0.2530551f, 0.2098698f), D3DXVECTOR3(0.8454028f, 0.4880935f, 0.2169304f), D3DXVECTOR3(0.6913592f, 0.6913589f, 0.2098698f), D3DXVECTOR3(0.4880935f, 0.8454028f, 0.2169304f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f)
		};
		UINT indices[] =
		{
			0, 1, 2, 0, 3, 1, 3, 4, 1, 3, 5, 4, 5, 6, 4, 5, 7, 6, 7, 8, 6, 7, 9, 8, 9, 10, 8, 9, 11, 10, 11, 12, 10, 11, 13, 12, 13, 14, 12, 13, 15, 14, 15, 16, 14, 15, 17, 16, 17, 18, 16, 17, 19, 18, 19, 20, 18, 19, 21, 20, 21, 22, 20, 21, 23, 22, 23, 24, 22, 23, 0, 24, 25, 26, 27, 26, 28, 27, 28, 29, 27, 29, 30, 27, 30, 31, 27, 31, 32, 27, 32, 33, 27, 33, 34, 27, 34, 35, 27, 35, 36, 27, 36, 37, 27, 37, 38, 27, 39, 40, 41, 40, 42, 41, 42, 43, 41, 43, 44, 41, 44, 45, 41, 45, 46, 41, 46, 47, 41, 47, 48, 41, 48, 49, 41, 49, 50, 41, 50, 51, 41, 51, 39, 41
		};
		const UINT count = (UINT)(sizeof(positions) / sizeof(D3DXVECTOR3));
		VertexColorNormal vertices[count];
		gizmoIndexCount[1] = (UINT)(sizeof(indices) / sizeof(UINT));
		for (UINT i = 0; i < count; ++i)
		{
			vertices[i].Position = positions[i];
			vertices[i].Normal = normals[i];
		}
		//Create Vertex Buffer
		{
			D3D11_BUFFER_DESC desc = { 0 };
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.ByteWidth = sizeof(VertexTextureNormalTangent) * count;
			desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

			D3D11_SUBRESOURCE_DATA data = { 0 };
			data.pSysMem = vertices;

			HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &gizmoVertexBuffer[1]);
			assert(SUCCEEDED(hr));
		}

		//Create Index Buffer
		{
			D3D11_BUFFER_DESC desc = { 0 };
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.ByteWidth = sizeof(indices);
			desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

			D3D11_SUBRESOURCE_DATA data = { 0 };
			data.pSysMem = indices;

			HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &gizmoIndexBuffer[1]);
			assert(SUCCEEDED(hr));
		}
	}

	//scale
	{
		D3DXVECTOR3 positions[] =
		{
			D3DXVECTOR3(-0.4519059f, 0.4519059f, 0.0f), D3DXVECTOR3(-0.4519059f, 0.4519059f, 0.9038118f), D3DXVECTOR3(-0.4519059f, -0.4519059f, 0.9038118f), D3DXVECTOR3(-0.4519059f, -0.4519059f, 0.0f), D3DXVECTOR3(0.4519059f, 0.4519059f, 0.0f), D3DXVECTOR3(0.4519059f, 0.4519059f, 0.9038118f), D3DXVECTOR3(-0.4519059f, 0.4519059f, 0.9038118f), D3DXVECTOR3(-0.4519059f, 0.4519059f, 0.0f), D3DXVECTOR3(0.4519059f, -0.4519059f, 0.0f), D3DXVECTOR3(0.4519059f, -0.4519059f, 0.9038118f), D3DXVECTOR3(0.4519059f, 0.4519059f, 0.9038118f), D3DXVECTOR3(0.4519059f, 0.4519059f, 0.0f), D3DXVECTOR3(-0.4519059f, -0.4519059f, 0.0f), D3DXVECTOR3(-0.4519059f, -0.4519059f, 0.9038118f), D3DXVECTOR3(0.4519059f, -0.4519059f, 0.9038118f), D3DXVECTOR3(0.4519059f, -0.4519059f, 0.0f), D3DXVECTOR3(-0.4519059f, -0.4519059f, 0.9038118f), D3DXVECTOR3(-0.4519059f, 0.4519059f, 0.9038118f), D3DXVECTOR3(0.4519059f, 0.4519059f, 0.9038118f), D3DXVECTOR3(0.4519059f, -0.4519059f, 0.9038118f), D3DXVECTOR3(-0.4519059f, -0.4519059f, 0.0f), D3DXVECTOR3(0.4519059f, -0.4519059f, 0.0f), D3DXVECTOR3(0.4519059f, 0.4519059f, 0.0f), D3DXVECTOR3(-0.4519059f, 0.4519059f, 0.0f)
		};
		D3DXVECTOR3 normals[] =
		{
			D3DXVECTOR3(-1.0f, 0.0f, 0.0f), D3DXVECTOR3(-1.0f, 0.0f, 0.0f), D3DXVECTOR3(-1.0f, 0.0f, 0.0f), D3DXVECTOR3(-1.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f)
		};
		UINT indices[] =
		{
			0, 1, 2, 2, 3, 0, 4, 5, 6, 6, 7, 4, 8, 9, 10, 10, 11, 8, 12, 13, 14, 14, 15, 12, 16, 17, 18, 18, 19, 16, 20, 21, 22, 22, 23, 20
		};
		const UINT count = (UINT)(sizeof(positions) / sizeof(D3DXVECTOR3));
		gizmoIndexCount[2] = (UINT)(sizeof(indices) / sizeof(UINT));
		VertexColorNormal vertices[count];
		for (UINT i = 0; i < count; ++i)
		{
			vertices[i].Position = positions[i];
			vertices[i].Normal = normals[i];
		}
		//Create Vertex Buffer
		{
			D3D11_BUFFER_DESC desc = { 0 };
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.ByteWidth = sizeof(VertexTextureNormalTangent) * count;
			desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

			D3D11_SUBRESOURCE_DATA data = { 0 };
			data.pSysMem = vertices;

			HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &gizmoVertexBuffer[2]);
			assert(SUCCEEDED(hr));
		}

		//Create Index Buffer
		{
			D3D11_BUFFER_DESC desc = { 0 };
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.ByteWidth = sizeof(indices);
			desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

			D3D11_SUBRESOURCE_DATA data = { 0 };
			data.pSysMem = indices;

			HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &gizmoIndexBuffer[2]);
			assert(SUCCEEDED(hr));
		}
	}
}

void GizmoComponent::ResetDeltas()
{
	_tDelta = D3DXVECTOR3(0, 0, 0);
	_lastIntersectionPosition = D3DXVECTOR3(0, 0, 0);
	_intersectPosition = D3DXVECTOR3(0, 0, 0);
}

void GizmoComponent::PickObject(D3DXVECTOR2 mousePosition, bool removeFromSelection)
{
	if (_selectionPool == NULL)
		return;

	RayCollider* ray = values->MainCamera->ConvertMouseToRay(values->Viewport, values->Perspective, _currentMouseState);
	float closest = INFINITY;
	float dist = INFINITY;
	GameModel* obj = NULL;
	for (GameModel* select : *_selectionPool)
	{
		if (Manifold::Solve(ray, select->GetCollider(), &dist) && dist < closest)
		{
			bool contain = false;
			for (GameModel* check : *selection)
			{
				if (check == select)
				{
					contain = true;
					break;
				}
			}
			if (contain == false)
			{
				obj = select;
				closest = dist;
			}
			if (removeFromSelection)
				selection->remove(select);
		}
	}
	if (obj != NULL)
		selection->push_back(obj);
}


