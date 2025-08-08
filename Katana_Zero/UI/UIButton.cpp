#include "pch.h"
#include "UIButton.h"
#include "../Game/Game.h"
#include "../Managers/ResourceManager.h"
#include "../Managers/InputManager.h"
#include "../Resources/Texture.h"

UIButton::UIButton(Vector2 pos, string key, int32 width, int32 height)
	: Super(pos)
{
	_texture = ResourceManager::GetInstance()->GetTexture(key);
}

void UIButton::Update()
{
	if (InputManager::GetInstance()->GetButtonDown(KeyType::LeftMouse))
	{
		wstring str = sButtonText;

		RECT rect;
		rect.left = (int32)vPos.x - (int32)(_texture->GetTextureSizeX() * 0.5f);
		rect.right = (int32)vPos.x + (int32)(_texture->GetTextureSizeX() * 0.5f);
		rect.top = (int32)vPos.y - (int32)(_texture->GetTextureSizeY() * 0.5f);
		rect.bottom = (int32)vPos.y + (int32)(_texture->GetTextureSizeY() * 0.5f);

		POINT mousePos = InputManager::GetInstance()->GetMousePos();
		if (IsInPoint(rect, mousePos))
		{
			if (onClickedEvent)
				onClickedEvent();
		}
	}
}

void UIButton::Render(HDC hdc)
{
	if (_texture)
	{
		_texture->Render(hdc, vPos);
	}

	if (sButtonText != L"")
	{
		SetTextAlign(hdc, TA_CENTER);
		SetTextColor(hdc, RGB(255, 255, 255));
		SetBkMode(hdc, TRANSPARENT);
		::TextOut(hdc, vPos.x, vPos.y - 5, sButtonText.c_str(), static_cast<int32>(sButtonText.size()));
	}
}

void UIButton::OnBtnHovered()
{
}

void UIButton::OnBtnUnHovered()
{
}
