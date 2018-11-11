#include "GameModel.h"

class GameAnimModel : public GameModel
{
public:
	GameAnimModel
	(
		wstring matFolder, wstring matFile
		, wstring meshFolder, wstring meshFile
	);
	virtual ~GameAnimModel();

	virtual void Update();
	virtual void PostRender();

	UINT AddClip(wstring file);
	UINT AddClip(class ModelClip* clip);

	vector<class ModelClip *>& Clips() { return clips; }
	void DeleteClip(int i) { clips.erase(clips.begin() + i); }
	void DeleteClip(wstring name);

	void LockRoot(UINT index, bool val);
	void Repeat(UINT index, bool val);
	void Speed(UINT index, float val);

	void Play(UINT index, bool bRepeat = false,
		float blendTime = 0.0f, float speed = 1.0f, float startTime = 0.0f);
	bool IsPlay();
	bool IsPause();
	void Pause();
	void Resume();
	void Stop();

	void RetargetBone();
	void RetargetClip();

	void SelectClip(int selectClip) { this->selectClip = selectClip; }

private:
	vector<class ModelClip *> clips;
	class ModelTweener* tweener;

	int selectClip;
};