#include "pch.h"
#include "UIButton.h"
#include "../Game/Game.h"
#include "../Managers/ResourceManager.h"
#include "../Managers/InputManager.h"
#include "../Resources/Texture.h"

UIButton::UIButton(Vector2 pos, string key, int32 width, int32 height, int32 fontSize, uint64 textColor)
	: Super(pos)
{
	_texture = ResourceManager::GetInstance()->GetTexture(key);
	if (_texture == nullptr) return;
	if (width != 0 || height != 0)
	{
		iSizeX = width;
		iSizeY = height;
		bIsStretch = true;
	}
	else
	{
		iSizeX = _texture->GetTextureSizeX();
		iSizeY = _texture->GetTextureSizeY();
	}
	iFontSize = fontSize;
	ulTextColor = textColor;
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
		_texture->RenderTexture(hdc, vPos, iSizeX, iSizeY);
	}

	if (sButtonText != L"")
	{
		SetBkColor(hdc, RGB(0, 0, 0));
		HFONT font = ResourceManager::GetInstance()->GetFont(iFontSize);
		HFONT oldFont = (HFONT)SelectObject(hdc, font);

		COLORREF oldColor = SetTextColor(hdc, ulTextColor);
		SetBkMode(hdc, TRANSPARENT);

		UINT oldAlign = GetTextAlign(hdc);
		float alignPos = vPos.x;

		if (_textAlign == EButtonTextAlign::LEFT)
		{
			SetTextAlign(hdc, TA_LEFT);
			alignPos -= iSizeX * 0.5f;
			alignPos += fMargin;
		}
		else if (_textAlign == EButtonTextAlign::RIGHT)
		{
			SetTextAlign(hdc, TA_RIGHT);
			alignPos += iSizeX * 0.5f;
			alignPos -= fMargin;
		}
		else
		{
			SetTextAlign(hdc, TA_CENTER);
		}

		::TextOut(hdc, alignPos, vPos.y - 5 - iFontSize * 3, sButtonText.c_str(), static_cast<int32>(sButtonText.size()));

		SetTextAlign(hdc, oldAlign);
		SelectObject(hdc, oldFont);
		SetTextColor(hdc, oldColor);
	}
}

void UIButton::OnBtnHovered()
{
}

void UIButton::OnBtnUnHovered()
{
}
