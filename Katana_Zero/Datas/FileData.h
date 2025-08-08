#pragma once
class FileData
{
public:
	virtual ~FileData(){}

	virtual string GetFileName() abstract;
	virtual void Load(const json& data) abstract;

	string Utf8ToAnsi(const string& utf8Str);
	wstring Utf8ToWide(const string& utf8Str);
};

