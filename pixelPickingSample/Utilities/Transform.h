#pragma once

#define RESET_POSITION 1
#define RESET_ROTATION 2
#define RESET_SCALE 4

#define AXIS_X 0
#define AXIS_Y 1
#define AXIS_Z 2

class Transform
{
public:
	Transform();
	~Transform();

	//Parents
	void AddChild(Transform* pNewChild);
	void AttachTo(Transform* pParent);
	void ReleaseParent();

	//Transform
	void Reset(int resetFlag = -1);

	//Position
	void SetWorldPosition(D3DXVECTOR3 position);
	//�θ� ������ ������ǥ, �θ� ������ ���� ��ǥ
	void SetLocalPosition(D3DXVECTOR3& position);
	void MovePositionSelf(D3DXVECTOR3 delta);
	void MovePositionWorld(D3DXVECTOR3 delta);
	void MovePositionLocal(D3DXVECTOR3 delta);

	//Scale
	void SetScale(D3DXVECTOR3& scale);			//����
	void SetScaling(D3DXVECTOR3& deltaScale);	//����ġ

											//Rotate
	void RotateWorld(D3DXVECTOR3 angle);
	void RotateSelf(D3DXVECTOR3 angle);
	void RotateLocal(D3DXVECTOR3 angle);

	void SetRotateWorld(const D3DXMATRIX matWorldRotate);
	void SetRotateWorld(D3DXQUATERNION& worldRotate);
	void SetRotateLocal(const D3DXMATRIX matLocalRotate);

	//Rotation (����� ȸ�� == QUATERNION)

	//etc..
	void LookPosition(D3DXVECTOR3 pos, D3DXVECTOR3 up = D3DXVECTOR3(0, 0, -1));
	void RotateSlerp(Transform* from, Transform* to, float t);	//�ڽ��� ȸ������ from�� to ������ ȸ������ŭ ȸ�� ����
	void PositionLerp(Transform* from, Transform* to, float t);
	void Interpolate(Transform* from, Transform* to, float t);

	void DefaultControl2();

	//Update()
	void UpdateTransform();

	//Get
	D3DXMATRIX GetWorld() const;
	D3DXMATRIX GetWorldInv() const;

	D3DXVECTOR3 GetWorldPosition();

	void GetUnitAxis(D3DXVECTOR3* pVecArr) const;
	D3DXVECTOR3 GetUnitAxis(int axisNum) const;
	D3DXVECTOR3 GetUp() const;
	D3DXVECTOR3 GetRight() const;
	D3DXVECTOR3 GetScaleLocal() const;
	D3DXVECTOR3 GetScaleWorld() const;
	D3DXMATRIX GetWorldRotateMatrix();

	D3DXQUATERNION GetWorldRotateQuaternion();
	void DrawInterface();

	void RenderGizmo(bool apprlyScale = false);

private:
	union	//�������� �ּҰ� ����
	{
		struct {
			D3DXVECTOR3 axis[3];
		};
		struct {
			D3DXVECTOR3 right;
			D3DXVECTOR3 up;
			D3DXVECTOR3 front;
		};
	};

	D3DXVECTOR3 position;
	D3DXVECTOR3 scale;
	D3DXQUATERNION rotation;

	//���� ���
	D3DXMATRIX local;

	//���� ��� (����)
	D3DXMATRIX world;

	bool bAutoUpdate;
	Transform* pParent;
	Transform* pFirstChild;		//ù��° �ڽ�
	Transform* pNextSibling;	//����
};