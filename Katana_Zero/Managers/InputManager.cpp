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
	// 키보드와 마우스 상태를 가져옴
	BYTE asciiKeys[KEY_TYPE_COUNT] = {};
	if (::GetKeyboardState(asciiKeys) == false) return;

	for (uint32 key = 0; key < KEY_TYPE_COUNT; ++key)
	{
		// 키가 눌린 상태면 true
		if (asciiKeys[key] & 0x80)
		{
			KeyState& state = _states[key];

			// 이전 프레임에도 키를 누른 상태였으면 press
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

			// 이전 프레임에 키를 누른 상태라면 지금이 키를 뗀 순간이다.
			if (state == KeyState::Press ||  state == KeyState::Down)
				state = KeyState::Up;
			else
				state = KeyState::None;
		}
	}

	// 마우스
	::GetCursorPos(&_mousePos);				// 커서의 좌표를 얻음
	::ScreenToClient(_hwnd, &_mousePos);	// 커서의 좌표를 창 해상도 기준으로 변환

	::GetCursorPos(&_subMousePos);
	::ScreenToClient(_subHwnd, &_subMousePos);
}