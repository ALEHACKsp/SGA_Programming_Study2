#pragma once

class Ray
{
public:
	Ray(D3DXVECTOR3& origin, D3DXVECTOR3 direction, float length = 1000.0f);
	~Ray();

	void Render();
	void PostRender();

	void Get(D3DXVECTOR3* origin, D3DXVECTOR3* direction) {
		*origin = this->origin;
		*direction = this->direction;
	}

	void Set(D3DXVECTOR3& origin, D3DXVECTOR3& direction) {
		this->origin = origin;
		this->direction = direction;
	}

	void SetLength(float length) { this->length = length; }
	float GetLength() { return length; }

private:
	D3DXVECTOR3 origin;
	D3DXVECTOR3 direction;

	float length;

	class DebugDrawLine* debug;
};