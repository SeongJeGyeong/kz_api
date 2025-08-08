#pragma once

#include "../Utils/Singleton.h"

enum class KeyState
{
	None,
	Press,
	Down,
	Up,

	End
};

enum class KeyType
{
	LeftMouse = VK_LBUTTON,
	RightMouse = VK_RBUTTON,

	Up = VK_UP,
	Down = VK_DOWN,
	Left = VK_LEFT,
	Right = VK_RIGHT,
	SpaceBar = VK_SPACE,
	ESC = VK_ESCAPE,

	KEY_1 = '1',
	KEY_2 = '2',
	KEY_3 = '3',

	W = 'W',
	A = 'A',
	S = 'S',
	D = 'D',
	L = 'L',
	Q = 'Q',
	E = 'E',
	T = 'T',

	F1 = VK_F1,
	F2 = VK_F2,
};

// �𸮾��� ���
constexpr int32 KEY_TYPE_COUNT = static_cast<int32>(UINT8_MAX) + 1;

class InputManager : public Singleton<InputManager>
{
	friend Singleton<InputManager>;

private:
	InputManager(){}

	HWND _hwnd = 0;
	HWND _subHwnd = 0;
	vector<KeyState> _states;
	POINT _mousePos = {};
	POINT _subMousePos = {};

	KeyState GetState(KeyType key) { return _states[static_cast<uint8>(key)]; }

public:
	void Init(HWND hwnd, HWND subHwnd);
	void Update();

	// ������ ���� ��
	bool GetButtonPressed(KeyType key) { return GetState(key) == KeyState::Press; }

	// �� ó�� ������ ��
	bool GetButtonDown(KeyType key) { return GetState(key) == KeyState::Down; }

	// �� ó�� �����ٰ� �� ��
	bool GetButtonUp(KeyType key) { return GetState(key) == KeyState::Up; }

	// ���콺 ��ǥ ������
	POINT GetMousePos() { return _mousePos; }
	POINT GetSubMousePos() { return _subMousePos; }
};

