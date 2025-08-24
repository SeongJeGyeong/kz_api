#include "pch.h"
#include "GameScene.h"
#include "../Objects/Actors/Player.h"
#include "../Components/TileRenderer.h"
#include "../Managers/ResourceManager.h"
#include "../Objects/Camera.h"
#include "../Objects/Actors/Enemy.h"
#include "../Objects/Actors/Bullet.h"
#include "../Objects/Actors/Boss.h"
#include "../Managers/CollisionManager.h"
#include "../Objects/Actors/Axe.h"
#include "../UI/UIImage.h"
#include "../UI/UIButton.h"
#include "../UI/UIBundle.h"
#include "../UI/UIProgressBar.h"
#include "../Managers/InputManager.h"
#include "../Game/Game.h"
#include "../Managers/TimeManager.h"
#include "../Resources/Texture.h"
#include "../Managers/SoundManager.h"

GameScene::GameScene(string mapFileName, string nextStage, string bgmName)
{
	sMapName = mapFileName;
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

	if (data["TileInfo"].is_null())
	{
		iSceneSizeX = 1280;
		iSceneSizeY = 720;
	}
	else
	{
		iSceneSizeX = data["MapSize"][0];
		iSceneSizeY = data["MapSize"][1];
	}
	_tileRenderer = new TileRenderer();
	_tileRenderer->InitComponent(iSceneSizeX, iSceneSizeY);
	for (int32 i = 0; i < ERenderLayer::LAYER_END; ++i)
	{
		_tileRenderer->AddTileTexture(ResourceManager::GetInstance()->GetTileMapForIndex(i));
	}

	if (data["TileInfo"].is_null())
	{
		_tileRenderer->AddWholeMap(2);
	}
	else
	{
		LoadTiles(data["TileInfo"]);
	}

	LoadColliders(data["ColliderInfo"]);
	LoadActors(data["ActorInfo"]);
	_sceneCamera->SetWorldSize(iSceneSizeX, iSceneSizeY);
	file.close();

	fCurrentElapsedTime = 0.f;
	_nextStage = nextStage;

	if (bgmName == "song_katanazero")
	{
		SoundManager::GetInstance()->PlayBGM(EBGMType::STAGE, 1.f, 1.f, true);
	}
	else if (bgmName == "song_bossbattle")
	{
		SoundManager::GetInstance()->PlayBGM(EBGMType::BOSS_ROOM, 1.f, 1.f, true);
	}

	UIInit();
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

		if (data[3].get<int>() == 2)
		{
			Enemy* enemy = new Enemy();
			enemy->Init({ screenPos.x, screenPos.y - 70.f });
			enemy->SetCamera(_sceneCamera);
			enemy->OnCreateBullet = bind(&GameScene::CreateBullet, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
			_EnemyList.push_back(enemy);
		}
		else if (data[3].get<int>() == 0)
		{
			_player = new Player();
			_player->Init({ screenPos.x, screenPos.y - 50.f });
			_player->SetPlayerCamera(_sceneCamera);
		}
		else
		{
			_boss = new Boss();
			_boss->Init({ screenPos.x, screenPos.y - 40.f });
			_boss->OnSpawnAxe = bind(&GameScene::SpawnAxe, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
		}
	}
}

void GameScene::Init()
{
	Super::Init();
	for (const auto& enemy : _EnemyList)
	{
		enemy->SetPlayer(_player);
	}
	if (_boss)
	{
		_boss->SetPlayer(_player);
	}
}

void GameScene::Destroy()
{
	Super::Destroy();

	for (auto& enemy : _EnemyList)
	{
		SAFE_DELETE(enemy);
	}
	_EnemyList.clear();
	for (auto& bullet : _BulletList)
	{
		SAFE_DELETE(bullet);
	}
	_BulletList.clear();
	for (auto& collider : _colliderList)
	{
		SAFE_DELETE(collider);
	}
	_colliderList.clear();

	SAFE_DELETE(_boss);
	SAFE_DELETE(_axe);
	SAFE_DELETE(_player);
	SAFE_DELETE(_sceneCamera);
}

void GameScene::Update(float deltaTime)
{
	if (bIsPaused)
	{
		_pauseUI.Update();
		return;
	}

	TimeManager* timeManager = TimeManager::GetInstance();
	fCurrentElapsedTime += timeManager->GetConstDeltaTime();
	float progress = (fTimeLimit - fCurrentElapsedTime) / fTimeLimit;
	_timerProgressBar->SetProgress(progress);
	_slowMotionBattery->SetBundleCount(timeManager->GetSlowMotionBatteryCount());

	if (_player)
	{
		_player->Update(deltaTime);
	}

	if (_boss)
	{
		_boss->Update(deltaTime);
	}

	for (Enemy* enemy : _EnemyList)
	{
		enemy->Update(deltaTime);
	}

	for (Bullet* bullet : _BulletList)
	{
		bullet->Update(deltaTime);
	}

	if (_axe)
	{
		_axe->Update(deltaTime);
	}

	for (Actor* collider : _colliderList)
	{
		collider->Update(deltaTime);
	}

	Super::Update(deltaTime);
}

void GameScene::PostUpdate(float deltaTime)
{
	if (InputManager::GetInstance()->GetButtonDown(KeyType::ESC))
	{
		bIsPaused = !bIsPaused;
		//OnPause(bIsPaused);
	}

	if (bIsPaused) return;

	Super::PostUpdate(deltaTime);

	if (_player)
	{
		_player->PostUpdate(deltaTime);
	}

	if (_boss)
	{
		_boss->PostUpdate(deltaTime);
	}

	for (Enemy* enemy : _EnemyList)
	{
		enemy->PostUpdate(deltaTime);
	}

	for (auto it = _BulletList.begin(); it != _BulletList.end();)
	{
		if ((*it)->GetIsDead())
		{
			SAFE_DELETE((*it));
			it = _BulletList.erase(it);
		}
		else
		{
			(*it)->PostUpdate(deltaTime);
			++it;
		}
	}

	if (_axe)
	{
		_axe->PostUpdate(deltaTime);
	}

	for (Actor* collider : _colliderList)
	{
		collider->PostUpdate(deltaTime);
	}
}

void GameScene::Render(HDC hdc)
{
	if (_tileRenderer)
	{
		_tileRenderer->RenderComponent(hdc);
	}

	for (Actor* collider : _colliderList)
	{
		collider->Render(hdc);
	}

	if(TimeManager::GetInstance()->GetSlowMotion() && _slowMotionMask)
	{
		_slowMotionMask->RenderTexture(hdc, Vector2(GWinSizeX / 2, GWinSizeY / 2), GWinSizeX, GWinSizeY, TimeManager::GetInstance()->GetMaskAlpha());
	}

	for (Enemy* enemy : _EnemyList)
	{
		enemy->Render(hdc);
	}

	if (_boss)
	{
		_boss->Render(hdc);
	}

	if (_player)
	{
		_player->Render(hdc);
	}

	if (_axe)
	{
		_axe->Render(hdc);
	}

	for (Bullet* bullet : _BulletList)
	{
		bullet->Render(hdc);
	}

	Super::Render(hdc);

	if (bIsPaused)
	{
		_pauseUI.Render(hdc);

		SetTextColor(hdc, RGB(255, 255, 0));

		HFONT font = ResourceManager::GetInstance()->GetFont(4);
		HFONT oldFont = (HFONT)SelectObject(hdc, font);

		wstring pausestr = std::format(L"KATANA ZERO 일시정지");
		::TextOut(hdc, 50, 680, pausestr.c_str(), static_cast<int32>(pausestr.size()));

		SelectObject(hdc, oldFont);
	}
}

void GameScene::UIInit()
{
	UIImage* HUD_Background = _UI.CreateImage(Vector2(GWinSizeX / 2, 27), "spr_hud", 1280, 46);
	_timerProgressBar = _UI.CreateProgressBar(Vector2(GWinSizeX / 2 + 6, 19), "spr_timer", 188, 22);
	UIImage* HUD_TimerHUD = _UI.CreateImage(Vector2(GWinSizeX / 2 - 10, 23), "spr_hud_timer", 224, 38);
	UIImage* HUD_BatteryHUD = _UI.CreateImage(Vector2(79, 27), "spr_hud_battery", 154, 38);
	_slowMotionBattery = _UI.CreateBundle(Vector2(28, 26), "spr_battery_part", 8, 20, TimeManager::GetInstance()->GetSlowMotionBatteryCount());
	_slowMotionBattery->SetMargin(2.f);

	UIImage* pauseBackground = _pauseUI.CreateImage(Vector2(GWinSizeX / 2, GWinSizeY / 2), "spr_pausemenu_bg_0");
	UIImage* pauseBackground2 = _pauseUI.CreateImage(Vector2(GWinSizeX / 2, GWinSizeY / 2 + 336), "spr_pausemenu_bg_2");
	UIButton* resumeBtn = _pauseUI.CreateButton(Vector2(GWinSizeX / 2, 120), "lobby_select_mask", L"이어하기", 1280, 50, 4);
	UIButton* retryBtn = _pauseUI.CreateButton(Vector2(GWinSizeX / 2, 180), "lobby_select_mask", L"재시작", 1280, 50, 4);
	UIButton* mainMenuBtn = _pauseUI.CreateButton(Vector2(GWinSizeX / 2, 240), "lobby_select_mask", L"메인 메뉴", 1280, 50, 4);

	//UIImage* pauseBackground = _UI.CreateImage(Vector2(GWinSizeX / 2, GWinSizeY / 2), "spr_pausemenu_bg_0");
	//UIImage* pauseBackground2 = _UI.CreateImage(Vector2(GWinSizeX / 2, GWinSizeY / 2 + 336), "spr_pausemenu_bg_2");
	//UIButton* resumeBtn = _UI.CreateButton(Vector2(GWinSizeX / 2, 120), "lobby_select_mask", L"이어하기", 1280, 50, 4);
	//UIButton* retryBtn = _UI.CreateButton(Vector2(GWinSizeX / 2, 180), "lobby_select_mask", L"재시작", 1280, 50, 4);
	//UIButton* mainMenuBtn = _UI.CreateButton(Vector2(GWinSizeX / 2, 240), "lobby_select_mask", L"메인 메뉴", 1280, 50, 4);

	resumeBtn->SetButtonTextAlign(EButtonTextAlign::LEFT);
	resumeBtn->SetMargin(20.f);
	retryBtn->SetButtonTextAlign(EButtonTextAlign::LEFT);
	retryBtn->SetMargin(20.f);
	mainMenuBtn->SetButtonTextAlign(EButtonTextAlign::LEFT);
	mainMenuBtn->SetMargin(20.f);

	resumeBtn->SetClickEvent([this]() { bIsPaused = !bIsPaused; });
	retryBtn->SetClickEvent([this]() { OnRetryGame(sMapName); });
	mainMenuBtn->SetClickEvent([this]() { OnExitToMainMenu(); });

	//pauseBackground->SetOpen(false);
	//pauseBackground2->SetOpen(false);
	//resumeBtn->SetOpen(false);
	//retryBtn->SetOpen(false);
	//mainMenuBtn->SetOpen(false);
	//OnPause = [pauseBackground, pauseBackground2, resumeBtn, retryBtn, mainMenuBtn](bool isOpen)
	//	{
	//		pauseBackground->SetOpen(isOpen);
	//		pauseBackground2->SetOpen(isOpen);
	//		resumeBtn->SetOpen(isOpen);
	//		retryBtn->SetOpen(isOpen);
	//		mainMenuBtn->SetOpen(isOpen);
	//	};

	_slowMotionMask = ResourceManager::GetInstance()->GetTexture("background_mask");
	_slowMotionMask->SetAlpha(0);
}

void GameScene::CreateBullet(Vector2 pos, Vector2 dir, float length, float radian)
{
	Bullet* bullet = new Bullet();
	bullet->Init(pos, dir, length, radian);
	_BulletList.push_back(bullet);
}

void GameScene::SpawnAxe(Vector2 pos, Actor* owner, Vector2 dir, bool throwOrSwing)
{
	if (_axe == nullptr)
	{
		_axe = new Axe();
	}
	_axe->Init(pos, owner, dir, throwOrSwing);
	_axe->SetIsActive(true);
	_axe->OnReturnAxe = bind(&Boss::ReturnAxe, _boss);
}