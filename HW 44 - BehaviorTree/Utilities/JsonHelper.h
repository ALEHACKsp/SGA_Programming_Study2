#pragma once

#include "stdafx.h"

class JsonHelper
{
public:
	JsonHelper(string folderName = "");
	~JsonHelper() {};

	void Read(string fileName, Json::Value* parent = NULL);
	void Save(string fileName, Json::Value* parent = NULL);
	void Print(Json::Value* parent = NULL);

	Json::Value& Root() { return root; }
	Json::Value& SubTree(string name) { return root[name]; }
	void Append(string name, Json::Value& value) { root[name].append(value); }

private:
	string folderName;
	Json::Value root;
};