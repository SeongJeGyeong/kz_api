#pragma once
#include "../Utils/Singleton.h"

class FileData;
class TextureData;

class FileManager : public Singleton<FileManager>
{
	friend Singleton<FileManager>;

private:
	FileManager(){}

	map<string, FileData*> _dataMap;
	map<string, TextureData*> _textureMap;

private:
	void LoadDataObject(fs::path directory, string key, FileData* fileData);
	void LoadTextureData(fs::path directory);
public:
	void Init();
	void Destroy();

	template<typename T>
	const T* GetFileData(string key) const
	{
		auto it = _dataMap.find(key);
		if (it == _dataMap.end())	return nullptr;

		return dynamic_cast<T*>(it->second);
	}



};

