#include "pch.h"
#include "UIBase.h"

UIBase::UIBase(Vector2 pos) : vPos(pos)
{
}

void UIBase::SetIsStretch(bool stretch, int32 sizeX, int32 sizeY)
{
	bIsStretch = stretch;
	iSizeX = sizeX;
	iSizeY = sizeY;
}
