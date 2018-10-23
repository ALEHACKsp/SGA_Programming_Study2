#pragma once

class Projection
{
//public:
//	// 높을 수록 정밀해지고 느려짐 보통 2^n으로 잡음
//	const float Width = 2048;
//	const float Height = 2048;

public:
	Projection(ExecuteValues* values);
	~Projection();

	void Add(class GameModel* obj);

	void Update();
	void PreRender();
	void Render();

private:
	ExecuteValues* values;

	Shader* shader;

	vector<class GameModel *> objs;

	class Fixity* fixity;
	class Perspective* perspective;
	ViewProjectionBuffer* vpBuffer;
};