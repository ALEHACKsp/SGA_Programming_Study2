#include "stdafx.h"
#include "TestComputeAppend.h"

TestComputeAppend::TestComputeAppend()
	: dataSize(64)
{
	shader = new Shader(Shaders + L"055_ComputeAppend.fx", true);

	vector<int> data;
	for (int i = 0; i < dataSize; i++)
		data.push_back(i);

	input = new CsAppend(sizeof(int), dataSize, &data[0]);
	output = new CsAppend(sizeof(Data), dataSize, NULL);

	ExecuteCs();
}

TestComputeAppend::~TestComputeAppend()
{
	SAFE_DELETE(shader);

	SAFE_DELETE(input);
	SAFE_DELETE(output);
}

void TestComputeAppend::Update()
{

}

void TestComputeAppend::PreRender()
{

}

void TestComputeAppend::Render()
{

}

void TestComputeAppend::PostRender()
{

}

void TestComputeAppend::ExecuteCs()
{
	//shader->AsShaderResource("cInput")->SetResource(consume);
	shader->AsUAV("Input")->SetUnorderedAccessView(input->UAV());
	shader->AsUAV("Output")->SetUnorderedAccessView(output->UAV());

	shader->Dispatch(0, 0, 2, 1, 1);


	vector<Data> result(dataSize);
	output->Read(&result[0]);

	FILE* file = fopen("Result2.txt", "w");

	for (int i = 0; i < dataSize; i++)
	{
		fprintf
		(
			file, "%d, %d, %d\n",
			result[i].Id, result[i].Value, result[i].Sum
		);
	}

	fclose(file);
}
