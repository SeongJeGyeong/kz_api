#pragma once
#include "../Utils/Singleton.h"

class Texture;

class ResourceManager : public Singleton<ResourceManager>
{
private:
	unordered_map<string, Texture*> _textures;
	vector<Texture*> _tileMapList;

public:
	void Init(HWND hwnd, fs::path directory);
	void Update(float deltaTime);
	void Destroy() override;

	void LoadTexture();

	Texture* GetTexture(string name);
	vector<Texture*> GetTileMapList() { return _tileMapList; }
	Texture* GetTileMapForIndex(int32 index) { return _tileMapList[index]; }
};