#include "stdafx.h"
#include "Cloud.h"
#include "./Effects/Particles.h"

Cloud::Cloud(ExecuteValues* values, CloudType cloudType)
	: values(values)
{
	clouds = new Particles(values, Textures + L"CloudSpriteSheetOrg.png");
	GenerateCloud(cloudType);
}

Cloud::~Cloud()
{
	SAFE_DELETE(clouds);
}

void Cloud::Update()
{
	D3DXVECTOR3 position;
	values->MainCamera->Position(&position);
	clouds->Position(position.x, 200, position.z);
	RotateCloud(1);
	clouds->Update();
}

void Cloud::Render()
{
	clouds->Render();
}

void Cloud::GenerateCloud(CloudType cloudType)
{
	float x, y, z;
	float d = 1;

	switch (cloudType)
	{
	case CloudType::CloudSplatter:
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
					d = .75f;
				if (y < 0)
					d = .5f;

				UINT whispRanges[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
				AddCloud(25, D3DXVECTOR3(x, y, z), 40, flatBase, flatBase * 5, d, whispRanges, sizeof(whispRanges) / sizeof(UINT));
			}
		}
		break;
	case CloudType::CloudField:
		{
			D3DXVECTOR3 cloudDim1 = D3DXVECTOR3(500, 20, 500);
			UINT whispRanges[] = { 0, 1, 2, 3, 4 };
			UINT whispRanges2[] = { 3, 4, 5, 6, 7, 8 };
			UINT whispRanges3[] = { 7, 8, 9, 10, 11 };
			UINT whispRanges4[] = { 0, 1, 2, 3, 4, 12, 13, 14, 15 };
			AddCloud(2000, D3DXVECTOR3(0, 0, 0), 60, cloudDim1, cloudDim1, 0.25f, whispRanges, sizeof(whispRanges) / sizeof(UINT));
			AddCloud(2000, D3DXVECTOR3(0, 30, 0), 60, cloudDim1, cloudDim1, 0.5f, whispRanges2, sizeof(whispRanges2) / sizeof(UINT));
			AddCloud(2000, D3DXVECTOR3(0, 60, 0), 60, cloudDim1, cloudDim1, 0.75f, whispRanges3, sizeof(whispRanges3) / sizeof(UINT));
			AddCloud(2000, D3DXVECTOR3(0, 90, 0), 60, cloudDim1, cloudDim1, 1.0f, whispRanges4, sizeof(whispRanges4) / sizeof(UINT));
		}
		break;
	case CloudType::CloudySky:
		{
			D3DXVECTOR3 episode1PlayArea = D3DXVECTOR3(1000, 1000, 1000);

			// Outer large clouds                    
			UINT whispRanges[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
			AddCloud(50, D3DXVECTOR3(0, 0, 0), 2500, D3DXVECTOR3(4000, 4000, 4000), D3DXVECTOR3(2000, 2000, 2000), .75f, whispRanges, sizeof(whispRanges) / sizeof(UINT));

			// clouds inplay
			D3DXVECTOR3 flatBase = D3DXVECTOR3(50, 5, 25);

			for (int c = 0; c < 50; c++)
			{
				d = 1;

				x = Math::Lerp(-episode1PlayArea.x, episode1PlayArea.x, Math::Random(0.0f, 1.0f));
				y = Math::Lerp(-episode1PlayArea.y, episode1PlayArea.y, Math::Random(0.0f, 1.0f));
				z = Math::Lerp(-episode1PlayArea.z, episode1PlayArea.z, Math::Random(0.0f, 1.0f));

				if (y < 200)
					d = .8f;
				if (y < 0 && y > -500)
					d = .75f;
				if (y < -500)
					d = .5f;

				UINT whispRanges2[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
				AddCloud(25, D3DXVECTOR3(x, y, z), 300, flatBase, flatBase * 5, d, whispRanges2, sizeof(whispRanges2) / sizeof(UINT));
			}
		}
		break;
	case CloudType::SpotClouds:
		{
			// Randomly place some clouds in the scene.
			D3DXVECTOR3 flatBase = D3DXVECTOR3(20, 2, 20);

			// Cloud are avolume
			float boxSize = 800;
			for (int c = 0; c < 300; c++)
			{
				// Place the cloud randomly in the area.
				x = Math::Lerp(-boxSize, boxSize, Math::Random(0.0f, 1.0f));
				y = Math::Lerp(0, boxSize / 1, Math::Random(0.0f, 1.0f));
				z = Math::Lerp(-boxSize, boxSize, Math::Random(0.0f, 1.0f));

				UINT whispRanges[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
				AddCloud(25, D3DXVECTOR3(x, y, z), 64, flatBase, flatBase * 5, 0.75f, whispRanges, sizeof(whispRanges) / sizeof(UINT));
			}
		}
		break;
	}
}

void Cloud::AddCloud(int whispCount, D3DXVECTOR3 position, float size, D3DXVECTOR3 min, D3DXVECTOR3 max, float colorMod, UINT * whispRange, UINT whispRangeNum)
{
	UINT si = 0;

	for (int w = 0; w < whispCount; w++)
	{
		float x = Math::Lerp(-min.x, max.x, Math::Random(0.0f, 1.0f));
		float y = Math::Lerp(-min.y, max.y, Math::Random(0.0f, 1.0f));
		float z = Math::Lerp(-min.z, max.z, Math::Random(0.0f, 1.0f));

		if (si >= whispRangeNum)
			si = 0;

		clouds->Add(position + D3DXVECTOR3(x, y, z), D3DXVECTOR3(size, size, size), D3DXVECTOR3(whispRange[si++] / 100.0f, 1, ((float)Math::Random(7, 10) / 10.0f) * colorMod));
	}
}

void Cloud::AddCloud(int whispCount, D3DXVECTOR3 position, float size, float radius, float colorMod, UINT * whispRange, UINT whispRangeNum)
{
	UINT si = 0;

	for (int w = 0; w < whispCount; w++)
	{
		float x = Math::Lerp(-radius, radius, Math::Random(0.0f, 1.0f));
		float y = Math::Lerp(-radius, radius, Math::Random(0.0f, 1.0f));
		float z = Math::Lerp(-radius, radius, Math::Random(0.0f, 1.0f));

		if (si >= whispRangeNum)
			si = 0;

		clouds->Add(position + D3DXVECTOR3(x, y, z), D3DXVECTOR3(size, size, size), D3DXVECTOR3(whispRange[si++] / 100.0f, 1, ((float)Math::Random(7, 10) / 10.0f) * colorMod));
	}
}

void Cloud::RotateCloud(float degreePerSec)
{
	D3DXVECTOR3 rotationDegree = clouds->RotationDegree();
	rotationDegree.y += degreePerSec * Time::Delta();
	if (rotationDegree.y > 360) rotationDegree.y -= 360.f;
	if (rotationDegree.y < 0) rotationDegree.y += 360.f;
	clouds->RotationDegree(rotationDegree);
}
