#pragma once
#include "../Utils/Singleton.h"

class Scene;
class EditorSub;
class Camera;
class Player;

class Game : public Singleton<Game>
{
	// protected, private ������� ���� �����ϰ� �������
	friend Singleton<Game>;

protected:
	// ������ �ܺο��� ��� ���ϰ� protected�� ����
	Game() {}

private:
	HWND	_hwnd;		// ������ �ڵ�
	HWND	_hwndSub;	// ���� ������â �ڵ�

	HDC		_hdc;	// ȭ�鿡 �׷����� ����(����Ʈ ����)

	// ���� ���۸�
	RECT	_rect;			// ������ ����
	HDC		_hdcBack = {};	// �� ����
	HBITMAP	_bmpBack = {};	// bitmap
	DWORD	_background = BLACKNESS;

	Scene* _currScene = nullptr;	// ���� Ȱ��ȭ�� ���� ������Ʈ/�׷��ش�.
	Scene* _nextScene = nullptr;	// ���� ���� ��� ������ ���� ƽ�� ����
	EditorSub* _subWindow = nullptr;

	HFONT _font;
	bool bDebugMode = false;

public:
	void Init(HWND hwnd, HWND hsubwnd);
	void Destroy();

	void Update();
	void Render();

	void ChangeGameScene();
	void ChangeLobbyScene();
	void ChangeEditorScene();
	void ExitGame();

	HWND GetHwnd() { return _hwnd; }
	HWND GetHwndSub() { return _hwndSub; }
	Scene* GetCurrScene() { return _currScene; }
	HFONT  GetFont() { return _font; }

	bool GetDebugMode() { return bDebugMode; }
	EditorSub* GetSubWindow() { return _subWindow; }

	Vector2 GetCurrentSceneSize();
	Camera* GetCurrentSceneCamera();
	Vector2 ConvertCurSceneScreenPos(Vector2 worldPos);
};

