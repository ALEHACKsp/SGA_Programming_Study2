#include "JsonHelper.h"
#include <iostream>
#include <fstream>

JsonHelper::JsonHelper(string folderName)
	:folderName(folderName)
{
}

void JsonHelper::Read(string fileName, Json::Value * parent)
{
	ifstream inFile;
	inFile.open(folderName + fileName, ifstream::binary);
	if (parent == NULL)
		inFile >> root;
	else
		inFile >> *parent;
	inFile.close();
}

void JsonHelper::Save(string fileName, Json::Value * parent)
{
	ofstream outFile;
	outFile.open(folderName + fileName, ios::out);
	if (parent == NULL)
		outFile << root;
	else
		outFile << *parent;
	outFile.close();
}

void JsonHelper::Print(Json::Value * parent)
{
	Json::StyledWriter writer;
	string out;
	if (parent == NULL)
		out = writer.write(root);
	else
		out = writer.write(*parent);
	cout << out;
}
