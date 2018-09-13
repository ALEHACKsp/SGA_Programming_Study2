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

	UINT AddClip(wstring file);
	UINT AddClip(class ModelClip* clip);

	void LockRoot(UINT index, bool val);
	void Repeat(UINT index, bool val);
	void Speed(UINT index, float val);

private:
	vector<class ModelClip *> clips;
};