#include "pch.h"
#include "InputManager.h"

void InputManager::Init(HWND hwnd, HWND subHwnd)
{
	_hwnd = hwnd;
	_subHwnd = subHwnd;
	_states.resize(KEY_TYPE_COUNT, KeyState::None);
}

void InputManager::Update()
{
	// Ű����� ���콺 ���¸� ������
	BYTE asciiKeys[KEY_TYPE_COUNT] = {};
	if (::GetKeyboardState(asciiKeys) == false) return;

	for (uint32 key = 0; key < KEY_TYPE_COUNT; ++key)
	{
		// Ű�� ���� ���¸� true
		if (asciiKeys[key] & 0x80)
		{
			KeyState& state = _states[key];

			// ���� �����ӿ��� Ű�� ���� ���¿����� press
			if (state == KeyState::Press || state == KeyState::Down)
			{
				state = KeyState::Press;
			}
			else
			{
				state = KeyState::Down;
			}
		}
		else
		{
			KeyState& state = _states[key];

			// ���� �����ӿ� Ű�� ���� ���¶�� ������ Ű�� �� �����̴�.
			if (state == KeyState::Press ||  state == KeyState::Down)
				state = KeyState::Up;
			else
				state = KeyState::None;
		}
	}

	// ���콺
	::GetCursorPos(&_mousePos);				// Ŀ���� ��ǥ�� ����
	::ScreenToClient(_hwnd, &_mousePos);	// Ŀ���� ��ǥ�� â �ػ� �������� ��ȯ

	::GetCursorPos(&_subMousePos);
	::ScreenToClient(_subHwnd, &_subMousePos);
}