#include "pch.h"
#include "LobbyScene.h"
#include "../Game/Game.h"
#include "../UI/UIButton.h"

void LobbyScene::Init()
{
	Super::Init();
	_sceneType = ESceneType::LOBBY;

	_UI.CreateImage(Vector2(GWinSizeX / 2, GWinSizeY / 2 - 400), "title_bg", 1400, 1575);
	_UI.CreateImage(Vector2(GWinSizeX / 2, GWinSizeY / 2 - 400), "title_fence", 1400, 1575);
	_UI.CreateImage(Vector2(GWinSizeX / 2, GWinSizeY / 2 + 200), "lobby_mask");

	UIButton* GameStartButton = _UI.CreateButton(Vector2(GWinSizeX / 2, GWinSizeY / 2 + 100), "select_mask", 0, 0, L"게임 시작");
	UIButton* mapEditorButton = _UI.CreateButton(Vector2(GWinSizeX / 2, GWinSizeY / 2 + 150), "select_mask", 0, 0, L"에디터 열기");
	UIButton* ExitButton = _UI.CreateButton(Vector2(GWinSizeX / 2, GWinSizeY / 2 + 200), "select_mask", 0, 0, L"게임 종료");

	// 함수 포인터 연결
	GameStartButton->SetClickEvent([this]() { OnClickStartGameBtn(); });
	mapEditorButton->SetClickEvent([this]() { OnClickOpenEditorBtn(); });
	ExitButton->SetClickEvent([this]() { OnClickExitBtn(); });
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
	HFONT font = Game::GetInstance()->GetFont();
	SelectObject(hdc, font);
}

void LobbyScene::OnClickStartGameBtn()
{
	Game::GetInstance()->ChangeGameScene();
}

void LobbyScene::OnClickOpenEditorBtn()
{
	Game::GetInstance()->ChangeEditorScene();
}

void LobbyScene::OnClickExitBtn()
{
	Game::GetInstance()->ExitGame();
}
