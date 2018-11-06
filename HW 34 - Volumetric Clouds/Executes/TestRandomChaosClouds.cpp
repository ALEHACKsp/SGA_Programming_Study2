#include "stdafx.h"
#include "TestRandomChaosClouds.h"

#include "../Particle/InstancedCloudManager.h"

TestRandomChaosClouds::TestRandomChaosClouds(ExecuteValues * values)
	:Execute(values)
{
	select = (int)SkyType::SpotClouds;

	for (int i = 0; i < 4; i++) {
		// cloudManager;
		cloudManager[i] = new InstancedCloudManager(values);

		allCloudSprites = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };

		speedTran = 1.0f;
		speedRot = 0.01f;

		SkyType skyType = (SkyType)i;

		float x, y, z;
		float d = 1;

		switch (skyType)
		{
		case TestRandomChaosClouds::SkyType::CloudSplatter:
		{
			float boxSize = 250;
			D3DXVECTOR3 flatBase = D3DXVECTOR3(10, 1, 5);

			for (int c = 0; c < 100; c++)
			{
				d = 1;
				x = Math::Lerp(-boxSize, boxSize, Math::Random(0.0f, 1.0f));
				y = Math::Lerp(-boxSize / 2, boxSize, Math::Random(0.0f, 1.0f));
				z = Math::Lerp(-boxSize, boxSize, Math::Random(0.0f, 1.0f));

				if (y < 200)
					d = 0.75f;
				if (y < 0)
					d = 0.5f;

				// cloudManager addCloud
				vector<int> whispRange = { 0,1,2,3,4,5,6,7,8,9 };

				cloudManager[i]->AddCloud(25, D3DXVECTOR3(x, y, z), 40, flatBase, flatBase * 5, d, whispRange);
			}
		}
		break;

		case TestRandomChaosClouds::SkyType::CloudField:
		{
			D3DXVECTOR3 cloudDim1 = D3DXVECTOR3(500, 20, 500);

			// addCloud 4
			vector<int> whispRange;
			whispRange = { 0,1,2,3,4 };
			cloudManager[i]->AddCloud(2000, D3DXVECTOR3(0, 0, 0), 60, cloudDim1, cloudDim1, 0.25f, whispRange);
			whispRange = { 3,4,5,6,7,8 };
			cloudManager[i]->AddCloud(2000, D3DXVECTOR3(0, 30, 0), 60, cloudDim1, cloudDim1, 0.5f, whispRange);
			whispRange = { 7,8,9,10,11 };
			cloudManager[i]->AddCloud(2000, D3DXVECTOR3(0, 60, 0), 60, cloudDim1, cloudDim1, 0.75f, whispRange);
			whispRange = { 0,1,2,3,4,12,13,14,15 };
			cloudManager[i]->AddCloud(2000, D3DXVECTOR3(0, 90, 0), 60, cloudDim1, cloudDim1, 1.0f, whispRange);

			cloudManager[i]->SortClouds();
		}
		break;

		case TestRandomChaosClouds::SkyType::CloudsSky:
		{
			D3DXVECTOR3 episode1PlayArea = D3DXVECTOR3(1000, 1000, 1000);

			// add outer large clouds
			cloudManager[i]->AddCloud(50, D3DXVECTOR3(0, 0, 0), 2500, D3DXVECTOR3(4000, 4000, 4000), D3DXVECTOR3(2000, 2000, 2000), 0.75f, allCloudSprites);

			// clouds inplay
			D3DXVECTOR3 flatBase = D3DXVECTOR3(50, 5, 25);

			for (int c = 0; c < 50; c++)
			{
				d = 1;

				x = Math::Lerp(-episode1PlayArea.x, episode1PlayArea.x, Math::Random(0.0f, 1.0f));
				y = Math::Lerp(-episode1PlayArea.y, episode1PlayArea.y, Math::Random(0.0f, 1.0f));
				z = Math::Lerp(-episode1PlayArea.z, episode1PlayArea.z, Math::Random(0.0f, 1.0f));

				if (y < 200)
					d = 0.8f;
				if (y < 0 && y > -500)
					d = 0.75f;
				if (y < -500)
					d = 0.5f;

				// addCloud
				vector<int> whispRange = { 0,1,2,3,4,5,6,7,8,9 };
				cloudManager[i]->AddCloud(25, D3DXVECTOR3(x, y, z), 300, flatBase, flatBase * 5, d, whispRange);
			}

			cloudManager[i]->SortClouds();
		}
		break;

		case TestRandomChaosClouds::SkyType::SpotClouds:
		{
			// Randomly place some clouds in  the scene.
			D3DXVECTOR3 flatBase = D3DXVECTOR3(20, 2, 20);

			// Clouds are avolume
			float boxSize = 800;
			for (int c = 0; c < 300; c++)
			{
				// Place the cloud randomly in the area.
				x = Math::Lerp(-boxSize, boxSize, Math::Random(0.0f, 1.0f));
				y = Math::Lerp(0, boxSize / 1, Math::Random(0.0f, 1.0f));
				z = Math::Lerp(-boxSize, boxSize, Math::Random(0.0f, 1.0f));

				// addCloud
				vector<int> whispRange = { 0,1,2,3,4,5,6,7,8,9 };
				cloudManager[i]->AddCloud(25, D3DXVECTOR3(x, y, z), 64, flatBase, flatBase * 5, 0.75f, whispRange);
			}

			cloudManager[i]->SortClouds();
		}
		break;
		}
	}
}

TestRandomChaosClouds::~TestRandomChaosClouds()
{
	for(int i=0; i<4; i++)
		SAFE_DELETE(cloudManager[i]);
}

void TestRandomChaosClouds::Update()
{
	// camera move - 이미 구현

	cloudManager[select]->Rotate(D3DXVECTOR3(0, 1, 0), 0.00005f);

	cloudManager[select]->Update();
}

void TestRandomChaosClouds::PreRender()
{

}

void TestRandomChaosClouds::Render()
{
	ImGui::Begin("Sky Type");

	cloudManager[select]->Render();

	ImGui::RadioButton("Cloud Splatter", &select, (int)SkyType::CloudSplatter);
	ImGui::RadioButton("Cloud Field", &select, (int)SkyType::CloudField);
	ImGui::RadioButton("Cloud Sky", &select, (int)SkyType::CloudsSky);
	ImGui::RadioButton("Spot Clouds", &select, (int)SkyType::SpotClouds);
	ImGui::End();
}

void TestRandomChaosClouds::PostRender()
{

}



