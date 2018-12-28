#pragma once

class GameModel;
class ModelBone;
class PickingObject
{
public:
	PickingObject(ExecuteValues* values);
	~PickingObject();

	//TODO: ���� ��Ʈ���� �̿��� ������Ʈ ������ �ٲ��� �ʿ� ����
	bool Picking(vector<GameModel*>& objects);
	void MultiPickingMouseDown(D3DXVECTOR3& mousePosition);
	bool MultiPickingMouseUp(D3DXVECTOR3& mousePosition, vector<GameModel*>& objects);

	void SetPickedObj(GameModel* object) { pickedModel = object; }
	void SetPickedPart(ModelBone* object) { pickedPart = object; }
	GameModel* GetPickedObj() { return pickedModel; }
	vector<GameModel*>* GetPickedObjs() { return &pickedModels; }
	void UnPicking();

	bool PickingBone();
	ModelBone* GetPickedPart() { return pickedPart; }

private:
	void ChangePickedObj(GameModel* object);
	void MakeFrustum();
	bool ContainPoint(D3DXVECTOR3& point);
private:
	ExecuteValues* values;
	GameModel* pickedModel;
	vector<GameModel*> pickedModels;
	ModelBone* pickedPart;

	D3DXVECTOR3 mousePosTemp[2];
	D3DXPLANE planes[6];
	float zFar;
};
