#include "pch.h"
#include "ResourceManager.h"
#include "../Resources/Texture.h"
#include "../Resources/Sprite.h"

void ResourceManager::Init(HWND hwnd, fs::path directory)
{
	fs::path path = directory / L"Json/Textures.json";
	std::ifstream file(path.c_str());
	if (!file.is_open())
	{
		MessageBox(nullptr, L"Failed to open JSON file", L"Error", MB_OK);
		return;
	}

	json data = json::parse(file);

	fs::path texturePath = directory / L"Textures/";

	LoadFile("Textures", data, texturePath);
}

void ResourceManager::Destroy()
{
}

void ResourceManager::LoadFile(const string parentKey, const json& obj, const fs::path& path)
{
	if (obj.is_object())
	{
		for (auto& [key, val] : obj.items())
		{
			LoadFile(key, val, path / key);
		}
	}
	else if (obj.is_array())
	{
		for (auto& item : obj)
		{
			string fileName = item["FileName"].get<string>();
			fs::path filePath = path / fileName;
			filePath.replace_extension(".bmp");

			if (parentKey == "Zero" || parentKey == "Effects")
			{
				LoadSpriteFile(item, filePath);
			}
			else
			{
				LoadTextureFile(parentKey, item, filePath);
			}
		}
	}
}

void ResourceManager::LoadTextureFile(const string parentKey, const json& obj, const fs::path& path)
{
	Texture* texture = new Texture();

	if (parentKey == "Tiles")
	{
		texture->Load(path);
		_tileMapList.push_back(texture);
	}
	else
	{
		texture->Load(path, obj["Alpha"].get<int>());
		_textures[obj["FileName"].get<string>()] = texture;
	}
}

void ResourceManager::LoadSpriteFile(const json& obj, const fs::path& path)
{
	Sprite* sprite = new Sprite();
	sprite->LoadSprite(
		path, 
		obj["FrameX"].get<int>(), 
		obj["FrameY"].get<int>(), 
		2, 
		obj["FrameCount"].get<int>(), 
		obj["Duration"].get<float>(),
		obj["Loop"].get<bool>(),
		obj["Alpha"].get<int>()
	);
	_sprites[obj["FileName"].get<string>()] = sprite;
}

Texture* ResourceManager::GetTexture(string name)
{
	auto it = _textures.find(name);
	if (it != _textures.end())
	{
		return it->second;
	}

	// 로딩이 안됐거나, 이름이 잘못됐거나
	return nullptr;
}

Sprite* ResourceManager::GetSprite(string name)
{
	auto it = _sprites.find(name);
	if (it != _sprites.end())
	{
		return it->second;
	}

	return nullptr;
}
