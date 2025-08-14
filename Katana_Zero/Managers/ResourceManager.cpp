#include "pch.h"
#include "ResourceManager.h"
#include "../Resources/Texture.h"

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

	fs::path UIPath = texturePath / L"UI/";

	Texture* texture1 = new Texture();
	texture1->Load(UIPath / L"lobby_mask.bmp", {0, 0}, 190);
	_textures["lobby_mask"] = texture1;

	Texture* texture2 = new Texture();
	texture2->Load(UIPath / L"spr_title_fence_0.bmp", { 0, 0 });
	_textures["title_fence"] = texture2;

	Texture* texture3 = new Texture();
	texture3->Load(UIPath / L"spr_title_background_0.bmp", { 0, 0 });
	_textures["title_bg"] = texture3;

	Texture* texture4 = new Texture();
	texture4->Load(UIPath / L"lobby_select_mask.bmp", { 0, 0 }, 100);
	_textures["select_mask"] = texture4;

	{
		fs::path tilePath = texturePath / L"Tiles/";

		Texture* bg = new Texture();
		bg->Load(tilePath / L"bg_mansion_background_0.bmp", { 0, 0 });
		_tileMapList.push_back(bg);

		Texture* fg = new Texture();
		fg->Load(tilePath / L"bg_mansion_foreground_0.bmp", { 0, 0 });
		_tileMapList.push_back(fg);

		Texture* gg = new Texture();
		gg->Load(tilePath / L"bg_mansion_garage_full_0.bmp", { 0, 0 });
		_tileMapList.push_back(gg);

		Texture* at = new Texture();
		at->Load(tilePath / L"actors_set.bmp", { 0, 0 });
		_tileMapList.push_back(at);
	}

	// 플레이어 애니메이션
	{
		fs::path playerPath = texturePath / L"Sprites/Zero/";

		Texture* idle = new Texture();
		idle->LoadSprite(playerPath / L"spr_idle.bmp", 36, 35, 11, 0.08f, true, {0, -10});
		_textures["zero_idle"] = idle;

		Texture* itr = new Texture();
		itr->LoadSprite(playerPath / L"spr_idle_to_run.bmp", 44, 32, 4, 0.06f, false, { 0, -10 });
		_textures["zero_idle_to_run"] = itr;

		Texture* run = new Texture();
		run->LoadSprite(playerPath / L"spr_run.bmp", 44, 32, 10, 0.07f, true, { 0, -5 });
		_textures["zero_run"] = run;

		Texture* rti = new Texture();
		rti->LoadSprite(playerPath / L"spr_run_to_idle.bmp", 52, 36, 5, 0.06f, false, { 0, -11 });
		_textures["zero_run_to_idle"] = rti;

		Texture* prec = new Texture();
		prec->LoadSprite(playerPath / L"spr_precrouch.bmp", 36, 40, 2, 0.06f, false, { 0, -13 });
		_textures["zero_PreCrouch"] = prec;

		Texture* c = new Texture();
		c->LoadSprite(playerPath / L"spr_crouch.bmp", 36, 40, 1, 0.06f, true, { 0, -13 });
		_textures["zero_Crouch"] = c;

		Texture* posc = new Texture();
		posc->LoadSprite(playerPath / L"spr_postcrouch.bmp", 36, 40, 2, 0.06f, false, { 0, -13 });
		_textures["zero_PostCrouch"] = posc;

		Texture* jump = new Texture();
		jump->LoadSprite(playerPath / L"spr_jump.bmp", 32, 42, 4, 0.06f, true, { -10, 0 });
		_textures["zero_jump"] = jump;

		Texture* fall = new Texture();
		fall->LoadSprite(playerPath / L"spr_fall.bmp", 42, 48, 4, 0.06f, true, { -10, -5 });
		_textures["zero_fall"] = fall;
	}

	fs::path effectPath = texturePath / L"Sprites/Effects/";
	Texture* slash = new Texture();
	slash->LoadSprite(effectPath / L"spr_slash.bmp", 106, 32, 5, 0.05f, false);
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
