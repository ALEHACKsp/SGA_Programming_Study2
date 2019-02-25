#pragma once

struct ModelKeyframeData
{
	float Time;

	D3DXVECTOR3 Scale;
	D3DXQUATERNION Rotation;
	D3DXVECTOR3 Translation;
};

struct ModelKeyframe
{
	wstring BoneName;
	vector<ModelKeyframeData> Transforms;
};


class ModelClip
{
public:
	ModelClip(wstring file);
	~ModelClip();

	float Duration() { return duration; }
	float FrameRate() { return frameRate; }
	UINT FrameCount() { return frameCount; }

	ModelKeyframe* Keyframe(wstring name);

	ID3D11Texture2D* Texture() { return texture; }
	void Texture(ID3D11Texture2D* tex) { texture = tex; }

	string ClipName() { return clipName; }
	void ClipName(string name) { clipName = name; }

	bool* Repeat() { return &repeat; }
private:
	wstring name;

	float duration;
	float frameRate;
	UINT frameCount;

	unordered_map<wstring, ModelKeyframe *> keyframeMap;

	string clipName;
	ID3D11Texture2D* texture;

	bool repeat;
};