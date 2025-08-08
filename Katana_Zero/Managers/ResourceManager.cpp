#include "pch.h"
#include "ResourceManager.h"
#include "../Resources/Texture.h"

void ResourceManager::Init(HWND hwnd, fs::path directory)
{
	fs::path dir1 = fs::current_path() / L"../GameResources/Textures/UI/lobby_mask.bmp";
	Texture* texture1 = new Texture();
	texture1->Load(dir1, 190);
	_textures["lobby_mask"] = texture1;

	fs::path fence = fs::current_path() / L"../GameResources/Textures/UI/spr_title_fence_0.bmp";
	Texture* texture2 = new Texture();
	texture2->Load(fence);
	_textures["title_fence"] = texture2;

	fs::path bg = fs::current_path() / L"../GameResources/Textures/UI/spr_title_background_0.bmp";
	Texture* texture3 = new Texture();
	texture3->Load(bg);
	_textures["title_bg"] = texture3;

	fs::path select = fs::current_path() / L"../GameResources/Textures/UI/lobby_select_mask.bmp";
	Texture* texture4 = new Texture();
	texture4->Load(select, 100);
	_textures["select_mask"] = texture4;

	{
		fs::path tile_bg = fs::current_path() / L"../GameResources/Textures/Tiles/bg_mansion_background_0.bmp";
		Texture* bg = new Texture();
		bg->Load(tile_bg);
		_tileMapList.push_back(bg);

		fs::path tile_fg = fs::current_path() / L"../GameResources/Textures/Tiles/bg_mansion_foreground_0.bmp";
		Texture* fg = new Texture();
		fg->Load(tile_fg);
		_tileMapList.push_back(fg);

		fs::path garage = fs::current_path() / L"../GameResources/Textures/Tiles/bg_mansion_garage_full_0.bmp";
		Texture* gg = new Texture();
		gg->Load(garage);
		_tileMapList.push_back(gg);

		fs::path actors = fs::current_path() / L"../GameResources/Textures/Tiles/actors_set.bmp";
		Texture* at = new Texture();
		at->Load(actors);
		_tileMapList.push_back(at);
	}

	// 플레이어 애니메이션
	{
		fs::path zero_idle = fs::current_path() / L"../GameResources/Textures/Sprites/Zero/spr_idle.bmp";
		Texture* idle = new Texture();
		idle->LoadSprite(zero_idle, 36, 35, 11, 0.1f, true);
		_textures["zero_idle"] = idle;

		fs::path zero_itr = fs::current_path() / L"../GameResources/Textures/Sprites/Zero/spr_idle_to_run.bmp";
		Texture* itr = new Texture();
		itr->LoadSprite(zero_itr, 44, 32, 4, 0.1f, false);
		_textures["zero_idle_to_run"] = itr;

		fs::path zero_run = fs::current_path() / L"../GameResources/Textures/Sprites/Zero/spr_run.bmp";
		Texture* run = new Texture();
		run->LoadSprite(zero_run, 44, 32, 10, 0.1f, true);
		_textures["zero_run"] = run;

		fs::path zero_rti = fs::current_path() / L"../GameResources/Textures/Sprites/Zero/spr_run_to_idle.bmp";
		Texture* rti = new Texture();
		rti->LoadSprite(zero_rti, 52, 36, 5, 0.1f, false);
		_textures["zero_run_to_idle"] = rti;
	}

	fs::path zero_slash = fs::current_path() / L"../GameResources/Textures/Sprites/Effects/spr_slash.bmp";
	Texture* slash = new Texture();
	slash->LoadSprite(zero_slash, 106, 32, 5, 0.05f, false);
	_textures["zero_slash"] = slash;
}

void ResourceManager::Destroy()
{
}

void ResourceManager::LoadTexture()
{
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
