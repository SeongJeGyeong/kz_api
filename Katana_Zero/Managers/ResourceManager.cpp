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

	LoadFile("Textures", "Textures", data, texturePath);

	_cursor = CreateCursorFromImage(texturePath / L"UI/spr_cursor.bmp");
}

void ResourceManager::Destroy()
{
}

void ResourceManager::LoadFile(const string currentKey, const string parentKey, const json& obj, const fs::path& path)
{
	if (obj.is_object())
	{
		for (auto& [key, val] : obj.items())
		{
			LoadFile(key, currentKey, val, path / key);
		}
	}
	else if (obj.is_array())
	{
		for (auto& item : obj)
		{
			string fileName = item["FileName"].get<string>();
			fs::path filePath = path / fileName;
			filePath.replace_extension(".bmp");

			if (parentKey == "Sprites")
			{
				LoadSpriteFile(item, filePath);
			}
			else
			{
				LoadTextureFile(currentKey, item, filePath);
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

	// �ε��� �ȵưų�, �̸��� �߸��ưų�
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

HCURSOR ResourceManager::CreateCursorFromImage(const wstring& path)
{
	HBITMAP hBitmap = (HBITMAP)LoadImageW(
		NULL,
		path.c_str(),
		IMAGE_BITMAP,
		44, 44,  // ���� ũ��
		LR_LOADFROMFILE | LR_CREATEDIBSECTION
	);

	if (!hBitmap) {
		return NULL;
	}

	// ��Ʈ�� ���� ��������
	BITMAP bm;
	GetObject(hBitmap, sizeof(BITMAP), &bm);

	int actualWidth = bm.bmWidth;
	int actualHeight = bm.bmHeight;

	// ����ũ ����
	HBITMAP hMask = CreateBitmap(actualWidth, actualHeight, 1, 1, NULL);

	// Ŀ�� ����
	ICONINFO ii = {};
	ii.fIcon = FALSE;
	ii.xHotspot = actualWidth / 2;
	ii.yHotspot = actualHeight / 2;
	ii.hbmMask = hMask;
	ii.hbmColor = hBitmap;

	HCURSOR hCursor = CreateIconIndirect(&ii);
	DeleteObject(hBitmap);
	DeleteObject(hMask);

	return hCursor;
}
