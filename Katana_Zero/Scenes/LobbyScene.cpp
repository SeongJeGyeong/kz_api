#include "pch.h"
#include "LobbyScene.h"
#include "../UI/UIButton.h"
#include "../Managers/ResourceManager.h"
#include "../Managers/SoundManager.h"

void LobbyScene::Init()
{
	Super::Init();
	_sceneType = ESceneType::LOBBY;

	_UI.CreateImage(Vector2(GWinSizeX / 2, GWinSizeY / 2 - 400), "spr_title_background_0", 1400, 1575);
	_UI.CreateImage(Vector2(GWinSizeX / 2, GWinSizeY / 2 - 400), "spr_title_fence_0", 1400, 1575);
	_UI.CreateImage(Vector2(GWinSizeX / 2, GWinSizeY / 2 + 150), "lobby_mask", 530, 200);

	UIButton* GameStartButton = _UI.CreateButton(Vector2(GWinSizeX / 2, GWinSizeY / 2 + 100), "lobby_select_mask", L"���� ����", 0, 0);
	UIButton* mapEditorButton = _UI.CreateButton(Vector2(GWinSizeX / 2, GWinSizeY / 2 + 150), "lobby_select_mask", L"������ ����", 0, 0);
	UIButton* ExitButton = _UI.CreateButton(Vector2(GWinSizeX / 2, GWinSizeY / 2 + 200), "lobby_select_mask", L"���� ����", 0, 0);

	// �Լ� ������ ����
	GameStartButton->SetClickEvent([this]() { OnStartGame("Stage2"); });
	mapEditorButton->SetClickEvent([this]() { OnOpenEditor(); });
	ExitButton->SetClickEvent([this]() { OnExitGame(); });

	SoundManager::GetInstance()->PlayBGM(EBGMType::MAIN_MENU, true);
}

void LobbyScene::Destroy()
{
	Super::Destroy();
}

void LobbyScene::Update(float deltaTime)
{
	Super::Update(deltaTime);
}

void LobbyScene::Render(HDC hdc)
{
	Super::Render(hdc);
	SetBkColor(hdc, RGB(0, 0, 0));
	HFONT font = ResourceManager::GetInstance()->GetFont(0);
	SelectObject(hdc, font);
}