#include "pch.h"
#include "UIImage.h"
#include "../Managers/ResourceManager.h"
#include "../Resources/Texture.h"

UIImage::UIImage(Vector2 pos, string key, int32 width, int32 height)
	: Super(pos)
{
	_texture = ResourceManager::GetInstance()->GetTexture(key);
}

UIImage::~UIImage()
{
}

void UIImage::Update()
{
}

void UIImage::Render(HDC hdc)
{
	if (_texture)
	{
		if (bIsStretch)
		{
			_texture->RenderStretched(hdc, vPos, iSizeX, iSizeY);
		}
		else
		{
			_texture->Render(hdc, vPos);
		}
	}
}
