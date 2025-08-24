#include "pch.h"
#include "Game.h"
#include "../Managers/InputManager.h"
#include "../Managers/TimeManager.h"
#include "../Managers/ResourceManager.h"
#include "../Scenes/Scene.h"
#include "../Scenes/LobbyScene.h"
#include "../Scenes/EditorScene.h"
#include "../Scenes/EditorSub.h"
#include "../Scenes/GameScene.h"
#include "../Managers/CollisionManager.h"
#include "../Objects/Camera.h"
#include "../Managers/SoundManager.h"

void Game::Init(HWND hwnd, HWND hsubwnd)
{
	_hwnd = hwnd;
	_hwndSub = hsubwnd;
	_hdc = ::GetDC(hwnd);	// �⺻ ��ȭ�� �Ѱܹޱ�

	// ���� ���۸� ���� �߰�
	::GetClientRect(hwnd, &_rect);

	_hdcBack = ::CreateCompatibleDC(_hdc); // �⺻ hdc�� ȣȯ�Ǵ� DC�� ����

	// �̰� �߰��� �ʿ��� ������ ������ ȣȯ hdc�� �츮�� ���ϴ� ũ���� ���۰� �ƴ϶� ���� ����
	// �����̴�. �츮�� �츮 ������ ��ü ������ �׸� ��ȭ���� �ʿ��ѰŶ�
	// ������ ��Ʈ���� ����, ����ٰ� ������ �׸����̴�.
	_bmpBack = ::CreateCompatibleBitmap(_hdc, _rect.right, _rect.bottom); // hdc�� ȣȯ�Ǵ� ��Ʈ�� ����

	// ����ٰ� ������ ����� HDC�� ���ӻ���� �´� �ؽ��ĸ� �����Ѵ�.
	HBITMAP prev = (HBITMAP)::SelectObject(_hdcBack, _bmpBack); // DC�� BMP�� ����
	::DeleteObject(prev);

	// �ܺ� ��Ʈ�� �ý��ۿ� �ӽ÷� ���
	fs::path directory = fs::current_path() / L"../GameResources/";
	AddFontResourceEx((directory / L"Fonts/DungGeunMo.ttf").c_str(), FR_PRIVATE, 0);

	// ���Ӱ� ���õ� �ʱ�ȭ�� �ʿ��� �׸���� ���⼭
	InputManager::GetInstance()->Init(_hwnd, _hwndSub);	// ��ü�� ����
	ResourceManager::GetInstance()->Init(_hwnd, directory);
	SoundManager::GetInstance()->Init(directory);
	CollisionManager::GetInstance()->Init();
	TimeManager::GetInstance()->Init();

	ChangeLobbyScene();
	//_curScene = new LobbyScene();
	//_curScene->Init();

	fs::path path = directory / L"Json/StageInfo.json";
	std::ifstream file(path.c_str());
	if (!file.is_open())
	{
		MessageBox(nullptr, L"Failed to open JSON file", L"Error", MB_OK);
		return;
	}

	_stageInfo = json::parse(file);
}

void Game::Destroy()
{
	InputManager::DestroyInstance();
	TimeManager::DestroyInstance();
	ResourceManager::DestroyInstance();
	CollisionManager::DestroyInstance();

	if (_curScene)
	{
		_curScene->Destroy();
	}
	SAFE_DELETE(_curScene);
}

void Game::Update()
{
	// CurScene ���� ��û�� ������, ���� ���� ������� �ϰ�, �ش� �� ������Ʈ�� �����Ѵ�.
	if (_nextScene)
	{
		if (_curScene)
		{
			_curScene->Destroy();
			delete _curScene;
			_curScene = nullptr;
		}

		_curScene = _nextScene;
		_curScene->Init();

		_nextScene = nullptr;
	}

	TimeManager::GetInstance()->Update();
	InputManager::GetInstance()->Update();

	HWND hwnd = ::GetForegroundWindow();
	if (_hwnd == hwnd && _curScene)
	{
		// update �Է� ó�� -> �浹 üũ -> postupdate ��ġ �̵�
		_curScene->Update(TimeManager::GetInstance()->GetDeltaTime());
		CollisionManager::GetInstance()->Update();
		_curScene->PostUpdate(TimeManager::GetInstance()->GetDeltaTime());
		CollisionManager::GetInstance()->PostUpdate();
	}
	else if (_hwndSub == hwnd && _subWindow)
	{
		_subWindow->Update();
	}

	if (InputManager::GetInstance()->GetButtonDown(KeyType::F1))
	{
		bDebugMode = !bDebugMode;
	}
}

void Game::Render()
{
	if (_curScene)
	{
		_curScene->Render(_hdcBack);
	}

	if (_subWindow)
	{
		_subWindow->Render();
	}

	uint32 fps = TimeManager::GetInstance()->GetFps();
	float deltaTime = TimeManager::GetInstance()->GetDeltaTime();
	SetTextColor(_hdcBack, RGB(255, 255, 255));
	{

		POINT mousePos = InputManager::GetInstance()->GetMousePos();
		wstring str = std::format(L"Mouse({0}, {1})", mousePos.x, mousePos.y);
		::TextOut(_hdcBack, 300, 10, str.c_str(), static_cast<int32>(str.size()));
	}

	{
		wstring str = std::format(L"FPS({0}), DT({1})", fps, deltaTime);
		::TextOut(_hdcBack, 200, 30, str.c_str(), static_cast<int32>(str.size()));
	}

	CollisionManager::GetInstance()->Render(_hdcBack);

	// ���� ��ȭ���� ������ ��.
	// �ѹ� �׸��� �� �׷�����, ������ ����Ʈ ���ۿ� ����.
	::BitBlt(_hdc, 0, 0, _rect.right, _rect.bottom, _hdcBack, 0, 0, SRCCOPY); // ��Ʈ �� : ��� ����

	// ����Ʈ ���ۿ� ���簡 ��������, ����۴� �ʱ�ȭ
	::PatBlt(_hdcBack, 0, 0, _rect.right, _rect.bottom, _background);
}

void Game::ChangeGameScene(string mapName)
{
	if (_stageInfo[mapName].is_null()) return;

	if (_nextScene)
	{
		delete _nextScene;
		_nextScene = nullptr;
	}

	_nextScene = new GameScene(_stageInfo[mapName]["FileName"], _stageInfo[mapName]["NextStage"], _stageInfo[mapName]["BGM"]);
	GameScene* gameScene = static_cast<GameScene*>(_nextScene);
	gameScene->OnRetryGame = [this](string mapName) { ChangeGameScene(mapName); };
	gameScene->OnExitToMainMenu = [this](){ ChangeLobbyScene(); };
	_background = BLACKNESS;

	HCURSOR newCurosr = ResourceManager::GetInstance()->GetCursor();
	SetClassLongPtr(_hwnd, GCLP_HCURSOR, (LONG_PTR)newCurosr);
}

void Game::ChangeLobbyScene()
{
	if (_nextScene)
	{
		delete _nextScene;
		_nextScene = nullptr;
	}

	_nextScene = new LobbyScene();
	LobbyScene* lobbyScene = static_cast<LobbyScene*>(_nextScene);
	lobbyScene->OnStartGame = [this](string mapName) { ChangeGameScene(mapName); };
	lobbyScene->OnOpenEditor = [this]() { ChangeEditorScene(); };
	lobbyScene->OnExitGame = [this]() { ExitGame(); };

	_background = BLACKNESS;
}

void Game::ChangeEditorScene()
{
	if (_nextScene)
	{
		delete _nextScene;
		_nextScene = nullptr;
	}

	_nextScene = new EditorScene();
	_background = WHITENESS;

	_subWindow = new EditorSub();
	_subWindow->Init();
	ShowWindow(_hwndSub, 10);
	UpdateWindow(_hwndSub);
}

void Game::ExitGame()
{
	DestroyWindow(_hwndSub);
	DestroyWindow(_hwnd);
}

Vector2 Game::GetCurrentSceneSize()
{
	if (_curScene)
	{
		return _curScene->GetSceneSize();
	}

	return Vector2();
}

Camera* Game::GetCurrentSceneCamera()
{
	if (_curScene)
	{
		return _curScene->GetSceneCamera();
	}

	return nullptr;
}

Vector2 Game::ConvertCurSceneScreenPos(Vector2 worldPos)
{
	if (_curScene == nullptr) return Vector2();

	Camera* camera = _curScene->GetSceneCamera();
	if (camera == nullptr) return Vector2();

	return camera->ConvertScreenPos(worldPos);
}

void Game::Restart()
{
	//if (_curScene)
	//{
	//	string mapFileName = _curScene->GetMapName();
	//	_curScene->Destroy();
	//	_curScene = new GameScene(mapFileName);
	//	_curScene->Init();
	//}
}