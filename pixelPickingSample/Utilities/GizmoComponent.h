#pragma once

enum class GizmoAxis
{
	X,
	Y,
	Z,
	XY,
	ZX,
	YZ,
	None
};

enum class GizmoMode
{
	Translate,
	Rotate,
	NonUniformScale,
	UniformScale
};

enum class TransformSpace
{
	Local,
	World
};

enum class PivotType
{
	ObjectCenter,
	SelectionCenter,
	WorldOrigin
};

class GizmoComponent
{
private:
	struct Quad;

public:
	GizmoComponent(ExecuteValues* values);
	~GizmoComponent();

	void Initialize();
	void Update();
	void Render();
	void PostRender();
	
	void SetSelectionPool(list<GameModel*>* selectables) { _selectionPool = selectables; }
	list<GameModel*>* GetSelectionPool() { return _selectionPool; }
	void NextPivotType();
	void SelectEntities(D3DXVECTOR2 mouseloc, bool addToSelection, bool removeFromSelection);
	void Clear();
	void Draw();

private:
	/// Returns true is any of the modifier keys is pressed.
	bool IsAnyModifierPressed();
	void ApplyColor(GizmoAxis axis, D3DXCOLOR color);
	void ApplyLineColor(int startindex, int count, D3DXCOLOR color);
	void SelectAxis(D3DXVECTOR3 mousePosition);
	void SetGizmoPosition();
	D3DXVECTOR3 GetSelectionCenter();
	void Draw2D();
	void CreateSelectionBox();
	void DrawSelectionBox();

protected:
	void CreateGizmoBuffer();
	void ResetDeltas();
	void PickObject(D3DXVECTOR2 mousePosition, bool removeFromSelection);


private:
	ExecuteValues* values;
	/// <summary>
	/// only active if atleast one entity is selected.
	/// </summary>
	bool _isActive;

	/// <summary>
	/// Enabled if gizmo should be able to select objects and axis.
	/// </summary>
	bool enabled;

	bool SelectionBoxesIsVisible = true;

	Shader* _lineShader;
	Shader* _meshShader;

	// -- Screen Scale -- //
	D3DXMATRIX _screenScaleMatrix;
	float _screenScale;

	// -- Position - Rotation -- //
	D3DXVECTOR3 _position = D3DXVECTOR3(0, 0, 0);
	D3DXMATRIX _rotationMatrix;

	D3DXVECTOR3 _localForward;
	D3DXVECTOR3 _localUp;
	D3DXVECTOR3 _localRight;

	// -- Matrices -- //
	D3DXMATRIX _objectOrientedWorld;
	D3DXMATRIX _axisAlignedWorld;
	D3DXMATRIX _modelLocalSpace[3];

	// used for all drawing, assigned by local- or world-space matrices
	D3DXMATRIX _gizmoWorld;

	// the matrix used to apply to your whole scene, usually matrix.identity (default scale, origin on 0,0,0 etc.)
	D3DXMATRIX sceneWorld;

	// -- Lines (Vertices) -- //
	VertexColor _translationLineVertices[18];
	ID3D11Buffer* _translationLineBuffer;

	const float LINE_LENGTH = 3.0f;
	const float LINE_OFFSET = 1.0f;

	// -- Quads -- //
	Quad* _quads[3];
	Shader* _quadShader;

	// -- Colors -- //
	D3DXCOLOR _axisColors[3];
	D3DXCOLOR _highlightColor;

	// -- UI Text -- //
	string _axisText[3];
	D3DXVECTOR3 _axisTextOffset;

public:
	// -- Modes & Selections -- //
	GizmoAxis ActiveAxis = GizmoAxis::None;
	GizmoMode ActiveMode = GizmoMode::Translate;
	TransformSpace ActiveSpace = TransformSpace::Local;
	PivotType ActivePivot = PivotType::SelectionCenter;

private:
	// -- BoundingBoxes -- //
	
	const float MULTI_AXIS_THICKNESS = 0.05f;
	const float SINGLE_AXIS_THICKNESS = 0.2f;

	class BoxCollider* xAxisBox;
	class BoxCollider* yAxisBox;
	class BoxCollider* zAxisBox;
	class BoxCollider* xzAxisBox;
	class BoxCollider* xyBox;
	class BoxCollider* yzBox;

	// -- BoundingSpheres -- //

	const float RADIUS = 1.0f;

	class SphereCollider* xSphere;
	class SphereCollider* ySphere;
	class SphereCollider* zSphere;

	/// <summary>
	/// The value to adjust all transformation when precisionMode is active.
	/// </summary>
	const float PRECISION_MODE_SCALE = 0.1f;

	// -- Selection -- //
	list<GameModel*>* selection;
	list<GameModel*>* _selectionPool = NULL;

	D3DXVECTOR3 _translationDelta;
	D3DXMATRIX _rotationDelta;
	D3DXVECTOR3 _scaleDelta;

	// -- Translation Variables -- //
	D3DXVECTOR3 _tDelta;
	D3DXVECTOR3 _lastIntersectionPosition;
	D3DXVECTOR3 _intersectPosition;

	//Ãß°¡
	DepthStencilState* depthState[2];
	BlendState* blendState[2];
	RasterizerState* rasterizerState[2];
	WorldBuffer* worldBuffer;
public:
	bool SnapEnabled = false;
	bool PrecisionModeEnabled;
	float TranslationSnapValue = 5;
	float RotationSnapValue = 30;
	float ScaleSnapValue = 0.5f;

private:
	D3DXVECTOR3 _translationScaleSnapDelta;
	float _rotationSnapDelta;

	Shader* _selectionBoxShader;
	ID3D11Buffer* _selectionBoxVertexBuffer;
	vector<VertexColor> _selectionBoxVertices;
	//public BoundingBox SelectionBox;

	//private LineRenderer _lineRenderer;
	//private bool _showLines;

	D3DXVECTOR3 _lastMouseState, _currentMouseState;

	ID3D11Buffer* gizmoVertexBuffer[3];
	ID3D11Buffer* gizmoIndexBuffer[3];
	UINT gizmoIndexCount[3];

private:
	struct Quad
	{
	public:
		D3DXVECTOR3 Origin;
		D3DXVECTOR3 UpperLeft;
		D3DXVECTOR3 LowerLeft;
		D3DXVECTOR3 UpperRight;
		D3DXVECTOR3 LowerRight;
		D3DXVECTOR3 Normal;
		D3DXVECTOR3 Up;
		D3DXVECTOR3 Left;

		ID3D11Buffer* VertexBuffer;
		ID3D11Buffer* IndexBuffer;

		Quad(D3DXVECTOR3 origin, D3DXVECTOR3 normal, D3DXVECTOR3 up,
			float width, float height)
		{
			Origin = origin;
			Normal = normal;
			Up = up;

			// Calculate the quad corners
			D3DXVec3Cross(&Left, &normal, &Up);
			D3DXVECTOR3 uppercenter = (Up * height / 2) + origin;
			UpperLeft = uppercenter + (Left * width / 2);
			UpperRight = uppercenter - (Left * width / 2);
			LowerLeft = UpperLeft - (Up * height);
			LowerRight = UpperRight - (Up * height);

			FillVertices();
		}

	private:
		void FillVertices()
		{
			VertexTextureNormal vertices[4];
			UINT indices[6];
			// Fill in texture coordinates to display full texture
			// on quad
			D3DXVECTOR2 textureUpperLeft = D3DXVECTOR2(0.0f, 0.0f);
			D3DXVECTOR2 textureUpperRight = D3DXVECTOR2(1.0f, 0.0f);
			D3DXVECTOR2 textureLowerLeft = D3DXVECTOR2(0.0f, 1.0f);
			D3DXVECTOR2 textureLowerRight = D3DXVECTOR2(1.0f, 1.0f);

			// Provide a normal for each vertex
			for (int i = 0; i < 4; i++)
			{
				vertices[i].Normal = Normal;
			}

			// Set the position and texture coordinate for each
			// vertex
			vertices[0].Position = LowerLeft;
			vertices[0].Uv = textureLowerLeft;
			vertices[1].Position = UpperLeft;
			vertices[1].Uv = textureUpperLeft;
			vertices[2].Position = LowerRight;
			vertices[2].Uv = textureLowerRight;
			vertices[3].Position = UpperRight;
			vertices[3].Uv = textureUpperRight;

			// Set the index buffer for each vertex, using
			// clockwise winding
			indices[0] = 0;
			indices[1] = 1;
			indices[2] = 2;
			indices[3] = 2;
			indices[4] = 1;
			indices[5] = 3;

			//Create Vertex Buffer
			{
				D3D11_BUFFER_DESC desc = { 0 };
				desc.Usage = D3D11_USAGE_DEFAULT;
				desc.ByteWidth = sizeof(VertexTextureNormalTangent) * 4;
				desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

				D3D11_SUBRESOURCE_DATA data = { 0 };
				data.pSysMem = vertices;

				HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &VertexBuffer);
				assert(SUCCEEDED(hr));
			}

			//Create Index Buffer
			{
				D3D11_BUFFER_DESC desc = { 0 };
				desc.Usage = D3D11_USAGE_DEFAULT;
				desc.ByteWidth = sizeof(UINT) * 6;
				desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

				D3D11_SUBRESOURCE_DATA data = { 0 };
				data.pSysMem = indices;

				HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &IndexBuffer);
				assert(SUCCEEDED(hr));
			}
		}
	};
};