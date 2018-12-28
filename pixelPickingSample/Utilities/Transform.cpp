#include "stdafx.h"
#include "Transform.h"

Transform::Transform()
	:pParent(NULL), pFirstChild(NULL), pNextSibling(NULL), bAutoUpdate(true)
{
	this->Reset();
	if (this->bAutoUpdate) {
		this->UpdateTransform();
	}
}

Transform::~Transform()
{
}

void Transform::AddChild(Transform * pNewChild)
{
	if (pNewChild->pParent == this)
		return;

	//이전 부모와 연결을 끊는다
	pNewChild->ReleaseParent();

	//부모의 상대적인 좌표값으로 갱신하기 위해 부모의 final 역행열값
	D3DXMATRIX matInvFinal;
	float temp;
	D3DXMatrixInverse(&matInvFinal, &temp, &this->world);
	D3DXVec3TransformCoord(&pNewChild->position, &pNewChild->position, &matInvFinal);

	D3DXVec3TransformCoord(&pNewChild->axis[0], &pNewChild->axis[0], &matInvFinal);
	D3DXVec3TransformCoord(&pNewChild->axis[1], &pNewChild->axis[1], &matInvFinal);
	D3DXVec3TransformCoord(&pNewChild->axis[3], &pNewChild->axis[3], &matInvFinal);
	
	pNewChild->scale.x = D3DXVec3Length(&pNewChild->right);
	pNewChild->scale.y = D3DXVec3Length(&pNewChild->up);
	pNewChild->scale.z = D3DXVec3Length(&pNewChild->front);

	pNewChild->pParent = this;
	Transform* pChild = this->pFirstChild;
	if (pChild == NULL) {
		this->pFirstChild = pNewChild;
	}
	else {
		while (pChild != NULL) {
			if (pChild->pNextSibling == NULL) {
				pChild->pNextSibling = pNewChild;
				break;
			}
			pChild = pChild->pNextSibling;
		}
	}
	this->UpdateTransform();
}

void Transform::AttachTo(Transform * pParent)
{
}

void Transform::ReleaseParent()
{
}

void Transform::Reset(int resetFlag)
{
	if (resetFlag & RESET_POSITION) {
		this->position = D3DXVECTOR3(0, 0, 0);
	}
	if (resetFlag & RESET_ROTATION) {
		this->right = D3DXVECTOR3(1, 0, 0);
		this->up = D3DXVECTOR3(0, 1, 0);
		this->front = D3DXVECTOR3(0, 0, 1);
	}
	if (resetFlag & RESET_SCALE) {
		this->scale = D3DXVECTOR3(1, 1, 1);
	}
}

void Transform::SetWorldPosition(D3DXVECTOR3 position)
{
	if (this->pParent != NULL) {
		D3DXMATRIX matInvParentFinal;
		float temp;
		D3DXMatrixInverse(&matInvParentFinal, &temp, &pParent->world);
		//world position은 부모의 월드와 곱연산을 한 결과이기 때문에 역행열을 곱해 자식만의 월드를 구한다.
		D3DXVec3TransformCoord(&position, &position, &matInvParentFinal);
	}
	this->position = position;

	if (this->bAutoUpdate) this->UpdateTransform();
}

void Transform::SetLocalPosition(D3DXVECTOR3& position)
{
	this->position = position;
	if (this->bAutoUpdate) this->UpdateTransform();
}

void Transform::MovePositionSelf(D3DXVECTOR3 delta)
{
	//이동 벡터
	D3DXVECTOR3 move;
	//자신의 이동 축을 얻는다
	D3DXVECTOR3 moveAxis[3];
	this->GetUnitAxis(moveAxis);
	move = move + moveAxis[0] * delta.x;
	move = move + moveAxis[1] * delta.y;
	move = move + moveAxis[2] * delta.z;

	D3DXVECTOR3 nowWorldPos = this->GetWorldPosition();
	this->SetWorldPosition(nowWorldPos + move);
}

void Transform::MovePositionWorld(D3DXVECTOR3 delta)
{
	D3DXVECTOR3 nowWorldPos = this->GetWorldPosition();
	this->SetWorldPosition(nowWorldPos + delta);
}

void Transform::MovePositionLocal(D3DXVECTOR3 delta)
{
}

void Transform::SetScale(D3DXVECTOR3& scale)
{
	this->scale = scale;
	if (this->bAutoUpdate) this->UpdateTransform();
}

void Transform::SetScaling(D3DXVECTOR3& deltaScale)
{
	this->scale += deltaScale;
	if (this->bAutoUpdate) this->UpdateTransform();
}

void Transform::RotateWorld(D3DXVECTOR3 angle)
{
	if (this->pParent) {
		D3DXVECTOR3 worldAxis[3];
		this->GetUnitAxis(worldAxis);

		D3DXMATRIX matInvParentFinal;
		float temp;
		D3DXMatrixInverse(&matInvParentFinal, &temp, &pParent->world);

		D3DXMATRIX matrot;
		D3DXMatrixRotationYawPitchRoll(&matrot, angle.y, angle.x, angle.z);

		D3DXVec3TransformNormal(&this->axis[0], &this->axis[0], &matrot);
		D3DXVec3TransformNormal(&this->axis[1], &this->axis[1], &matrot);
		D3DXVec3TransformNormal(&this->axis[2], &this->axis[2], &matrot);

		if (this->bAutoUpdate)
			this->UpdateTransform();
	}
	else {
		RotateLocal(angle);
	}
}

void Transform::RotateSelf(D3DXVECTOR3 angle)
{
	D3DXMATRIX matrot;
	D3DXMatrixRotationYawPitchRoll(&matrot, angle.y, angle.x, angle.z);

	D3DXVec3TransformNormal(&this->axis[0], &this->axis[0], &matrot);
	D3DXVec3TransformNormal(&this->axis[1], &this->axis[1], &matrot);
	D3DXVec3TransformNormal(&this->axis[2], &this->axis[2], &matrot);

	if (this->bAutoUpdate) this->UpdateTransform();
}

void Transform::RotateLocal(D3DXVECTOR3 angle)
{
	D3DXMATRIX matrot;
	D3DXMatrixRotationYawPitchRoll(&matrot, angle.y, angle.x, angle.z);

	D3DXVec3TransformNormal(&this->axis[0], &this->axis[0], &matrot);
	D3DXVec3TransformNormal(&this->axis[1], &this->axis[1], &matrot);
	D3DXVec3TransformNormal(&this->axis[2], &this->axis[2], &matrot);

	if (this->bAutoUpdate) this->UpdateTransform();
}

void Transform::SetRotateWorld(const D3DXMATRIX matWorldRotate)
{
}

void Transform::SetRotateWorld(D3DXQUATERNION & worldRotate)
{
	D3DXQUATERNION quatRot = worldRotate;
	D3DXMATRIX matRotate;
	D3DXMatrixRotationQuaternion(&matRotate, &quatRot);

	D3DXMATRIX matRot = D3DXMATRIX(matRotate);
	if (this->pParent) {
		D3DXMATRIX matInvParentFinal;
		float temp;
		D3DXMatrixInverse(&matInvParentFinal, &temp, &pParent->world);

		matRot = matRot * matInvParentFinal;
	}

	this->right = D3DXVECTOR3(1, 0, 0);
	this->up = D3DXVECTOR3(0, 1, 0);
	this->front = D3DXVECTOR3(0, 0, 1);

	D3DXVec3TransformNormal(&this->axis[0], &this->axis[0], &matRot);
	D3DXVec3TransformNormal(&this->axis[1], &this->axis[1], &matRot);
	D3DXVec3TransformNormal(&this->axis[2], &this->axis[2], &matRot);

	if (this->bAutoUpdate)
		this->UpdateTransform();
}

void Transform::SetRotateLocal(const D3DXMATRIX matLocalRotate)
{
}

void Transform::LookPosition(D3DXVECTOR3 pos, D3DXVECTOR3 up)
{
}

void Transform::RotateSlerp(Transform * from, Transform * to, float t)
{
	//t  보간 값
	// 0 <= t <= 1
	// 0에 가까우면 from에 가깝고
	// 1에 가까우면 to에 가깝다

	t = Math::Clamp(t, 0, 1);
	D3DXQUATERNION fromQuat = from->GetWorldRotateQuaternion();
	D3DXQUATERNION toQuat = to->GetWorldRotateQuaternion();

	if (Math::FloatEqual(t, 0)) {
		this->SetRotateWorld(fromQuat);
	}
	else if (Math::FloatEqual(t, 1.0f)) {
		this->SetRotateWorld(toQuat);
	}
	else {
		D3DXQUATERNION result;
		D3DXQuaternionSlerp(&result, &fromQuat, &toQuat, t);
		this->SetRotateWorld(result);
	}
}

void Transform::PositionLerp(Transform * from, Transform * to, float t)
{
	t = Math::Clamp(t, 0, 1);

	D3DXVECTOR3 fromWorldPos = from->GetWorldPosition();
	D3DXVECTOR3 toWorldPos = to->GetWorldPosition();

	if (Math::FloatEqual(t, 0)) {
		this->SetWorldPosition(from->GetWorldPosition());
	}
	else if (Math::FloatEqual(t, 1.0f)) {
		this->SetWorldPosition(to->GetWorldPosition());
	}
	else {
		D3DXVECTOR3 result;
		D3DXVec3Lerp(&result, &fromWorldPos, &toWorldPos, t);
		this->SetWorldPosition(result);
	}
}

void Transform::Interpolate(Transform * from, Transform * to, float t)
{
	bool bPrevAutoUpdate = this->bAutoUpdate;
	this->bAutoUpdate = false;
	//숙제
	//scale 선형보간 SetScale
	t = Math::Clamp(t, 0, 1.0f);

	D3DXVECTOR3 resultScale;
	D3DXVECTOR3 resultPosition;
	D3DXQUATERNION resultRotate;

	if (Math::FloatEqual(t, 0)) {
		resultScale = from->scale;
		resultPosition = from->GetWorldPosition();
		resultRotate = from->GetWorldRotateQuaternion();
	}
	else if (Math::FloatEqual(t, 1.0f)) {
		resultScale = to->scale;
		resultPosition = to->GetWorldPosition();
		resultRotate = to->GetWorldRotateQuaternion();
	}
	else {
		D3DXVECTOR3 fromScale = from->scale;
		D3DXVECTOR3 toScale = to->scale;
		D3DXVECTOR3 fromPosition = from->GetWorldPosition();
		D3DXVECTOR3 toPosition = to->GetWorldPosition();
		D3DXQUATERNION fromQuat = from->GetWorldRotateQuaternion();
		D3DXQUATERNION toQuat = to->GetWorldRotateQuaternion();

		D3DXVec3Lerp(&resultScale, &fromScale, &toScale, t);
		D3DXVec3Lerp(&resultPosition, &fromPosition, &toPosition, t);
		D3DXQuaternionSlerp(&resultRotate, &fromQuat, &toQuat, t);
	}
	this->bAutoUpdate = false;
	this->SetScale(resultScale);
	this->SetWorldPosition(resultPosition);
	this->SetRotateWorld(resultRotate);
	this->bAutoUpdate = bPrevAutoUpdate;

	if (this->bAutoUpdate)
		this->UpdateTransform();
}

void Transform::DefaultControl2()
{
	float deltaTime = Time::Get()->Delta();
	float deltaMove = 100.0f * deltaTime;
	if (Mouse::Get()->Press(1) == false) {
		if (Keyboard::Get()->Press('A')) this->MovePositionSelf(D3DXVECTOR3(-deltaMove, 0, 0));
		else if (Keyboard::Get()->Press('D')) this->MovePositionSelf(D3DXVECTOR3(deltaMove, 0, 0));
		if (Keyboard::Get()->Press('W')) this->MovePositionSelf(D3DXVECTOR3(0, -deltaMove, 0));
		else if (Keyboard::Get()->Press('S')) this->MovePositionSelf(D3DXVECTOR3(0, deltaMove, 0));
	}
}

void Transform::UpdateTransform()
{
	//자신의 정보로 matLocal 행렬을 갱신
	D3DXMatrixIdentity(&this->local);
	D3DXMatrixIdentity(&this->world);
	
	D3DXVECTOR3 scaledRight = this->right * this->scale.x;
	D3DXVECTOR3 scaledUp = this->up * this->scale.y;
	D3DXVECTOR3 scaledFront = this->front * this->scale.y;

	this->local._11 = scaledRight.x;
	this->local._12 = scaledRight.y;
	this->local._13 = scaledRight.z;

	this->local._21 = scaledUp.x;
	this->local._22 = scaledUp.y;
	this->local._23 = scaledUp.z;

	this->local._21 = scaledFront.x;
	this->local._22 = scaledFront.y;
	this->local._23 = scaledFront.z;

	this->local._41 = position.x;
	this->local._42 = position.y;
	this->local._43 = position.z;


	if (this->pParent == NULL) {
		this->world = local;
	}
	else {
		this->world = local * this->pParent->world;
	}

	Transform* pChild = this->pFirstChild;
	while (pChild != NULL) {
		pChild->UpdateTransform();
		pChild = pChild->pNextSibling;
	}
}

D3DXMATRIX Transform::GetWorld() const
{
	return world;
}

D3DXMATRIX Transform::GetWorldInv() const
{
	D3DXMATRIX inv;
	float temp;
	D3DXMatrixInverse(&inv, &temp, &world);
	return inv;
}

D3DXVECTOR3 Transform::GetWorldPosition()
{
	D3DXVECTOR3 position = this->position;

	if (this->pParent)
		D3DXVec3TransformCoord(&position, &position, &pParent->world);

	return position;
}

void Transform::GetUnitAxis(D3DXVECTOR3* pVecArr) const
{
	D3DXVec3Normalize(&pVecArr[0], &axis[0]);
	D3DXVec3Normalize(&pVecArr[1], &axis[1]);
	D3DXVec3Normalize(&pVecArr[2], &axis[2]);
	if (this->pParent) {
		D3DXVec3TransformNormal(&pVecArr[0], &pVecArr[0], &this->pParent->world);
		D3DXVec3TransformNormal(&pVecArr[1], &pVecArr[1], &this->pParent->world);
		D3DXVec3TransformNormal(&pVecArr[2], &pVecArr[2], &this->pParent->world);
	}
}

D3DXVECTOR3 Transform::GetUnitAxis(int axisNum) const
{
	D3DXVECTOR3 result;
	D3DXVec3Normalize(&result, &this->axis[axisNum]);
	if (this->pParent) {
		D3DXVec3TransformNormal(&result, &result, &this->pParent->world);
	}
	return result;
}

D3DXVECTOR3 Transform::GetUp() const
{
	return this->GetUnitAxis(AXIS_Y);
}

D3DXVECTOR3 Transform::GetRight() const
{
	return this->GetUnitAxis(AXIS_X);
}

D3DXVECTOR3 Transform::GetScaleLocal() const
{
	return this->scale;
}

D3DXVECTOR3 Transform::GetScaleWorld() const
{
	D3DXVECTOR3 scale = this->scale;
	if (this->pParent)
	{
		D3DXVECTOR3 pScale = this->pParent->GetScaleWorld();
		scale.x *= pScale.x;
		scale.y *= pScale.y;
		scale.z *= pScale.z;
	}
	return scale;
}

D3DXMATRIX Transform::GetWorldRotateMatrix()
{
	D3DXMATRIX matRotate;
	D3DXMatrixIdentity(&matRotate);
	D3DXVECTOR3 axis[3];
	this->GetUnitAxis(axis);
	matRotate._11 = axis[0].x;
	matRotate._12 = axis[0].y;
	matRotate._13 = axis[0].z;

	matRotate._21 = axis[1].x;
	matRotate._22 = axis[1].y;
	matRotate._23 = axis[1].z;

	matRotate._31 = axis[2].x;
	matRotate._32 = axis[2].y;
	matRotate._33 = axis[2].z;

	return matRotate;
}

D3DXQUATERNION Transform::GetWorldRotateQuaternion()
{
	D3DXQUATERNION quat;
	D3DXMATRIX matWorld = this->GetWorldRotateMatrix();

	D3DXQuaternionRotationMatrix(&quat, &matWorld);
	return quat;
}

void Transform::DrawInterface()
{
	ImGui::Begin("Transform");
	{
		ImGui::Text("Position");
		ImGui::InputFloat("X", &position.x, 1.0f);
		ImGui::InputFloat("Y", &position.y, 1.0f);
		ImGui::InputFloat("Z", &position.z, 1.0f);

		ImGui::Text("AxisX");
		ImGui::InputFloat("X", &right.x, 1.0f);
		ImGui::InputFloat("Y", &right.y, 1.0f);
		ImGui::InputFloat("Z", &right.z, 1.0f);

		ImGui::Text("AxisY");
		ImGui::InputFloat("X", &up.x, 1.0f);
		ImGui::InputFloat("Y", &up.y, 1.0f);
		ImGui::InputFloat("Z", &up.z, 1.0f);

		ImGui::Text("Scale");
		ImGui::InputFloat("X", &scale.x, 1.0f);
		ImGui::InputFloat("Y", &scale.y, 1.0f);
		ImGui::InputFloat("Z", &scale.z, 1.0f);

		if (ImGui::Button("Reset")) this->Reset();
		this->UpdateTransform();
	}
	ImGui::End();
}

void Transform::RenderGizmo(bool apprlyScale)
{
	D3DXVECTOR3 worldPos = this->GetWorldPosition();
	D3DXVECTOR3 axis[3];
	if (apprlyScale) {
	}
	else {
		this->GetUnitAxis(axis);
	}
	D3DXVECTOR3 xAsix = worldPos + axis[0] * 100;
	//GIZMO->Line(worldPos, xAsix, 0xFFFF0000);
	D3DXVECTOR3 yAsix = worldPos + axis[1] * 100;
	//GIZMO->Line(worldPos, yAsix, 0xFF00FF00);
	D3DXVECTOR3 zAsix = worldPos + axis[2] * 100;
	//GIZMO->Line(worldPos, zAsix, 0xFF0000FF);
}
