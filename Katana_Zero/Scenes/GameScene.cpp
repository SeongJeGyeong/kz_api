#include "pch.h"
#include "GameScene.h"
#include "../Objects/Actors/Player.h"
#include "../Objects/Actors/Tile_FG.h"
#include "../Components/TileRenderer.h"
#include "../Managers/ResourceManager.h"
//#include "../Components/Collider.h"
//#include "../Components/AABBCollider.h"
//#include "../Components/LineCollider.h"

GameScene::GameScene(string mapFileName)
{
	fs::path directory = fs::current_path() / L"../GameResources/Json/";
	fs::path path = directory / mapFileName;
	std::ifstream file(path.c_str());
	if (!file.is_open())
	{
		MessageBox(nullptr, L"Failed to open JSON file", L"Error", MB_OK);
		return;
	}

	for (int32 i = 0; i < ERenderLayer::LAYER_END; ++i)
	{
		_tileRenderer[i] = new TileRenderer();
		_tileRenderer[i]->SetTileMap(ResourceManager::GetInstance()->GetTileMapForIndex(i));
	}

	json data = json::parse(file);
	LoadTiles(data["TileInfo"]);
	LoadColliders(data["ColliderInfo"]);

	file.close();
}

void GameScene::LoadTiles(json tileData)
{
	for (auto data : tileData["Background"])
	{
		Vector2 screenPos = { (float)data[1] * TILE_SIZE + TILE_SIZE / 2, (float)data[2] * TILE_SIZE + TILE_SIZE / 2 };

		_tileRenderer[0]->AddTileInfo({ screenPos.x, screenPos.y }, { data[3], data[4] });

		//Tile_FG* tile = new Tile_FG();
		//tile->Init({ screenPos.x, screenPos.y }, { data[3], data[4] }, data[0]);
		//_tileList.push_back(tile);
	}

	for (auto data : tileData["Foreground"])
	{
		Vector2 screenPos = { (float)data[1] * TILE_SIZE + TILE_SIZE / 2, (float)data[2] * TILE_SIZE + TILE_SIZE / 2 };

		_tileRenderer[1]->AddTileInfo({ screenPos.x, screenPos.y }, { data[3], data[4] });
	}

	for (auto data : tileData["Actor"])
	{
		Vector2 screenPos = { (float)data[1] * TILE_SIZE + TILE_SIZE / 2, (float)data[2] * TILE_SIZE + TILE_SIZE / 2 };

		_tileRenderer[2]->AddTileInfo({ screenPos.x, screenPos.y }, { data[3], data[4] });
	}
}

void GameScene::LoadColliders(json colliderData)
{
	for (int32 type = 0; type < EColliderMode::COL_END; ++type)
	{
		string colliderType;
		switch ((EColliderMode)type)
		{
		case COL_BOX:
			colliderType = "Box";
			break;
		case COL_PLATFORM:
			colliderType = "Platform";
			break;
		case COL_WALL_V:
			colliderType = "Wall_V";
			break;
		case COL_WALL_H:
			colliderType = "Wall_H";
			break;
		case COL_STAIR:
			colliderType = "Stair";
			break;
		default:
			break;
		}

		for (auto data : colliderData[colliderType])
		{
			Vector2 startPoint = { data["Start"][0], data["Start"][1] };
			Vector2 endPoint = { data["End"][0], data["End"][1] };

			Actor* collider = new Actor();
			collider->Init({ (endPoint.x + startPoint.x) * 0.5f, (endPoint.y + startPoint.y) * 0.5f });
			if (type == EColliderMode::COL_BOX)
			{
				collider->CreateAABBCollider(endPoint.x - startPoint.x, endPoint.y - startPoint.y, ECollisionLayer::GROUND);
			}
			else
			{
				switch ((EColliderMode)type)
				{
				case COL_PLATFORM:
					collider->CreateLineCollider(startPoint, endPoint, ECollisionLayer::PLATFORM);
					break;
				case COL_WALL_V:
					collider->CreateLineCollider(startPoint, endPoint, ECollisionLayer::WALL);
					break;
				case COL_WALL_H:
					collider->CreateLineCollider(startPoint, endPoint, ECollisionLayer::CEILING);
					break;
				case COL_STAIR:
					collider->CreateLineCollider(startPoint, endPoint, ECollisionLayer::STAIR);
					break;
				}
			}
			_colliderList.push_back(collider);
		}
	}
}

void GameScene::Init()
{
	Super::Init();

	_player = new Player();
	_player->Init({ GWinSizeX / 2 + 200, GWinSizeY / 2 });
}

void GameScene::Destroy()
{
	Super::Destroy();
}

void GameScene::Update(float deltaTime)
{
	Super::Update(deltaTime);

	if (_player)
	{
		_player->Update(deltaTime);
	}

	for (Actor* collider : _colliderList)
	{
		collider->Update(deltaTime);
	}

	/*for (auto tile : _tileList)
	{
		tile->Update(deltaTime);
	}*/
}

void GameScene::PostUpdate(float deltaTime)
{
	Super::PostUpdate(deltaTime);
}

void GameScene::Render(HDC hdc)
{
	Super::Render(hdc);

	for (int32 i = 0; i < ERenderLayer::LAYER_END; ++i)
	{
		_tileRenderer[i]->RenderComponent(hdc);
	}

	for (Actor* collider : _colliderList)
	{
		collider->Render(hdc);
	}

	if (_player)
	{
		_player->Render(hdc);
	}
}