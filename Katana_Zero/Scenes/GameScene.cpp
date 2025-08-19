#include "pch.h"
#include "GameScene.h"
#include "../Objects/Actors/Player.h"
#include "../Components/TileRenderer.h"
#include "../Managers/ResourceManager.h"
#include "../Objects/Camera.h"
#include "../Objects/Actors/Enemy.h"

GameScene::GameScene(string mapFileName)
{
	_sceneCamera = new Camera();
	_sceneCamera->Init();

	fs::path directory = fs::current_path() / L"../GameResources/Json/";
	fs::path path = directory / mapFileName;
	std::ifstream file(path.c_str());
	if (!file.is_open())
	{
		MessageBox(nullptr, L"Failed to open JSON file", L"Error", MB_OK);
		return;
	}

	json data = json::parse(file);
	iSceneSizeX = data["MapSize"][0];
	iSceneSizeY = data["MapSize"][1];


	_tileRenderer = new TileRenderer();
	_tileRenderer->InitComponent(iSceneSizeX, iSceneSizeY);
	for (int32 i = 0; i < ERenderLayer::LAYER_END; ++i)
	{
		_tileRenderer->AddTileTexture(ResourceManager::GetInstance()->GetTileMapForIndex(i));
	}

	LoadTiles(data["TileInfo"]);
	LoadColliders(data["ColliderInfo"]);
	LoadActors(data["TileInfo"]);
	_sceneCamera->SetWorldSize(iSceneSizeX, iSceneSizeY);
	file.close();
}

void GameScene::LoadTiles(json tileData)
{
	for (auto data : tileData["Background"])
	{
		Vector2 screenPos = { data[1].get<float>() * TILE_SIZE + TILE_SIZE * 0.5f, data[2].get<float>() * TILE_SIZE + TILE_SIZE * 0.5f};

		_tileRenderer->AddTileInfo(data[0].get<int>(), {screenPos.x, screenPos.y}, {data[3], data[4]});
	}

	for (auto data : tileData["Foreground"])
	{
		Vector2 screenPos = { data[1].get<float>() * TILE_SIZE + TILE_SIZE * 0.5f, data[2].get<float>() * TILE_SIZE + TILE_SIZE * 0.5f };

		_tileRenderer->AddTileInfo(data[0].get<int>(), {screenPos.x, screenPos.y}, {data[3], data[4]});
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
		case COL_PORTAL:
			colliderType = "Portal";
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
			else if (type == EColliderMode::COL_PORTAL)
			{
				collider->CreateAABBCollider(endPoint.x - startPoint.x, endPoint.y - startPoint.y, ECollisionLayer::PORTAL);
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

void GameScene::LoadActors(json actorData)
{
	for (auto data : actorData["Actor"])
	{
		Vector2 screenPos = { (float)data[1] * TILE_SIZE + TILE_SIZE / 2, (float)data[2] * TILE_SIZE + TILE_SIZE / 2 };

		if (data[3].get<int>() == 0)
		{
			_player = new Player();
			_player->Init({ screenPos.x, screenPos.y - 50.f });
			_player->SetPlayerCamera(_sceneCamera);
		}
		else
		{
			Enemy* enemy = new Enemy();
			enemy->Init({ screenPos.x, screenPos.y - 70.f });
			enemy->SetCamera(_sceneCamera);
			_EnemyList.push_back(enemy);
		}
	}
}

void GameScene::Init()
{
	Super::Init();
}

void GameScene::Destroy()
{
	Super::Destroy();
}

void GameScene::Update(float deltaTime)
{
	if (_player)
	{
		_player->Update(deltaTime);
	}

	for (Enemy* enemy : _EnemyList)
	{
		enemy->Update(deltaTime);
	}

	for (Actor* collider : _colliderList)
	{
		collider->Update(deltaTime);
	}

	Super::Update(deltaTime);
}

void GameScene::PostUpdate(float deltaTime)
{
	Super::PostUpdate(deltaTime);

	if (_player)
	{
		_player->PostUpdate(deltaTime);
	}

	for (Enemy* enemy : _EnemyList)
	{
		enemy->PostUpdate(deltaTime);
	}

	for (Actor* collider : _colliderList)
	{
		collider->PostUpdate(deltaTime);
	}
}

void GameScene::Render(HDC hdc)
{
	Super::Render(hdc);

	if (_tileRenderer)
	{
		_tileRenderer->RenderComponent(hdc);
	}

	for (Actor* collider : _colliderList)
	{
		collider->Render(hdc);
	}

	for (Enemy* enemy : _EnemyList)
	{
		enemy->Render(hdc);
	}

	if (_player)
	{
		_player->Render(hdc);
	}
}