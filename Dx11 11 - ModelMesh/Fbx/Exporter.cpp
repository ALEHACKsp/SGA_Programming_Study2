#include "stdafx.h"
#include "Exporter.h"

Fbx::Exporter::Exporter(wstring file)
{
	manager = FbxManager::Create();
	scene = FbxScene::Create(manager, ""); // 2번째 인자 씬 이름

	ios = FbxIOSettings::Create(manager, IOSROOT);
	ios->SetBoolProp(IMP_FBX_TEXTURE, true);
	manager->SetIOSettings(ios);
}

Fbx::Exporter::~Exporter()
{
}
