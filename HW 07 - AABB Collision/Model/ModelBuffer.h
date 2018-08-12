#pragma once

#include "stdafx.h"

#include "Model.h"

class ModelBuffer
{
public:
	ModelBuffer() {}
	~ModelBuffer()
	{
		map<wstring, Model*>::iterator iter;
		iter = models.begin();
		for (; iter != models.end();) {
			if (iter->second != NULL) {
				SAFE_DELETE(iter->second);
				iter = models.erase(iter);
			}
			else
				++iter;
		}
		models.clear();
	}

	Model* AddModel(wstring modelName, wstring folder, wstring matFile,
		wstring meshFile)
	{
		// 중복 체크
		if (models.count(modelName) > 0)
			return GetModel(modelName);

		Model * model = new Model();

		models[modelName] = model;
		model->ReadMaterial(folder, matFile);
		model->ReadMesh(folder, meshFile);

		void * newModel;
		models[modelName]->Clone(&newModel);
		return (Model*)newModel;
	}

	Model* GetModel(wstring modelName)
	{
		// 존재 체크
		assert(models.count(modelName) > 0);

		void * model;
		models[modelName]->Clone(&model);
		return (Model*)model;
	}
private:
	map<wstring, Model*> models;
};